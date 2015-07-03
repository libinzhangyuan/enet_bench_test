#ifndef __ENET_SERVER_H_
#define __ENET_SERVER_H_

#include <memory>
#include <enet/enet.h>

typedef std::shared_ptr<ENetPeer> peer_ptr_t;
typedef std::shared_ptr<std::string> msg_ptr_t;

class EnetClient : public std::enable_shared_from_this<EnetClient>
{
public:
    EnetClient(const std::string& server_ip, int server_port, size_t test_msg_size);
    ~EnetClient(void);

    void Run(void);

private:
    void Connect(void);
    void RunLoopOnce(void);
    void SendMsg(const std::string& msg);
    void HandleRecvedMsg(const msg_ptr_t& msg);
    void SendTestMsg(void);

private:
    ENetAddress server_addr_;
    size_t test_msg_size_;
    std::shared_ptr<ENetHost> client_ptr_;
    std::shared_ptr<ENetPeer> peer_ptr_;
    bool loop_running_;
};

#endif // __ENET_SERVER_H_
