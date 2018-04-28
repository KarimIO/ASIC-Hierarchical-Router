#include "DefPin.h"
#include <sstream>
#include<iostream>
using namespace std;

DefPin::DefPin()
{
	name=net=layer=orientation="";
}

void DefPin::parse(string line)
{
	stringstream ss(line); 
	//cout << line << endl;
	string first, tmp;
	ss >> first;
	if (first.compare("-") == 0)
	{
		ss >> name >> tmp >> net;
	}
	else
	{
		ss >> first;
		if (first.compare("LAYER") == 0)
		{
			string x1, x2, y1, y2;			
			ss >> layer >> tmp >> x1 >> y1 >> tmp >> tmp >> x2 >> y2;
			layer_pos[0] = stoi(x1);
			layer_pos[1] = stoi(y1);
			layer_pos[2] = stoi(x2);
			layer_pos[3] = stoi(y2);
		}
		else
		{
			string x1, y1;

			ss >> tmp >> x1 >> y1 >> tmp >> orientation;
			placed_pos[0] = stoi(x1);
			placed_pos[1] = stoi(y1);
		}
	}
	
}
string DefPin::output()
{
	string out = "- " + name + " + NET " + name + "\n";
	if (layer.compare("") != 0)
	{
		out += "  + LAYER " + layer + " ( " + to_string(layer_pos[0]) + " " + to_string(layer_pos[1]) + " ) ( " + to_string(layer_pos[2]) + " " + to_string(layer_pos[3]) + " )";
	}
	if (orientation.compare("") != 0)
	{
		out += "\n";
		out += "  + PLACED ( " + to_string(placed_pos[0]) + " " + to_string(placed_pos[1]) + " ) " + orientation;
	}
	out += " ;\n";
	return out;
}
DefPin::~DefPin()
{

}
