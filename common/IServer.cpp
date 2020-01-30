#include "IServer.h"

#include <sys/select.h>
#include <iostream>
#include <arpa/inet.h>

IServer::IServer(int port) :
    m_maxClients(10)
  , m_port(port)
  , m_listeningSocket(-1)
  , m_maxFd(0)
{
}

void IServer::run()
{
    FD_ZERO(&m_master);
    FD_SET(m_listeningSocket, &m_master);
    while (1)
    {
        fd_set copy = m_master;

        if (select(m_maxFd + 1, &copy, nullptr, nullptr, nullptr) == -1)
        {
            std::cout << "Error select" << std::endl;
            std::cout << "Errno: " << strerror(errno) << std::endl;
            exit(-1);
        }
        if (FD_ISSET(m_listeningSocket, &copy))
        {
            _registerNewClient();
        }
        else
        {
            _onMessageReceived(copy);
        }
    }
}

void IServer::_openSocketForNewClients()
{
    sockaddr_in sin;

    std::cout << "Create server listening on port: " << m_port << std::endl;
    m_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listeningSocket == -1)
    {
        std::cout << "Error initalizing socket, program will exit" << std::endl;
        exit(-1);
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(m_port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(m_listeningSocket, (const struct sockaddr*)&sin, sizeof(sin)) == -1)
    {
        std::cout << "Error binding socket, program will exit" << std::endl;
        exit(-1);
    }
    if (listen(m_listeningSocket, m_maxClients) == -1)
    {
        std::cout << "Error listening socket, program will exit" << std::endl;
        exit(-1);
    }
    m_maxFd = m_listeningSocket;
    std::cout << "Server created" << std::endl;
}

void IServer::_registerNewClient(void)
{
    int clientSock;
    socklen_t sinSize;
    struct sockaddr_storage clientAddr;
    char clientIp[INET6_ADDRSTRLEN];

    sinSize = sizeof(clientAddr);
    clientSock = accept(m_listeningSocket, (struct sockaddr*)&clientAddr, &sinSize);
    if (clientSock == -1)
    {
        std::cout << "Unable to accept client" << std::endl;
    }
    else
    {
        inet_ntop(clientAddr.ss_family, _getInAddr((struct sockaddr*)&clientAddr), clientIp, sizeof(clientIp));
        std::cout << "Creating client with socket: " << clientSock << std::endl;
        std::cout << "Creating client with address: " << clientIp << std::endl;
        FD_SET(clientSock, &m_master);
        if (clientSock > m_maxFd)
            m_maxFd = clientSock;
        m_clients.push_back({clientSock, clientIp, 0});
    }
}

void* IServer::_getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
