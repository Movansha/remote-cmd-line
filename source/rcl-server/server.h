#pragma once

#include "include/asio.hpp"

using asio::ip::tcp;
using std::string;

//----------

extern bool connection;

extern asio::io_context c_context;
extern tcp::socket c_socket;
extern tcp::acceptor c_acceptor;

//----------

string read_from_client(tcp::socket& c_socket);
bool send_to_client(tcp::socket& c_socket, const string& content);

namespace server {
	void stop();
	void start();
}
