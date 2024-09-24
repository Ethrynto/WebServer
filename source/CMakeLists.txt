
set(CPP_FILES 
	"main.cpp" 
	"System/FileManager.cpp" 
	"Network/HttpServer.cpp"
)

set(HEADER_FILES 
	"System/FileManager.h"
	"Network/HttpServer.h")

# Add source to this project's executable.
# add_executable (MicroTools "MicroTools.cpp" "Networking/Client.h" "Networking/Client.cpp" "System/Array.h")
add_executable (${PROJECT_NAME} ${CPP_FILES} ${HEADER_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.8)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
