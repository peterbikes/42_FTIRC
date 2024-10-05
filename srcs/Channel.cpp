#include "../includes/All.hpp"
#include "../includes/Channel.hpp"

Channel::Channel() : members(), operators(),
					 name(), topic(),
					 password(), limit(0),
					 inviteFlag(false), topicFlag(false), keyFlag(false), limitFlag(false),
					 activeBot(false) {
	std::time_t		  currentTime = std::time(NULL);
	std::stringstream ss;
	ss << currentTime;
	startTime = ss.str();
}

Channel::Channel(std::string const &name) : members(), operators(),
											name(name), topic(),
											password(), limit(0),
											inviteFlag(false), topicFlag(false),
											keyFlag(false), limitFlag(false),
											activeBot(false) {
	std::time_t		  currentTime = std::time(NULL);
	std::stringstream ss;
	ss << currentTime;
	startTime = ss.str();
}

Channel &Channel::operator=(const Channel &other) {
	if (this == &other) {
		return *this;
	}
	this->name = other.name;
	this->topic = other.topic;
	this->members = other.members;
	this->operators = other.operators;
	this->limit = other.limit;
	this->inviteFlag = other.inviteFlag;
	this->topicFlag = other.topicFlag;
	this->keyFlag = other.keyFlag;
	this->limitFlag = other.limitFlag;
	this->activeBot = other.activeBot;
	this->startTime = other.startTime;
	return *this;
}

Channel::Channel(const Channel &other) {
	*this = other;
}

Channel::~Channel() {
}

std::string Channel::getChannelModes() const {
	std::string modes;
	if (this->inviteFlag)
		modes += "i";
	if (this->keyFlag)
		modes += "k";
	if (this->limitFlag)
		modes += "l";
	if (this->topicFlag)
		modes += "t";
	return modes;
}

bool Channel::isMember(const Client &member) const {
	if (this->members.find(member.getNick()) != this->members.end()) {
		return true;
	}
	return false;
}

bool Channel::isOperator(std::string const &op) const {
	if (std::find(this->operators.begin(), this->operators.end(), op) == this->operators.end()) {
		return false;
	}
	return true;
}

bool Channel::addMember(Client const &member, std::string const &key) {
	if (this->inviteFlag && !this->isInvited(member.getNick())) {
		std::string rpl_473 = ERR_INVITEONLYCHAN(member.getNick(), this->name);
		sendToClient(rpl_473, member);
		return false;
	}
	if (this->limitFlag) {
		if (this->getMembers().size() >= limit) {
			std::string rpl_471 = ERR_CHANNELISFULL(member.getNick(), this->name);
			sendToClient(rpl_471, member);
			return false;
		}
	}
	if (this->keyFlag) {
		if (this->password != key) {
			std::string rpl_475 = ERR_BADCHANNELKEY(member.getNick(), this->name);
			sendToClient(rpl_475, member);
			return false;
		}
	}
	if (this->isMember(member))
		return false;
	std::string nick = member.getNick();
	this->members[member.getNick()] = Client(member);
	this->removeInvited(member.getNick());
	return true;
}

bool Channel::removeMember(Client const &member, std::string const &reason) {
	if (!this->isMember(member)) {
		std::string rpl_442 = ERR_NOTONCHANNEL(member.getNick(), this->name);
		sendToClient(rpl_442, member);
		return false;
	}
	std::string partMessage = ":" + member.getNick() + " PART " + this->name + " " + reason + "\r\n";
	sendToClient(partMessage, member);
	this->members.erase(member.getNick());
	return true;
}

bool Channel::removeMember(Client const &member) {
	if (!this->isMember(member)) {
		std::string rpl_442 = ERR_NOTONCHANNEL(member.getNick(), this->name);
		sendToClient(rpl_442, member);
		return false;
	}
	this->members.erase(member.getNick());
	return true;
}

void Channel::printMembers() const {
	std::cout << "Members of " << this->name << ":" << std::endl;
	for (std::map<std::string, Client>::const_iterator it = this->members.begin(); it != this->members.end(); ++it) {
		std::cout << it->first << std::endl;
	}
}

