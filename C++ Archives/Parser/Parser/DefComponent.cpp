#include "DefComponent.h"
#include <sstream>
using namespace std;

DefComponent::DefComponent()
{
	name = gate = orientation = "";
}

void DefComponent::parse(string line)
{
	stringstream ss(line); 
	string tmp;
	
	ss >> tmp >> name >> gate;
	if (ss>>tmp)
	{
		string a1, a2;
		ss >> tmp >> tmp >> a1 >> a2 >> tmp >> orientation;
		x = stoi(a1);
		y = stoi(a2);
	}
}
string DefComponent::output()
{
	string out = "- " + name + " " + gate;
	if (orientation.compare("") != 0)
	{
		out += " + PLACED ( " + to_string(x) + " " + to_string(y) + " ) " + orientation;
	}
	out += " ;\n";
	return out;
	
}
DefComponent::~DefComponent()
{

}
