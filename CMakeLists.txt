cmake_minimum_required(VERSION 3.15...3.31)
project(WebServer VERSION 1.0 LANGUAGES CXX)

# Install the policy for Boost
cmake_policy(SET CMP0167 NEW)

# Install the C++ standart
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Integration with vcpkg
if(DEFINED ENV{VCPKG_ROOT})
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Assembly options
option(ENABLE_HTTPS "Enable HTTPS support" OFF)
option(ENABLE_LOGGING "Enable logging with spdlog" ON)
option(STATIC_BUILD "Build statically" OFF)

# Static assembly
if(STATIC_BUILD)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Library Search
find_package(Boost 1.74 REQUIRED COMPONENTS system)
find_package(nlohmann_json CONFIG REQUIRED)
if(ENABLE_HTTPS)
	find_package(OpenSSL REQUIRED)
endif()
if(ENABLE_LOGGING)
	find_package(spdlog CONFIG REQUIRED)
endif()

# Adding sources
add_subdirectory(source)

# Pathfinding
if(PRODUCTION_BUILD)
	target_compile_definitions(${PROJECT_NAME} PRIVATE DOMAINS_PATH="./domains/")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PRODUCTION_BUILD=1)
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE DOMAINS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/domains/")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PRODUCTION_BUILD=0)
endif()

# Linking libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
		Boost::system
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