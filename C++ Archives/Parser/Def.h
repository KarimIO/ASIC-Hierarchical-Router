#include<string>
#include<vector>
#include<map>
#include "DefComponent.h"
#include "DefNet.h"
#include "DefPin.h"
#include "Tracks.h"
#include<fstream>
using namespace std;

#ifndef DEF_H
#define DEF_H
class Def
{
public:
	Def(); 
	void parse(string file);
	string output();
	void write(string file);
	~Def();

	vector<DefNet> netlist, speciallist;
	vector<DefPin> pinlist;
	vector<Tracks> tracklist;
	vector<DefComponent> complist;
	string model_name, version, buschars;
	int units, diearea_x1, diearea_x2, diearea_y1, diearea_y2;
		
};
#endif 