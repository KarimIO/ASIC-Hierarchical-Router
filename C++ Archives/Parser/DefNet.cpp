#include "DefNet.h"
#include <sstream>
#include<iostream>
using namespace std;

DefNet::DefNet()
{
	name="";
	special = false;
}

void DefNet::parse(string line)
{
	stringstream ss(line); 
	string first, tmp;
	//cout << line << endl;
	ss >> first;
	if (first.compare("-") == 0)
	{
		ss >> name;
	}
	else if (first.compare("(")==0)
	{

		string gate, pin;
		ss >> gate >> pin;
		gate_to_pin.push_back(make_pair(gate, pin));
	}
	else
	{
		Routed r;
		r.parse(line);
		routes.push_back(r);
	}
}
string DefNet::output()
{
	string out = "- " + name;
	for (auto it : gate_to_pin)
	{
		out += "\n  ( " + it.first + " " + it.second + " ) ";
	}
	//cout << "Route size" << routes.size() << " g2p " << gate_to_pin.size() << endl;
	for (auto it : routes)
	{
		out += "\n" + it.output();
	}
	if (routes.size())out += " ";
	out += ";\n";
	return out;

}
DefNet::~DefNet()
{

}
