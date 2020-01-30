#include "PeerServer.h"

#include <iostream>
#include <sys/select.h>
#include <unistd.h>

PeerServer::PeerServer(int port) :
    IServer(port)
{
    _openSocketForNewClients();
}

void PeerServer::_onMessageReceived(fd_set copy)
{
    std::cout << "Message received" << std::endl;
    int i = 0;
    while (i < m_maxFd)
    {
        if (FD_ISSET(m_clients[i].sockFd, &copy))
        {
            std::cout << " Client: " << m_clients[i].sockFd << std::endl;
            std::cout << "     IP: " << m_clients[i].ip << std::endl;
            char buf[1024];
            int messageSize;
            messageSize = recv(m_clients[i].sockFd, buf, 1024, 0);
            buf[messageSize] = '\0';
            std::cout << "Message: " << buf << std::endl;
            if (messageSize == 0)
            {
                FD_CLR(m_clients[i].sockFd, &m_master);
                close(m_clients[i].sockFd);
                m_clients.erase(m_clients.begin() + i);
            }
            break;
        }
        i++;
    }
}
