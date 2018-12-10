#ifndef ORDER_PARSER_H_
#define ORDER_PARSER_H_
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "cstdio"

#include "uart.h"

int ParseOrder(char* order, Uart& uart);

int AddModeOrder(rapidjson::Document& d);

int ModifyModeOrder(rapidjson::Document& d);

int DeleteModeOrder(rapidjson::Document& d);

int DropMode(rapidjson::Document& d);

int EnableMode(rapidjson::Document& d);

int DisableMode(rapidjson::Document& d);

int SendOrder(rapidjson::Document& d, Uart& uart);

int SendMode(int modeindex, Uart& uart);

int AddTrigger(rapidjson::Document& d);

int DeleteTrigger(rapidjson::Document& d);

int ReadJsonFromFile(rapidjson::Document& df, const char* file);

int WriteJsonToFile(rapidjson::Document& df, const char* file);

int DocumentSerialize(rapidjson::Document& d, char* buffer, int len);
#endif
