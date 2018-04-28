
#include "Def.h"
#include<string>
#include <sstream>
#include<iostream>
using namespace std;

Def::Def()
{
}

void Def::parse(string file)
{
	ifstream in(file);
	if (in.is_open() == false)
	{
		cerr << "couldn't open " << file << "\n";
		exit(1);
		return;
	}
	string line,first;
	int i = 15;
	while (getline(in, line))
	{

		//cout << line << endl;
		stringstream ss(line);	

		if (!(ss >> first))continue;
		//if(i==0)break;
		//i--;
		if (first.compare("DIEAREA") == 0 )
		{
			string tmp;
			ss >> tmp >> diearea_x1 >> diearea_y1 >> tmp >> tmp >> diearea_x2 >> diearea_y2;
			
		}
		else if (first.compare("BUSBITCHARS") == 0)
		{
			ss >> buschars;
		}
		else if (first.compare("DESIGN") == 0)
		{
			ss >> model_name;
		}
		else if (first.compare("UNITS") == 0)
		{
			string tmp;
			ss >> tmp >> tmp >> tmp;
			units = stoi(tmp);
		}
		else if (first.compare("TRACKS") == 0)
		{
			Tracks t;
			t.parse(line);
			tracklist.push_back(t);
		}
		else if (first.compare("COMPONENTS") == 0)
		{
			//cout << "GOT COMP\n";
			ss >> first;
			int num = stoi(first);
			while (num--)
			{
				getline(in, line);
				DefComponent c;
				c.parse(line);
				complist.push_back(c);
			}
		}
		else if (first.compare("NETS") == 0 )
		{
			//cout << "GOT NETS\n";
			ss >> first;
			int num = stoi(first);
			getline(in, line);
			while (num--)
			{
				stringstream ss2(line);
				ss2 >> first;
				DefNet dn;

				bool start = true;
				while ((first[0] != '-'&& first.compare("END") != 0) || start)
				{
					start = false;
					dn.parse(line);
					getline(in, line);
					stringstream ss3(line);
					ss3 >> first;
				}
				netlist.push_back(dn);

			}
		}
		else if (first.compare("PINS") == 0)
		{
			//cout << "GOT PINS\n";
			ss >> first;
			int num = stoi(first);
			getline(in, line);
			while (num--)
			{
				stringstream ss2(line);
				ss2 >> first;
				bool start = true;
				DefPin dp;
				while ((first[0] != '-' && first.compare("END") != 0) || start)
				{
					start = false;
					dp.parse(line);
					getline(in, line);
					stringstream ss3(line);
					ss3 >> first;
				}
				pinlist.push_back(dp);

			}
		}
		else if (first.compare("SPECIALNETS") == 0)
		{
			ss >> first;
			int num = stoi(first);
			getline(in, line);
			while (num--)
			{
				stringstream ss2(line);
				ss2 >> first;
				DefNet dn;
				bool start = true;
				while ((first[0] != '-'&& first.compare("END") != 0) || start)
				{
					start = false;
					if (first[0] != ';')dn.parse(line);
					getline(in, line);
					stringstream ss3(line);
					ss3 >> first;
				}
				speciallist.push_back(dn);

			}
		
		}
	}
}
void Def::write(string file)
{
	ofstream out(file);
	if (out.is_open() == false)
	{
		cerr << "couldn't open " << file << "\n";
		exit(1);
		return;
	}
	out << output();
	out.close();
	cout << "DONE WRITING\n";
}
string Def::output()
{
	string out;
	out += "VERSION 5.6 ;\nNAMESCASESENSITIVE ON ;\nDIVIDERCHAR \"/\" ;\n";
	out += "BUSBITCHARS " + buschars + " ;\n";
	out += "DESIGN " + model_name + " ;\n";
	out += "UNITS DISTANCE MICRONS "+to_string(units)+" ;\n\n";

	out += "DIEAREA ( " + to_string(diearea_x1) + " " + to_string(diearea_y1) + " ) ( " + to_string(diearea_x2) + " " + to_string(diearea_y2) + " ) ;\n\n";


	for (auto it : tracklist)
		out += it.output();
	out += "\n";

	out += "COMPONENTS " + to_string(complist.size()) + " ;\n";
	for (auto it : complist)
		out += it.output();
	out += "END COMPONENTS\n\n";

	out += "PINS " + to_string(pinlist.size()) + " ;\n";
	for (auto it : pinlist)
		out += it.output();
	out += "END PINS\n\n";

	out += "NETS " + to_string(netlist.size()) + " ;\n";
	for (auto it : netlist)
		out += it.output();
	out += "END NETS\n\n";

	out += "SPECIALNETS " + to_string(speciallist.size()) + " ;\n";
	for (auto it : speciallist)
		out += it.output();
	out += "END SPECIALNETS\n\n";

	out += "END DESIGN\n";
	return out;
}
Def::~Def()
{

}
