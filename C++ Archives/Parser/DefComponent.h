#include<string>
#include<vector>
#include<map>

using namespace std;

#ifndef DEFCOMPONENT_H
#define DEFCOMPONENT_H
class DefComponent
{
public:
	DefComponent();
	void parse(string line);
	string output();
	~DefComponent();

	string name, gate, orientation;
	int x, y;

};
#endif 