/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NetworkManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/02 17:39:53 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/06 20:14:04 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NETWORKSERVER_HPP
# define NETWORKSERVER_HPP
# include <iostream>
# include <string>
# include <list>
# include <queue>
# include "shaderpixel.h"

# define CLIENT_PORT			5446
# define SERVER_PORT			5447

# define MAX_UNIFORM_LENGTH		64
# define MAX_SHADER_NAME		128
# define MAX_UNIFORM_DATAS		4

# define CLUSTER_MAX_ROWS		13
# define CLUSTER_MAX_ROW_SEATS	23

# define NOW					-1

# define E1						1
# define E2						2
# define E3						3

typedef std::function< void (Timeval *timing, const std::string & shaderName) >	ShaderSwitchCallback;
typedef std::function< void (Timeval *timing, const std::string & shaderName) > ShaderUniformCallback;

enum 		NetworkStatus
{
	SUCCESS,						// obvious
	ERROR,							// a studid/strange error occured
	SERVER_RESERVED_COMMAND,		// you try to run a server command as a client.
	ANOTHER_COMMAND_IS_RUNNING,		// obvious
	MISSING_CLIENTS,				// the command was not executed on all selected clients
};

class		NetworkManager
{
	private:

		enum class		ClientStatus
		{
			UNKNOWN,					//unknow client status
			DISCONNECTED,				//the client is disconnected
			WAITING_FOR_COMMAND,		//nothing is running, client wait for command
			SHADER_IS_RUNNING,			//a shader is running
			WAITING_FOR_SHADER_SWITCH,	//a shader is running and the client is waiting to switch the shader at the specified time
		};

		struct			Client
		{
			int				seat;
			int				row;
			int				cluster;
			int				fd;
			int				satus;
			Timeval	averageTimeDelta;

			Client(int row, int seat, int cluster, int fd)
			{
				this->seat = seat;
				this->row = row;
				this->cluster = cluster;
				this->fd = fd;
			}
		};

		/*	Packet struct which handle all possible packets:
 		 *		Query client status
 		 *		Update client current program uniform
 		 *		Switch client program
 		 * */

		struct			Packet
		{
			int				type;
			Timeval	timing; //time to wait to execute datas
			union
			{
				struct //query client status
				{
					int		status;
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

		std::list< Client >		_clients;
		std::queue< Message >	_messageQueue;
		bool					_isServer;
		int						_fd;

		ShaderSwitchCallback	_shaderSwitchCallback = NULL;
		ShaderUniformCallback	_shaderUniformCallback = NULL;

		int						_SendPacketToAllClients(const Packet & packet);
		int						_SendPacketToGroup(const int groupId, const Packet & packet);

	public:
		NetworkManager(bool server = false);
		NetworkManager(const NetworkManager&) = delete;
		virtual ~NetworkManager(void);

		NetworkManager &	operator=(NetworkManager const & src) = delete;

		int		ConnectCluster(int clusterNumber);
		int		CloseAllConnections(void);
		void	GetSyncOffsets(void);
		int		Update(void);

		//Client callbacks:
		void			SetShaderSwitchCallback(ShaderSwitchCallback callback);
		void			SetShaderUniformCallback(ShaderUniformCallback callback);
		

		static int		RunShaderOnGroup(int groupId, const std::string & shaderName);
		static int		UpdateUniformOnGroup(int group, const std::string uniformName, ...);
		static int		SwitchShaderOnGroup(int groupId, const std::string & shaderName);
		static int		CreateNewGroup(void);
		static int		AddIMacToGroup(int row, int seat, int floor = 1);

		bool	IsServer(void) const;
};

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r);

#endif
