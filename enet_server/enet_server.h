#ifndef __ENET_SERVER_H_
#define __ENET_SERVER_H_

#include <memory>
#include <enet/enet.h>

typedef std::shared_ptr<ENetPeer> peer_ptr_t;
typedef std::shared_ptr<std::string> msg_ptr_t;

class EnetServer : public std::enable_shared_from_this<EnetServer>
{
public:
    EnetServer(const std::string& ip_bind_to, int port);
    ~EnetServer(void);

    void Run(void);

private:
    void RunOnce(void);
    void HandleRecvedMsg(ENetPeer* peer, const msg_ptr_t& msg);
    void SendMsg(ENetPeer* peer, const msg_ptr_t& msg);

private:
    ENetAddress address_;
    std::shared_ptr<ENetHost> server_ptr_;
    bool loop_running_;
};

#endif // __ENET_SERVER_H_
