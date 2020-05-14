#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <sys/types.h>
#include <string>
#include <fstream>
#include <iostream>

#include <map>

#include "include/lib.hpp"

using FileDescriptor = int;
using FileInfo = std::string;

class Connector{
public:
    
    virtual int connect(std::string host, uint port, std::string user, std::string passwd)=0;
    virtual int open(std::string path, int oflag, int mode)=0;
    virtual ssize_t read(FileDescriptor fd, char *buf, size_t count)=0;
    virtual ssize_t write(FileDescriptor fd, const char* buf, size_t count)=0;
    virtual bool ls(std::string path, unsigned int options, std::vector<FileInfo>& dirs)=0;
    virtual int close(FileDescriptor fd)=0;
};


class LocalFSConnector: public Connector{
public:
    int connect(std::string host, uint port, std::string user, std::string passwd);
    FileDescriptor open(std::string path, int oflag, int mode);
    ssize_t read(FileDescriptor fd, char *buf, size_t count);
    ssize_t write(FileDescriptor fd, const char* buf, size_t count);
    int close(FileDescriptor fd);

    bool ls(std::string path, unsigned int options, std::vector<FileInfo>& dirs);

    ~LocalFSConnector();
private:
    std::map<FileDescriptor, std::fstream*> open_files{}; 
};


class NFSConnector: public Connector{
public:
    NFSConnector();
    int connect(std::string host, uint port, std::string user, std::string passwd);
    FileDescriptor open(std::string path, int oflag, int mode);
    ssize_t read(FileDescriptor fd, char *buf, size_t count);
    ssize_t write(FileDescriptor fd, const char* buf, size_t count);
    int close(FileDescriptor fd);

    bool ls(std::string path, unsigned int options, std::vector<FileInfo>& dirs){
        std::cout<<"sorki, to jeszcze nie zostało zaimplementowane\n";
    }
private:
    NFSClient* client=nullptr;
};



#endif