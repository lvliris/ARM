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

void CollaborativeFiltering::CosDistance(ndVector_i UserData)
{
	
}

void CollaborativeFiltering::CosSimilarityDistance(ndVector_i UserData)
{
	
}


UserBasedCF::UserBasedCF()
{
}

UserBasedCF::~UserBasedCF()
{
}

void UserBasedCF::LoadData(ndVector_i UserData)
{
}

void UserBasedCF::PatternMining()
{
}

Vector_i UserBasedCF::Recommend(Vector_i v)
{
	Vector_i t;
	return t;
}

void UserBasedCF::FindSimilarNeighbors(int id, Vector_i &neighbors)
{

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

void UserBasedCF::CosDistance(ndVector_i v)
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
}

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
	lamda = 0.8;
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
	const int hours_of_day = 24;
	const int seconds_of_hour = 3600;
	UserSize = hours_of_day * seconds_of_hour / TIME_QUANT;
	ItemSize = addr_index.size();
	//debug_msg("vector size: %d,%d\n", UserSize, ItemSize);
	cout << "vector size: " << UserSize << ' ' << ItemSize << endl;

	//vectorize the log file
	vector<vector<int> > UserData(UserSize, Vector_i(ItemSize, 0));
	Vectorize(file, UserData);

	//resize the sItems to ItemSize * ItemSize
	Resize(sItems, ItemSize, ItemSize);

	//calculate the similarity
	CosDistance(UserData);

	//update the similarity
	UpdateSimilarity();

	//debug
	PrintVector(sItemsAvg);
}

void ItemBasedCF::UpdateSimilarity()
{
	int new_size = sItems.size();
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
	
	if(sItemsAvg.size() != new_size)
	{
		Resize(sItemsAvg, new_size, new_size);
	}

	cout << "updating item-similarity\n";
	//debug_msg("updating item-similarity\n");
	/*cout << "old:" << endl;
	PrintVector(sItemsAvg);
	cout << "new:" << endl;
	PrintVector(sItems);
	sleep(5);*/
	
	//update the similarity by move average
	//1. fixed lamda
	//2. recomputed lamda accoring to the relative weights
	for(int i = 0; i < new_size; i++)
	{
		for(int j = 0; j < new_size; j++)
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
	int items = sItems.size();
	Vector_i visited(items, 0);
	Patterns.clear();
	for (int i = 0; i < items; i++)
	{
		if (visited[i])
			continue;
		q.push(i);
		visited[i] = 1;
		Vector_i neighbors(items, 0);
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
	int items = sItems.size();
	for (int i = 0; i < items; i++)
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
	cout << "recommending..." << endl;
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
