#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "orderparser.h"

int UpdateAddr(char* long_addr, char* short_addr);

int ConvertAddr(const char* long_addr, char* short_addr);

int Json2Str(rapidjson::Document& d, char* str);

int Str2Json(char* str, rapidjson::Document& d);
#endif
