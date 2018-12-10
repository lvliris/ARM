#ifndef DEV_STATE_H_
#define DEV_STATE_H_

#include "converter.h"
#include <string>
#include <map>

#define DEBUG true

#if DEBUG
#define debug_msg(fmt, ...) printf("%s:%s:%d: "fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)

#else
#define debug_msg(fmt, ...) 
#define debug(fmt, ...) 

#endif

#define log(fmt, ...) printf(fmt, ##__VA_ARGS__)

struct State
{
	State(){valid = true;}
	State(bool validation){valid = validation;}
	State(int _state)
	{
		valid = true;
		state = _state;
	}

	State(std::string _type, std::string _short_addr, int _state)
	{
		valid = true;
		type = _type;
		short_addr = _short_addr;
		state = _state;
	}

	//use the feed back data like <010012345678Z701********FF> to construct
	//but this couples a lot with orderparser, it's better not to use it
	State(std::string state)
	{
		valid = true;
		type = state.substr(1, 4);
		state = state[14] - '0';
	}

	bool IsValid(){return valid;}

	std::string type;
	std::string short_addr;

	int state;
	bool valid;
};


class DevState
{
private:
	//make the constructor private to avoid new instances
	DevState();
	DevState(const DevState&){}
	DevState& operator=(const DevState&){}

	//pointer to the single instance
	static DevState* instance;

	//configure files
	static char* addr_file;
	static char* state_file;

	//states of all the devices
	std::map<std::string, struct State> StateMap;
	rapidjson::Document StateDoc;

public:
	//interface to return the single instance
	static DevState* GetInstance();

	//read the state from file, or set default state
	void Init();

	//launch the thread to save the state periodically
	void Launch();

	//return the state of the device whose long addr is long_addr
	struct State GetState(std::string long_addr);

	//when receive the feedback state, update it
	int UpdateState(std::string long_addr, struct State& state);

	//thread function
	static void* SaveState(void* __this);
};


#endif
