#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <liburing.h>

#define RING_SIZE 256
#define SOCK_QUEUE_SIZE 64
#define MSG_SIZE 128

class Server {
private:
    int port;
    struct io_uring ring;
    int sock;
    std::ofstream file;

    enum class channel_type { accept, read, write };
    struct channel {
        int sock;
        channel_type type;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(struct sockaddr_in);
        char buf[MSG_SIZE];
    };


public:
    Server() = delete;
    Server(int port);
    ~Server();
    void run();
    void submit_accept(struct channel* chan);
    void submit_read(struct channel* chan);
    void schedule_write_accepted(struct channel* chan);
};
