#include<string>
#include<vector>
#include<map>

using namespace std;

#ifndef TRACKS_H
#define TRACKS_H
class Tracks
{
public:
	Tracks();
	void parse(string line);
	string output();
	~Tracks();

	string name, pos, _do, step, layer;

};
#endif 