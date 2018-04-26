

#include<string>
#include<vector>
#include<map>

using namespace std;

#ifndef DEFPIN_H
#define DEFPIN_H
class DefPin
{
public:
	DefPin();
	void parse(string line);
	string output();
	~DefPin();

	string name, net, layer, orientation;
	int placed_pos[2], layer_pos[4];
};
#endif 