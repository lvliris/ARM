#ifndef USER_PATTERN_H_
#define USER_PATTERN_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>

typedef std::vector<float> Vector_f;
typedef std::vector<int> Vector_i;

typedef std::vector<std::vector<float> > ndVector_f;
typedef std::vector<std::vector<int> > ndVector_i;

class CollaborativeFiltering
{
public:
	int UserSize;
	int ItemSize;
	int LogIndex;

	float sThresh;	//similarity threshold
	float lamda;	//parameter for moving average

	ndVector_f eDistance;		//Euclidian distance
	ndVector_f eSimilarity;		//Euclidian similarity

	ndVector_f cDistance;		//cosine distance

	ndVector_f csDistance;		//cosine similar distance

	ndVector_i Patterns;		//the scene patterns to be mined in user's history

	CollaborativeFiltering();
	~CollaborativeFiltering();

	//load the data to process better
	virtual void LoadData(ndVector_i UserData);

	//find similar neighbors
	virtual void FindSimilarNeighbors(int id, Vector_i &neighbors);

	//realize mining the patterns of the user
	virtual void PatternMining();

	/***********************************************************************
	function: given part of the orders, recommend the pattern accordingly
	input: a vector that includes some orders
	output: a vector that contain the input orders in the recognized pattern
	***********************************************************************/
	virtual Vector_i Recommend(Vector_i v);

	virtual void EuclidianDistance(ndVector_i UserData);
	virtual float EuclidianSimilarity(float dist);
	virtual void CosDistance(ndVector_i UserData);
	virtual void CosSimilarityDistance(ndVector_i UserData);
};


class UserBasedCF : public CollaborativeFiltering
{
public:
	UserBasedCF();
	~UserBasedCF();

	ndVector_f sUsers;

	virtual void LoadData(ndVector_i UserData);

	virtual void PatternMining();

	virtual void FindSimilarNeighbors(int id, Vector_i &neighbors);
	virtual bool ValidPattern(Vector_i pattern);

	virtual Vector_i Recommend(Vector_i v);

	virtual void EuclidianDistance(ndVector_i UserData);
	virtual void CosDistance(ndVector_i UserData);
	virtual void CosSimilarityDistance(ndVector_i UserData);
};


class ItemBasedCF : public CollaborativeFiltering
{
public:
	ItemBasedCF();
	~ItemBasedCF();

	ndVector_f sItems;
	ndVector_f sItemsAvg;

	std::queue<int> q;

	virtual void LoadData(char* file);
	virtual void UpdateSimilarity();
	virtual void PatternMining();
	virtual Vector_i Recommend(Vector_i v);
	virtual void FindSimilarNeighbors(int id, Vector_i &neighbors, Vector_i &visited);
	virtual bool ValidPattern(Vector_i pattern);

	virtual void EuclidianDistance(ndVector_i UserData);
	virtual void CosDistance(ndVector_i &UserData);
	virtual void CosSimilarityDistance(ndVector_i UserData);
};


#endif
