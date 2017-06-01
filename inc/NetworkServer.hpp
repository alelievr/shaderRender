#ifndef NETWORKSERVER_HPP
# define NETWORKSERVER_HPP
# include <iostream>
# include <string>
# include <list>

# define CLIENT_PORT			5446
# define SERVER_PORT			5447

# define MAX_UNIFORM_LENGTH		64
# define MAX_SHADER_NAME		128
# define MAX_UNIFORM_DATAS		4

# define CLUSTER_MAX_ROWS		13
# define CLUSTER_MAX_ROW_SEATS	23

# define NOW					-1

enum 		NetworkStatus
{
	SUCCESS,						// obvious
	ERROR,							// a studid/strange error occured
	SERVER_RESERVED_COMMAND,		// you try to run a server command as a client.
	ANOTHER_COMMAND_IS_RUNNING,		// obvious
	MISSING_CLIENTS,				// the command was not executed on all selected clients
};

class		NetworkServer
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

			Client(int row, int seat, int cluster, int fd)
			{
				this->seat = seat;
				this->row = row;
				this->cluster = cluster;
				this->fd = fd;
			}
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

		struct			Packet
		{
			int				type;
			struct timeval	timing; //time to wait to execute datas
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

		std::list< Client >	_clients;
		bool				_isServer;

		int					_SendPacketToAllClients(const Packet & packet);

	public:
		NetworkServer(bool server = false);
		NetworkServer(const NetworkServer&) = delete;
		virtual ~NetworkServer(void);

		NetworkServer &	operator=(NetworkServer const & src) = delete;

		int	ConnectCluster(int clusterNumber);

		void	GetSyncOffsets(void);

		int		CreateNewGroup(void);

		int		AddIMacToGroup(int row, int seat, int floor = 1);

		int		RunShaderOnGroup(int groupId, const std::string & shaderName);

		int		UpdateUniformOnGroup(int group, const std::string uniformName, ...);

		int		SwitchShaderOnGroup(int groupId, const std::string & shaderName);

		bool	IsServer(void) const;
};

std::ostream &	operator<<(std::ostream & o, NetworkServer const & r);

#endif
