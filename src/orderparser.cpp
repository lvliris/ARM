#include "orderparser.h"
#include "converter.h"
#include "devstate.h"
#include <iostream>
#include <string>
#include <exception>

#define MAX_MODE_NUM 100

using namespace rapidjson;
using namespace std;

const char* MODE_FILE = "config/mode";
const char* TRIGGER_FILE = "config/trigger.json";

int ParseOrder(char* order, Uart& uart)
{
	cout << "parse order" << endl;
	//parse the order
	StringStream s(order);
	Document d;
	d.ParseStream(s);
	if(d.IsObject())
		cout << "is object" << endl;
	//SendOrder(d);

	//process the order
	Value::ConstMemberIterator iter = d.FindMember("opt");
	if(iter == d.MemberEnd())
	{
		cout << "can not find member opt" << endl;
		return -1;
	}

	Value& opt = d["opt"];
	if(opt == "set")
	{
		Value& act = d["info"]["act"];
		if(act == "add")
			AddModeOrder(d);
		else if(act == "mod")
			ModifyModeOrder(d);
		else if(act == "del")
			DeleteModeOrder(d);
		else if(act == "drop")
			DropMode(d);
		else if(act == "disable")
			DisableMode(d);
		else if(act == "enable")
			EnableMode(d);
		else if(act == "trigger")
			AddTrigger(d);
		else if(act == "distrigger")
			DeleteTrigger(d);
	}
	else if(opt == "send")
	{
		iter = d["info"].FindMember("mode_index");
		if(iter == d["info"].MemberEnd() || iter->value.IsNull())
			SendOrder(d, uart);
		else
		{
			int modeindex = iter->value.GetInt();
			//cout << modeindex << endl;
			SendMode(modeindex, uart);
		}
	}
	else if(opt == "getid")
	{
		uart.Write("<01004444U0**********FF>", strlen("<01004444U0**********FF>"));
	}
}

int ReadJsonFromFile(Document& df, const char* file)
{
	//open the config MODE_FILE, read only
	FILE *fp = fopen(file, "a+");
	if(fp == NULL)
	{
		cout << "open MODE_FILE error" << endl;
		return -1;
	}

	//get json from the file
	char readBuffer[10240];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	//Document df;
	df.ParseStream(is);

	fclose(fp);
	return 0;
}

int WriteJsonToFile(Document& df, const char* file)
{
	FILE* fp = fopen(file, "w");
	if(fp == NULL)
	{
		cout << "open MODE_FILE error" << endl;
		return -1;
	}

	//write json to the file
	char writeBuffer[10240];
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);
	df.Accept(writer);

	fclose(fp);
	return 0;
}

int AddModeOrder(Document& d)
{
	int modeindex = d["info"]["mode_index"].GetInt();
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" << endl;
		return -1;
	}
	cout << "adding mode..." << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);

	Document df;
	if(ReadJsonFromFile(df, filepath) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an array, Reset" << endl;
		df.SetObject();
		Value array(kArrayType);
		df.AddMember("enable", true, df.GetAllocator());
		df.AddMember("orders", array, df.GetAllocator());
	}

	d["opt"] = "send";

	//add the order into MODE_FILE
	df["orders"].PushBack(d, df.GetAllocator());

	cout << "file path: " << filepath << endl;
	if(WriteJsonToFile(df, filepath) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
	else
	{
		cout << "add mode order success" << endl;
		return 0;
	}
}

int ModifyModeOrder(Document& d)
{
	int modeindex = d["info"]["mode_index"].GetInt();
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" << endl;
		return -1;
	}
	cout << "modifying mode..." << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);

	Document df;
	if(ReadJsonFromFile(df, filepath) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the mode file is supposed to be an object, Reset" << endl;
		int ret = AddModeOrder(d);
		return ret;
	}

	bool find = false;
	//search the order to modify
	for(SizeType i = 0; i < df["orders"].Size(); i++)
	{
		if(df["orders"][i]["long_addr"] == d["long_addr"]
				&& df["orders"][i]["info"]["sub_seq"] == d["info"]["sub_seq"])
		{
			find = true;
			df["orders"][i]["info"]["state"] = d["info"]["state"];
			debug_msg("find exist order\n");
			break;
		}
	}

	//if the order was not found, add it
	if(!find)
	{
		debug_msg("not found, add a new order\n");
		int ret = AddModeOrder(d);
		return ret;
	}

	//write json to the file
	if(WriteJsonToFile(df, filepath) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
	else
	{
		cout << "modify mode order success" << endl;
		return 0;
	}
}

int DeleteModeOrder(Document& d)
{
	int modeindex = d["info"]["mode_index"].GetInt();
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" << endl;
		return -1;
	}
	cout << "delete mode..." << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);

	Document df;
	if(ReadJsonFromFile(df, filepath) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an array, Reset" << endl;
		return -1;
	}


	Value& orders = df["orders"];
	for(Value::ConstValueIterator iter = orders.Begin(); iter != orders.End(); iter++)
	{
		if((*iter)["long_addr"] == d["long_addr"] && 
				(*iter)["info"]["sub_seq"] == d["info"]["sub_seq"])
		{
			orders.Erase(iter);
			break;
		}
	}

	//write json to the file
	if(WriteJsonToFile(df, filepath) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
	else
	{
		cout << "delete mode order success" << endl;
		return 0;
	}
}

