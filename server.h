#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read

class Server{
    public:
        Server();
        int sendMessage(std::string message);
        void closeSocket();

    private:
        int sockfd;
        sockaddr_in sockaddr;
};