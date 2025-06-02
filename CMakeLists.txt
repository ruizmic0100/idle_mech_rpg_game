cmake_minimum_required(VERSION 3.20)
project(MechIdleRPG CXX)
message("${CMAKE_PROJECT_NAME} ---")

message("System Info ---")
message(STATUS "${CMAKE_LIBRARY_ARCHITECTURE}")
message(STATUS "${CMAKE_SYSTEM}")

# (INJECTING JSON FILES)Defining the data folder path so it can be copied to build directory to be later consumed by the executable
set(SOURCE_DATA_FOLDER "${CMAKE_SOURCE_DIR}/data")
set(DESTINATION_DATA_FOLDER_NAME "data")

# (INJECTING WEB FILES)Defining the data folder path so it can be copied to build directory to be later consumed by the executable
set(SOURCE_WEB_FOLDER "${CMAKE_SOURCE_DIR}/web")
set(DESTINATION_WEB_FOLDER_NAME "web")

# (INJECTING JSON FILES)Defining the path where the json files will be copied to in the build directory
set(JSON_DESTINATION_PATH "${CMAKE_BINARY_DIR}/${DESTINATION_DATA_FOLDER_NAME}")
# (INJECTING WEB FILES)Defining the path where the web files will be copied to in the build directory
set(WEB_DESTINATION_PATH "${CMAKE_BINARY_DIR}/${DESTINATION_WEB_FOLDER_NAME}")
#make_directory(${DESTINATION_PATH})
#message(STATUS "Created directory: ${DESTINATION_PATH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("TODO(MSR): CMAKE Compiler Setup ---")

message("")
message(" ---------------- ")
message("")


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

# (INJECTING JSON FILES)Step 1. Finding all the files in the data folder
file(GLOB_RECURSE JSON_FILES 
	LIST_DIRECTORIES false 
	RELATIVE "${SOURCE_DATA_FOLDER}"
	"${SOURCE_DATA_FOLDER}/*"
)

# (INJECTING WEB FILES)Step 1. Finding all the files in the data folder
file(GLOB_RECURSE WEB_FILES 
	LIST_DIRECTORIES false 
	RELATIVE "${SOURCE_WEB_FOLDER}"
	"${SOURCE_WEB_FOLDER}/*"
)

# (INJECTING JSON FILES)Step 1.5 Prepare lists for DEPENDS
set(JSON_FILE_DEPENDENCIES "")
# (INJECTING WEB FILES)Step 1.5 Prepare lists for DEPENDS
set(WEB_FILE_DEPENDENCIES "")

foreach(RELATIVE_JSON_FILE ${JSON_FILES})
	list(APPEND JSON_FILE_DEPENDENCIES "${SOURCE_DATA_FOLDER}/${RELATIVE_JSON_FILE}")
	list(APPEND JSON_FILE_OUTPUTS "${DESTINATION_PATH}/${RELATIVE_JSON_FILE}")
endforeach()
foreach(RELATIVE_WEB_FILE ${WEB_FILES})
	list(APPEND WEB_FILE_DEPENDENCIES "${SOURCE_WEB_FOLDER}/${RELATIVE_WEB_FILE}")
	list(APPEND WEB_FILE_OUTPUTS "${DESTINATION_PATH}/${RELATIVE_WEB_FILE}")
endforeach()


# (INJECTING JSON FILES)Step 2. Then for each file, add a command to copy it
if(JSON_FILES)
	add_custom_command(
		OUTPUT ${JSON_FILE_OUTPUTS} # NOTE(MSR): This is crucial to do because this tells CMake what files to create/update
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
				"${SOURCE_DATA_FOLDER}"
				"${JSON_DESTINATION_PATH}"
		DEPENDS ${JSON_FILE_DEPENDENCIES}			# Explicitly list source files as dependencies
		COMMENT "Copying json file: ${JSON_FILE_OUTPUTS}"
	)
	
	# Creating a custom target that depends on these outputs to ensure this command runs
	add_custom_target(copy_project_json_files
		DEPENDS ${JSON_FILE_OUTPUTS}
	)

	# Make the main executable depend on this custom target
	add_dependencies(mech_idle_rpg copy_project_json_files)

else()
	message(STATUS "No json files found in ${SOURCE_DATA_FOLDER} to copy.")

endif()
# (INJECTING WEB FILES)Step 2. Then for each file, add a command to copy it
if(WEB_FILES)
	add_custom_command(
		OUTPUT ${WEB_FILE_OUTPUTS} # NOTE(MSR): This is crucial to do because this tells CMake what files to create/update
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
				"${SOURCE_WEB_FOLDER}"
				"${WEB_DESTINATION_PATH}"
		DEPENDS ${WEB_FILE_DEPENDENCIES}			# Explicitly list source files as dependencies
		COMMENT "Copying json file: ${WEB_FILE_OUTPUTS}"
	)
	
	# Creating a custom target that depends on these outputs to ensure this command runs
	add_custom_target(copy_project_web_files
		DEPENDS ${WEB_FILE_OUTPUTS}
	)

	# Make the main executable depend on this custom target
	add_dependencies(mech_idle_rpg copy_project_web_files)

else()
	message(STATUS "No web files found in ${SOURCE_WEB_FOLDER} to copy.")

endif()

# Link libraries
# For std::thread, might need pthreads
find_package(Threads REQUIRED)
target_link_libraries(mech_idle_rpg PRIVATE Threads::Threads)


install(TARGETS mech_idle_rpg DESTINATION bin)
