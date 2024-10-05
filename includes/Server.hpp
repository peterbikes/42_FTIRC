#ifndef SERVER_HPP
# define SERVER_HPP

# include "All.hpp"
# include "Channel.hpp"
# include "Client.hpp"

class Server {
private:
	int							   serverSocket;
	struct sockaddr_in			   serverAddr, client_addr;
	std::map<std::string, Channel> channels;
	std::vector<Client *>		   clients;
	std::string					   hostname;
	std::string					   startTime;
	int							   port;
	std::string					   pass;

	void monitoringRemoveClient(fd_set &master_set, int &client_fd);
	void monitoringExistingClient(fd_set &master_set, int &client_fd, char buffer[1024]);
	void monitoringAddClient(fd_set &master_set, int &max_sd);
	void handleMessage(std::string buffer, std::vector<Client *>::iterator client_it);
	void initialMessageTreatment(Client *client, std::string const &buffer);
	bool messageOfTheDay(Client &client);

	bool	isClient(std::string const &nick) const;
	Client *getClient(std::string const &nick) const;

	void cmdJoin(struct message &input, Client &client);
	void cmdPrivmsg(std::vector<std::string> const &params, Client const &client);
	void cmdNotice(std::vector<std::string> const &params, Client const &client);
	void cmdWho(std::vector<std::string> const &params, Client const &client);
	void cmdPart(struct message &input, Client &client);
	void cmdBot(std::vector<std::string> const &params, const Client &client);
	void cmdTopic(std::vector<std::string> const &params, Client const &setter);
	void cmdQuit(struct message &input, Client &client);
	void cmdInvite(std::vector<std::string> const &params, Client const &inviter);
	void cmdMode(std::vector<std::string> const &params, Client const &setter);
	void cmdKick(message &input, Client const &kicker);
	void cmdPing(std::vector<std::string> const &params, Client const &client);

	void		sendMsg(Client const &client, std::string const &target, std::string const &msg);
	std::string createUserList(std::map<std::string, Client> const &members, std::string const &channelName);
	void		sendJoinMsg(Client &client, std::string const &channelName);
	void		joinChannel(Client &client, std::string const &channelName, std::string const &key);
	void		partChannel(Client &client, std::string const &channelName, std::string const &reason);

public:
	Server(int port, const std::string &password);
	~Server();

	void		monitorConnections();
	bool		start();
	int			getPort() const;
	std::string getHostname() const;
	std::string getPassword() const;
	std::string getStartTime() const;
	std::string getClientsOn() const;
	std::string getChannelsOn() const;
	void		printChannelMembers() const;
};
#endif // SERVER_HPP
