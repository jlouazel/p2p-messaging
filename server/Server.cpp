#include "Server.h"

#include <iostream>
#include <string>
#include <unistd.h>
#include <errno.h>

Server::Server(int port) :
    IServer(port)
{
    _openSocketForNewClients();
}

void Server::_onMessageReceived(fd_set copy)
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
                _disconnectClient(i);
            }
            else if (strcmp(buf, "/get") == 0)
            {
                _sendClientList(m_clients[i].sockFd);
            }
            else if (strncmp(buf, "PORT ", 5) == 0)
            {
                _registerClientPort(m_clients[i].sockFd, buf);
            }
            break;
        }
        i++;
    }
}

void Server::_sendMessage(int clientSocket, std::string message)
{
    std::cout << "Send message to client: " << clientSocket << std::endl;
    ssize_t ret = send(clientSocket, message.c_str(), message.size() + 1, 0);
    std::cout << "Message sent: size (" << ret << ")" << std::endl;
}

void Server::_sendClientList(int clientSocket)
{
    std::string clientsList = "LIST\n";

    for (clientInfo client : m_clients)
    {
        if (client.sockFd != clientSocket)
        {
            clientsList += client.ip + " " + std::to_string(client.port) + "\n";
        }
    }
    if (clientsList == "LIST\n")
    {
        clientsList += "You are alone here";
    }
    std::cout << "Send client list to client: " << clientSocket << std::endl;
    _sendMessage(clientSocket, clientsList);
}

void Server::_registerClientPort(int clientSocket, std::string clientPort)
{
    int port = std::stoi(clientPort.substr(5));

    for (size_t i = 0; i < m_clients.size(); i++)
    {
        if (m_clients[i].sockFd == clientSocket)
        {
            std::cout << "Set port: " << port << " for client: " << m_clients[i].sockFd << std::endl;
            m_clients[i].port = port;
        }
    }
}

void Server::_disconnectClient(int clientId)
{
    FD_CLR(m_clients[clientId].sockFd, &m_master);
    close(m_clients[clientId].sockFd);
    m_clients.erase(m_clients.begin() + clientId);
}
