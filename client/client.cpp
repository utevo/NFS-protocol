#include <iostream>
#include <string>
#include <fcntl.h>

#include "../libraries/client/lib.hpp"


char *HOST = "localhost";
int PORT = 9000;
char *USER = "user";
char *PASSWORD = "password";


int main(){

    NFSClient client;
    std::cout << "try to connect" << std::endl;
    client.connect4(HOST, PORT, USER, PASSWORD);
    std::cout << "connected" << std::endl;

    char *path = "../README.md";
    int fd = client.open(path, O_RDONLY, 0);
    std::cout << "fd: " << fd << std::endl;
    std::cout << "error: " << error << std::endl;

    char buffer[1024];
    int len = client.read(fd, buffer, 1023);
    buffer[len] = 0;
    std::cout << "len: " << len << std::endl;
    std::cout << "error: " << error << std::endl;
    std::cout << "buffer: " << buffer << std::endl;
}