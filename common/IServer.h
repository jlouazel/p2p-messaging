#ifndef ISERVER_H
#define ISERVER_H

#include <netinet/in.h>
#include <vector>
#include <string>

class IServer
{
public:
    IServer() = delete;
    IServer(int port);
    virtual ~IServer() = default;
    void run(void);

protected:
    void _openSocketForNewClients(void);
    void _registerNewClient(void);
    virtual void _onMessageReceived(fd_set copy) = 0;


    struct clientInfo
    {
        int sockFd;
        std::string ip;
        int port;
    };

    int m_maxClients;
    int m_port;
    int m_listeningSocket;
    int m_maxFd;
    fd_set m_master;
    std::vector<clientInfo> m_clients;

private:
    void* _getInAddr(struct sockaddr *sa);
};

#endif // ISERVER_H
