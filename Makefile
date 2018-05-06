all:	main.cpp
	g++ -o router DefComponent.cpp Def.cpp DefNet.cpp DefPin.cpp main.cpp Routed.cpp Tracks.cpp GridLees.cpp -g -std=c++11