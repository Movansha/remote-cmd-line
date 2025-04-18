#include "config.h"
#include "functions.hpp"
#include "mainwindow.h"

#include "client.h"
#include "include/asio.hpp"

#include <QString>
#include <string>

#include <vector>
#include <fstream>

#include <thread>
#include <chrono>

//----------

using asio::ip::tcp;
using std::uint32_t;
using std::vector;

using std::string;

using std::thread;

//*****

static bool connection = false;

asio::io_context c_context;
tcp::socket c_socket = tcp::socket(c_context);

tcp::resolver c_resolver = tcp::resolver(c_context);
tcp::resolver::iterator c_endpoint;

//----------

//----------

string read_from_server(tcp::socket& c_socket) {
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

bool send_to_server(tcp::socket& c_socket, const string& content) {
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

static void process_commands(asio::io_context& c_context, tcp::socket& c_socket) {
	while (true) {
		string command = read_from_server(c_socket);
		QString qstr_command = QString::fromStdString(command);

		if (command == "failed" && connection == true) {
			if (c_socket.is_open()) c_socket.close();
			if (!c_context.stopped()) c_context.stop();

			emit MainWindow::instance->add_to_output("Connection lost!" "\n");
			break;
		}

		else if (command == "failed") {
			break;
		}

		// Custom commands -->

		else if (command == "beep")
		{
			string freq = read_from_server(c_socket);
			string duration = read_from_server(c_socket);

			if (freq == "failed" || duration == "failed") {
				break;
			}

			thread(Beep, std::stoi(freq), std::stoi(duration)).detach();
			emit MainWindow::instance->add_to_output("Played a beep sound by the server." "\n");

			send_to_server(c_socket, "[Client] Played a beep sound.");
		}

		else if (command == "message") {
			string title = read_from_server(c_socket);
			string message = read_from_server(c_socket);

			if (title == "failed" || message == "failed")
			{
				break;
			}

			thread(show_messagebox, title, message).detach();
			emit MainWindow::instance->add_to_output("Received a message from the server." "\n");

			send_to_server(c_socket, "[Client] Showed the message.");
		}

		else if (command == "screenshot")
		{
			take_screenshot();

			bool sent = send_file_to_server(c_socket, get_temp_ss_loc());

			if (sent) {
				send_to_server(c_socket, "[Client] Screenshot sent.");
				emit MainWindow::instance->add_to_output("Taken a screenshot by the server." "\n");
			}
		}

		else if (command == "disconnect")
		{
			MainWindow::instance->close();
			break;
		}

		// Custom commands <--

		else {
			string command_output;

			command += " > " + local_temp;
			execute_command("cmd.exe /C " + command);

			std::this_thread::sleep_for(std::chrono::milliseconds(400));
			emit MainWindow::instance->add_to_output("Command executed: " + qstr_command + "\n");

			try {
				vector<string> lines;

				std::ifstream output_file(local_temp);
				if (!output_file.is_open()) throw std::ios_base::failure("File couldn't open!");

				string temp;

				while (std::getline(output_file, temp)) {
					lines.push_back(temp);
				}

				output_file.close();

				for (string content : lines) {
					command_output += " | " + content + "\n";
				}
			}

			catch (...) {
				command_output = "[Client] No command output was taken." "\n";
			}

			if (command_output == "") command_output = "[Client] Command executed." "\n";

			send_to_server(c_socket, command_output);
		}
	}
}

//----------

void client::disconnect() {
	connection = false;

	if (c_socket.is_open()) c_socket.close();
	if (!c_context.stopped()) c_context.stop();
}

void client::connect() {
	connection = true;

	std::thread connect_thread([&]() {
		string port = std::to_string(int(config_json["port-number"]));;

		int counter = 0;
		QString qstr_counter;

		while (connection == true) {
			try {
				qstr_counter = QString::fromStdString(std::to_string(counter));
				bool sent;

				emit MainWindow::instance->add_to_output("[" + qstr_counter + "] Trying to connect..." "\n");

				c_endpoint = c_resolver.resolve(config_json["ip-adress"], port);
				asio::connect(c_socket, c_endpoint);

				//*****

				emit MainWindow::instance->add_to_output("Connected!" "\n");
				sent = send_to_server(c_socket, config_json["password"]);

				if (!sent) {
					disconnect();

					emit MainWindow::instance->add_to_output("[!] An error occured, disconnected!" "\n");
					break;
				}

				emit MainWindow::instance->add_to_output("Server verifies the password...");

				if (read_from_server(c_socket) == "OK") {
					emit MainWindow::instance->add_to_output("Verified, connection established!" "\n");
					emit MainWindow::instance->disable_disconnect(false);

					process_commands(c_context, c_socket);
				}

				else {
					disconnect();

					emit MainWindow::instance->add_to_output("Password did not match, disconnected!" "\n");
					emit MainWindow::instance->disable_disconnect(true);

					break;
				}
			}

			catch (...) {
				std::this_thread::sleep_for(std::chrono::seconds(5));
				counter += 1;
			}
		}
	} );

	connect_thread.detach();
}
