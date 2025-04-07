cmake_minimum_required(VERSION 3.20)
project(MechIdleRPG CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(vendor/json/single_include/nlohmann) # For json location

include_directories(src)

# List all .cpp files here
set(SOURCES
	src/main.cpp
	src/Game.cpp
	src/Mech.cpp
	src/Equipment.cpp
	src/Item.cpp
)

add_executable(mech_idle_rpg ${SOURCES})

# Link libraries
# For std::thread, might need pthreads
find_package(Threads REQUIRED)
target_link_libraries(mech_idle_rpg PRIVATE Threads::Threads)

# TODO(MSR): Ensure data files are accessible (.json files). Need to make cmake copy them automatically.

install(TARGETS mech_idle_rpg DESTINATION bin)
