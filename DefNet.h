#include<string>
#include<vector>
#include<map>
#include "Routed.h"
using namespace std;

#ifndef DEFNET_H
#define DEFNET_H
class DefNet
{
public:
	DefNet();
	string output();
	void parse(string line);
	~DefNet();

	string name;
	vector<pair<string, string> > gate_to_pin;
	vector<Routed> routes;
	bool special;

};
#endif 