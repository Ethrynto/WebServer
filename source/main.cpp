#include <iostream>
#include "Network/HttpServer.h"
#include "System/FileManager.h"

int main(int argc, char* argv[])
{
    //int serverPort = 8080;
    //if (argc >= 2)
    //{
    //    /* Custom port (convert char** arg to int for port) */
    //    serverPort = atoi(argv[1]);
    //}
    //
    //Network::HttpServer server(serverPort);
    //server.start();

    System::FileManager test("C:\\Users\\doniff\\source\\repos\\WebServer\\domains\\");
    std::cout << test.countFolders();
}
