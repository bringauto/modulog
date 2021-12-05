#pragma once
#include <asio.hpp>
#include "TcpConnection.h"

class TcpServer {
public:
    TcpServer(asio::io_context& io_context, std::mutex& messageMutex, std::condition_variable& messageConditionVariable, int& totalReceivedMsgs)
    : io_context_(io_context),
            acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1234)),
            messageMutex_(messageMutex), messageConditionVariable_(messageConditionVariable), totalReceivedMsgs_(totalReceivedMsgs){}
    void start_accept();
   TcpConnection::pointer popConnection();

private:
    void handle_accept(TcpConnection::pointer new_connection, const asio::error_code& error);
    // ---- attributes
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<TcpConnection::pointer> allConnectionsVector;
    std::vector<TcpConnection::pointer> lastConnectionsVector;
    std::string serverName_ = "server";
    // Synchro vars:
    std::mutex& messageMutex_;
    std::condition_variable& messageConditionVariable_;
    int& totalReceivedMsgs_;
};