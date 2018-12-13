#include "userpattern.h"
#include "converter.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

template<typename DType>
void Resize(vector<vector<DType> > &data, int rows, int cols)
{
	int old_size = data.size();
	for(int i = 0; i < old_size; i++)
	{
		data[i].resize(cols);
	}
	data.resize(rows, vector<DType>(cols, 0));
}

CollaborativeFiltering::CollaborativeFiltering()
{
	const int hours_of_day = 24;
	const int seconds_of_hour = 3600;
	UserSize = hours_of_day * seconds_of_hour / TIME_QUANT;
	ItemSize = 0;
}

CollaborativeFiltering::~CollaborativeFiltering()
{

}

void CollaborativeFiltering::LoadData(ndVector_i UserData){}

void CollaborativeFiltering::FindSimilarNeighbors(int id, Vector_i &neighbors){}

void CollaborativeFiltering::PatternMining(){}

Vector_i CollaborativeFiltering::Recommend(Vector_i v)
{
	Vector_i t;
	return t;
}

void CollaborativeFiltering::EuclidianDistance(ndVector_i UserData)
{
	
}

inline float CollaborativeFiltering::EuclidianSimilarity(float dist)
{
	return 1 / (1 + dist);
}

float CollaborativeFiltering::CosDistance(Vector_i &x, Vector_i &y)
{
	if(y.size() != x.size())
	{
		cout << "Error: different vector size" << endl;
	}
	
	int size = x.size();
	int sum_xy = 0;
	int sum_x2 = 0;
	int sum_y2 = 0;
	for(int i = 0; i < size; i++)
	{
		sum_xy += x[i] * y[i];
		sum_x2 += x[i] * x[i];
		sum_y2 += y[i] * y[i];
	}
	
	if(sum_x2 == 0 || sum_y2 == 0)
	{
		return 0;
	}
	else
	{
		return sum_xy / sqrt(sum_x2 * sum_y2);
	}
}

void CollaborativeFiltering::CosSimilarityDistance(ndVector_i UserData)
{
	
}


UserBasedCF::UserBasedCF()
{
	sThresh = 0.3;
	tThresh = 100;
	mThresh = 15;
	lamda = 0.95;
}

UserBasedCF::~UserBasedCF()
{
}

void UserBasedCF::LoadData(ndVector_i UserData)
{
}

void UserBasedCF::PatternMining()
{
	Habits.clear();
	for(int i = 0; i < PatternTimesVar.size(); i++)
	{
		if(PatternTimesVar[i] < tThresh)
		{
			Habits[PatternTimesMean[i]] = i;
		}
	}
}

Vector_i UserBasedCF::Recommend(int time)
{
	cout << "UserBasedCF recommending..." << endl;
	map<int, int>::iterator iter;
	Vector_i res;
	
	//just print all the habits
	for(iter = Habits.begin(); iter != Habits.end(); iter++)
	{
		int exe_time = iter->first;
		int pattern_index = iter->second;
		res.push_back(exe_time);
		for (int j = 0; j < Patterns[pattern_index].size(); j++)
		{
			cout << Patterns[pattern_index][j] << ' ';
		}
		cout << "-- " << exe_time << endl;
	}
	
	//real recommending
	/*if((iter = Habits.find(time)) != Habits.end())
	{
		res = Patterns[iter->second];
	}*/
	
	return res;
}

void UserBasedCF::FindSimilarNeighbors(int id, Vector_i &neighbors)
{

}

