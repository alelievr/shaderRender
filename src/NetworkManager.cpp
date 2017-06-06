/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NetworkManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/02 17:40:05 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/06 20:14:41 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NetworkManager.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <functional>

NetworkManager::NetworkManager(bool server)
{
	struct sockaddr_in		sin;

	this->_isServer = server;

	if (server)
	{
		if ((_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			perror("socket"), exit(-1);
		sin.sin_family = AF_INET;
		sin.sin_port = htons(SERVER_PORT);
		sin.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(_fd, reinterpret_cast< struct sockaddr* >(&sin), sizeof(sin)) == -1)
			perror("bind"), exit(-1);
		std::cout << "server started !" << std::endl;
	}
	else
	{
		//TODO: open client port, bind it, and listent for the server poke
	}
}

NetworkManager::~NetworkManager(void)
{
}

// Private functions

int		NetworkManager::_SendPacketToAllClients(const Packet & packet)
{
	(void)packet;
	return SUCCESS;
}

int		NetworkManager::_SendPacketToGroup(const int groupId, const Packet & packet)
{
	(void)groupId;
	(void)packet;
	return SUCCESS;
}

//public functions

int		NetworkManager::ConnectCluster(int clusterNumber)
{
	struct sockaddr_in			sin;

	if (!_isServer)
		return (std::cout << "not in server mode !" << std::endl, SERVER_RESERVED_COMMAND);
	if (clusterNumber <= 0 || clusterNumber > 3)
		return (std::cout << "bad cluster name !" << std::endl, ERROR);
	for (int r = 0; r < CLUSTER_MAX_ROWS; r++)
		for (int s = 0; s < CLUSTER_MAX_ROW_SEATS; s++)
		{
			std::string	ip = "10.1" + std::to_string(clusterNumber) + "." + std::to_string(r) + "." + std::to_string(s);

			Client		c(r, s, clusterNumber, -1);

			c.fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(CLIENT_PORT);
			sin.sin_addr.s_addr = inet_addr(ip.c_str());
			if (connect(c.fd, reinterpret_cast< const struct sockaddr *>(&sin), sizeof(sin)) == -1)
				std::cout << "cannot connect to iMac at e" + std::to_string(clusterNumber) + "r" + std::to_string(r) + "p" + std::to_string(s);
		}
	return SUCCESS;
}

int		NetworkManager::CloseAllConnections(void)
{
	Packet		closePacket;
	_SendPacketToAllClients(closePacket);
	return SUCCESS;
}

void	NetworkManager::GetSyncOffsets(void)
{
}

int		NetworkManager::Update(void)
{
	while (_messageQueue.size() != 0)
	{
		Message m = _messageQueue.front();
		_messageQueue.pop();

		_SendPacketToGroup(m.groupId, m.packet);
	}
	return SUCCESS;
}

// Public static fuctions

int		NetworkManager::CreateNewGroup(void)
{
	return SUCCESS;
}

int		NetworkManager::AddIMacToGroup(int row, int seat, int floor)
{
	(void)row;
	(void)seat;
	(void)floor;
	return SUCCESS;
}

int		NetworkManager::RunShaderOnGroup(int groupId, const std::string & shaderName)
{
	SwitchShaderOnGroup(groupId, shaderName);
	return SUCCESS;
}

int		NetworkManager::UpdateUniformOnGroup(int groupId, const std::string uniformName, ...)
{
	(void)groupId;
	(void)uniformName;
	return SUCCESS;
}

int		NetworkManager::SwitchShaderOnGroup(int groupId, const std::string & shaderName)
{
	(void)groupId;
	(void)shaderName;
	return SUCCESS;
}

//Client callbacks:
void	NetworkManager::SetShaderSwitchCallback(std::function< void (Timeval *timing, const std::string & shaderName) > callback)
{
	_shaderSwitchCallback = callback;
}

//Utils

bool		NetworkManager::IsServer(void) const { return (this->_isServer); }

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
