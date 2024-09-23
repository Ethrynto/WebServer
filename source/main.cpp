#include <iostream>
#include <vector>
#include <thread>

#include "Network/HttpServer.h"
#include "System/FileManager.h"
#include "Network/Server.h"

int main(int argc, char* argv[])
{
    System::FileManager* domains = new System::FileManager(DOMAINS_PATH);
    int serverPort = 8080;


    delete domains;
}
