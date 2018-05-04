all:	main.cpp
	g++ -o router Grid.cpp DefComponent.cpp Def.cpp DefNet.cpp DefPin.cpp main.cpp Routed.cpp Tracks.cpp GridLees.cpp GridBase.cpp -g