#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "All.hpp"
# include "Client.hpp"

class Client;

class Channel {
private:
	std::map<std::string, Client> members;
	std::vector<std::string>	  operators;
	std::vector<std::string>	  invites;
	std::string					  name;
	std::string					  topic;
	std::string					  password;
	size_t						  limit;
	bool						  inviteFlag;
	bool						  topicFlag;
	bool						  keyFlag;
	bool						  limitFlag;
	std::string					  startTime;
	bool						  activeBot;

	std::string getChannelModes() const;
	void		addInvited(std::string const &invited);
	bool		removeOperator(std::string const &op);
	void		removeInvited(std::string const &invited);
	bool		isInvited(std::string const &invited) const;
	void		modeInvite(Client const &setter, bool set);
	void		modeTopic(Client const &setter, bool set);
	void		modeLimit(Client const &setter, std::string const &newLimit, bool set);
	void		modeOP(Client const &setter, Client const &op, bool set);
	void		modeKey(Client const &setter, std::string const &newKey, bool set);

public:
	Channel();
	Channel(std::string const &name);
	Channel &operator=(const Channel &other);
	Channel(const Channel &other);
	~Channel();

	std::map<std::string, Client> getMembers() const;
	std::string					  getName() const;
	bool						  getBotState() const;
	std::string					  getTopic() const;

	void toogleBotState();
	void broadcast(std::string const &message, int sender_socket) const;
	void updateMap(std::string const &oldNick, std::string const &newNick, Client const &client);
	void pangramBot(std::string message) const;

	bool isMember(Client const &member) const;
	bool isOperator(std::string const &op) const;
	bool removeMember(Client const &member);
	bool removeMember(Client const &member, std::string const &reason);
	bool addMember(Client const &member, std::string const &key);
	bool addOperator(std::string const &op);

	// Channel operations
	void cmdKick(Client const &kicker, Client const &kicked, std::string &reason);
	void cmdMode(Client const &setter, std::vector<std::string> const &params);
	void cmdInvite(Client const &inviter, Client const &invited);
	void cmdTopic(Client const &setter, std::vector<std::string> const &params);

	// debug funcs
	void printMembers() const;
	void printOperators() const;
};
#endif // CHANNEL_HPP
