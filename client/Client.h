#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <sys/select.h>
#include <vector>
#include <string>

class Client
{
public:
    Client() = delete;
    Client(char* serverIp, int serverPort, int listeningPort);
    ~Client() = default;

    void run(void);

private:
    std::string m_serverIp;
    int m_serverPort;
    int m_listeningPort;
    int m_testPort;

    int m_serverSocket;
    int m_maxFd;
    sockaddr_in m_sin;
    int m_maxPeerClients;
    fd_set m_master;

    struct peerInfo
    {
        std::string ip;
        int port;
    };
    std::vector<peerInfo> m_peersInfo;

    void _connectToServer(void);
    int _connectToPeer(int peerId);
    int _connectTo(std::string ip, int port);
    void _sendMessageToServer(std::string message);
    void _sendMessageToPeer(int peerSocket, std::string message);
    void _sendMessage(int receiver, std::string message);
    int _getPeerId(std::string buffer);
    std::string _getMessageForPeer(std::string buffer);
    void _updatePeers(std::string buffer);
    void _printPeers(void);
    void _onUserInput(void);
    void _onServerMessageReceived(void);
    void _displayHelpMessage(void);
};

#endif // CLIENT_H
