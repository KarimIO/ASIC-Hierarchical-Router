cmake_minimum_required (VERSION 2.6)
project (Router)

set(PROJ_SOURCES Grid.cpp DefComponent.cpp Def.cpp DefNet.cpp DefPin.cpp main.cpp Routed.cpp Tracks.cpp)
set(PROJ_HEADERS Grid.h DefComponent.h Def.h DefNet.h DefPin.h Routed.h Tracks.h)

# add the executable
add_executable(Router ${PROJ_SOURCES} ${PROJ_HEADERS})