void UserBasedCF::FindPatternTime(ndVector_i &patterns, ndVector_i &user_data)
{
	Vector_i exe_time;
	if(patterns.empty())
	{
		return;
	}

	int num_patterns = patterns.size();
	int num_users = user_data.size();
	//cout << "user_data.size: " << num_users << endl;

	//get the execute time of each pattern
	for(int i = 0; i < num_patterns; i++)
	{
		int j;
		int mode_len = 0;
		int start_time = 0;
		for(j = 0; j < num_users; j++)
		{
			float cosdis = CosDistance(patterns[i], user_data[j]);
			//cout << cosdis << ' ';
			if(cosdis > sThresh)
			{
				if(mode_len == 0)
					start_time = j;
				mode_len++;
				continue;
			}
			else
			{
				if(mode_len > mThresh)
				{
					exe_time.push_back(start_time);
					cout << "execute time: " << start_time << endl;
					break;
				}
				mode_len = 0;
			}
		}
		//cout << endl;
		if(j == num_users)
		{
			exe_time.push_back(-1);
		}
	}

	//the patterns with greatest similarity are seen as the same pattern
	//update their execute time
	int num_patterns_pre = Patterns.size();
	Vector_f exe_time_var(num_patterns, 0.0);
	for(int i = 0; i < num_patterns; i++)
	{
		float max_pattern_dis = 0.0;
		int index = -1;
		for(int j = 0; j < num_patterns_pre; j++)
		{
			float pattern_dis = CosDistance(patterns[i], Patterns[j]);
			if(pattern_dis > max_pattern_dis)
			{
				max_pattern_dis = pattern_dis;
				index = j;
			}
		}
		if(index != -1)
		{
			//maintain the last value if not found in log
			if(exe_time[i] == -1)
			{
				exe_time[i] = PatternTimesMean[index];
				exe_time_var[i] = 0.0;
			}
			else
			{
				exe_time_var[i] = pow(exe_time[i] - PatternTimesMean[index], 2);
				//moving average
				exe_time[i] = (1 - lamda) * exe_time[i] + lamda * PatternTimesMean[index];
				exe_time_var[i] = (1 - lamda) * exe_time_var[i] + lamda * PatternTimesVar[index];
			}
		}
	}

	Patterns.assign(patterns.begin(), patterns.end());
	PatternTimesMean = exe_time;
	PatternTimesVar = exe_time_var;

	//debug
	cout << " average pattern execute time and variance: ";
	for(int i = 0; i < PatternTimesVar.size(); i++)
	{
		cout << '(' << PatternTimesMean[i] << ',' << PatternTimesVar[i] << ')';
	}
	cout << endl;
}

bool UserBasedCF::ValidPattern(Vector_i pattern)
{
	int num = 0;
	for (Vector_i::iterator iter = pattern.begin(); iter != pattern.end(); iter++)
	{
		if (*iter == 1)
			num++;
	}
	return num > 1;
}

void UserBasedCF::EuclidianDistance(ndVector_i v)
{
	if (UserSize < 2)
		return;

	for (int i = 0; i < UserSize; i++)
	{
		for (int j = i + 1; j < UserSize; j++)
		{
			int sum = 0;
			for (int k = 0; k < ItemSize; k++)
			{
				int dv = v[i][k] - v[j][k];
				sum += dv * dv;
			}
			float dist = sqrt(sum);
			sUsers[i][j] = EuclidianSimilarity(dist);
		}
	}
}

/*void UserBasedCF::CosDistance(ndVector_i v)
{
	if (UserSize < 2)
		return;

	for (int i = 0; i < UserSize; i++)
	{
		for (int j = i + 1; j < UserSize; j++)
		{

			int sum_x2 = 0;
			int sum_y2 = 0;
			int sum_xy = 0;

			for (int k = 0; k < ItemSize; k++)
			{
				sum_x2 += v[i][k] * v[i][k];
				sum_y2 += v[j][k] * v[j][k];
				sum_xy += v[i][k] * v[j][k];
			}

			sUsers[i][j] = sum_xy / sqrt(sum_x2 * sum_y2);
		}
	}
}*/