std::string Channel::getName() const {
	return this->name;
}

std::string Channel::getTopic() const {
	return this->topic;
}

bool Channel::addOperator(std::string const &op) {
	if (this->isOperator(op)) {
		return false;
	}
	this->operators.push_back(op);
	return true;
}

bool Channel::removeOperator(std::string const &op) {
	if (!this->isOperator(op)) {
		return false;
	}
	this->operators.erase(std::find(operators.begin(), operators.end(), op));
	return true;
}

void Channel::addInvited(const std::string &invited) {
	if (this->isInvited(invited)) {
		return;
	}
	this->invites.push_back(invited);
}

void Channel::removeInvited(const std::string &invited) {
	if (!this->isInvited(invited)) {
		return;
	}
	this->invites.erase(std::find(invites.begin(), invites.end(), invited));
}

bool Channel::isInvited(const std::string &invited) const {
	if (std::find(this->invites.begin(), this->invites.end(), invited) == this->invites.end()) {
		return false;
	}
	return true;
}

void Channel::printOperators() const {
	std::cout << "Operators of " << this->name << ":" << std::endl;
	for (std::vector<std::string>::const_iterator it = this->operators.begin(); it != this->operators.end(); ++it) {
		std::cout << *it << std::endl;
	}
}

void Channel::broadcast(std::string const &message, int sender_socket) const {
	for (std::map<std::string, Client>::const_iterator it = this->members.begin(); it != this->members.end(); ++it) {
		if (it->second.getSocket() == sender_socket) {
			continue;
		}
		sendToClient(message, it->second);
	}
}

std::map<std::string, Client> Channel::getMembers() const {
	return this->members;
}

void Channel::modeInvite(const Client &setter, bool set) {
	std::string mode;

	if (!this->isOperator(setter.getNick())) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(rpl_482, setter);
		return;
	}
	if (!set) {
		this->inviteFlag = false;
		mode = " -i ";
	} else {
		this->inviteFlag = true;
		mode = " +i ";
	}
	std::string msg = ":" + setter.getNick() + "!" + setter.getUsername() + "@" +
					  setter.getHostname() + " MODE " + this->name + mode + "\r\n";
	broadcast(msg, -1);
}

void Channel::modeTopic(const Client &setter, bool set) {
	std::string mode;

	if (!this->isOperator(setter.getNick())) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(rpl_482, setter);
		return;
	}
	if (!set) {
		this->topicFlag = false;
		mode = " -t ";
	} else {
		this->topicFlag = true;
		mode = " +t ";
	}
	std::string msg = ":" + setter.getNick() + "!" + setter.getUsername() + "@" +
					  setter.getHostname() + " MODE " + this->name + mode + "\r\n";
	broadcast(msg, -1);
}

void Channel::modeLimit(const Client &setter, std::string const &newLimit, bool set) {
	std::string mode;

	if (!this->isOperator(setter.getNick())) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(rpl_482, setter);
		return;
	}
	if (!set) {
		this->limitFlag = false;
		mode = " -l ";
	} else {
		this->limitFlag = true;
		this->limit = static_cast<int>(std::strtol(newLimit.c_str(), NULL, 10));
		mode = " +l ";
	}
	std::string msg = ":" + setter.getNick() + "!" + setter.getUsername() + "@" +
					  setter.getHostname() + " MODE " + this->name + mode + newLimit + "\r\n";
	this->broadcast(msg, -1);
}

void Channel::modeKey(Client const &setter, std::string const &newKey, bool set) {
	std::string mode;

	if (!this->isOperator(setter.getNick())) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(rpl_482, setter);
		return;
	} else if (!set) {
		this->keyFlag = false;
		this->password.clear();
		mode = " -k ";
	} else {
		this->keyFlag = true;
		this->password = newKey;
		mode = " +k ";
	}
	std::string msg = ":" + setter.getNick() + "!" + setter.getUsername() + "@" +
					  setter.getHostname() + " MODE " + this->name + mode + newKey + "\r\n";
	this->broadcast(msg, -1);
}

