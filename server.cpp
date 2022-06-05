#include "server.h"

Server::Server()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(1234); // htons is necessary to convert a number to

    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    // Start listening.Hold at most 10 connections in the queue
    if (listen(sockfd, 10) < 0) {
        std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server is waiting for connection!" << std::endl;
    // Grab a connection from the queue
    auto addrlen = sizeof(sockaddr);
    this->sockfd = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    if (this->sockfd < 0) {
        std::cout << "Failed to grab connection. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
}
int Server::sendMessage(std::string message)
{
    send(this->sockfd, message.c_str(), message.size(), 0);
}
void Server::closeSocket()
{
    close(sockfd);
}