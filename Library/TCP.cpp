// TCP.cpp : Copyright (C) 2024 https://www.youtube.com/@ProgrammerCpp
#include "TCP.h"
#include <utility>
#include <stdexcept>

Library::TCP::~TCP()
{
	if (s != INVALID_SOCKET) {
		if (::closesocket(s) == SOCKET_ERROR)
			OutputDebugStringA((__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): TCP failed with " + std::to_string(WSAGetLastError()) + '\n').c_str());
	}
}
Library::TCP::TCP(_In_ SOCKET const s) noexcept
	: s(s)
{
}
Library::TCP::TCP(TCP&& r) noexcept
	: s(r.s)
{
	r.s = INVALID_SOCKET;
}
auto Library::TCP::operator=(TCP&& r) noexcept->TCP&
{
	std::swap(s, r.s);
	return *this;
}
Library::TCP::operator bool() const noexcept
{
	return s != INVALID_SOCKET;
}
void Library::TCP::send_string
( _In_ const char*       first
, _In_ const char* const last 
) const
{
	while(const auto n1 = last - first) {
		const auto n2 = send(s, first, (int)n1, 0);
		if (n2 == SOCKET_ERROR)
			throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): send failed with " + std::to_string(WSAGetLastError()));
		else
			first += n1;
	}
}
void Library::TCP::send_string(_In_ const std::string& t) const
{
	send_string(&t.front(), &t.back() + 1);
}
std::string Library::TCP::recv_string(_Inout_ std::stringstream& stm) const
{
	std::string reply;
	for (;;) {
		std::string follow;
		if (std::getline(stm, follow)) {
			reply += follow;
			if (!stm.eof()) {
				reply += '\n';
				return reply;
			}
		}
		stm.clear();
		char buf[256];
		switch (const auto n = recv(s, buf, sizeof buf, 0)) {
		case SOCKET_ERROR:
			throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): recv failed with " + std::to_string(WSAGetLastError()));
		case 0:
			return reply;
		default:
			stm.write(buf, (unsigned)n);
			continue;
		}
	}
}
const std::string& Library::TCP::send_message(_In_ const std::string& message) const
{
	send_string(message + '\n');
	return message;
}
std::string Library::TCP::recv_message(_Inout_ std::stringstream& stm) const
{
	auto t = recv_string(stm);
	if (t.empty())
		return {};
	else if (t.back() == '\n')
		return t.substr(0, t.length() - 1);
	else
		return {};//改行で終わっておらず、正規なメッセージとして認められない
}
auto Library::TCP::bind(_In_ const sockaddr_in& name) const->const TCP&
{
	if (::bind(s, (const sockaddr*)&name, sizeof name) == SOCKET_ERROR)
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) ": bind failed with " + std::to_string(WSAGetLastError()));
	else
		return *this;
}

auto Library::TCP::listen(_In_ int const backlog) const->const TCP&
{
	if (::listen(s, backlog) == SOCKET_ERROR)
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) ": listen failed with " + std::to_string(WSAGetLastError()));
	else
		return *this;
}
auto Library::TCP::accept(_Out_ sockaddr_in& addr) const->TCP
{
	int addrlen = sizeof addr;
	const auto s2 = ::accept(s, (sockaddr*)&addr, &addrlen);
	if (s2 == INVALID_SOCKET)
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) ": accept failed with " + std::to_string(WSAGetLastError()));
	else
		return { s2 };
}
auto Library::TCP::connect
( _In_ const sockaddr_in& name
)->TCP
{
	const auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): socket failed with " + std::to_string(WSAGetLastError()));
	else if (::connect(s, (const sockaddr*)&name, sizeof name) == SOCKET_ERROR)
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): connect failed with " + std::to_string(WSAGetLastError()));
	else {
		return { s };
	}
}
auto Library::TCP::connect
( _In_  const std::string     & host_or_addr
, _In_  u_short           const port_number 
, _Out_ sockaddr_in           & addr        
)->TCP
{
	addr =
	{ /*ADDRESS_FAMILY sin_family ;*/AF_INET
	, /*USHORT         sin_port   ;*/htons(port_number)
	, /*IN_ADDR        sin_addr   ;*/{}
	, /*CHAR           sin_zero[8];*/{}
	};
	if ((addr.sin_addr.S_un.S_addr = inet_addr(host_or_addr.c_str())) == INADDR_NONE) {
		const auto host = gethostbyname(host_or_addr.c_str());
		if (host && host->h_addrtype == AF_INET && host->h_length == 4) {
			addr.sin_addr = *(const in_addr*)host->h_addr_list[0];
			return connect(addr);
		}
		else
			throw std::runtime_error("該当するサーバが見つかりませんでした。");
	}
	else
		return connect(addr);
}
