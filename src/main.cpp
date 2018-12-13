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
	//uart.Init(57600, uartcallback);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm* t = localtime(&(tv.tv_sec));
	char logfile[20] = {0};
	sprintf(logfile, "%d-%d-%d.log", t->tm_year+1900, t->tm_mon+1, t->tm_mday);

	//OrderPoll op;

	int mode_num = atoi(argv[1]);
	int habit_num = atoi(argv[2]);
	int habit_time;
	vector<vector<int> > modes;
	vector<int> habits;
	for(int i = 0; i < mode_num; i++)
	{
		vector<int> mode;
		char mode_path[50] = {0};
		sprintf(mode_path, "config/mode%d.json", i);
		mode = ReadModeFromFile(mode_path);
		modes.push_back(mode);
		if(i < habit_num)
		{
			Document d;
			ReadJsonFromFile(d, mode_path);
			habit_time = d["time"].GetInt();
			habits.push_back(habit_time);
		}
	}
	cout << "----------mode gt----------" << endl;
	PrintVector(modes);
	sleep(3);

	ItemBasedCF icf;
	UserBasedCF ucf;
	
	//load the log files
	struct dirent *filename;
	DIR *dir;
	int iter = 0;
	float last_time_var = 0;
	float last_precision = 0;
	float last_recall = 0;
	Document df;
	Value precision(kArrayType);
	Value recall(kArrayType);
	Value habitvar(kArrayType);
	Value habitnum(kArrayType);
	df.SetObject();

	dir = opendir("log/");
	while((filename = readdir(dir)) != NULL) 
	{
		//skip the . and ..
		if(!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
		{
			continue;
		}
		//loading file
		char* file_name = filename->d_name;
		debug("loading file: %s\n", file_name);
		icf.LoadData(file_name);

		//perform pattern mining
		icf.PatternMining();

		//perform habit mining
		ucf.FindPatternTime(icf.Patterns, icf.UserData);
		ucf.PatternMining();

		//print the recognized patterns
		vector<int> temp;
		//icf.Recommend(temp);
		temp = ucf.Recommend(0);
		
		//evaluate the result
		if(!temp.empty())
		{
			last_time_var = RecognitionEvaluate(temp, habits);
		}
		habitvar.PushBack(last_time_var, df.GetAllocator());
		habitnum.PushBack(temp.size(), df.GetAllocator());

		vector<float> res;
		res = RecognitionEvaluate(icf.Patterns, modes);
		if(!res.empty())
		{
			last_precision = res[0];
			last_recall = res[1];
		}
		precision.PushBack(last_precision, df.GetAllocator());
		recall.PushBack(last_recall, df.GetAllocator());
		cout << "iteration " << iter++ << ": precision and recall: " << res[0] << ',' << res[1] << endl;
	}

	df.AddMember("precision", precision, df.GetAllocator());
	df.AddMember("recall", recall, df.GetAllocator());
	df.AddMember("habitvar", habitvar, df.GetAllocator());
	df.AddMember("habitnum", habitnum, df.GetAllocator());

	if(WriteJsonToFile(df, "config/evaluation.json") > 0)
	{
		cout << "write evaluation result to file success!!!" << endl;
	}

	for(int i = 0; i < habits.size(); i++)
	{
		cout << habits[i] << endl;
	}
	
	//find the habit
	ucf.PatternMining();
	ucf.Recommend(0);
	icf.Recommend(vector<int>(1));
	cout << "----------mode gt----------" << endl;
	PrintVector(modes);

	//save the final patterns
	printf("saving patterns...\n");
	//SavePatterns(icf.Patterns);

	//hold on
	while(false)
	{
		sleep(100);
	}
} 