void Channel::cmdKick(const Client &kicker, const Client &kicked, std::string &reason) {
	// not an operator
	if (!this->isOperator(kicker.getNick())) {
		ERR_CHANOPRIVSNEEDED(kicker.getNick(), this->name);
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(kicker.getNick(), this->name);
		sendToClient(rpl_482, kicker);
		return;
	}
	if (!this->isMember(kicked)) {
		std::string rpl_442 = ERR_NOTONCHANNEL(kicker.getNick(), this->name);
		sendToClient(rpl_442, kicker);
		return;
	}
	if (reason.empty()) {
		reason = "Not specified";
	}
	std::string message = ":" + kicker.getNick() + "!" + kicker.getUsername() + "@" + kicker.getHostname() + " KICK " + this->name + " " + kicked.getNick() + " " + reason + "\r\n";
	this->broadcast(message, -1);
	this->removeMember(kicked);
}

void Channel::cmdTopic(const Client &setter, std::vector<std::string> const &params) {
	if (!this->isMember(setter)) {
		std::string rpl_442 = ERR_NOTONCHANNEL(setter.getNick(), this->name);
		sendToClient(rpl_442, setter);
		return;
	}
	if (params.size() == 1) {
		std::string topicStr, rpl;
		if (this->topic.empty())
			rpl = RPL_NOTOPIC(setter.getNick(), this->name);
		else {
			topicStr = this->topic;
			rpl = RPL_TOPIC(setter.getNick(), this->name, topicStr);
		}
		sendToClient(rpl, setter);
		return;
	}
	if (!this->isOperator(setter.getNick()) && this->topicFlag) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(rpl_482, setter);
		return;
	}
	if (params[1].empty()) {
		this->topic.clear();
	} else
		this->topic = params[1];
	std::string message = ":" + setter.getNick() + "!" + setter.getUsername() + "@" + setter.getHostname() + " TOPIC " + this->name + " :" + this->topic + "\r\n";
	this->broadcast(message, -1);
}

std::map<std::string, std::string> parseModes(std::vector<std::string> params) {
	std::map<std::string, std::string> modes;
	bool							   add = true;
	std::string						   arg;
	int								   k = 0;
	std::string						   mode;

	for (size_t i = 1 + k; i + k < params.size(); ++i) {
		i += k;
		for (size_t j = 0; j < params[i].size(); ++j) {
			if (params[i][j] == '+') {
				add = true;
				continue;
			} else if (params[i][j] == '-') {
				add = false;
				continue;
			}
			if (params[i][j] != 't' && params[i][j] != 'i') {
				if (i + k < params.size() - 1) {
					arg = params[i + k + 1];
					k++;
				}
			}
			if (add)
				mode = "+" + std::string(1, params[i][j]);
			else
				mode = "-" + std::string(1, params[i][j]);
			modes[mode] = arg;
		}
	}
	return modes;
}

void Channel::cmdMode(const Client &setter, std::vector<std::string> const &params) {
	std::string msg;

	if (params.size() == 1) {
		std::string mode = this->getChannelModes();
		std::string rpl_324 = RPL_CHANNELMODEIS(setter.getNick(), this->name, mode);
		sendToClient(rpl_324, setter);
		std::string rpl_329 = RPL_CREATIONTIME(setter.getNick(), this->name, this->startTime());
		sendToClient(rpl_329, setter);
		return;
	}
	if (!this->isOperator(setter.getNick())) {
		msg = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(msg, setter);
		return;
	}
	std::map<std::string, std::string> modes = parseModes(params);
	for (std::map<std::string, std::string>::iterator it = modes.begin(); it != modes.end(); it++) {
		std::string mode = it->first;
		if (members.find(it->second) == members.end()) {
			std::string rpl_401 = ERR_NOSUCHNICK(setter.getNick(), it->second);
			sendToClient(rpl_401, setter);
			continue;
		}
		switch (mode[1]) {
		case 'o':
			this->modeOP(setter, members[it->second], mode[0] == '+');
			break;
		case 'l':
			this->modeLimit(setter, it->second, mode[0] == '+');
			break;
		case 't':
			this->modeTopic(setter, mode[0] == '+');
			break;
		case 'k':
			this->modeKey(setter, it->second, mode[0] == '+');
			break;
		case 'i':
			this->modeInvite(setter, mode[0] == '+');
			break;
		default: // TODO: RPL
			msg = ERR_UNKNOWNMODE(setter.getNick(), this->getName(), mode);
		}
		this->broadcast(msg, -1);
	}
}

