#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>

#include "../common/IServer.h"

class Server : public IServer
{
public:
    Server() = delete;
    Server(int port);
    virtual ~Server() = default;

private:
    virtual void _onMessageReceived(fd_set copy);

    void _sendMessage(int clientSocket, std::string message);
    void _sendClientList(int clientSocket);
    void _registerClientPort(int clientSocket, std::string clientPort);
    void _disconnectClient(int clientId);
};

#endif // SERVER_H
