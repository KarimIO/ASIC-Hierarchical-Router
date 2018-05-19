#include <iostream>
#include <sstream>
#include <vector>
#include "GridLees.h"
#include "Parser.h"

using namespace std;
const int DEF_FACTOR = 100;

void gridmaker(Parser p){
	//sanity check
	int x1 = p.d.diearea_x1;
	int y1 = p.d.diearea_y1;
	int x2 = p.d.diearea_x2;
	int y2 = p.d.diearea_y2;
	int xsize = (x2-x1);
	int ysize = (y2-y1);
	cout << "Die Area: " << xsize << " " << ysize << endl;
	int pinx1,pinx2,piny1,piny2;
	int gatew,gateh;
    int pinX,pinY;
	GridLees grid(x1, y1, x2, y2, p.d.tracklist.size());
	grid.setDebugMode(false, false, true, true);
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
	map<string, vector<pair<int,pair<int,int> > > > net_pins; //layer, x, y
	map<string,pair<int,int> >  gate_origin;
	vector<Obstacle> absolute_obstacles;
	float total_area = 0;
	for(auto c:p.d.complist){
		name_to_gate[c.name]=c.gate;
		gate_origin[c.name]=make_pair(c.x,c.y);
		total_area += (DEF_FACTOR*DEF_FACTOR*p.gates_size[c.gate].first*p.gates_size[c.gate].second);
	}
	float die_area_total = xsize*ysize;
	cout << "TOTAL_AREA= " << total_area << "  DIE_AREA: " << die_area_total << " CORE_UTIL: " <<total_area/die_area_total << endl;
	map<string, DefPin> add_pin_location;
	for (auto pp : p.d.pinlist)
	{
		add_pin_location[pp.name] = pp;
	}
	for(auto net:p.d.netlist)
	{
		for(auto g:net.gate_to_pin)
		{
			if (g.first.compare("PIN") != 0)
			{
				string gname = name_to_gate[g.first];
				int x = gate_origin[g.first].first + x1;
				int y = gate_origin[g.first].second + y1;
				//cout<<"GATE_POS: "<<x<<" "<<y<<endl<<endl;
				//cout << net.name << " " << g.first << endl;
				for (auto it2 : p.gates_pins[gname])
				{
					if (it2.pin.compare(g.second) == 0)
					{
						pinx1 = it2.x1*DEF_FACTOR;
						piny1 = it2.y1*DEF_FACTOR;
						pinx2 = it2.x2*DEF_FACTOR;
						piny2 = it2.y2*DEF_FACTOR;
						pinX = (pinx1 + pinx2) / 2;
						pinY = (piny1 + piny2) / 2;
						net_pins[net.name].push_back(make_pair(0,make_pair(pinX + x, pinY + y)));

						// cout<<net.name<<" "<<g.second<<" "<<pinX+x<<" " <<pinY+y<<endl;
						break;
					}
				}
				for (auto it2 : p.obs_list[gname])
				{
					Obstacle obs = it2;
					obs.x1 = (obs.x1*DEF_FACTOR) + x;
					obs.x2 = (obs.x2*DEF_FACTOR) + x;
					obs.y1 = (obs.y1*DEF_FACTOR) + y;
					obs.y2 = (obs.y2*DEF_FACTOR) + y;
					absolute_obstacles.push_back(obs);
					//cout<<it2.x1<<" "<<it2.y1<<" "<<it2.x2<<" "<<it2.y2<<endl;
					//cout<<"AFTER: "<<obs.x1<<" "<<obs.y1<<" "<<obs.x2<<" "<<obs.y2<<endl;
				}
			}
			else
			{
				int layer = (add_pin_location[g.second].layer[5] - '0') - 1;
				pinX = add_pin_location[g.second].placed_pos[0];
				pinY = add_pin_location[g.second].placed_pos[1];
				net_pins[net.name].push_back(make_pair(layer, make_pair(pinX, pinY)) );
			}
			
		}
	}
	cout << endl;

	
	for (auto block : absolute_obstacles)
	{
		int layer = int(block.layer[5]-'0') - 1;
		grid.addBlockArea(Coord(block.x1, block.y1, layer), Coord(block.x2, block.y2, layer));
	}
	for(auto np:net_pins)
	{
		for(int i=0;i<np.second.size()-1;i++)
		{
			//cout<<np.second[i].first<<" "<<np.second[i].second<<"\t"<<np.second[i+1].first<<" "<<np.second[i+1].second<<endl;
			grid.addPath(Path(Coord(np.second[i].second.first, np.second[i].second.second, np.second[i].first), Coord(np.second[i + 1].second.first, np.second[i + 1].second.second, np.second[i + 1].first)));

		}
	}
    if (grid.route()) {
		grid.getPaths();
    	std::cout << "Successfully routed.\n";
	}
	else {
		std::cout << "Failed to find routes.\n";
	}
	vector<OutputPath> out_paths = grid.getPaths();
	cout << "SIZE of PATHS " << out_paths.size() << endl;
	bool start;
	for (int i = 0; i < out_paths.size(); i++)
	{
		start = true;
		for (int k = 0; k < out_paths[i].paths.size(); k++)
		{
			Routed temp_r;
			OutputSegment temp_seg = out_paths[i].paths[k];
			temp_r.layer = "metal"+ to_string(temp_seg.layer);
			temp_r.special = temp_r.fst = false;
			if (temp_seg.zdir != NoZ)
			{
				if (temp_seg.zdir == Out)
					temp_r.dest_layer = "M" + to_string(temp_seg.layer + 1) + "_M" + to_string(temp_seg.layer);
				else
					temp_r.dest_layer = "M" + to_string(temp_seg.layer) + "_M" + to_string(temp_seg.layer-1);
			}
			temp_r.xys.push_back(make_pair(to_string(temp_seg.startx), to_string(temp_seg.starty)));
			temp_r.xys.push_back(make_pair(to_string(temp_seg.endx), to_string(temp_seg.endy)));
			if (start)
			{
				temp_r.fst = true;
			}
			start = false;
			p.d.netlist[i].routes.push_back(temp_r);
			cout << "pushing route: " << temp_r.output() << endl;;
		}
	}
	p.d.write("testing_out.def");
	cout << "done writing\n";
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

#if 1

	string lef_path = "Files/osu035.lef";
	string def_path = "Files/mux4x1.def";

	if (argc > 1) {
		def_path = argv[1];

		if (argc > 2) {
			lef_path = argv[2];
		}
	}

	Parser p(def_path,lef_path);
	if(!p.parseLEF()) {
		cerr << "Error parsing LEF\n";
		exit(1);
	}

	p.parseDEF();

	gridmaker(p);

#else
	GridLees grid(0, 0, 50, 50, 2);
	//grid.setDebugMode(true, true, true, false);
	grid.addLayer(0, 11, 5, false);
	grid.addLayer(0, 6, 10, true);
	grid.addPath(Path(Coord(0, 0, 0), Coord(50, 50, 1)));
	grid.addPath(Path(Coord(25, 0, 0), Coord(15, 25, 0)));
	grid.addPath(Path(Coord(20, 0, 0), Coord(20, 50, 0)));
	grid.addPath(Path(Coord(20, 0, 1), Coord(20, 50, 1)));
	grid.addPath(Path(Coord(50, 10, 0), Coord(50, 49, 0)));
	grid.addBlockArea(Coord(15, 15, 0), Coord(20, 20, 0));
    if (grid.route()) {
    	std::cout << "Successfully routed.\n";
		std::vector<OutputPath> p = grid.getPaths();
		for (int i = 0; i < p.size(); ++i) {
			std::cout << "Path #" << i + 1 << "\n";
			for (int j = 0; j < p[i].paths.size(); ++j) {
				OutputSegment &s = p[i].paths[j];
				int newlayer = (s.zdir == NoZ) ? s.layer : (s.zdir == In) ? (s.layer - 1) : (s.layer + 1);
				std::cout << "\Layer: " << s.layer << "\n";
				std::cout << "\tFrom: " << s.startx << ", " << s.starty << "\n";
				std::cout << "\tTo: " << s.endx << ", " << s.endy << "\n";
				std::cout << "\tTo layer: " << newlayer << "\n";
			}
		}
	}
	else {
	 	std::cout << "Failed to find routes.\n";
	}
#endif

#ifdef _WIN32
	system("pause");
#endif

    return 0;
}