int DropMode(Document& d)
{
	int modeindex = d["info"]["mode_index"].GetInt();
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" << endl;
		return -1;
	}
	cout << "drop mode..." << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);

	//delete the mode file
	remove(filepath);
}

int EnableMode(Document& d)
{
	int modeindex = d["info"]["mode_index"].GetInt();
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" << endl;
		return -1;
	}
	cout << "enable mode..." << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);

	Document df;
	if(ReadJsonFromFile(df, filepath) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an array, Reset" << endl;
		return -1;
	}

	df["enable"] = true;

	//write json to the file
	if(WriteJsonToFile(df, filepath) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
	else
	{
		cout << "enable mode order success" << endl;
		return 0;
	}
}

int DisableMode(Document& d)
{
	int modeindex = d["info"]["mode_index"].GetInt();
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" << endl;
		return -1;
	}
	cout << "disable mode..." << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);
	//cout << "file path: " << filepath << endl;

	Document df;
	if(ReadJsonFromFile(df, filepath) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an array, Reset" << endl;
		return -1;
	}

	df["enable"] = false;

	//write json to the file
	if(WriteJsonToFile(df, filepath) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
	else
	{
		cout << "disable mode order success" << endl;
		return 0;
	}
}

int SendOrder(Document& d, Uart& uart)
{
	cout << "send order" << endl;
	StringBuffer buffer;
	Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(buffer);
	d.Accept(writer);

	cout << buffer.GetString() << endl;
	char order[256] = { 0 };
	if(Json2Str(d, order) > 0)
	{
		cout << order;
		uart.Write(order, strlen(order));
	}
	//UartSend(buffer.GetString);
}

int SendMode(int modeindex, Uart& uart)
{
	if(modeindex > MAX_MODE_NUM)
	{
		cout << "modeindex " << modeindex << " overflow" <<endl;
		return -1;
	}
	cout << "send mode " << modeindex << endl;

	char filepath[256] = {0};
	sprintf(filepath, "%s%d.json", MODE_FILE, modeindex);

	Document df;
	if(ReadJsonFromFile(df, filepath) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an array, Reset" << endl;
		return -1;
	}
	
	if(df["enable"] == false)
	{
		cout << "the mode is disable" << endl;
		return 0;
	}

	//iterate the document
	Value& orders = df["orders"];
	for(Value::ConstValueIterator iter = orders.Begin(); iter != orders.End(); iter++)
	{
		Document d;
		d.CopyFrom(*iter, d.GetAllocator());
		SendOrder(d, uart);
	}
}

int AddTrigger(Document& d)
{
	cout << "adding trigger..." << endl;
	Document df;
	if(ReadJsonFromFile(df, TRIGGER_FILE) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an object, Reset" << endl;
		df.SetObject();
	}
	
	cout << d["long_addr"].GetString() <<endl;
	Value::ConstMemberIterator itr = df.FindMember(d["long_addr"].GetString());

	//cout << "find member" << endl;
	//add trigger
	try
	{
		if(itr != df.MemberEnd())
		{
			//cout << "addr found, adding...";
			df[itr->name.GetString()] = d["info"]["mode_index"].GetInt();
		}
		else
		{
			//cout << "adding..." << endl;
			Value key(d["long_addr"], df.GetAllocator());
			Value val;
			val.SetInt(d["info"]["mode_index"].GetInt());
			df.AddMember(key, val, df.GetAllocator());
		}
	}
	catch(exception& e)
	{
		cout << "exception occours when adding trigger" << endl;
		return -1;
	}

	//write json to the file
	if(WriteJsonToFile(df, TRIGGER_FILE) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
	else
	{
		cout << "add trigger success" << endl;
		return 0;
	}
}

int DeleteTrigger(Document& d)
{
	cout << "deleting trigger..." << endl;

	Document df;
	if(ReadJsonFromFile(df, TRIGGER_FILE) < 0)
	{
		cout << "read file error" << endl;
		return -1;
	}

	if(!df.IsObject())
	{
		cout << "ERROR: the file is supposed to be an object, Reset" << endl;
		df.SetObject();
		return -1;
	}

	//delete the trigger
	if(df.HasMember(d["long_addr"]))
	{
		if(df.RemoveMember(d["long_addr"]))
		{
			cout << "delete trigger success" << endl;
		}
	}
	
	//write json to the file
	if(WriteJsonToFile(df, TRIGGER_FILE) < 0)
	{
		cout << "write json to file error" << endl;
		return -1;
	}
}

int DocumentSerialize(Document& d, char* buffer, int len)
{
	if(d.IsNull())
		return -1;

	StringBuffer strbuffer;
	Writer<StringBuffer, Document::EncodingType, ASCII<> > writer(strbuffer);
	d.Accept(writer);

	const char* str = strbuffer.GetString();
	int strlength = strlen(str);
	if(strlength > len)
		return -1;
	else
	{
		memcpy(buffer, str, strlength);
		return strlength;
	}
}
