cmake_minimum_required (VERSION 2.6)
project (Router)

set(PARSER_SOURCES Parser.cpp DefComponent.cpp Def.cpp DefNet.cpp DefPin.cpp Routed.cpp Tracks.cpp)
source_group("Parser\\Source Files" FILES ${PARSER_SOURCES})

set(PARSER_HEADERS Parser.h DefComponent.h Def.h DefNet.h DefPin.h Routed.h Tracks.h)
source_group("Parser\\Header Files" FILES ${PARSER_HEADERS})

set(ROUTER_SOURCES GridLees.cpp)
source_group("Router\\Source Files" FILES ${ROUTER_SOURCES})

set(ROUTER_HEADERS GridLees.h GridBase.h)
source_group("Router\\Header Files" FILES ${ROUTER_HEADERS})

set(FILES main.cpp ${PARSER_SOURCES} ${PARSER_HEADERS} ${ROUTER_SOURCES} ${ROUTER_HEADERS})

# add the executable
add_executable(Router ${FILES})
