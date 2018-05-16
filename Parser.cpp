
#include "Parser.h"
#include <sstream>
#include<iostream>
using namespace std;

Parser::Parser(string def, string lef)
{
    def_file = def;
    lef_file = lef;
}

bool Parser::parseLEF()
{
	ifstream in;
	in.open(lef_file);
	if (!in.is_open())
	{
		cerr << "Couldn't find/open the lef file\n";
		return false;
	}
	string line, first, tmp, cur;
	bool flag = 0; // 0 no gates yet, 1gate
	string pin = "";
	while (getline(in, line))
	{
		stringstream ss(line);
		ss >> first;
		if (first == "MACRO")
		{
			ss >> cur;
			while (getline(in, line))
			{
				stringstream ss(line);
				ss >> first;
				if (first.compare("SIZE") == 0)
				{
					double w, h;
					ss >> tmp;
					w = stod(tmp);
					ss >> tmp;
					ss >> tmp;
					h = stod(tmp);
					gates_size[cur].first = h;
					gates_size[cur].second = w;
					
				}
				else if (first.compare("PIN") == 0)
				{
					ss >> pin;
					//cout << pin << endl;
					vector<Rect> v;
					if(gates_pins.find(cur)==gates_pins.end())gates_pins[cur] = v;
				}
				else if (first.compare("RECT") == 0 && pin!="")
				{
					string xx1, xx2, yy1, yy2;
					ss >> xx1 >> yy1 >> xx2 >> yy2;
					Rect r;
					r.pin = pin;
					r.x1 = stod(xx1);
					r.x2 = stod(xx2);
					r.y1 = stod(yy1);
					r.y2 = stod(yy2);
					gates_pins[cur].push_back(r);
					
					
				}
				else if(first.compare("OBS")==0)
				{
					string cur_layer;
					while(getline(in,line))
					{
						stringstream ss5(line);
						ss5>>first;
						
						if(first.compare("LAYER")==0)
						{
							ss5>>cur_layer;
						}
						else if(first.compare("END")==0)
							break;
						else
						{
							Obstacle obs;
							obs.layer=cur_layer;
							string xx1, xx2, yy1, yy2;
							ss5 >> xx1 >> yy1 >> xx2 >> yy2;
							obs.x1 = stod(xx1);
							obs.x2 = stod(xx2);
							obs.y1 = stod(yy1);
							obs.y2 = stod(yy2);
							if(cur_layer[0]=='m')
							obs_list[cur].push_back(obs);
							
						}
					}
				}
				else if (first.compare("END") == 0 && pin != "")
				{
					pin = "";
				}
				else if (first.compare("END") == 0 )
				{

					string tmp;
					ss >> tmp;
					if (tmp.compare(cur)==0)
						break;
				}
				
			}
		}
		
	}
	return true;
}


void Parser::parseDEF()
{
	d.parse(def_file);
}
void Parser::write_output(string outfile){
	d.write(outfile);
}

void Parser::printLEF(){
	cout << "\n\n-------------------------SIZES--------------------\n";
	for (auto it : gates_size)
	{
		cout << "Gate: " << it.first << " w=" << it.second.first << " h=" << it.second.second << endl;
	}
	cout << "\n\n-------------------------PINS--------------------\n";
	for (auto it : gates_pins)
	{
		cout << "Gate: " << it.first << endl;
		for (auto it2 : it.second)
		{
			cout << "Pin " << it2.pin << " x1=" << it2.x1 << " y1=" << it2.y1 << " x2=" << it2.x2 << " y2=" << it2.y2 << endl;
		}
		cout<< "Obstacles:" <<endl;
		for (auto obs : obs_list[it.first])
		{
			cout <<obs.layer << " x1=" << obs.x1 << " y1=" << obs.y1 << " x2=" << obs.x2 << " y2=" << obs.y2 << endl;
		}
		cout << endl;
	}
}


Parser::~Parser()
{

}
