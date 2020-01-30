#ifndef PEERSERVER_H
#define PEERSERVER_H

#include "../common/IServer.h"

class PeerServer : public IServer
{
public:
    PeerServer() = delete;
    PeerServer(int port);
    virtual ~PeerServer() = default;

private:
    virtual void _onMessageReceived(fd_set copy);
};

#endif // PEERSERVER_H
