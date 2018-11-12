#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <curl/easy.h>

class HttpClient
{
public:
	HttpClient(char* MasterCode);
	~HttpClient();
	void SetURL(char* url);
	void SetCallBack(size_t (*callback)(void*, size_t, size_t, void*), void* data);
	void GetOrder();
	int SendState(char* state);

private:
	size_t (*HttpCallBack)(void* ptr, size_t size, size_t nmemb, void* stream);
	char _url[256];
	char _mastercode[9];
	CURL* _curl;
};
#endif
