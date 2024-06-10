// TCP.h : Copyright (C) 2024 https://www.youtube.com/@ProgrammerCpp
#pragma once
#define	_WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#include <sstream>
namespace Library {
	class TCP {
		SOCKET s;
	public:
		~TCP();
		TCP(_In_ SOCKET const s = INVALID_SOCKET) noexcept;
		TCP(TCP&& r) noexcept;
		TCP& operator=(TCP&& r) noexcept;
		explicit operator bool() const noexcept;

		void send_string
		( _In_ const char*       first
		, _In_ const char* const last 
		) const;
		void send_string(_In_ const std::string& t) const;
		std::string recv_string(_Inout_ std::stringstream& stm) const;

		const std::string& send_message(_In_ const std::string& message) const;
		std::string recv_message(_Inout_ std::stringstream& stm) const;

		const TCP& bind(_In_ const sockaddr_in& name) const;
		const TCP& listen(_In_ int backlog) const;
		TCP accept(_Out_ sockaddr_in& addr) const;
		static TCP connect
		( _In_ const sockaddr_in& name
		);
		static TCP connect
		( _In_  const std::string     & host_or_addr
		, _In_  u_short           const port_number 
		, _Out_ sockaddr_in           & addr        
		);
	};
}
