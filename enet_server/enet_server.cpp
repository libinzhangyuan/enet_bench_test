#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <ctime>
#include <sstream>
#include <thread>

#include "test_util.h"
#include "enet_server.h"

/* get system time */
static inline void itimeofday(long *sec, long *usec)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    if (sec) *sec = time.tv_sec;
    if (usec) *usec = time.tv_usec;
}

/* get clock in millisecond 64 */
static inline uint64_t iclock64(void)
{
    long s, u;
    uint64_t value;
    itimeofday(&s, &u);
    value = ((uint64_t)s) * 1000 + (u / 1000);
    return value;
}


std::string get_cur_time_str()
{
    time_t tmpcal_ptr = {0};
    struct tm *tmp_ptr = NULL;
    tmpcal_ptr = time(NULL);
    tmp_ptr = localtime(&tmpcal_ptr);
    std::ostringstream osstrm;
    osstrm << tmp_ptr->tm_hour << ":" << tmp_ptr->tm_min << "." << tmp_ptr->tm_sec;
    return osstrm.str();
}



EnetServer::EnetServer(const std::string& /*ip_bind_to*/, int port) :
    loop_running_(false)
{
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address_.host = ENET_HOST_ANY;
    address_.port = port;

    ENetHost* server = enet_host_create (& address_ /* the address to bind the server host to */, 
            32      /* allow up to 32 clients and/or outgoing connections */,
            2      /* allow up to 2 channels to be used, 0 and 1 */,
            0      /* assume any amount of incoming bandwidth */,
            0      /* assume any amount of outgoing bandwidth */);
    if (server == NULL)
    {
        fprintf (stderr, 
                "An error occurred while trying to create an ENet server host.\n");
        exit (EXIT_FAILURE);
    }

    server_ptr_.reset(server, enet_host_destroy);
}

EnetServer::~EnetServer(void)
{
}

void EnetServer::HandleRecvedMsg(ENetPeer* peer, const msg_ptr_t& msg)
{
    SendMsg(peer, msg);
}

void EnetServer::SendMsg(ENetPeer* peer, const msg_ptr_t& msg)
{
    ENetPacket *packet = enet_packet_create(msg->c_str(), msg->size(), ENET_PACKET_FLAG_RELIABLE);

    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    enet_peer_send(peer, 0, packet);
}


void EnetServer::Run(void)
{
    loop_running_ = true;

    while (loop_running_)
    {
        RunOnce();
    }
}

void EnetServer::RunOnce(void)
{
    ENetEvent event;
    /* Wait up to 1000 milliseconds for an event. */
    int ret = enet_host_service(server_ptr_.get(), & event, 1000);
    if (ret == 0)
    {
        std::cout << "."; std::cout.flush();
        return;
    }

    if (ret < 0)
    {
        std::cout << "enet_host_service end with error: " << ret << std::endl;
        loop_running_ = false;
        return;
    }

    switch (event.type)
    {
        case ENET_EVENT_TYPE_NONE:
            std::cout << "."; std::cout.flush();
            return;

        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n", 
                    event.peer->address.host,
                    event.peer->address.port);
            /* Store any relevant client information here. */
            static char client_info[] = "client info";
            event.peer->data = client_info;
            return;

        case ENET_EVENT_TYPE_RECEIVE:
            {
            printf("A packet of length %lu containing %s was received from %s on channel %d.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    (char*)(event.peer->data),
                    event.channelID);

            auto recved_msg = std::make_shared<std::string>( (const char*)(event.packet->data), (size_t)(event.packet->dataLength) );
            HandleRecvedMsg(event.peer, recved_msg);
            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy(event.packet);
            return;
            }

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char*)(event.peer -> data));
            /* Reset the peer's client information. */
            event.peer->data = NULL;
            return;

        default:
            std::cout << "unhandle event.type: " << event.type << std::endl;
            loop_running_ = false;
            return;
    }
}
