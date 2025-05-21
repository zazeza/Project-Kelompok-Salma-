#pragma once
#include <string>
#include <memory>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Thread.h>
#include "database/database.h"

class ServerConnection : public Poco::Net::TCPServerConnection {
public:
    ServerConnection(const Poco::Net::StreamSocket& socket, std::shared_ptr<Database> db);
    void run();
private:
    std::shared_ptr<Database> database;
};

class AttendanceServer {
public:
    AttendanceServer(const std::string& configPath);
    void start();
    void stop();
private:
    Poco::Net::ServerSocket _serverSocket;
    Poco::Net::TCPServer _server;
    std::shared_ptr<Database> _database;
};

