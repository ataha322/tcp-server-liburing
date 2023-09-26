#include "server.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "invalid number of arguments!\n";
        exit(1);
    }
    int port = std::stoi(argv[1]);

    Server server(port);
    server.run();
}
