#include "converter.h"
#include <string>
#include <iostream>
#include <stdio.h>

#define ORDER_HEAD      '<'
#define ORDER_END       '>'
#define ORDER_ALARM     'P'
#define ORDER_SET       'S'
#define ORDER_SEND      'T'
#define ORDER_CONTROL   'X'
#define ORDER_GETID     'U'
#define ORDER_ADD       'Y'
#define ORDER_OPEN      'H'
#define ORDER_CLOSE     'G'
#define ORDER_M         'M'
#define ORDER_DELETE    'R'
#define ORDER_STATE     'Z'

#define ORDER_OPTS_POS_OLD  9
#define ORDER_OPTS_POS_NEW  13

#define ADDR_FILE "config/addr.json"	

using namespace std;
using namespace rapidjson;

int UpdateAddr(char* long_addr, char* short_addr)
{
	cout << "updating address" << endl;
	Document df;
	if(ReadJsonFromFile(df, ADDR_FILE) < 0)
	{
		cout << "read addr file failed" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
	    cout << "ERROR: the file is supposed to be an object, Reset" << endl;
	    df.SetObject();
    }

	//restore the addr
	char l_addr[9] = { 0 };
	char s_addr[5] = { 0 };
	memcpy(l_addr, long_addr, 8);
	memcpy(s_addr, short_addr, 4);
	//find the long addr and update
	Value::ConstMemberIterator itr = df.FindMember(l_addr);
	if(itr != df.MemberEnd())
	{
		Value s;
		s.SetString(s_addr, 4, df.GetAllocator());
		df[l_addr] = s;
	}
	else
	{
		Value key(l_addr, df.GetAllocator());
		Value val(s_addr, df.GetAllocator());
		df.AddMember(key, val, df.GetAllocator());
	}

	//write json to file
	if(WriteJsonToFile(df, ADDR_FILE) < 0)
	{
		cout << "write json to addr file failed" << endl;
		return -1;
	}
	else
	{
		cout << "update addr success" << endl;
		return 0;
	}
}


int ConvertAddr(const char* long_addr, char* short_addr)
{
	cout << "converting address" << endl;
	Document df;
	if(ReadJsonFromFile(df, ADDR_FILE) < 0)
	{
		cout << "read addr file failed" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
	    cout << "ERROR: the file is supposed to be an object" << endl;
		return -1;
		}

	Value::ConstMemberIterator itr = df.FindMember(long_addr);
	if(itr != df.MemberEnd())
	{
		const char* s_addr = df[long_addr].GetString();
		memcpy(short_addr, s_addr, strlen(s_addr));
		return 0;
	}
	else
	{
		cout << "ERROR: failed to find the long addr" << endl;
		return -1;
	}
}


int Json2Str(rapidjson::Document& d, char* str)
{
	if(!d.IsObject() || str == NULL)
		return -1;

	//init the string
	memcpy(str, "<00000000U0**********FF>", 24);

	Value& opt = d["opt"];
	if(opt == "send")
	{
		str[ORDER_OPTS_POS_OLD] = 'X';
	}
	else if(opt == "set")
	{
		str[ORDER_OPTS_POS_OLD] = 'S';
	}
	else if(opt == "getid")
	{
		str[ORDER_OPTS_POS_OLD] = 'U';
	}
	else
	{
		return 0;
	}

	Value& state = d["info"]["state"];
	if(state.IsNull())
		return -1;

	if(state == "on")
	{
		str[ORDER_OPTS_POS_OLD + 1] = ORDER_OPEN;
	}
	else if(state == "off")
	{
		str[ORDER_OPTS_POS_OLD + 1] = ORDER_CLOSE;
	}
	else
	{
		str[ORDER_OPTS_POS_OLD + 1] = ORDER_M;
	}

	if(d["type"].IsNull() || d["addr"].IsNull() || d["info"]["data"].IsNull())
		return -1;

	str[0] = ORDER_HEAD;
	memcpy(str + 1, d["type"].GetString(), 4);
	memcpy(str + 5, d["addr"].GetString(), 4);

	int datalen = strlen(d["info"]["data"].GetString());
	memcpy(str + 11, d["info"]["data"].GetString(), datalen);

	str[11 + datalen] = ORDER_END;
	str[12 + datalen] = '\r';
	str[13 + datalen] = '\n';
	
	return 1;
}


