# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0079 NEW) # Settinc policy

set(PROJECT_NAME "WebServer")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
project (${PROJECT_NAME})

# BOOST setting
set(BOOST_ROOT "C:/libraries/Boost") # The boost root path
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)
  
# Find BOOST package
find_package (Boost REQUIRED COMPONENTS system filesystem thread) 
  

# If BOOST is finded
if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${asio_SOURCE_DIR}/asio/include) # to link the header files
else()
	include_directories()
endif ()

# Add "source" directory
add_subdirectory("source")

if(PRODUCTION_BUILD)
	# setup the DOMAINS_PATH macro to be in the root folder of your exe
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC DOMAINS_PATH="./domains/") 

	# remove the option to debug asserts.
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
	# This is useful to get an DOMAINS_PATH in your IDE during development
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC DOMAINS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/domains/")

	# add the option to debug asserts.
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()
# target_compile_definitions("${PROJECT_NAME}" PUBLIC API="") 


# If BOOST is finded
if (Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) # to link library   
else()
	target_link_libraries(${PROJECT_NAME}) # to link library
endif ()

