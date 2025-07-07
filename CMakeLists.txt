cmake_minimum_required(VERSION 3.31)
project(WebServer VERSION 1.0 LANGUAGES CXX)

# Set policy for Boost
cmake_policy(SET CMP0167 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define _WIN32_WINNT before finding Boost
if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00) # Target Windows 10
endif()

# Integrate with vcpkg
if(DEFINED ENV{VCPKG_ROOT})
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Build options
option(ENABLE_HTTPS "Enable HTTPS support" OFF)
option(ENABLE_LOGGING "Enable logging with spdlog" ON)
option(STATIC_BUILD "Build statically" OFF)
option(PRODUCTION_BUILD "Enable production build" OFF)

# Static build
if(STATIC_BUILD)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Include GLFW
add_subdirectory(extern/glfw)

# Add ImGui library
add_library(imgui STATIC
		extern/imgui/imgui.cpp
		extern/imgui/imgui_draw.cpp
		extern/imgui/imgui_tables.cpp
		extern/imgui/imgui_widgets.cpp
		extern/imgui/backends/imgui_impl_opengl3.cpp
		extern/imgui/backends/imgui_impl_glfw.cpp
)

target_include_directories(imgui PUBLIC
		extern/imgui
		extern/imgui/backends
		extern/glfw/include
)

# Find libraries
find_package(Boost REQUIRED COMPONENTS system filesystem asio)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
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
add_executable(WebServer
		source/main.cpp
		source/Network/WebServer.cpp
		source/Network/RequestHandler.cpp
		source/System/HtaccessConfig.cpp
		source/Debug/Log.cpp
)

# Copy domains and resources directories to the output directory
if(EXISTS "${CMAKE_SOURCE_DIR}/domains")
	add_custom_command(TARGET WebServer POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_SOURCE_DIR}/domains"
			"$<TARGET_FILE_DIR:WebServer>/domains"
			COMMENT "Copying domains directory to output directory"
	)
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/resources")
	add_custom_command(TARGET WebServer POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_SOURCE_DIR}/resources"
			"$<TARGET_FILE_DIR:WebServer>/resources"
			COMMENT "Copying resources directory to output directory"
	)
endif()

# Define production build
if(PRODUCTION_BUILD)
	target_compile_definitions(WebServer PRIVATE PRODUCTION_BUILD=1)
else()
	target_compile_definitions(WebServer PRIVATE PRODUCTION_BUILD=0)
endif()

# Link libraries
target_link_libraries(WebServer PRIVATE
		Boost::system
		Boost::filesystem
		Boost::asio
		nlohmann_json::nlohmann_json
		imgui
		glfw
		OpenGL::GL
)
if(WIN32)
	target_link_libraries(WebServer PRIVATE ws2_32 wsock32)
endif()
if(ENABLE_HTTPS)
	target_link_libraries(WebServer PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
if(ENABLE_LOGGING)
	target_link_libraries(WebServer PRIVATE spdlog::spdlog)
endif()

# Include directories
target_include_directories(WebServer PRIVATE
		${CMAKE_SOURCE_DIR}/source
		${CMAKE_SOURCE_DIR}/source/Network
		${CMAKE_SOURCE_DIR}/source/System
		${CMAKE_SOURCE_DIR}/source/Debug
)

# Compiler-specific flags for MinGW
if(MINGW)
	target_compile_options(WebServer PRIVATE -D_WIN32_WINNT=0x0A00)
endif()

# Install
install(TARGETS WebServer DESTINATION bin)
install(DIRECTORY domains DESTINATION .)
install(DIRECTORY resources DESTINATION .)