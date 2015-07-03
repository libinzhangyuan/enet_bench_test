#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <ctime>
#include <sstream>
#include <thread>
#include <vector>
#include <algorithm>

#include "test_util.h"
#include "enet_client.h"

#define SEND_TEST_MSG_INTERVAL 50

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

#define CLOCK_INTERVAL_STR "_"
msg_ptr_t make_test_str(size_t test_str_size)
{
    std::ostringstream ostr;
    ostr << iclock64();
    std::string msg_str = ostr.str();
    msg_str += test_str(CLOCK_INTERVAL_STR, test_str_size - msg_str.size());
    return msg_ptr_t(new std::string(msg_str));
}

uint64_t get_time_from_msg(const std::string& msg)
{
    std::size_t pos = msg.find(CLOCK_INTERVAL_STR);
    if (pos == std::string::npos)
    {
        std::cout << "wrong msg: " << msg << std::endl;
        return 0;
    }
    const std::string& time_str = msg.substr(0, pos);
    return std::atoll(time_str.c_str());
}



EnetClient::EnetClient(const std::string& server_ip, int server_port, size_t test_msg_size) :
    test_msg_size_(test_msg_size),
    loop_running_(false)
{
    //address_.host = ENET_HOST_ANY;
    enet_address_set_host (&server_addr_, server_ip.c_str());
    server_addr_.port = server_port;

    ENetHost* client = enet_host_create (NULL /* create a client host */,
            1 /* only allow 1 outgoing connection */,
            2 /* allow up 2 channels to be used, 0 and 1 */,
            57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
            14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
    if (client == NULL)
    {
        fprintf (stderr, 
                "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }
    client_ptr_.reset(client, enet_host_destroy);
}

EnetClient::~EnetClient(void)
{
}

void EnetClient::Connect(void)
{
    std::cout << "before enet_host_connect" << std::endl;
    /* Initiate the connection, allocating the two channels 0 and 1. */
    ENetPeer* peer = enet_host_connect(client_ptr_.get(), &server_addr_, 2, 0);    
    if (peer == NULL)
    {
        fprintf (stderr, "No available peers for initiating an ENet connection.\n");
        exit (EXIT_FAILURE);
    }
    peer_ptr_.reset(peer, enet_peer_reset);

    /* Wait up to 5 seconds for the connection attempt to succeed. */
    ENetEvent event;
    if (enet_host_service(client_ptr_.get(), &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts ("Connection to some.server.net:1234 succeeded.");
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        peer_ptr_.reset();
        puts ("Connection to some.server.net:1234 failed.");
    }
}

void EnetClient::SendTestMsg(void)
{
    auto msg = make_test_str(test_msg_size_);
    SendMsg(*msg);
}

void EnetClient::SendMsg(const std::string& msg)
{
    ENetPacket * packet = enet_packet_create (msg.c_str(), msg.size(), ENET_PACKET_FLAG_RELIABLE);

    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    enet_peer_send(peer_ptr_.get(), 0, packet);
}

std::vector<int> recv_package_interval_;
std::vector<int> recv_package_interval10_;
void EnetClient::HandleRecvedMsg(const msg_ptr_t& msg)
{
    if (msg->size() != test_msg_size_)
    {
        std::cout << "recv wrong msg size: " << msg->size() << std::endl << *msg << std::endl;
        loop_running_ = false;
        return;
    }

    // good test msg.
    //

    // log
    //
    uint64_t send_time = get_time_from_msg(*msg);
    uint64_t cur_time = iclock64();

    static uint64_t static_last_refresh_time = 0;
    static size_t static_recved_bytes = 0;
    static_recved_bytes += msg->size();

    {
        static size_t static_good_recv_count = 0;

        static_good_recv_count++;

        uint64_t interval = cur_time - send_time;
        recv_package_interval_.push_back(interval);
        recv_package_interval10_.push_back(interval);
        std::cout << interval << "\t";

        if (static_good_recv_count % 10 == 0)
        {
            int average10 = 0;
            for (int x : recv_package_interval10_)
                average10 += x;
            average10 = (average10 / 10);

            int average_total = 0;
            for (int x: recv_package_interval_)
                average_total += x;
            average_total = average_total / recv_package_interval_.size();

            std::cout << "max: " << *std::max_element( recv_package_interval10_.begin(), recv_package_interval10_.end() ) <<
                "  average 10: " << average10 <<
                "  average total: " << average_total;
            if (cur_time - static_last_refresh_time > 10 * 1000)
            {
                std::cout << " " << static_cast<double>(static_recved_bytes * 10 / (cur_time - static_last_refresh_time)) / 10 << "KB/s(in)";
                static_last_refresh_time = cur_time;
                static_recved_bytes = 0;
            }
            std::cout << std::endl;
            std::cout << get_cur_time_str() << " ";
            recv_package_interval10_.clear();
        }
        std::cout.flush();
    }
}

void EnetClient::Run(void)
{
    Connect();

    loop_running_ = true;
    while (loop_running_)
    {
        // send test msg every 50 milliseconds
        {
            static uint64_t last_send_test_msg_time = 0;

            const uint64_t cur_time = iclock64();
            if ( (cur_time - last_send_test_msg_time) > SEND_TEST_MSG_INTERVAL )
            {
                last_send_test_msg_time = ( cur_time  - (cur_time % SEND_TEST_MSG_INTERVAL) );
                SendTestMsg();
            }
        }

        RunLoopOnce();
    }
}

void EnetClient::RunLoopOnce(void)
{
    ENetEvent event;
    /* Wait up to 2 milliseconds for an event. */
    int ret = enet_host_service(client_ptr_.get(), & event, 2);
    if (ret == 0)
    {
        //std::cout << "."; std::cout.flush();
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
            //std::cout << "."; std::cout.flush();
            break;

        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n", 
                    event.peer->address.host,
                    event.peer->address.port);
            static char stored_info[] = "stored info";
            event.peer->data = stored_info;
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            {
            /*printf("A packet of length %lu containing %s was received from %s on channel %d.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    (char*)(event.peer->data),
                    event.channelID);*/
            auto recved_msg = std::make_shared<std::string>( (const char*)(event.packet->data), (size_t)(event.packet->dataLength) );
            HandleRecvedMsg(recved_msg);
            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy(event.packet);
            break;
            }

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char*)(event.peer -> data));
            /* Reset the peer's client information. */
            event.peer->data = NULL;
            break;

        default:
            std::cout << "unhandle event.type: " << event.type << std::endl;
            loop_running_ = false;
            break;
    }
}
