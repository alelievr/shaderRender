/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NetworkManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/02 17:39:53 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/07 01:45:42 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NETWORKSERVER_HPP
# define NETWORKSERVER_HPP
# include <iostream>
# include <string>
# include <list>
# include <map>
# include <queue>
# include <sys/select.h>
# include <netdb.h>
# include <sys/socket.h>
# include <stdio.h>
# include <arpa/inet.h>
# include "shaderpixel.h"

# define CLIENT_PORT			5446
# define SERVER_PORT			5447

# define MAX_UNIFORM_LENGTH		64
# define MAX_SHADER_NAME		128
# define MAX_UNIFORM_DATAS		4
# define MAX_MESSAGE_LENGTH		256

# define CLUSTER_MAX_ROWS		13
# define CLUSTER_MAX_ROW_SEATS	23

# define NOW					-1

# define E1						1
# define E2						2
# define E3						3

# define IP_LENGHT				sizeof("127.127.127.127")

typedef std::function< void (Timeval *timing, const std::string & shaderName) >	ShaderSwitchCallback;
typedef std::function< void (Timeval *timing, const std::string & shaderName) > ShaderUniformCallback;

enum class	NetworkStatus
{
	Success,						// obvious
	Error,							// a studid/strange error occured
	ServerReservedCommand,			// you try to run a server command as a client.
	ClientReservedCommand,			// you try to run a client command as a client.
	AnotherCommandIsRunning,		// obvious
	NotConnectedToServer,			// obvious
	MissingClients,					// the command was not executed on all selected clients
};

class		NetworkManager
{
	private:

		enum class		ClientStatus
		{
			Unknown,					//unknow client status
			Disconnected,				//the client is disconnected
			WaitingForCommand,			//nothing is running, client wait for command
			ShaderIsRunning,			//a shader is running
			WaitingForShaderSwitch,		//a shader is running and the client is waiting to switch the shader at the specified time
		};

		enum class		PacketType
		{
			Status,
			UniformUpdate,
		};

		enum class		UniformType
		{
			Float1,
			Float2,
			Float3,
			Float4,
			Int1,
		};

		struct			Client
		{
			int				seat;
			int				row;
			int				cluster;
			int				groupId;
			ClientStatus	satus;
			unsigned int	ip;
			Timeval			averageTimeDelta;

			Client(int row, int seat, int cluster, const char *cip)
			{
				struct sockaddr_in	connection;

				this->seat = seat;
				this->row = row;
				this->cluster = cluster;
				this->groupId = 0;
				satus = ClientStatus::Unknown;
				inet_aton(cip, &connection.sin_addr);
				ip = connection.sin_addr.s_addr;
			}
		};

		/*	Packet struct which handle all possible packets:
 		 *		Query client status
 		 *		Update client current program uniform
 		 *		Switch client program
 		 * */

		struct			Packet
		{
			PacketType		type;
			int				groupId;
			int				ip;
			Timeval			timing; //time to wait to execute datas
			union
			{
				struct //query client status
				{
					ClientStatus	status;
					int				seat;
					int				row;
					int				cluster;
					int				:32;
				};
				struct //String packet (for errors/warning/debug)
				{
					char			message[MAX_MESSAGE_LENGTH];
				};
				struct //update cient uniform
				{
					int		programIndex;
					char	uniformName[MAX_UNIFORM_LENGTH];
					int		uniformType;
					union
					{
						int		uniformInt;
						int		uniformInts[MAX_UNIFORM_DATAS];
						float	uniformFloat;
						float	uniformFloats[MAX_UNIFORM_DATAS];
					};
				};
				struct //switch client 
				{
					char	shaderName[MAX_SHADER_NAME];
				};
			};
		};

		struct			Message
		{
			const int		groupId;
			const Packet	packet;

			Message(int gId, const Packet p) : groupId(gId), packet(p) {}
		};

		std::map< int, std::list< Client > > _clients;
		std::queue< Message >	_messageQueue;
		bool					_isServer;
		int						_serverSocket;
		int						_clientSocket;
		bool					_connectedToServer;
		Client *				_me;
		char					_serverIp[IP_LENGHT];
		fd_set					_serverFdSet;
		static int				_localGroupId;

		ShaderSwitchCallback	_shaderSwitchCallback = NULL;
		ShaderUniformCallback	_shaderUniformCallback = NULL;

		NetworkStatus			_SendPacketToAllClients(const Packet & packet) const;
		NetworkStatus			_SendPacketToGroup(const int groupId, const Packet & packet);
		NetworkStatus			_SendPacketToServer(const Packet & packet) const;

		void					_ServerSocketEvent(void);
		void                	_ClientSocketEvent(const struct sockaddr_in & connection, const Packet & packet);

		void					_FillLocalInfos(void);

		//Create packet functions:
		void					_InitPacketHeader(Packet *p, const Client & client, const PacketType type) const;
		Packet					_CreatePokeStatusPacket(void) const;
		Packet					_CreatePokeStatusResponsePacket(const Client & client, const Packet & oldPacket) const;

	public:
		NetworkManager(bool server = false);
		NetworkManager(const NetworkManager&) = delete;
		virtual ~NetworkManager(void);

		NetworkManager &	operator=(NetworkManager const & src) = delete;

		NetworkStatus		ConnectCluster(int clusterNumber);
		NetworkStatus		CloseAllConnections(void);
		void				GetSyncOffsets(void);
		NetworkStatus		Update(void);

		//Client callbacks:
		void			SetShaderSwitchCallback(ShaderSwitchCallback callback);
		void			SetShaderUniformCallback(ShaderUniformCallback callback);
		

		static NetworkStatus	RunShaderOnGroup(int groupId, const std::string & shaderName);
		static NetworkStatus	UpdateUniformOnGroup(int group, const std::string uniformName, ...);
		static NetworkStatus	SwitchShaderOnGroup(int groupId, const std::string & shaderName);
		static int				CreateNewGroup(void);
		static NetworkStatus	AddIMacToGroup(int row, int seat, int floor = 1);

		bool	IsServer(void) const;
};

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r);

#endif
