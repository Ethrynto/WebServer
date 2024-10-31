cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0079 NEW) # Settings policy

set(PROJECT_NAME "WebServer")
project (${PROJECT_NAME})

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# BOOST setting
set(BOOST_ROOT "D:/Programming Projects/CPP Libraries/Boost") # The boost root path
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

# Find BOOST package
find_package (Boost REQUIRED)

# If BOOST is found
if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${asio_SOURCE_DIR}/asio/include) # to link the header files
endif ()

# Add "source" directory
add_subdirectory("source")

if(PRODUCTION_BUILD)
	target_compile_definitions("${PROJECT_NAME}" PUBLIC DOMAINS_PATH="./domains/")	# setup the DOMAINS_PATH
	target_compile_definitions("${PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 		# remove the option to debug asserts.
else()
	target_compile_definitions("${PROJECT_NAME}" PUBLIC DOMAINS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/domains/") 	# This is useful to get an DOMAINS_PATH in your IDE during development
	target_compile_definitions("${PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 									# add the option to debug asserts.
endif()

# If OS is Windows
if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32)
endif()

# If BOOST is found
if (Boost_FOUND)
	include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${asio_SOURCE_DIR}/asio/include) # to link the header files
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) # to link library
endif ()