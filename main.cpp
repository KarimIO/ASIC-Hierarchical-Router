#include <iostream>
#include <sstream>
#include <vector>
#include "GridLees.h"
#include "Parser.h"

using namespace std;

void gridmaker(Parser p){
	//sanity check
	int x1 = p.d.diearea_x1;
	int y1 = p.d.diearea_y1;
	int x2 = p.d.diearea_x2;
	int y2 = p.d.diearea_y2;
	int xsize = (x2-x1);
	int ysize = (y2-y1);
	cout<<xsize<<" "<<ysize<<endl;
	int pinx1,pinx2,piny1,piny2;
	int gatew,gateh;
    int pinX,pinY;
	GridLees grid(x1, y1, x2, y2, p.d.tracklist.size());
	DefPin pin = p.d.pinlist[0];
	//cout<<p.placed_pos[0]<<endl;
	DefNet n = p.d.netlist[0];
	DefComponent c;
	//cout<<n.gate_to_pin<<endl;
	for (int t = 0; t < p.d.tracklist.size(); ++t) {
		Tracks track = p.d.tracklist[t];
		grid.addLayer(stoi(track.pos), stoi(track._do), stoi(track.step), track.name == "X");
	}
	
	map<string,string> name_to_gate;
	map<string, vector<pair<int,int> > > net_pins;
	map<string,pair<int,int> >  gate_origin;
	for(auto c:p.d.complist){
		name_to_gate[c.name]=c.gate;
		gate_origin[c.name]=make_pair(c.x,c.y);
	}
	for(auto net:p.d.netlist)
	{
		vector<pair<int,int> > vp;
		net_pins[net.name]=vp;
		for(auto g:net.gate_to_pin)
		{
			string gname = name_to_gate[g.first];
			int x=gate_origin[g.first].first;
			int y=gate_origin[g.first].second;
			for(auto it2: p.gates_pins[gname])
			{
				if(it2.pin.compare(g.second)==0)
				{
					pinx1 = it2.x1*100;
					piny1 = it2.y1*100;
					pinx2 = it2.x2*100;
					piny2 = it2.y2*100;
					pinX = pinx1+pinx2 / 2; 
					pinY = piny1+piny2 / 2;
					net_pins[net.name].push_back(make_pair(pinX+x,pinY+y));
					
					// cout<<net.name<<" "<<g.second<<" "<<pinX+x<<" " <<pinY+y<<endl;
					break;
				}
			}
		}
	}
	for(auto np:net_pins)
	{
		for(int i=0;i<np.second.size()-1;i++)
		{
			cout<<np.second[i].first<<" "<<np.second[i].second<<"\t"<<np.second[i+1].first<<" "<<np.second[i+1].second<<endl;
			grid.addPath(Path(Coord(np.second[i].first,np.second[i].second, 0), Coord(np.second[i+1].first,np.second[i+1].second, 0)));

		}
	}

    if (grid.route()) {
    	std::cout << "Successfully routed.\n";
	}
	else {
		std::cout << "Failed to find routes.\n";
	}

	// for (auto it : gates_size)
	// {
	// 	cout << "Gate: " << it.first << " w=" << it.second.first << " h=" << it.second.second << endl;

	// }
	// for (auto it : gates_pins)
	// {
	// 	cout << "Gate: " << it.first << endl;

	// 	for (auto it2 : it.second)
	// 	{
	// 		pinx1 = it2.x1*10;
	// 		piny1 = it2.y1*10;
	// 		pinx2 = it2.x2*10;
	// 		piny2 = it2.y2*10;
	// 		pinX = pinx1+pinx2 / 2; 
	// 		pinY = piny1+piny2 / 2;

	// 		grid.addPath(Path(Coord(2, 1), Coord(6, 5)));

	// 		//cout<<<<endl;
	// 		cout << "Pin " << it2.pin << " x1=" << pinx1 << " y1=" << piny1 << " x2=" << pinx2 << " y2=" << piny2 << endl;
	// 	}
	// 	cout << endl;
	// }
	
}



int main(int argc, char *argv[]) {
	
	string lef_path = "Files/osu035.lef";
	string def_path = "Files/simple_pic_unroute.def";
	Parser p(def_path,lef_path);
	if(!p.parseLEF())
	{
		cerr<<"Error parsing LEF\n";
		exit(1);
	}
	p.parseDEF();

	gridmaker(p);

	// GridLees grid(0, 0, 50, 50, 2);

	// grid.setDebugMode(true, true, true);

	// grid.addLayer(0, 11, 5, false);
	// grid.addLayer(0, 6, 10, true);

	// grid.addPath(Path(Coord(0, 0, 0), Coord(50, 50, 1)));
	// grid.addPath(Path(Coord(25, 0, 0), Coord(15, 25, 0)));
	// grid.addPath(Path(Coord(20, 0, 0), Coord(20, 50, 0)));
	// grid.addPath(Path(Coord(20, 0, 1), Coord(20, 50, 1)));
	// grid.addPath(Path(Coord(50, 10, 0), Coord(50, 49, 0)));

    // if (grid.route()) {
    // 	std::cout << "Successfully routed.\n";
	// 	grid.printPaths();
	// }
	// else {
	// 	std::cout << "Failed to find routes.\n";
	// }
	

	// std::string path;
	// if (argc > 1)
	// 	path = argv[1];
	// else
	// 	path = "Files/rca4.def";

	// // Parse(path);

#ifdef _WIN32
	system("pause");
#endif

    return 0;
}
