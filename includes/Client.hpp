#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "All.hpp"
# include "Channel.hpp"

class Channel;

class Client {
private:
	int				   socket;
	struct sockaddr_in address;
	std::string		   pass;
	std::string		   realname;
	std::string		   username;
	std::string		   nick;
	std::string		   hostname;
	bool			   validNick;
	bool			   validPass;
	bool			   connectionReady;
	bool			   connected;
	bool			   modSent;

public:
	Client();
	Client(int client_socket, struct sockaddr_in client_address, std::string const &hostname);
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client();

	std::vector<Channel *> channels;

	int				   getSocket() const;
	const std::string &getNick() const;
	const std::string &getPass() const;
	const std::string &getRealname() const;
	const std::string &getUsername() const;
	const std::string &getHostname() const;
	std::string		   getAddress() const;
	bool			   getValidPass() const;
	bool			   getconnectionReady() const;
	bool			   getConnected() const;
	bool			   getModSent() const;
	bool			   getValidNick() const;

	void setConnectionReady(bool connectionReady);
	void setConnected(bool connected);
	void setModSent(bool modSent);
	void setPass(std::string buffer, std::string server_password);
	void setNick(std::string buffer, std::vector<Client *> clients);
	void setUser(std::string buffer);

	bool checkDuplicatedNick(std::string nick, std::vector<Client *> &clients);
	void printInfo() const;

	// protocol methods:
	void cmdNick(const std::string &newNick, std::vector<Client *> &clients);
	void cmdQuit(std::vector<Client *> &clients, std::string const &reason);

	// buffer operations
	std::string buffer;
	std::string message;
	bool		messageReady;

	bool parseBuffer();
};
#endif // CLIENT_HPP
