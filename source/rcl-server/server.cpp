#include "config.h"
#include "mainwindow.h"

#include "server.h"

#include "include/asio.hpp"

#include <vector>
#include <thread>

//----------

using asio::ip::tcp;
using std::uint32_t;
using std::vector;

using std::thread;

using std::string;

//*****

bool connection = false;

asio::io_context c_context;
tcp::socket c_socket = tcp::socket(c_context);
tcp::acceptor c_acceptor = tcp::acceptor(c_context, tcp::endpoint(tcp::v4(), 12580)); //default port number

//----------

//----------

string read_from_client(tcp::socket& c_socket) {
    asio::error_code ec;
    uint32_t content_len;

    asio::read(c_socket, asio::buffer(&content_len, sizeof(content_len)), ec);

    if (ec) return "failed";

    content_len = ntohl(content_len);
    vector<char> buffer(content_len);

    asio::read(c_socket, asio::buffer(buffer), ec);

    if (ec) return "failed";

    return string(buffer.begin(), buffer.end());
}

bool send_to_client(tcp::socket& c_socket, const string& content) {
    try {
        uint32_t content_len = static_cast<uint32_t>(content.size());
        content_len = htonl(content_len);

        asio::write(c_socket, asio::buffer(&content_len, sizeof(content_len)));
        asio::write(c_socket, asio::buffer(content));

        return true;
    }
    catch (...) {
        return false;
    }
}

//----------

void server::stop() {
    if (c_acceptor.is_open()) c_acceptor.close();

    if (c_socket.is_open()) {
        c_socket.shutdown(tcp::socket::shutdown_both);
        c_socket.close();
    }

    if (!c_context.stopped()) c_context.stop();

    connection = false;
}

void server::start() {
    thread start_thread( [&]() {
        server::stop();

        asio::error_code ec;
        connection = true;

        c_acceptor = tcp::acceptor(c_context, tcp::endpoint(tcp::v4(), config_json["port-number"]));
        c_acceptor.accept(c_socket, ec);

        if (!ec && connection == true) {
            string connected_ip = c_socket.remote_endpoint().address().to_string();
            MainWindow::instance->add_to_output("Connected IP: " + QString::fromStdString(connected_ip) + "\n");

            //*****

            MainWindow::instance->add_to_output("Verifying the password...");

            if (read_from_client(c_socket) == config_json["password"]) {
                MainWindow::instance->add_to_output("Verified, connection established!" "\n");

                send_to_client(c_socket, "OK");
                emit MainWindow::instance->disable_func_buttons(false);
            }

            else {
                server::stop();
                MainWindow::instance->add_to_output("Couldn't verified, server stopped!" "\n");

                send_to_client(c_socket, "KO");
                emit MainWindow::instance->disable_func_buttons(true);
            }
        }

        if (ec && connection == true) {
            server::stop();
            MainWindow::instance->add_to_output("[!] An error occured, server stopped!");

            emit MainWindow::instance->disable_func_buttons(true);
        }

        if (ec) server::stop();

    } );

    start_thread.detach();
}
