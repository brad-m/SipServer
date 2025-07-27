#include "UdpServer.hpp"
#include <sstream>
#include <vector>
#include <thread>
#include <cstring>
#include <algorithm>

UdpServer::UdpServer(std::string ip, int port, OnNewMessageEvent event) : _ip(std::move(ip)), _port(port), _onNewMessageEvent(event), _keepRunning(false)
{

#if defined _WIN32 || defined _WIN64
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cerr << "Failed. Error Code: " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}
#endif

	if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		std::cerr << "socket creation failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = inet_addr(_ip.c_str());
	_servaddr.sin_port = htons(port);

	if (bind(_sockfd, reinterpret_cast<const struct sockaddr*>(&_servaddr), sizeof(_servaddr)) < 0)
	{
		std::cerr << "bind failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

UdpServer::~UdpServer()
{
	closeServer();
}

void UdpServer::startReceive()
{
	_keepRunning = true;
	_receiverThread = std::thread([=]()
		{
			char  buffer[BUFFER_SIZE];
			std::string message;
			sockaddr_in senderEndPoint;
			std::memset(&senderEndPoint, 0, sizeof(senderEndPoint));
			int len = sizeof(senderEndPoint);

			while (_keepRunning)
			{
				unsigned int receivedBytes;
				std::memset(&senderEndPoint, 0, sizeof(senderEndPoint));
#ifdef __linux__
				receivedBytes = recvfrom(_sockfd, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&senderEndPoint), (socklen_t*)&len);
#elif defined _WIN32 || defined _WIN64
				receivedBytes =recvfrom(_sockfd, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&senderEndPoint), &len);
				if (receivedBytes == SOCKET_ERROR)
					continue;

				if (receivedBytes <= BUFFER_SIZE)
				{
					message = std::string(buffer, receivedBytes);
				}
#endif
				if (!_keepRunning) return;
				_onNewMessageEvent(std::move(message), senderEndPoint);
			}
		});
}

int UdpServer::send(sockaddr_in address, std::string buffer)
{
	return sendto(_sockfd, buffer.c_str(), std::strlen(buffer.c_str()),
		0, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address));
}

void UdpServer::closeServer()
{
	_keepRunning = false;
	shutdown(_sockfd, 2);
#ifdef __linux__
	close(_sockfd);
#elif defined _WIN32 || defined _WIN64
	closesocket(_sockfd);
#endif
	_receiverThread.join();
}
