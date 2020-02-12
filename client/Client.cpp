#include "Client.h"

#include <cstring>
#include <cerrno>
#include <iostream>
#include <netdb.h>
#include <thread>

#include "../common/Tools.h"

Client::Client(char* serverIp, int serverPort, int listeningPort) :
    m_serverIp(serverIp)
  , m_serverPort(serverPort)
  , m_listeningPort(listeningPort)
  , m_maxPeerClients(10)
{
    _connectToServer();
    std::cout << "Welcome, type \"/?\" to list the available commands" << std::endl;
}

void Client::run()
{
    FD_ZERO(&m_master);
    FD_SET(0, &m_master);
    FD_SET(m_serverSocket, &m_master);

    while (1)
    {
        fd_set copy = m_master;
        if (select(m_serverSocket + 1, &copy, nullptr, nullptr, nullptr) == -1)
        {
            std::cout << "Select error" << std::endl;
            exit(-1);
        }
        if (FD_ISSET(0, &copy))
        {
            _onUserInput();
        }
        else if (FD_ISSET(m_serverSocket, &copy))
        {
            _onServerMessageReceived();
        }
    }
}

void Client::_connectToServer()
{
    m_serverSocket = _connectTo(m_serverIp, m_serverPort);
    _sendMessageToServer("PORT " + std::to_string(m_listeningPort));
}

int Client::_connectToPeer(int peerId)
{
    int peerSocket;

    std::cout << "connect to peer " << m_peersInfo[peerId].ip << " on port " << m_peersInfo[peerId].port << std::endl;
    peerSocket = _connectTo(m_peersInfo[peerId].ip, m_peersInfo[peerId].port);
    return peerSocket;
}

int Client::_connectTo(std::string ip, int port)
{
    sockaddr_in sin;
    struct hostent* hostInfo;
    int retSocket;

    retSocket = socket(AF_INET, SOCK_STREAM, 0);
    hostInfo = gethostbyname(ip.c_str());
    if (hostInfo == nullptr)
    {
        std::cout << "ERROR" << std::endl;
    }
    sin.sin_addr = *(in_addr*)hostInfo->h_addr;
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;
    if (connect(retSocket, (sockaddr*)&sin, sizeof(sockaddr)) == -1)
    {
        std::cout << "ERROR connecting" << std::endl;
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }
    return retSocket;
}

void Client::_sendMessageToServer(std::string message)
{
    _sendMessage(m_serverSocket, message);
}

void Client::_sendMessageToPeer(int peerSocket, std::string message)
{
    std::cout << "Send message to peer" << std::endl;
    _sendMessage(peerSocket, message);
}

void Client::_sendMessage(int receiver, std::string message)
{
    size_t size = send(receiver, message.c_str(), message.size() + 1, 0);
    std::cout << "Sent: " << size << std::endl;
}

int Client::_getPeerId(std::string buffer)
{
    buffer = buffer.substr(3);
    std::size_t endOfPeer = buffer.find_first_of(" ");
    buffer = buffer.substr(0, endOfPeer);
    return stoi(buffer);
}

std::string Client::_getMessageForPeer(std::string buffer)
{
    buffer = buffer.substr(3);
    std::size_t endOfIp = buffer.find_first_of(" ");
    buffer = buffer.substr(endOfIp);
    return buffer;
}

void Client::_updatePeers(std::string buffer)
{
    m_peersInfo.clear();
    buffer = buffer.substr(5);
    if (buffer == "You are alone here")
        return;
    std::vector<std::string> peers = Tools::split(buffer, '\n');
    for (std::string peer : peers)
    {
        std::vector<std::string> peerInfo = Tools::split(peer, ' ');
        m_peersInfo.push_back({peerInfo[0], std::stoi(peerInfo[1])});
    }
}

void Client::_printPeers(void)
{
    for (size_t i = 0; i < m_peersInfo.size(); i++)
    {
        std::cout << i << ") " << m_peersInfo[i].ip << ", " << m_peersInfo[i].port << std::endl;
    }
}

void Client:: _onUserInput()
{
    std::string buf;
    std::getline(std::cin, buf);
    if (buf.size() > 0)
    {
        if (strncmp(buf.c_str(), "/w ", 3) == 0)
        {
            int peerId = _getPeerId(buf);
            std::cout << "send message to peer: " << peerId << std::endl;
            std::string message = _getMessageForPeer(buf);
            std::cout << "Message: " << message << std::endl;
            int peerSocket = _connectToPeer(peerId);
            _sendMessageToPeer(peerSocket, message);
        }
        else if (strcmp(buf.c_str(), "/?") == 0)
        {
            _displayHelpMessage();
        }
        else
        {
            _sendMessageToServer(buf);
        }
    }
}

void Client::_onServerMessageReceived()
{
    char buf[1024];
    int messageSize;
    messageSize = recv(m_serverSocket, buf, 1024, 0);
    if (messageSize < 0)
    {
        std::cout << "Error during receive" << std::endl;
    }
    else if (messageSize == 0)
    {
        std::cout << "Connection closed" << std::endl;
        exit(-1);
    }
    else
    {
        buf[messageSize] = '\0';
        if (strncmp(buf, "LIST\n", 5) == 0)
        {
            std::cout << "List of peers: " << std::endl;
            _updatePeers(buf);
            _printPeers();
        }
        else
        {
            std::cout << "Buffer: " << buf << std::endl;
        }
    }
}

void Client::_displayHelpMessage(void)
{
    std::cout << "----- HELP ------" << std::endl;
    std::cout << "/get" << "\t\t\t List peers <id> <ip> <port>" << std::endl;
    std::cout << "/w <peerId> <message>" << "\t Send a private message to a peer" << std::endl;
    std::cout << "-----------------" << std::endl;
}