void Channel::modeOP(const Client &setter, const Client &op, bool set) {
	std::string mode;

	if (!this->isOperator(setter.getNick())) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(setter.getNick(), this->name);
		sendToClient(rpl_482, setter);
		return;
	}
	if (set) {
		if (!this->addOperator(op.getNick())) {
			return;
		}
		mode = " +o ";
	} else {
		if (!this->removeOperator(op.getNick())) {
			return;
		}
		mode = " -o ";
	}
	std::string msg = ":" + setter.getNick() + "!" + setter.getNick() + "@" +
					  setter.getHostname() + " MODE " + this->name + mode + op.getNick() + "\r\n";
	this->broadcast(msg, -1);
}

void Channel::cmdInvite(const Client &inviter, const Client &invited) {
	if (!this->isOperator(inviter.getNick()) && this->inviteFlag) {
		std::string rpl_482 = ERR_CHANOPRIVSNEEDED(inviter.getNick(), this->name);
		sendToClient(rpl_482, inviter);
		return;
	}
	if (this->isMember(invited)) {
		std::string rpl_443 = ERR_USERONCHANNEL(inviter.getNick(), this->name, invited.getNick());
		sendToClient(rpl_443, inviter);
		return;
	}
	if (!this->isMember(inviter)) {
		std::string rpl_442 = ERR_NOTONCHANNEL(inviter.getNick(), this->name);
		sendToClient(rpl_442, inviter);
		return;
	}
	std::string rpl_341 = RPL_INVITING(inviter.getNick(), this->name, invited.getNick());
	sendToClient(rpl_341, inviter);
	std::string invite = ":" + inviter.getNick() + "!" + inviter.getUsername() + "@" +
						 inviter.getHostname() + " INVITE " + invited.getNick() + " " + this->name + "\r\n";
	sendToClient(invite, inviter);
	this->addInvited(invited.getNick());
}

void Channel::updateMap(std::string const &oldNick, std::string const &newNick, Client const &client) {
	if (this->isOperator(oldNick)) {
		this->removeOperator(oldNick);
		this->addOperator(newNick);
	}
	if (this->isInvited(oldNick)) {
		this->removeInvited(oldNick);
		this->addInvited(newNick);
	}
	if (this->isMember(this->members[oldNick])) {
		this->members.erase(this->members.find(oldNick));
		this->members[newNick] = client;
	}
}

bool Channel::getBotState() const {
	return this->activeBot;
}
void Channel::toogleBotState() {
	this->activeBot = !this->activeBot;
}

void Channel::pangramBot(std::string message) const {
	if (!this->activeBot)
		return;

	char		control = 'a';
	bool		isPangram = false;
	std::string lowercase;

	for (std::string::iterator i = message.begin(); i != message.end(); i++)
		lowercase += std::tolower(*i);
	for (std::string::iterator i = lowercase.begin(); i != lowercase.end(); i++) {
		if (*i == control) {
			control++;
			i = lowercase.begin() - 1;
		}
		if (control > 'z') {
			isPangram = true;
			break;
		}
	}
	if (!isPangram)
		return;
	std::string is_pangram = ":🤖!🤖@0.0.0.0 NOTICE " + this->name + " Pangram detected! -> " + message + "\r\n";
	this->broadcast(is_pangram, -1);
}
