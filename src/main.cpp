#include "orderparser.h"
#include "uart.h"
#include "httpclient.h"
#include "converter.h"
#include "orderpoll.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

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
	char cmd[1024];
	OrderPoll op;
	/*HttpClient hc("AA00FF01");
	hc.SetURL("http://101.201.211.87:8080/zfzn02/servlet/ElectricOrderServlet?masterCode=");
	hc.SetCallBack(httpcallback);
	hc.GetOrder();//*/
	while(1)
	{
		cout << "runnin..." << endl;
		sleep(5);
		//hc.GetOrder();
		/*sleep(1);
		memset(cmd, 0, 1024);
		cin.getline(cmd, 64);
		cout << "send to uart: " << cmd << endl;
		Document d;
		cout << "string to json" << endl;
		if(Str2Json(cmd, d) < 0)
			continue;
		cout << "serialize document:" << endl;
		DocumentSerialize(d, cmd, 1024);
		cout << cmd << "\nparse order" << endl;
		ParseOrder(cmd);
		Json2Str(d, cmd);
		cout << cmd;
		//uart.Write(cmd, strlen(cmd));//*/
	}
} 
