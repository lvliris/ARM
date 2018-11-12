#include "httpclient.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#define GET_ORDER_SECONDS 1

char errorbuffer[1024];

void* getorder(void* arg)
{
	CURL* curl = (CURL*)arg;
	CURLcode ret;
	while(1)
	{
		ret = curl_easy_perform(curl);
		if(ret != CURLE_OK)
		{
			printf("error occurred when sending GET request\n");
			printf(errorbuffer);
			break;
		}
		sleep(GET_ORDER_SECONDS);
	}
}

HttpClient::HttpClient(char* MasterCode)
{
	//reset the array
	memset(_url, 0, 256);
	memset(_mastercode, 0, 9);
	memcpy(_mastercode, MasterCode, 8);

	//initialize the call back function
	HttpCallBack = NULL;

	//inti curl
	curl_global_init(CURL_GLOBAL_ALL);
	_curl = curl_easy_init();
}

HttpClient::~HttpClient()
{
	curl_easy_cleanup(_curl);
	curl_global_cleanup();
}

void HttpClient::SetURL(char* url)
{
	int len = strlen(url);
	memcpy(_url, url, len);
	printf("url:%s\n", _url);
}

void HttpClient::SetCallBack(size_t (*callback)(void*, size_t, size_t, void*), void* data)
{
	HttpCallBack = callback;
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, HttpCallBack);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, data);
}

void HttpClient::GetOrder()
{
	int url_len = strlen(_url);
	int mc_len = strlen(_mastercode);
	if(url_len == 0 || mc_len == 0)
		return;

	//generate the complete url
	char url[256] = { 0 };
	memcpy(url, _url, url_len);
	memcpy(url + url_len, _mastercode, 8);

	printf(url);
	//set the url, callback function
	curl_easy_setopt(_curl, CURLOPT_URL, url);
	curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorbuffer);
	
	//perform the GET request
	pthread_t tid;
	int ret = pthread_create(&tid, 0, getorder, _curl);
	/*CURLcode res = curl_easy_perform(_curl);
	if(res != CURLE_OK)
	{
		printf("error\n");
		printf(errorbuffer);
	}*/
}

int HttpClient::SendState(char* state)
{
	int url_len = strlen(_url);
	int mc_len = strlen(_mastercode);
	if(url_len == 0 || mc_len == 0)
		return -1;

	//generate the complete url
	char url[256] = { 0 };
	memcpy(url, _url, url_len);
	memcpy(url + url_len, _mastercode, mc_len);

	//set the url, callback function
	curl_easy_setopt(_curl, CURLOPT_URL, url);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, state);

	//perform the POST request
	CURLcode ret = curl_easy_perform(_curl);
	if(ret != CURLE_OK)
	{
		printf("error occurred when sending POST request\n");
		return -1;
	}

	return 0;
}

