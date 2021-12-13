#include "../include/TcpConnection.h"


TcpConnection::pointer TcpConnection::create(asio::io_context& io_context, std::string& connectionName, std::shared_ptr<MessageProcessor> messageProcessor)
{
    return pointer(new TcpConnection(io_context, connectionName, messageProcessor)); //TODO refactor creation...
}

asio::ip::tcp::socket& TcpConnection::get_socket()
{
    return socket_;
}

void TcpConnection::start_read()
{
    asio::async_read(socket_, asio::buffer(&msgLength, sizeof(msgLength)), asio::transfer_exactly(sizeof(msgLength)),
                           std::bind(&TcpConnection::handle_read_msg_size, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}



void TcpConnection::handle_read_msg_size(const asio::error_code& error,
                                         size_t bytes_transferred)
{
    if(!error){
        std::cout << "Msg size on conn. " << connectionName_ <<": " << msgLength << std::endl;
        msgBuffer_ = std::make_shared<asio::streambuf>(msgLength);
        read_msg_content();
    }else{
        std::cerr << connectionName_ << " err: " << error.message() << " (maybe connection closed?)";
        signal_err_exit();
        return;
    }
 }

 void TcpConnection::read_msg_content(){
    int restToRead = msgLength - alreadyRead_;
    if(restToRead >MAX_PACKET_SIZE){
        asio::async_read(socket_, *msgBuffer_,asio::transfer_exactly(MAX_PACKET_SIZE),
                         std::bind(&TcpConnection::handle_read_msg_content, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }else{
        asio::async_read(socket_, *msgBuffer_,asio::transfer_exactly(restToRead),
                         std::bind(&TcpConnection::handle_read_msg_content, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

}

void TcpConnection::handle_read_msg_content(const asio::error_code &error, size_t bytes_transferred) {
    if(!error){
        alreadyRead_ += bytes_transferred;
        std::cout << "Content bytes read: " << alreadyRead_ << "/" << msgLength <<std::endl;
        std::istream istream(&(*msgBuffer_));
        std::string msgPart(std::istreambuf_iterator<char>(istream), {});
        finalMessage_ += msgPart;
        msgBuffer_->consume(bytes_transferred);
        if(alreadyRead_ != msgLength){
            read_msg_content();
        }else{
            alreadyRead_ = 0;
            std::cout << connectionName_ << " received: " << finalMessage_ << std::endl;
            messageProcessor_->processMessage(finalMessage_);
            finalMessage_ = "";
            start_read();
        }
    }else{
        std::cerr << connectionName_ << ": error in reading msg content: " << error.message() << std::endl;
        signal_err_exit();
        return;
    }
}

void TcpConnection::send_message(const std::string& msg) {
    asio::error_code errorWrite;
    uint32_t msgSize = msg.length();
    asio::write(socket_, asio::buffer(&msgSize, sizeof(msgSize)), errorWrite); // TODO these writes on one line?
    asio::write(socket_, asio::buffer(msg), errorWrite);
    if(errorWrite){
        signal_err_exit();
        std::cerr << "Error in sending." << errorWrite.message();
    }
}



std::shared_ptr<MessageProcessor> TcpConnection::getMessageProcessor_() {
    return messageProcessor_;
}

void TcpConnection::signal_err_exit() {
    auto exitControlMsg = std::make_shared<ControlMessage>(ControlMessage::CONTROL_MSG_TYPE::EXIT_ERR, "");
    MessageSerializer messageSerializer(exitControlMsg);
    messageProcessor_->processMessage(messageSerializer.serialize());
}