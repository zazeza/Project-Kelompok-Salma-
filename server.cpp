#include "server.h"
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/MessageHeader.h>
#include <Poco/Exception.h>
#include <iostream>
#include <sstream>

#include "server.h"
#include "utils/logger.h"

ServerConnection::ServerConnection(const Poco::Net::StreamSocket& socket, std::shared_ptr<Database> db)
    : Poco::Net::TCPServerConnection(socket), database(db) {
    socket().setReceiveTimeout(3000); // 3 seconds read timeout
}

void ServerConnection::run() {
    try {
        Poco::Net::SocketStream stream(socket());
        std::string line;
        
        // membaca RFID ID and timestamp
        std::getline(stream, line);
        std::istringstream iss(line);
        std::string rfidId, timestamp;
        iss >> rfidId >> timestamp;
        
        // menambahkan record to database
        if (database->addRecord(rfidId, timestamp)) {
            stream << "SUCCESS\n";
            Logger::getInstance().log(Logger::Level::INFO, 
                "Successfully recorded attendance for RFID: " + rfidId);
        } else {
            stream << "ERROR\n";
            Logger::getInstance().log(Logger::Level::ERROR, 
                "Failed to record attendance for RFID: " + rfidId);
        }
    } catch (Poco::Exception& exc) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Connection error: " + std::string(exc.what()));
    } catch (std::exception& exc) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Unexpected error: " + std::string(exc.what()));
    }
}

AttendanceServer::AttendanceServer(const std::string& configPath)
    : _database(std::make_shared<Database>("data")) {
    
    // Load configuration
    std::ifstream configFile(configPath);
    if (!configFile) {
        throw std::runtime_error("Failed to load configuration file");
    }
    
    json config;
    configFile >> config;
    
    // Configure server
    _serverSocket = Poco::Net::ServerSocket(config["server"]["port"]);
    
    Poco::Net::TCPServerParams* pParams = new Poco::Net::TCPServerParams;
    pParams->setMaxThreads(config["server"]["max_connections"]);
    pParams->setMaxQueued(config["server"]["max_queued"]);
    
    _server = new Poco::Net::TCPServer(
        new Poco::Net::TCPServerConnectionFactoryImpl<ServerConnection>(_database),
        _serverSocket,
        pParams
    );
}

void AttendanceServer::start() {
    try {
        _server->start();
        Logger::getInstance().log(Logger::Level::INFO, 
            "Attendance server started successfully");
        std::cout << "Attendance server started" << std::endl;
    } catch (const std::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Failed to start server: " + std::string(e.what()));
        throw;
    }
}

void AttendanceServer::stop() {
    try {
        _server->stop();
        Logger::getInstance().log(Logger::Level::INFO, 
            "Attendance server stopped");
        std::cout << "Attendance server stopped" << std::endl;
    } catch (const std::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Error stopping server: " + std::string(e.what()));
        throw; 
    }



}

// print all records 
// trial and error 
// comment azizah

    




