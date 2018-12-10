#include "devstate.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <pthread.h>
#include <sys/io.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace rapidjson;

DevState* DevState::instance = NULL;
char* DevState::state_file = "config/state.json";
char* DevState::addr_file = "config/addr.json";

DevState::DevState()
{
	cout << "device state initialization" << endl;
	Init();

	//start to save the state every minute
	Launch();
}

DevState* DevState::GetInstance()
{
	if(instance == NULL)
	{
		instance = new DevState();
	}

	return instance;
}

void DevState::Init()
{
	fstream fin;
	//fin.open(state_file, ios::in);
	int res = access(state_file, 0);
	if(res != -1)
	{
		cout << "state file exist" << endl;
		//fin.close();

		ReadJsonFromFile(StateDoc, state_file);

		//iterate the states, save in memory
		for(Value::ConstMemberIterator iter = StateDoc.MemberBegin();
				iter != StateDoc.MemberEnd(); iter++)
		{
			string long_addr(iter->name.GetString());
			string type(iter->value["type"].GetString());
			string short_addr(iter->value["short_addr"].GetString());
			int _state = iter->value["state"].GetInt();
			State state(type, short_addr, _state);

			//save in memory
			StateMap[long_addr] = state;
			debug_msg("%s-%d\n", long_addr.c_str(), _state);
		}
	}
	else
	{
		cout << "state file not exist, set default state of devices from addr file" << endl;
		//fin.close();

		Document da;
		if(ReadJsonFromFile(da, addr_file) < 0)
		{
			cout << "read addr file failed" << endl;
			return;
		}
		
		if(!da.IsObject())
		{
			da.SetObject();
		}

		//document for saving the state
		//Document StateDoc;
		StateDoc.SetObject();

		//iterate the address, set the default state
		for(Value::ConstMemberIterator iter = da.MemberBegin();
				iter != da.MemberEnd(); iter++)
		{
			string long_addr(iter->name.GetString());
			string short_addr(iter->value.GetString());
			string type("0100");
			int _state = 0;
			State state(type, short_addr, _state);

			//save in memory
			StateMap[long_addr] = state;

			//used for saving in file
			//there is another easier way to assign value
			//what a pity to know it too late :(
			//I'll change it when i wanna do
			//Value sType = "01";
			//Value ssState = _state;
			Value sState(kObjectType);
			Value sType;
			Value sShortAddr;
			Value sLongAddr;
			Value ssState;
			sType.SetString(type.c_str(), type.size(), StateDoc.GetAllocator());
			sShortAddr.SetString(short_addr.c_str(), short_addr.size(), StateDoc.GetAllocator());
			sLongAddr.SetString(long_addr.c_str(), long_addr.size(), StateDoc.GetAllocator());
			ssState.SetInt(_state);
			sState.AddMember("type", sType, StateDoc.GetAllocator());
			sState.AddMember("short_addr", sShortAddr, StateDoc.GetAllocator());
			sState.AddMember("state", ssState, StateDoc.GetAllocator());
			debug_msg("sstate:\ntype:%s\nshort addr:%s\nstate:%d\n", sState["type"].GetString(), sState["short_addr"].GetString(), sState["state"].GetInt());

			//add to the document for saving
			StateDoc.AddMember(sLongAddr, sState, StateDoc.GetAllocator());
			debug_msg("long addr:%s\n", long_addr.c_str());
		}

		//debug
		char temp[1024] = {0};
		DocumentSerialize(StateDoc, temp, 1024);
		debug_msg("%s\n", temp);
		for(Value::ConstMemberIterator iter = StateDoc.MemberBegin();
				iter != StateDoc.MemberEnd(); iter++)
		{
			//debug_msg("%s-%d\n", iter->name.GetString(), iter->value["state"].GetInt());
		}

		if(WriteJsonToFile(StateDoc, state_file) == 0)
		{
			debug_msg("write to file success\n");
		}
		else
		{
			debug_msg("write to file failed\n");
		}
	}
}

void DevState::Launch()
{
	pthread_t pth;
	pthread_create(&pth, NULL, SaveState, (void*)this);
}

struct State DevState::GetState(string long_addr)
{
	map<string, State>::iterator iter = StateMap.find(long_addr);
	if(iter != StateMap.end())
	{
		return iter->second;
	}
	else
	{
		//return a invalid state if the address is not found
		State istate(false);
		debug_msg("address not found\n");
		return istate;
	}
}

int DevState::UpdateState(string long_addr, struct State& state)
{
	//save to map
	if(!state.short_addr.empty())
	{
		StateMap[long_addr].short_addr = state.short_addr;
	}
	StateMap[long_addr].type = state.type;
	StateMap[long_addr].state = state.state;

	//debug_msg("statedoc is object: %d\n", StateDoc.IsObject());
	//save to document
	Value::ConstMemberIterator iter = StateDoc.FindMember(long_addr.c_str());
	if(iter != StateDoc.MemberEnd())
	{
		//change the state, not change the short address if not given
		if(!state.short_addr.empty())
		{
			Value new_addr(state.short_addr.c_str(), StateDoc.GetAllocator());
			StateDoc[long_addr.c_str()]["short_addr"] = new_addr;
			debug_msg("update short address\n");
		}

		Value new_type(state.type.c_str(), StateDoc.GetAllocator());
		Value new_state(state.state);
		StateDoc[long_addr.c_str()]["type"] = new_type;
		StateDoc[long_addr.c_str()]["state"] = new_state;
		debug_msg("update state\n");
	}
	else
	{
		//add a new item
		Value sLongAddr(long_addr.c_str(), StateDoc.GetAllocator());

		Value sState(kObjectType);
		Value sType(state.type.c_str(), StateDoc.GetAllocator());
		Value sShortAddr(state.short_addr.c_str(), StateDoc.GetAllocator());
		Value ssState(state.state);

		sState.AddMember("type", sType, StateDoc.GetAllocator());
		sState.AddMember("short_addr", sShortAddr, StateDoc.GetAllocator());
		sState.AddMember("state", ssState, StateDoc.GetAllocator());

		StateDoc.AddMember(sLongAddr, sState, StateDoc.GetAllocator());
	}
}

void* DevState::SaveState(void* __this)
{
	//save the states every minute
	DevState* _this = (DevState*)__this;
	while(true)
	{
		debug_msg("saving states...\n");
		WriteJsonToFile(_this->StateDoc, _this->state_file);
		sleep(6);
	}
}
