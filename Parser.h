#include<string>
#include<vector>
#include<map>
#include "Def.h"
#include<fstream>
using namespace std;

#ifndef PARSER_H
#define PARSER_H

struct Rect{
	string pin;
	float x1, x2, y1, y2;
};
class Parser
{
public:
	Parser(string def, string lef); 
	~Parser();
    bool parseLEF();
    void parseDEF();
    bool write_output(string outfile);
    void print();

    map<string, pair<float, float> > gates_size;
    map<string, vector<Rect> > gates_pins;
    string lef_file,def_file;
    Def d;
};
#endif 
