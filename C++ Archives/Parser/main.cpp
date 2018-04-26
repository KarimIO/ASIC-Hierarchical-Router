#include<iostream>
#include<sstream>
#include"Def.h"
using namespace std;

map<string, pair<float, float> > gates;
bool getGateSize(string lef_file);

int main()
{
	//Extract Def file add more info output to test.def
	Def d;
	d.parse("Files/simple_pic_unroute.def");

	Tracks t;
	t.name = "test";
	t.pos = "444";
	t._do = "100";
	t.step = "50";
	t.layer = "test_metal";
	d.tracklist.push_back(t);

	DefPin tp;
	tp.name = "test_pin";
	tp.layer = "test_metal"; 
	tp.orientation="FN";
	tp.placed_pos[0] = tp.placed_pos[1] = tp.layer_pos[0] = tp.layer_pos[1] = tp.layer_pos[2] = tp.layer_pos[3]= 100;
	d.pinlist.push_back(tp);

	DefComponent tc;
	tc.name = "test_gate_1";
	tc.gate = "test_gate";
	tc.orientation = "FS";
	tc.y = tc.x = 200;
	d.complist.push_back(tc);

	DefNet tn;
	tn.name = "tst_net";
	Routed tr;
	tr.layer = "tst_layer";
	tr.dest_layer = "M5_tst";
	tr.fst = 1;
	tr.xys.push_back(make_pair("*", "*"));
	tn.routes.push_back(tr);
	tn.gate_to_pin.push_back(make_pair("tst_gate", "A"));
	d.netlist.push_back(tn);

	//d.write("Files/test.def");

	//get the gates width and heigh from the lef file
	if (!getGateSize("Files/osu035.lef")){
		cerr << "Couldn' get info from lef file\n";
	}
	system("pause");
	return 0;
}

bool getGateSize(string lef_file)
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
				if (first == "SIZE")
				{
					double w, h;
					ss >> tmp;
					w = stod(tmp);
					ss >> tmp;
					ss >> tmp;
					h = stod(tmp);
					gates[cur].first = h;
					gates[cur].second = w;
					//cout<<"Gate: "<<cur<<"  w= "<<w<<" h="<<h<<endl;
					break;
				}
				
			}
		}
		
	}
	return true;
}