#include "Tracks.h"
#include<sstream>
using namespace std;

Tracks::Tracks()
{
	name = pos = _do = step = layer = "";
}

void Tracks::parse(string line)
{
	stringstream ss(line);
	string tmp;
	ss >> tmp >> name >> pos >> tmp >> _do >> tmp >> step >> tmp >> layer;
}
string Tracks::output()
{
	return "TRACKS " + name + " " + pos + " DO " + _do + " STEP " + step + " LAYER " + layer + " ;\n";
}
Tracks::~Tracks()
{

}
