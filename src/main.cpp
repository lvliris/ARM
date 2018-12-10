#include "orderparser.h"
#include "uart.h"
#include "httpclient.h"
#include "converter.h"
#include "orderpoll.h"
#include "userpattern.h"
#include "devstate.h"

#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace rapidjson;

//Uart uart("/dev/ttyUSB0");

void uartcallback(char* data, int len)
{
	//char* cdata = (char*)data;
	printf("\nuart callback function\n");
	printf(data);
	//Document d = Str2Json(data);
	//SendOrder(d);
	//uart.Write(data, len);
	printf("\nuart callback finish\n");
}


size_t httpcallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	printf("\nhttp callback function\n");
	printf((char*)ptr);
	printf("http callback finish\n");
	return nmemb;
}


int main(int argc, char* argv[])
{   
    char* json[10];
    json[0] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"add\",\"mode_seq\":1, \"sub_seq\":1, \"state\": \"off\"}}";

    json[1] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"mod\",\"mode_seq\":1, \"sub_seq\":1, \"state\": \"on\"}}";

    json[2] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"del\",\"mode_seq\":1, \"sub_seq\":1, \"state\": \"off\"}}";

    json[3] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"enable\",\"mode_seq\":1, \"sub_seq\":1, \"state\": \"off\"}}";

    json[4] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"disable\",\"mode_seq\":1, \"sub_seq\":1, \"state\": \"off\"}}";

    json[5] = "{\"long_addr\": \"12345678\",\"opt\":\"send\", \"info\":{\"act\":\"disable\",\"mode_seq\":1, \"sub_seq\":1, \"state\": \"off\"}}";

    json[6] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"trigger\",\"mode_index\":1, \"sub_seq\":1, \"state\": \"off\"}}";

    json[7] = "{\"long_addr\": \"12345678\",\"opt\":\"set\", \"info\":{\"act\":\"distrigger\",\"sub_seq\":1, \"state\": \"off\"}}";
    
    //int index = atoi(argv[1]); 
	//cout << json[index] << endl;
    //ParseOrder(json[index]);

	//uart.Init(57600, uartcallback);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm* t = localtime(&(tv.tv_sec));
	char logfile[20] = {0};
	sprintf(logfile, "%d-%d-%d.log", t->tm_year+1900, t->tm_mon+1, t->tm_mday);

	char cmd[1024] = {0};
	//OrderPoll op;
	char* state[10];
	state[0] = "<010012345678Z701********FF>";
	state[1] = "<010022222222Z701********FF>";
	state[2] = "<010012345678Z601********FF>";
	state[3] = "<010022222222Z601********FF>";
	state[4] = "<010011111111Z701********FF>";
	state[5] = "<010033333333Z701********FF>";
	state[6] = "<010011111111Z601********FF>";
	state[7] = "<010033333333Z601********FF>";
	Document d;
	cout << "testing..." << endl;

	/*string teststr;
	State s;
	DevState* ds = DevState::GetInstance();
	cin >> teststr;
	s = ds->GetState(teststr);
	cout << s.type << ' ' << s.state << endl;
	cin >> teststr;
	s = ds->GetState(teststr);
	cout << s.type << ' ' << s.state << endl;*/

	int round = 0;
	int date = 1;
	char file_name[20] = {0};
	/*while(round < 800)
	{
		debug_msg("sampling...\n");
		if( Str2Json(state[round%8], d))
		{
			DevState* ds = DevState::GetInstance();
			string addr = d["long_addr"].GetString();
			int new_state = d["info"]["state"].GetInt();
			State s(new_state);
			ds->UpdateState(addr, s);
			memset(cmd, 0, 1024);
			DocumentSerialize(d, cmd, 1024);
			cout << "state " << cmd << endl;
			if(round % 80 == 0)
			{
				sprintf(file_name, "log/2018-11-%d.log", date++);
			}
			SaveHistory(cmd, file_name);
		}
		if(round%2)
			sleep(3);
		round++;
	}//*/

	ItemBasedCF icf;
	/*DevState* ds = DevState::GetInstance();
	string addr = "12345678";
	State s("0300", "AAAA", 0);
	ds->UpdateState(addr, s);*/
	
	//load the file
	struct dirent *filename;
	DIR *dir;
	dir = opendir("log/");
	while((filename = readdir(dir)) != NULL) 
	//for(int i = 1; i <= 10; i++)
	{
		printf("load file %s\n", filename->d_name);
		if(!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
		{
			continue;
		}
		//vector<vector<int> > data(200, vector<int>(5, 0));
		//sprintf(file_name, "2018-11-%d.log", date++);
		char* file_name = filename->d_name;
		debug("%s\n", file_name);
		//Vectorize(file_name, data);
		icf.LoadData(file_name);

		//perform pattern mining
		icf.PatternMining();

		//print the recognized patterns
		vector<int> temp;
		icf.Recommend(temp);
	}

	//load the second file
	/*debug("2018-11-18.log");
	Vectorize("2018-11-18.log", data);
	icf.LoadData(data);*/


	//save the patterns
	printf("saving patterns...\n");
	SavePatterns(icf.Patterns);

	//hold on
	while(true)
	{
		sleep(100);
	}
} 
