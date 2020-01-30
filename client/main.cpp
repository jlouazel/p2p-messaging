#include "Client.h"
#include "PeerServer.h"

#include <iostream>
#include <thread>

void runPeerServer(int peerPort)
{
    std::unique_ptr<PeerServer> server(new PeerServer(peerPort));
    server->run();
}

void runPeerClient(char* serverIp, int serverPort, int listeningPort)
{
    std::unique_ptr<Client> client(new Client(serverIp, serverPort, listeningPort));
    client->run();
}

int main(int argc, char**argv)
{
    if (argc != 4)
    {
        std::cout << "Usage: ./client <serverIP> <serverPort> <peerPort>" << std::endl;
        return 0;
    }

    std::cout << "Creating threads" << std::endl;
    std::thread serverThread(runPeerServer, atoi(argv[3]));
    std::thread clientThread(runPeerClient, argv[1], atoi(argv[2]), atoi(argv[3]));

    std::cout << "Synchronizing thread" << std::endl;

    serverThread.join();
    clientThread.join();
    return 0;
}
