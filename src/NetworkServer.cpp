#include "NetworkServer.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>

NetworkServer::NetworkServer(bool server)
{
	this->_isServer = server;
}

NetworkServer::~NetworkServer(void)
{
}

int		NetworkServer::_SendPacketToAllClients(const Packet & packet)
{
	(void)packet;
	return SUCCESS;
}

int		NetworkServer::ConnectCluster(int clusterNumber)
{
	struct sockaddr_in			sin;

	if (_isServer)
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

void	NetworkServer::GetSyncOffsets(void)
{
}

int		NetworkServer::CreateNewGroup(void)
{
	return SUCCESS;
}

int		NetworkServer::AddIMacToGroup(int row, int seat, int floor)
{
	(void)row;
	(void)seat;
	(void)floor;
	return SUCCESS;
}

int		NetworkServer::RunShaderOnGroup(int groupId, const std::string & shaderName)
{
	SwitchShaderOnGroup(groupId, shaderName);
	return SUCCESS;
}

int		NetworkServer::UpdateUniformOnGroup(int groupId, const std::string uniformName, ...)
{
	(void)groupId;
	(void)uniformName;
	return SUCCESS;
}

int		NetworkServer::SwitchShaderOnGroup(int groupId, const std::string & shaderName)
{
	(void)groupId;
	(void)shaderName;
	return SUCCESS;
}

bool		NetworkServer::IsServer(void) const { return (this->_isServer); }

std::ostream &	operator<<(std::ostream & o, NetworkServer const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