int Str2Json(char* order, Document& d)
{
	if(order == NULL)
		return -1;

	Value type;
	Value long_addr;
	Value addr;
	Value opt;
	Value info(kObjectType);
	Value act;
	Value seq;
	Value state;
	Value mode_index;
	Value enable;
	Value data;

	d.SetObject();

	if(order[0] == '#')
	{
		if(order[ORDER_OPTS_POS_NEW] == ORDER_ADD)
		{
			UpdateAddr(order + 5, order + 15);
			return -1;
		}
		//OLD FORMAT ORDER, TO BE CHANGED
		if(order[ORDER_OPTS_POS_OLD] == ORDER_GETID)
		{
			opt = "gotid";
			data.SetString(order + 1, 8, d.GetAllocator());
		}
	}
	else
	{
		if(order[0] != ORDER_HEAD)
			return -1;
		char* orderend = strchr(order, ORDER_END);
		if(orderend == NULL)
			return -1;
		//OLD FORMAT ORDER, TO BE CHANGED
		if(order[ORDER_OPTS_POS_OLD] == ORDER_GETID)
		{
			opt = "getid";
		}
		else if(order[ORDER_OPTS_POS_OLD] == ORDER_SEND)
		{
			opt = "send";
			int modeindex = atoi(order + ORDER_OPTS_POS_OLD + 4);
			mode_index.SetInt(modeindex);
		}
	}

	//init the document
	char opt_c = order[ORDER_OPTS_POS_NEW];
	char opt_f = order[ORDER_OPTS_POS_NEW + 1];
	char type_t = order[2];

	if(opt_c == ORDER_CONTROL || opt_c == ORDER_SET || opt_c == ORDER_STATE)
	{
		//d[opt]
		if(opt_c == ORDER_CONTROL || opt_c == ORDER_STATE)
		{
			opt = "send";
		}
		else
		{
			opt = "set";
			if(type_t == 'A' || type_t == 'D')
			{
				if(opt_f == ORDER_DELETE)
					act = "distrigger";
				else
					act = "trigger";
			}
			else
			{
				if(opt_f == ORDER_DELETE)
					act = "del";
				else
					act = "mod";
			}
			int modeidx = atoi(order + ORDER_OPTS_POS_NEW + 4);
			mode_index.SetInt(modeidx);
		}

		//d[info][state]
		char act_c = order[ORDER_OPTS_POS_NEW + 1];
		if(act_c == ORDER_CLOSE)
		{
			state = "off";		
		}
		else
		{
			state = "on";
		}

		//d[info][sub_seq]
		int seq_i = order[ORDER_OPTS_POS_NEW + 3] - '0';
		seq.SetInt(seq_i);

		//d[info][data]
		data.SetString(order + ORDER_OPTS_POS_NEW + 2, 
				strlen(order) - ORDER_OPTS_POS_NEW - 3,
				d.GetAllocator());

		//d[type]
	    type.SetString(order + 1, 4, d.GetAllocator());

		//d[long_addr]
		long_addr.SetString(order + 5, 8, d.GetAllocator());

		//d[addr]
		char s_addr[5] = {0};
		ConvertAddr(long_addr.GetString(), s_addr);
		addr.SetString(s_addr, 4, d.GetAllocator());
	}
	else if(opt_c == ORDER_SEND)
	{
		char act_c = order[ORDER_OPTS_POS_NEW + 1];
		int modeidx = atoi(order + ORDER_OPTS_POS_NEW + 4);
		mode_index.SetInt(modeidx);
		switch(act_c)
		{
			case ORDER_OPEN:
				opt = "send";
				break;
			case ORDER_CLOSE:
				opt = "set";
				act = "disable";
				break;
			case ORDER_SET:
				opt = "set";
				act = "enable";
				break;
			case ORDER_DELETE:
				opt = "set";
				act = "drop";
				break;
		}
	}

	d.AddMember("type", type, d.GetAllocator());
	d.AddMember("long_addr", long_addr, d.GetAllocator());
	d.AddMember("addr", addr, d.GetAllocator());
	d.AddMember("opt", opt, d.GetAllocator());
	info.AddMember("act", act, d.GetAllocator());
	info.AddMember("state", state, d.GetAllocator());
	info.AddMember("sub_seq", seq, d.GetAllocator());
	info.AddMember("mode_index", mode_index, d.GetAllocator());
	info.AddMember("data", data, d.GetAllocator());
	d.AddMember("info", info, d.GetAllocator());

	return 1;
}
