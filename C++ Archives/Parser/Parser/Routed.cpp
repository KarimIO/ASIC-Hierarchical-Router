#include "Routed.h"
#include <sstream>
#include<iostream>
using namespace std;

Routed::Routed()
{
	layer =  dest_layer = "";
	special = fst = false;
}

void Routed::parse(string line)
{
	stringstream ss(line); 
	string tmp, first;
	ss >> first;
	//cout << first << "\t";
	string x, y;
	if (first.compare("+") == 0)
	{
		fst = true;
		ss >> tmp; //ROUTED Line
	}
	ss >> layer;
	
	ss >> tmp;
	if (isdigit(tmp[0]))
	{
		special = true;
		special_width = stoi(tmp);
		//ss >> tmp;
	}
	while (ss>>tmp && tmp[0] =='(')
	{
		ss >> x >> y >> tmp;
		xys.push_back(make_pair(x, y));
	}
	if (tmp[0] == 'M')
	{
		dest_layer = tmp;
	}
	
}
string Routed::output()
{

	string out;
	if (fst)
		out += "+ ROUTED ";
	else
		out += "  NEW ";
	out += layer + " ";
	if (special)
		out += to_string(special_width);
	for (auto p : xys)
	{
		out += " ( " + p.first + " " + p.second + " )";
	}
	if(dest_layer.compare("")!=0)
		out += " "+ dest_layer;
	return out;
	
}
Routed::~Routed()
{

}
