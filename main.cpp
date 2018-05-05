#include <iostream>
#include <sstream>
#include <vector>
#include "GridLees.h"
#include "Def.h"

using namespace std;
struct Rect{
	string pin;
	float x1, x2, y1, y2;
};
map<string, pair<float, float> > gates_size;
map<string, vector<Rect> > gates_pins;
void print();
bool extractLEF(string lef_file);

bool Parse()
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
	if (!extractLEF("Files/osu035.lef")){
		cerr << "Couldn' get info from lef file\n";
	}
	print();

	return 0;
}

bool extractLEF(string lef_file)
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

void print()
{
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
		cout << endl;
	}
}

int main() {
    GridLees grid(10, 8);
	grid.addPath(Path(Coord(2, 1), Coord(5, 5)));
	grid.addPath(Path(2, 3, 7, 5));
	//grid.addPath(Path(2, 2, 7, 5));

	std::vector<Coord> blocks;
	blocks.emplace_back(4, 4);
    blocks.emplace_back(4, 5);
    blocks.emplace_back(4, 6);
    blocks.emplace_back(5, 4);
    blocks.emplace_back(6, 4);

	grid.setBlockers(blocks.size(), blocks.data());

    grid.simulate();

    //Parse();

#ifdef _WIN32
	system("pause");
#endif

    return 0;
}