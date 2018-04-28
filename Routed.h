#include<string>
#include<vector>
#include<map>

using namespace std;

#ifndef ROUTED_H
#define ROUTED_H
class Routed
{
public:
	Routed();
	void parse(string line);
	string output();
	~Routed();

	string layer, dest_layer;
	vector<pair<string, string> > xys;
	int special_width;
	bool fst, special;
};
#endif 