void UserBasedCF::CosSimilarityDistance(ndVector_i v)
{
	if (UserSize < 2)
		return;

	for (int i = 0; i < UserSize; i++)
	{
		for (int j = i + 1; j < UserSize; j++)
		{

			float sum_x = 0;
			float sum_y = 0;
			float sum_x2 = 0;
			float sum_y2 = 0;
			float sum_xy = 0;

			for (int k = 0; k < ItemSize; k++)
			{
				sum_x += v[i][k];
				sum_y += v[j][k];
			}

			float avg_x = (float)sum_x / ItemSize;
			float avg_y = (float)sum_y / ItemSize;

			for (int k = 0; k < ItemSize; k++)
			{
				sum_x2 += (v[i][k] - avg_x) * (v[i][k] - avg_x);
				sum_y2 += (v[j][k] - avg_y) * (v[j][k] - avg_y);
				sum_xy += (v[i][k] - avg_x) * (v[j][k] - avg_y);
			}

			sUsers[i][j] = sum_xy / sqrt(sum_x2 * sum_y2);
		}
	}
}


ItemBasedCF::ItemBasedCF()
{
	sThresh = 0.3;
	lamda = 0.95;
	LogIndex = 1;
}

ItemBasedCF::~ItemBasedCF()
{
}

void ItemBasedCF::LoadData(char* file)
{
	//PrintVector(UserData);

	//used for getting the item number
	map<string, int> addr_index = MapAddr2Index();

	//resize the sItems to ItemSize * ItemSize
	if(ItemSize != addr_index.size())
	{
		ItemSize = addr_index.size();
		Resize(UserData, UserSize, ItemSize);
		Resize(sItems, ItemSize, ItemSize);
		Resize(sItemsAvg, ItemSize, ItemSize);
	}
	//debug_msg("vector size: %d,%d\n", UserSize, ItemSize);
	//cout << "vector size: " << UserSize << ' ' << ItemSize << endl;

	//vectorize the log file
	Vectorize(file, UserData);

	//calculate the similarity
	CosDistance(UserData);

	//update the similarity
	UpdateSimilarity();

	//debug
	//PrintVector(sItemsAvg);
}

void ItemBasedCF::UpdateSimilarity()
{
	/*if(sItemsAvg.empty())
	{
		//debug_msg("items similarities empty, initializing...\n");
		cout << "items similarities empty, initializing...\n";
		Resize(sItemsAvg, new_size, new_size);
		
		for(int i = 0; i < new_size; i++)
		{
			for(int j = 0; j < new_size; j++)
			{
				sItemsAvg[i][j] = sItems[i][j];
			}
		}
		return;
	}*/
	
	//cout << "updating item-similarity\n";
	//debug_msg("updating item-similarity\n");
	/*cout << "old:" << endl;
	PrintVector(sItemsAvg);
	cout << "new:" << endl;
	PrintVector(sItems);
	sleep(5);*/
	
	//update the similarity by move average
	//1. fixed lamda
	//2. recomputed lamda accoring to the relative weights
	for(int i = 0; i < ItemSize; i++)
	{
		for(int j = 0; j < ItemSize; j++)
		{
			//sItemsAvg[i][j] = lamda * sItemsAvg[i][j] + (1 - lamda)/(1 - pow(lamda, LogIndex)) * sItems[i][j];
			//sItemsAvg[i][j] = lamda * sItemsAvg[i][j] + (1 - lamda) * sItems[i][j];
			sItemsAvg[i][j] = lamda * sItemsAvg[i][j] + (1 - lamda) * sItems[i][j];
			if(sItemsAvg[i][j] > 1)
			{
				sItemsAvg[i][j] = 1;
			}
		}
	}
	LogIndex++;
}

void ItemBasedCF::PatternMining()
{
	Vector_i visited(ItemSize, 0);
	Patterns.clear();
	for (int i = 0; i < ItemSize; i++)
	{
		if (visited[i])
			continue;
		q.push(i);
		visited[i] = 1;
		Vector_i neighbors(ItemSize, 0);
		neighbors[i] = 1;
		while (!q.empty())
		{
			int id = q.front();
			visited[id] = 1;
			FindSimilarNeighbors(id, neighbors, visited);
			q.pop();
		}

		//if the pattern include 2 more items, it's a valid pattern
		if (ValidPattern(neighbors))
		{
			Patterns.push_back(neighbors);
		}
	}
}

