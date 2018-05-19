all:	main.cpp
	@echo "Compiling Router..."
	@g++ -o router DefComponent.cpp Def.cpp DefNet.cpp DefPin.cpp main.cpp Parser.cpp Routed.cpp Tracks.cpp GridLees.cpp -g -std=c++11
	@echo "Router Compiled."