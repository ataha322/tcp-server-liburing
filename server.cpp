#include "server.hpp"

Server::Server(int port) {
    /* set the port */
    this->port = port;

    /* open the file to write into */
    this->file.open(std::to_string(port) + ".txt");

    if (!file.is_open()) {
        std::cerr << "failed to open the file!\n";
        exit(1);
    }

    /* initialize the ring */ 
	int ret = io_uring_queue_init(RING_SIZE, &this->ring ,0);
	if (ret < 0) {
		std::cerr << "ring init failed!\n";
		exit(1);
	}

    /* TCP-only socket */
	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock == -1) {
		std::cerr << "socket creation failed!\n";
		exit(1);
	}

    /* addresses */
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	/* bind the socket to the address */
	if (bind(this->sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
		std::cerr << "binding failed!\n";
		close(this->sock);
		exit(1);
	}

    /* listen */
    if (::listen(this->sock, SOCK_QUEUE_SIZE) == -1) {
        std::cerr << "listening failed!\n";
        close(this->sock);
        exit(1);
    }
}

void Server::submit_accept(struct channel* chan) {
    /* accept submission entry, a single one for all connections */
    struct io_uring_sqe* sqe = io_uring_get_sqe(&this->ring);
    chan->type = channel_type::accept;
    io_uring_prep_accept(sqe, this->sock,
            (struct sockaddr*) &chan->client_addr, &chan->client_len, 0);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&this->ring);
}

void Server::submit_read(struct channel* chan) {
    /* read submission, socket needs to be client */
    struct io_uring_sqe* sqe = io_uring_get_sqe(&this->ring);
    chan->type = channel_type::read;
    io_uring_prep_read(sqe, chan->sock, &chan->buf, MSG_SIZE, 0);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&this->ring);
}

/* this implements a 3 second delay and ACCEPTED message */
void Server::schedule_write_accepted(struct channel* chan) {
    struct channel* write_chan = new channel;
    *write_chan = *chan;
    write_chan->type = channel_type::write;

    struct io_uring_sqe* sqe;
    sqe = io_uring_get_sqe(&this->ring);

    struct __kernel_timespec delay = {
        .tv_sec = 3,
        .tv_nsec = 0
    };

    io_uring_prep_timeout(sqe, &delay, 0, 0);
    io_uring_sqe_set_data(sqe, write_chan);
    io_uring_submit(&this->ring);
}

void Server::submit_send(struct channel* chan) {

}

void Server::run() {
    channel* chan_acc = new channel;
    submit_accept(chan_acc);

    struct io_uring_cqe* cqe;
    while(1) {
        if (io_uring_wait_cqe(&this->ring, &cqe) < 0) {
            std::cerr << "cannot get cqe!\n";
            exit(1);
        }

        if ((void*)cqe->user_data == nullptr) continue;

        channel* chan = (channel*) cqe->user_data;
        switch(chan->type) {
            case channel_type::accept: {
                channel* read_chan = new channel;
                read_chan->client_addr = chan->client_addr;
                read_chan->sock = cqe->res;
                submit_accept(chan);
                submit_read(read_chan); 
                break;
            }
            case channel_type::read: {
                if (cqe->res == 0) {
                    ::close(chan->sock);
                    delete chan;
                } else {
                    file << chan->buf;
                    file.flush();
                    schedule_write_accepted(chan);
                    submit_read(chan);
                }
                break;
            }

            case channel_type::write:
                /* do nothing */ 
                break;
        }
        io_uring_cqe_seen(&this->ring, cqe);
    }
    
    delete chan_acc;
}

Server::~Server() {
    file.close();
    close(this->sock);
    io_uring_queue_exit(&ring);
}
