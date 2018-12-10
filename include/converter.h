#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "orderparser.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

//quantize the time into discrete
#define TIME_QUANT 100
#define RP_OFFSET 50

#define ADDR_FILE "config/addr.json"    
#define LOG_DIR "log/"

int UpdateAddr(char* long_addr, char* short_addr);

int ConvertAddr(const char* long_addr, char* short_addr);

int Json2Str(rapidjson::Document& d, char* str);

int Str2Json(char* str, rapidjson::Document& d);

int SaveHistory(char* data, char* file_name);

int GetTimeofToday(time_t t);

std::map<std::string, int> MapAddr2Index();

std::vector<std::string> GetAddrList(std::map<std::string, int> &addr_index);

void Vectorize(char* file_name, std::vector<std::vector<int> >&user_data);

void SavePatterns(const std::vector<std::vector<int> > &patterns);

template<typename DType>
void PrintVector(std::vector<std::vector<DType> > &v)
{
	//std::ofstream fout("output.txt");

    //std::cout << "-----------------------------------------------------" << std::endl;
    for (int i = 0; i < v.size(); i++)
    {
        for (int j = 0; j < v[i].size(); j++)
        {
            	std::cout << std::left << std::setw(4) << std::setprecision(2)<< v[i][j] << ' ';
		//fout << std::left << std::setw(4) << std::setprecision(2)<< v[i][j] << ' ';
        }
	std::cout << std::endl;
        //fout << std::endl;
    }

    //fout.close();
}


#endif
