cmake_minimum_required(VERSION 3.31.6)
project(WebServer VERSION 1.0 LANGUAGES CXX)

# Set policy for Boost
cmake_policy(SET CMP0167 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Integrate with vcpkg
if(DEFINED ENV{VCPKG_ROOT})
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Build options
option(ENABLE_HTTPS "Enable HTTPS support" OFF)
option(ENABLE_LOGGING "Enable logging with spdlog" ON)
option(STATIC_BUILD "Build statically" OFF)

# Static build
if(STATIC_BUILD)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Find libraries
find_package(Boost REQUIRED COMPONENTS system filesystem beast)
find_package(nlohmann_json CONFIG REQUIRED)
if(ENABLE_HTTPS)
	find_package(OpenSSL REQUIRED)
endif()
if(ENABLE_LOGGING)
	find_package(spdlog CONFIG REQUIRED)
endif()

# Enable dynamic linking for Boost
add_definitions(-DBOOST_ALL_DYN_LINK)
remove_definitions(-DBOOST_FILESYSTEM_NO_LIB)

# Add source files
add_subdirectory(source)

# Define paths
if(PRODUCTION_BUILD)
	target_compile_definitions(${PROJECT_NAME} PRIVATE DOMAINS_PATH="./domains/")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PRODUCTION_BUILD=1)
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE DOMAINS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/domains/")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PRODUCTION_BUILD=0)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
		Boost::system
		Boost::filesystem
		Boost::beast
		nlohmann_json::nlohmann_json
)
if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32)
endif()
if(ENABLE_HTTPS)
	target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
if(ENABLE_LOGGING)
	target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
endif()

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY domains DESTINATION .)