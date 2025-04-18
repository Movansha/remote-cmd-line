#pragma once

#include "include/asio.hpp"

using asio::ip::tcp;
using std::string;

//----------

extern asio::io_context c_context;
extern tcp::socket c_socket;

extern tcp::resolver c_resolver;
extern tcp::resolver::iterator c_endpoint;

//----------

string read_from_server(tcp::socket& c_socket);
bool send_to_server(tcp::socket& c_socket, const string& content);

namespace client {
	void disconnect();
	void connect();
}
