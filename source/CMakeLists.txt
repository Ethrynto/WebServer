set(SOURCE_FILES
		main.cpp

        Network/WebServer.cpp
		Network/WebServer.h
		Network/RequestHandler.cpp
		Network/RequestHandler.h

		Debug/Log.cpp
		Debug/Log.h
		System/HtaccessConfig.cpp
		System/HtaccessConfig.h
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCE_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.8)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.