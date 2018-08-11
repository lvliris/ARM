#include "orderparser.h"
#include "uart.h"
#include <stdlib.h>

void testcallback(char* data, int len)
{
	//char* cdata = (char*)data;
	printf(data);
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
    
    int index = atoi(argv[1]); 
    ParseOrder(json[index]);

	Uart uart(argv[2]);
	uart.Init(57600, testcallback);
} 
