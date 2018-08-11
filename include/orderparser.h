#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "cstdio"

int ParseOrder(char* order);

int AddModeOrder(rapidjson::Document& d);

int ModifyModeOrder(rapidjson::Document& d);

int DeleteModeOrder(rapidjson::Document& d);

int EnableMode(rapidjson::Document& d);

int DisableMode(rapidjson::Document& d);

int SendOrder(rapidjson::Document& d);

int SendMode(int modeindex);

int AddTrigger(rapidjson::Document& d);

int DeleteTrigger(rapidjson::Document& d);
