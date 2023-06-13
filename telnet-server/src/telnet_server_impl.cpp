#include "telnet_server_impl.h"
#include "telnet_session.h"

#include <thread>
#include <iostream>

using boost::asio::ip::tcp;

TelnetServerImpl::TelnetServerImpl(boost::asio::io_service& io_service, const TelnetConfig& cfg)
    : _io_service(io_service), _acceptor(_io_service, tcp::endpoint(tcp::v4(), cfg.server_port))
{
    TelnetSession::_username = cfg.username;
    TelnetSession::_passwd = cfg.passwd;
}

void TelnetServerImpl::start_accept()
{
    std::cout << "accept session...\n";
    TelnetSession* new_session = new TelnetSession(_io_service);
    _acceptor.async_accept(new_session->socket(),
        std::bind(&TelnetServerImpl::handle_accept, this, new_session, std::placeholders::_1));
}

void TelnetServerImpl::handle_accept(TelnetSession* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        std::thread([new_session]() {
            new_session->start();
        }).detach();
    }
    else
    {
        std::cout << "TelnetServerImpl::handle_accept error: " << error << std::endl;
        delete new_session;
    }

    start_accept();
}

