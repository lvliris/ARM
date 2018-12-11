#include "devstate.h"
#include <string>
#include <fstream>
#include <stdio.h>
#include <sys/time.h>

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

using namespace std;
using namespace rapidjson;

const char* RP_MODE_DIR = "config/rp/";

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
	Value time;

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

		//overwrite when it is state
		if(opt_c == ORDER_STATE)
		{
			/*DevState* ds = DevState::GetInstance();
			string sLongAddr(order+5, 8);
			State sState = ds->GetState(sLongAddr);*/
			state = order[ORDER_OPTS_POS_NEW + 1] - '0';
			debug_msg("feed back state: %d\n", state.GetInt());

			//find the changed bit, then judge 1 or 0
			/*if((state_back ^ sState.state) & state_back)
			{
				state = "on";
			}
			else
			{
				state = "off";
			}*/
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
	else
	{
		debug_msg("undefined order\n");
		return -1;
	}

	//add time stamp
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time.SetUint64(tv.tv_sec);

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
	d.AddMember("time", time, d.GetAllocator());

	return 1;
}


int SaveHistory(char* data, char* file_name)
{
	struct timeval tv;
	struct tm *t;
	time_t ts;

	//get the time since 1970
	gettimeofday(&tv, NULL);
	ts = tv.tv_sec;

	//get the broken-down time since 1900
	t = localtime(&ts);

	//format time
	char timestamp[24] = {0};
	sprintf(timestamp, "%d-%d-%d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);

	//char file_name[100] = {0};
	//sprintf(file_name, "%s%s.log", LOG_DIR, timestamp);

	//open the logfile of the day, add a history entry
	ofstream fout;
	fout.open(file_name, ios::out | ios::app);
	fout << data << endl;
	fout.close();

	//cout << data << endl;
}


int GetTimeofToday(time_t t)
{
	struct tm *tb = localtime(&t);
	int hour = tb->tm_hour;
	int min = tb->tm_min;
	int sec = tb->tm_sec;

	return 3600*hour + 60*min + sec;
}


map<string, int> MapAddr2Index()
{
	//read the address document
	Document df;
	if(ReadJsonFromFile(df, ADDR_FILE) < 0)
	{
		cout << "read addr file failed" << endl;
	}
	
	//iterate the documen, get a map of address to index
	map<string, int> addr_index;
	int index = 0;
	for(Value::ConstMemberIterator itr = df.MemberBegin();
			itr != df.MemberEnd(); itr++)
	{
		string addr(itr->name.GetString());

		//if the type is light, there may be more than one subdevice
		DevState* ds = DevState::GetInstance();
		State s = ds->GetState(addr);
		int dtype = 0;
		if(s.type == "0100" || s.type == "0200" || s.type == "0300")
		{
			dtype = s.type[1] - '0';
		}
		//debug("addr: %s\ndtype: %d\nstype: %s\n", addr.c_str(), dtype, s.type.c_str());

		string sub[3] = {"1", "2", "3"};
		if(dtype > 0 && dtype < 4)
		{
			for(int i = 0; i < dtype; i++)
			{
				string addr_sub = addr + sub[i];
				addr_index[addr_sub] = index++;
				//cout << addr_sub << '-' << index - 1 << endl;
				//debug("addr2inex:*****************\n%s-%d\n", addr_sub.c_str(), index - 1);
			}
		}
		else
		{
			addr_index[addr] = index++;
			//debug("addr2inex:****************\n%s-%d\n", addr.c_str(), index - 1);
		}
	}

	return addr_index;
}


vector<string> GetAddrList(map<string, int> &addr_index)
{
	//initialize the address list with 100 entries
	vector<string> addr_list(100, string(20, '\0'));
	for(map<string, int>::iterator iter = addr_index.begin();
			iter != addr_index.end();)
	{
		if(iter->second < addr_list.size())
		{
			addr_list[iter->second] = iter->first;
			iter++;
		}
		else
		{
			//increase the size of the vector
			//add the address in next iteration in case insufficient size
			addr_list.resize(addr_list.size() + 100, string(20, '\0'));
		}
	}

	return addr_list;
}


//read the log file, vectorize the log by calculate the relative time to
//the beginning of today.
//this function should be called one time at the end of the day 
void Vectorize(char* file_name, vector<vector<int> > &user_data)
{
	char log_path[100] = {0};
	sprintf(log_path, "%s%s", LOG_DIR, file_name);

	//get the addr to index map
	map<string, int> addr_index = MapAddr2Index();
	/*for(map<string, int>::iterator iter = addr_index.begin();
			iter != addr_index.end(); iter++)
	{
		debug("-------------------------\n%s: %d\n", iter->first.c_str(), iter->second);
	}*/

	//open the log file
	ifstream lin(log_path);

	//read the history entries
	char entry[1024];
	int time_index = 0;
	int last_time_index = 0;
	int dev_index = 0;
	int data_size = user_data.size();
	int dev_num = user_data[0].size();
	while(!lin.eof())
	{
		memset(entry, 0, 1024);
		lin.getline(entry, 1024);
		static int entries = 1;
		//cout << entries++ << ':' << entry << endl;

		if(strlen(entry) == 0)
			break;

		Document d;
		d.Parse(entry);

		//debug_msg("parse log: %d\n", d.IsObject());

		//seen as the same time within 100 seconds
		time_t t = d["time"].GetUint64();
		string addr = d["long_addr"].GetString();
		int state = d["info"]["state"].GetInt();
		string sub[3] = {"1", "2", "3"};
		int sub_seq = d["info"]["sub_seq"].GetInt() - 1;
		if(sub_seq < 0)
			sub_seq = 0;
		addr += sub[sub_seq];
		//cout << "log addr: " << addr << endl;

		time_index = GetTimeofToday(t) / TIME_QUANT % data_size;
		dev_index = addr_index[addr];

		if(time_index > user_data.size())
		{
			cout << "index overflow " << time_index << ',' << dev_index << endl;
			return;
		}

		for(int i = last_time_index; i < time_index; i++)
		{
			for(int j = 0; j < dev_num; j++)
			{
				user_data[i+1][j] = user_data[i][j];
			}
		}

		last_time_index = time_index;

		if(state & (1 << sub_seq))
		{
			user_data[time_index][dev_index] = 1;
		}
		else
		{
			user_data[time_index][dev_index] = 0;
		}

		//cout << "index: " << time_index << ' ' << dev_index << endl;
	}
	lin.close();

	//merge the adjacent vector to avoid pattern cut off
	int rows = user_data.size();
	int cols = user_data[0].size();
	//PrintVector(user_data);
	for(int i = 0; i < rows - 1; i++)
	{
		int count = 0;
		for(int j = 0; j < cols; j++)
		{
			//user_data[i][j] += user_data[i+1][j];
			//set to 1 if it is bigger than 1
			if(user_data[i][j] > 1)
				user_data[i][j] = 1;
			if(user_data[i][j] > 0)
				count++;
		}
		/*if(count > 1)
		{
			for(int j = 0; j < cols; j++)
				cout << setw(2) << user_data[i][j] << ' ';
			cout << endl;
		}//*/
	}
	//PrintVector(user_data);
}


void SavePatterns(const vector<vector<int> > &patterns)
{
	//get the addr to index map
	map<string, int> addr_index = MapAddr2Index();

	//get the address list
	vector<string> addr_list = GetAddrList(addr_index);

	//iterate the patterns and save them to file
	int num_patterns = patterns.size();
	vector<int>::iterator iter;
	for(int i = 0; i < num_patterns; i++)
	{
		for(int j = 0; j < patterns[i].size(); j++)
		{
			if(patterns[i][j] != 0)
			{
				//action state
				string sState;
				if(patterns[i][j] == 1)
					sState = "on";
				else
					sState = "off";

				//address with long address and subseq
				string addr = addr_list[j];
				string long_addr = addr.substr(0, 8);
				int sub_seq = addr[addr.size() - 1] - '0';
				int mode_index = RP_OFFSET + i;
				char data[20] = {0};
				sprintf(data, "%02d%2d******FF", sub_seq, mode_index);
				debug_msg("%s-%s\n", long_addr.c_str(), data);

				//save the recognized patterns in json format to file
				DevState* state = DevState::GetInstance();
				State s = state->GetState(long_addr);

				Document d;
				d.SetObject();

				//build info
				Value info(kObjectType);
				Value dAct;
				Value dState;
				Value dSub_seq;
				Value dMode_index;
				Value dData;

				dAct.SetString("mod", d.GetAllocator());
				dState.SetString(sState.c_str(), d.GetAllocator());
				dSub_seq.SetInt(sub_seq);
				dMode_index.SetInt(mode_index);
				dData.SetString(data, d.GetAllocator());

				info.AddMember("act", dAct, d.GetAllocator());
				info.AddMember("state", dState, d.GetAllocator());
				info.AddMember("sub_seq", dSub_seq, d.GetAllocator());
				info.AddMember("mode_index", dMode_index, d.GetAllocator());
				info.AddMember("data", dData, d.GetAllocator());

				//build order
				Value dType;
				Value dLongAddr;
				Value dShortAddr;
				Value dOpt;
				
				dType.SetString(s.type.c_str(), d.GetAllocator());
				dLongAddr.SetString(long_addr.c_str(), d.GetAllocator());
				dShortAddr.SetString(s.short_addr.c_str(), d.GetAllocator());
				dOpt.SetString("set", d.GetAllocator());

				d.AddMember("type", dType, d.GetAllocator());
				d.AddMember("long_addr", dLongAddr, d.GetAllocator());
				d.AddMember("addr", dShortAddr, d.GetAllocator());
				d.AddMember("opt", dOpt, d.GetAllocator());
				d.AddMember("info", info, d.GetAllocator());

				//save the pattern by add mode order
				ModifyModeOrder(d);
			}
		}
	}
}