void ItemBasedCF::FindSimilarNeighbors(int id, Vector_i &neighbors, Vector_i &visited)
{
	for (int i = 0; i < ItemSize; i++)
	{
		if (sItemsAvg[id][i] > sThresh && !visited[i])
		{
			neighbors[i] = 1;
			q.push(i);
		}
	}
}

bool ItemBasedCF::ValidPattern(Vector_i pattern)
{
	int num = 0;
	for (Vector_i::iterator iter = pattern.begin(); iter != pattern.end(); iter++)
	{
		if (*iter == 1)
			num++;
	}
	return num > 1;
}

Vector_i ItemBasedCF::Recommend(Vector_i v)
{
	cout << "ItemBasedCF recommending..." << endl;
	for (int i = 0; i < Patterns.size(); i++)
	{
		for (int j = 0; j < Patterns[i].size(); j++)
		{
			cout << Patterns[i][j] << ' ';
		}
		cout << endl;
	}
	Vector_i t;
	return t;
}

void ItemBasedCF::EuclidianDistance(ndVector_i v)
{
	if (ItemSize < 2)
		return;

	for (int i = 0; i < ItemSize; i++)
	{
		for (int j = i + 1; j < ItemSize; j++)
		{

			int sum = 0;
			for (int k = 0; k < UserSize; k++)
			{
				int dv = v[k][i] - v[k][j];
				sum += dv * dv;
			}

			float dist = sqrt(sum);
			sItems[i][j] = EuclidianSimilarity(dist);
		}
	}
}


void ItemBasedCF::CosDistance(ndVector_i &v)
{
	if (ItemSize < 2)
		return;

	for (int i = 0; i < ItemSize; i++)
	{
		for (int j = i + 1; j < ItemSize; j++)
		{
			int sum_x2 = 0;
			int sum_y2 = 0;
			int sum_xy = 0;

			for (int k = 0; k < UserSize; k++)
			{
				sum_x2 += v[k][i] * v[k][i];
				sum_y2 += v[k][j] * v[k][j];
				sum_xy += v[k][i] * v[k][j];
			}

			if (sum_x2 == 0 || sum_y2 == 0)
			{
				sItems[i][j] = 0;
				sItems[j][i] = 0;
			}
			else
			{
				//cout << "x.*y " << sum_xy << endl;
				//cout << "x2y2: " << sum_x2 << ' ' << sum_y2 << endl;
				sItems[i][j] = sum_xy / sqrt(sum_x2 * sum_y2);
				sItems[j][i] = sItems[i][j];
				//cout << sum_x2 * sum_y2 << " res: " << sItems[i][j] << endl;
			}
		}
	}
}

void ItemBasedCF::CosSimilarityDistance(ndVector_i v)
{
	if (ItemSize < 2)
		return;

	for (int i = 0; i < ItemSize; i++)
	{
		for (int j = i + 1; j < ItemSize; j++)
		{

			float sum_x = 0;
			float sum_y = 0;
			float sum_x2 = 0;
			float sum_y2 = 0;
			float sum_xy = 0;

			for (int k = 0; k < UserSize; k++)
			{
				sum_x += v[k][i];
				sum_y += v[k][j];
			}

			float avg_x = (float)sum_x / ItemSize;
			float avg_y = (float)sum_y / ItemSize;

			for (int k = 0; k < UserSize; k++)
			{
				sum_x2 += (v[k][i] - avg_x) * (v[k][i] - avg_x);
				sum_y2 += (v[k][j] - avg_y) * (v[k][j] - avg_y);
				sum_xy += (v[k][i] - avg_x) * (v[k][j] - avg_y);
			}

			sItems[i][j] = sum_xy / sqrt(sum_x2 * sum_y2);
		}
	}
}
