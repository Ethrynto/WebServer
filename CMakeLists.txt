# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

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
find_package (Boost REQUIRED) 
  
# If BOOST is finded
if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR}) # to link the header files
endif ()

# Add "source" directory
add_subdirectory("source")

# target_compile_definitions("${PROJECT_NAME}" PUBLIC CURRENCY_APIKEY="") 
# target_compile_definitions("${PROJECT_NAME}" PUBLIC CRYPTO_APIKEY="") 

# If BOOST is finded
if (Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) # to link library
endif ()