#include "../includes/All.hpp"
#include "../includes/Server.hpp"

// SERVER CONSTRUCTION | DESTRUCTION

Server::Server(int port, const std::string &password)
	: serverSocket(-1), serverAddr(), client_addr(), port(port), pass(password) {
	memset(&this->serverAddr, 0, sizeof(this->serverAddr));
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.s_addr = INADDR_ANY;
	this->serverAddr.sin_port = htons(this->port);
	this->startTime = getCurrentDateTime();
	this->hostname = inet_ntoa(this->serverAddr.sin_addr);
}

Server::~Server() {
	for (std::vector<Client *>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
		delete (*it);
	if (this->serverSocket != -1)
		close(this->serverSocket);
	std::cout << "FT_IRC terminated" << std::endl;
}

bool Server::start() {
	if ((this->serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return false;
	int yes = 1;
	if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		return false;
	if (bind(this->serverSocket, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) == -1)
		return false;
	if (listen(this->serverSocket, 10) == -1)
		return false;
	printServerInfo(this);
	return true;
}

bool Server::isClient(const std::string &nick) const {
	for (std::vector<Client *>::const_iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
		if ((*it)->getNick() == nick)
			return true;
	}
	return false;
}

Client *Server::getClient(std::string const &nick) const {
	for (std::vector<Client *>::const_iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
		if ((*it)->getNick() == nick)
			return *it;
	}
	return NULL;
}

std::string Server::getHostname() const {
	return this->hostname;
}

std::string Server::getPassword() const {
	return this->pass;
}

std::string Server::getStartTime() const {
	return this->startTime;
}

int Server::getPort() const {
	return this->port;
}

// SERVER CONNECTION MONITORING

void Server::monitoringRemoveClient(fd_set &master_set, int &client_fd) {
	close(client_fd);
	FD_CLR(client_fd, &master_set);
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if ((*it)->getSocket() == client_fd) {
			(*it)->cmdQuit(clients, "crashed!");
			delete *it;
			clients.erase(it);
			return;
		}
	}
}

void Server::monitoringExistingClient(fd_set &master_set, int &client_fd, char buffer[1024]) {
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if ((*it)->getSocket() == client_fd) {
			(*it)->buffer += buffer;
			while (true) {
				(*it)->parseBuffer();
				if (!(*it)->messageReady)
					break;
				(*it)->messageReady = false;
				handleMessage((*it)->message, it);
				if(!(*it)->getConnected()){
					close((*it)->getSocket());
					FD_CLR((*it)->getSocket(), &master_set);
					delete *it;
					clients.erase(it);
					break;
				}
			}
			break;
		}
	}
}

void Server::monitoringAddClient(fd_set &master_set, int &max_sd) {
	int		  client_socket;
	socklen_t addr_len = sizeof(client_addr);
	if ((client_socket = accept(serverSocket, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
		std::cerr << ACCEPT_ERR;
		return;
	}
	FD_SET(client_socket, &master_set);
	if (client_socket > max_sd)
		max_sd = client_socket;
	Client new_client(client_socket, client_addr, this->hostname);
	clients.push_back(new Client(new_client));
}

void Server::monitorConnections() {
	fd_set master_set, read_set;
	FD_ZERO(&master_set);
	FD_SET(serverSocket, &master_set);
	int max_sd = serverSocket;
	while (stop_requested == 0) {
		read_set = master_set;
		if (select(max_sd + 1, &read_set, NULL, NULL, NULL) == -1) {
			std::cerr << SELECT_ERR;
			break;
		}
		for (int i = 0; i <= max_sd; ++i) {
			if (!FD_ISSET(i, &read_set))
				continue;
			if (i == serverSocket) {
				this->monitoringAddClient(master_set, max_sd);
				continue;
			}
			char buffer[1024] = {0};
			int	 bytes_read = recv(i, buffer, sizeof(buffer) - 1, 0);
			if (bytes_read <= 0)
				this->monitoringRemoveClient(master_set, i);
			else {
				this->monitoringExistingClient(master_set, i, buffer);
			}
		}
	}
}

// SERVER MESSAGE HANDLING

void Server::initialMessageTreatment(Client *client, std::string const &buffer) {
	std::string::size_type pos = 0;
	while (pos < buffer.size()) {
		if (buffer.find("PASS", pos) != std::string::npos) {
			client->setPass(buffer, this->pass);
			pos = buffer.find("PASS", pos) + 4;
		}
		if (buffer.find("NICK", pos) != std::string::npos) {
			client->setNick(buffer, this->clients);
			pos = buffer.find("NICK", pos) + 4;
		}
		if (buffer.find("USER", pos) != std::string::npos) {
			client->setUser(buffer);
			pos = buffer.find("USER", pos) + 4;
		} else
			break;
		if (!client->getNick().empty() && !client->getPass().empty())
			client->setConnectionReady(true);
	}
}

void Server::handleMessage(std::string buffer, std::vector<Client *>::iterator client_it) {
	buffer = buffer.substr(0, buffer.find("\r\n"));
	if (!(*client_it)->getModSent())
		this->initialMessageTreatment(*client_it, buffer);
	if ((*client_it)->getconnectionReady() && !(*client_it)->getModSent())
		if (this->messageOfTheDay(*(*client_it))) return;

	struct message input = parseInput(buffer);

	if (input.cmd == "NICK") {
		if (!(*client_it)->getValidNick()) (*client_it)->setNick(buffer, this->clients);
		else (*client_it)->cmdNick(input.params[0], this->clients);
	}
	else if (input.cmd == "PING")
		cmdPing(input.params, *(*client_it));
	else if (input.cmd == "KICK")
		cmdKick(input, *(*client_it));
	else if (input.cmd == "MODE")
		cmdMode(input.params, *(*client_it));
	else if (input.cmd == "INVITE")
		cmdInvite(input.params, *(*client_it));
	else if (input.cmd == "JOIN")
		cmdJoin(input, *(*client_it));
	else if (input.cmd == "PRIVMSG")
		cmdPrivmsg(input.params, *(*client_it));
	else if (input.cmd == "NOTICE")
		cmdNotice(input.params, *(*client_it));
	else if (input.cmd == "WHO")
		cmdWho(input.params, *(*client_it));
	else if (input.cmd == "PART")
		cmdPart(input, *(*client_it));
	else if (input.cmd == "BOT" || input.cmd == "bot")
		cmdBot(input.params, *(*client_it));
	else if (input.cmd == "FTIRC" || input.cmd == "ftirc")
		debugServer(*this, input.params);
	else if (input.cmd == "TOPIC")
		cmdTopic(input.params, *(*client_it));
	else if (input.cmd == "QUIT")
		cmdQuit(input, *(*client_it));
}

bool	Server::messageOfTheDay(Client &client) {
	if (!client.getValidPass())	return false;
	if(!isValidNickName(client.getNick())){
		std::string err_432 = ERR_ERRONEUSNICKNAME(client.getNick(), client.getNick());
		send(client.getSocket(), err_432.c_str(), err_432.size(), 0);
	}
	if (client.checkDuplicatedNick(client.getNick(), this->clients)) {
		std::string err_433 = ERR_NICKNAMEINUSE(client.getNick(), client.getNick());
		send(client.getSocket(), err_433.c_str(), err_433.size(), 0);
		return false;
	}
	std::string rpl_001 = RPL_WELCOME(client.getNick());
	std::string rpl_002 = RPL_YOURHOST(client.getNick());
	std::string rpl_003 = RPL_CREATED(client.getNick(), this->startTime);
	std::string rpl_004 = RPL_MYINFO(client.getNick());
	std::string rpl_375 = RPL_MOTDSTART(client.getNick());
	std::string rpl_372 = RPL_MOTD(client.getNick());
	std::string rpl_376 = RPL_ENDOFMOTD(client.getNick());

	send(client.getSocket(), rpl_001.c_str(), rpl_001.size(), 0);
	send(client.getSocket(), rpl_002.c_str(), rpl_002.size(), 0);
	send(client.getSocket(), rpl_003.c_str(), rpl_003.size(), 0);
	send(client.getSocket(), rpl_004.c_str(), rpl_004.size(), 0);
	send(client.getSocket(), rpl_375.c_str(), rpl_375.size(), 0);
	send(client.getSocket(), rpl_372.c_str(), rpl_372.size(), 0);
	send(client.getSocket(), rpl_376.c_str(), rpl_376.size(), 0);
	client.setConnected(true);
	client.setModSent(true);
	return true;
}

// COMMANDS

void Server::sendMsg(Client const &client, std::string const &target, std::string const &msg) {
	std::string privmsgMsg = ":" + client.getNick() + " PRIVMSG " + target + " " + msg + "\r\n";
	if (target.find_first_of("#&") != std::string::npos) {
		if (this->channels.find(target) == this->channels.end()) {
			std::string err_403 = ERR_NOSUCHCHANNEL(client.getNick(), target);
			sendToClient(err_403, client);
			return;
		} else {
			this->channels[target].broadcast(privmsgMsg, client.getSocket());
			this->channels[target].pangramBot(msg);
		}
	} else {
		if (this->isClient(target))
			sendToClient(privmsgMsg, *this->getClient(target));
		else {
			std::string rpl_401 = ERR_NOSUCHNICK(client.getNick(), target);
			sendToClient(rpl_401, client);
		}
	}
}

void Server::cmdPrivmsg(std::vector<std::string> const &params, Client const &client) {
	if (params.empty()) {
		std::string err_411 = ERR_NORECIPIENT(client.getNick(), "PRIVMSG");
		sendToClient(err_411, client);
		return;
	}
	if (params.size() < 2) {
		std::string err_412 = ERR_NOTEXTTOSEND(client.getNick());
		sendToClient(err_412, client);
		return;
	}
	std::vector<std::string> targets = split(params[0], ',');

	for (std::vector<std::string>::const_iterator it = targets.begin(); it != targets.end(); ++it) {
		this->sendMsg(client, *it, params[1]);
	}
}

void Server::cmdNotice(std::vector<std::string> const &params, Client const &client) {
	if (params.empty())
		return;
	if (params.size() < 2)
		return;

	std::vector<std::string> targets = split(params[0], ',');

	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it) {
		std::string privmsgMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + client.getHostname() + " NOTICE " + *it + " " + params[1] + "\r\n";
		if (it->find_first_of("#&") != std::string::npos) {
			if (this->channels.find(*it) == this->channels.end())
				continue;
			else {
				this->channels[*it].broadcast(privmsgMsg, client.getSocket());
				this->channels[*it].pangramBot(params[0]);
			}
		} else {
			if (this->isClient(*it))
				sendToClient(privmsgMsg, *this->getClient(*it));
			else
				continue;
		}
	}
}

std::string Server::createUserList(std::map<std::string, Client> const &members, std::string const &channelName) {
	std::string userList;
	for (std::map<std::string, Client>::const_iterator members_it = members.begin(); members_it != members.end(); ++members_it) {
		if (channels[channelName].isOperator(members_it->first))
			userList += "@" + members_it->first + " ";
		else
			userList += members_it->first + " ";
	}
	return userList;
}

void Server::sendJoinMsg(Client &client, std::string const &channelName) {
	std::string joinMsg = ":" + client.getNick() + " JOIN " + channelName + "\r\n";
	channels[channelName].broadcast(joinMsg, -1);

	std::string userList = createUserList(channels[channelName].getMembers(), channelName);

	if (!channels[channelName].getTopic().empty()) {
		std::string rpl_332 = RPL_TOPIC(client.getNick(), channelName, channels[channelName].getTopic());
		sendToClient(rpl_332, client);
		std::string rpl_333 = RPL_TOPICWHOTIME(client.getNick(), channelName);
		sendToClient(rpl_333, client);
	}
	std::string rpl_353 = RPL_NAMREPLY(client.getNick(), channels[channelName].getName(), userList);
	sendToClient(rpl_353, client);
	std::string rpl_366 = RPL_ENDOFNAMES(client.getNick(), channels[channelName].getName());
	sendToClient(rpl_366, client);
	client.channels.push_back(&channels[channelName]);
}

void Server::joinChannel(Client &client, std::string const &channelName, std::string const &key) {
	if (!isValidChannelName(channelName)) {
		std::string err_476 = ERR_BADCHANMASK(client.getNick(), channelName);
		sendToClient(err_476, client);
		return;
	}
	if (channels.find(channelName) == channels.end()) {
		channels[channelName] = Channel(channelName);
		channels[channelName].addOperator(client.getNick());
	}
	if (channels[channelName].addMember(client, key)) {
		sendJoinMsg(client, channelName);
	}
}

void Server::cmdJoin(struct message &input, Client &client) {
	if (input.params.empty()) {
		std::string err_461 = ERR_NEEDMOREPARAMS(client.getNick(), "JOIN");
		sendToClient(err_461, client);
		return;
	}
	if (input.params.size() < 2)
		input.params.push_back("");

	std::string						   channelNames = input.params[0];
	std::string						   keys = input.params[1];
	std::map<std::string, std::string> channel_key;

	channel_key = create_channel_map(channelNames, keys);
	for (std::map<std::string, std::string>::iterator channels_it = channel_key.begin(); channels_it != channel_key.end(); ++channels_it) {
		joinChannel(client, channels_it->first, channels_it->second);
	}
}

void Server::partChannel(Client &client, std::string const &channelName, std::string const &reason) {
	if (channels.find(channelName) == channels.end()) {
		std::string err_403 = ERR_NOSUCHCHANNEL(client.getNick(), channelName);
		sendToClient(err_403, client);
		return;
	}
	if (channels[channelName].removeMember(client, reason)) {
		std::string partMessage = ":" + client.getNick() + " PART " + channelName + " " + reason + "\r\n";
		channels[channelName].broadcast(partMessage, -1);
		for (std::vector<Channel *>::iterator channel_it = client.channels.begin(); channel_it != client.channels.end(); channel_it++) {
			if ((*channel_it)->getName() == channelName) {
				client.channels.erase(channel_it);
				break;
			}
		}
	}
	if (channels[channelName].getMembers().empty())
		channels.erase(channelName);
}

void Server::cmdPart(struct message &input, Client &client) {

	if (input.params.empty()) {
		std::string err_461 = ERR_NEEDMOREPARAMS(client.getNick(), "PART");
		sendToClient(err_461, client);
		return;
	}
	if (input.params.size() < 2)
		input.params.push_back("Leaving");

	std::string						   channelNames = input.params[0];
	std::string						   reason = input.params[1];
	std::map<std::string, std::string> channel_key;

	channel_key = create_channel_map(channelNames, reason);
	for (std::map<std::string, std::string>::iterator channels_it = channel_key.begin(); channels_it != channel_key.end(); ++channels_it) {
		partChannel(client, channels_it->first, channels_it->second);
	}
}

void Server::cmdWho(std::vector<std::string> const &params, Client const &client) {
	if (params[0].find_first_of("#&") == std::string::npos) {
		return;
	}
	std::string					  user;
	std::map<std::string, Client> members = channels[params[0]].getMembers();
	for (std::map<std::string, Client>::const_iterator members_it = members.begin(); members_it != members.end(); ++members_it) {
		std::string flag = " H";
		if (channels[params[0]].isOperator(members_it->first))
			flag += "@";
		user = members_it->first;
		std::string rpl_352 = RPL_WHOREPLY(client.getNick(), params[0], members_it->second.getUsername(), user, members_it->second.getRealname(), flag);
		sendToClient(rpl_352, client);
	}
	std::string rpl_315 = RPL_ENDOFWHO(client.getNick(), params[0]);
	sendToClient(rpl_315, client);
}

void Server::cmdTopic(std::vector<std::string> const &params, Client const &setter) {
	if (params.empty()) {
		std::string rpl_461 = ERR_NEEDMOREPARAMS(setter.getNick(), "TOPIC");
		sendToClient(rpl_461, setter);
		return;
	}
	if (this->channels.find(params[0]) == this->channels.end()) {
		std::string rpl_403 = ERR_NOSUCHCHANNEL(setter.getNick(), params[0]);
		sendToClient(rpl_403, setter);
		return;
	} else {
		this->channels[params[0]].cmdTopic(setter, params);
	}
}

void Server::cmdQuit(struct message &input, Client &client) {

	if (input.params.empty()) {
		input.params.push_back("");
	}
	client.cmdQuit(this->clients, input.params[0]);
	for (std::vector<Channel *>::const_iterator channel_it = client.channels.begin(); channel_it != client.channels.end(); ++channel_it) {
		(*channel_it)->removeMember(client);
		if ((*channel_it)->getMembers().empty()) {
			this->channels.erase((*channel_it)->getName());
		}
	}
	client.setConnected(false);
}

void Server::cmdInvite(std::vector<std::string> const &params, Client const &inviter) {
	if (params.size() < 2) {
		std::string rpl_461 = ERR_NEEDMOREPARAMS(inviter.getNick(), "INVITE");
		sendToClient(rpl_461, inviter);
		return;
	}
	if (channels.find(params[1]) == channels.end()) {
		std::string rpl_403 = ERR_NOSUCHCHANNEL(inviter.getNick(), params[1]);
		sendToClient(rpl_403, inviter);
		return;
	}
	if (!this->isClient(params[0])) {
		std::string rpl_401 = ERR_NOSUCHNICK(inviter.getNick(), params[0]);
		sendToClient(rpl_401, inviter);
		return;
	}
	channels[params[1]].cmdInvite(inviter, *this->getClient(params[0]));
}

void Server::cmdMode(std::vector<std::string> const &params, Client const &setter) {
	if (params.empty())
		return;
	if (channels.find(params[0]) == channels.end()) {
		std::string rpl_403 = ERR_NOSUCHCHANNEL(setter.getNick(), params[0]);
		sendToClient(rpl_403, setter);
		return;
	}
	if (params[0].find_first_of("#&") != std::string::npos) {
		channels[params[0]].cmdMode(setter, params);
	} else if (!isClient(params[0])) {
		std::string rpl_401 = ERR_NOSUCHNICK(setter.getNick(), params[0]);
		sendToClient(rpl_401, setter);
		return;
	}
}

void Server::cmdKick(struct message &input, Client const &kicker) {
	if (input.params.size() < 2) {
		std::string rpl_461 = ERR_NEEDMOREPARAMS(kicker.getNick(), input.cmd);
		sendToClient(rpl_461, kicker);
		return;
	}
	if (input.params.size() < 3)
		input.params.push_back("Yaa gone!!");

	if (channels.find(input.params[0]) == channels.end()) {
		std::string rpl_403 = ERR_NOSUCHCHANNEL(kicker.getNick(), input.params[0]);
		sendToClient(rpl_403, kicker);
		return;
	}
	if (!this->isClient(input.params[1])) {
		std::string rpl_401 = ERR_NOSUCHNICK(kicker.getNick(), input.params[1]);
		sendToClient(rpl_401, kicker);
		return;
	}
	channels[input.params[0]].cmdKick(kicker, *this->getClient(input.params[1]), input.params[2]);
}

void Server::cmdPing(std::vector<std::string> const &params, Client const &client) {
	if (params.empty())
		return;
	std::string pong_response = ":" + this->hostname + " PONG :" + params[0] + "\r\n";
	sendToClient(pong_response, client);
}

void Server::cmdBot(std::vector<std::string> const &params, const Client &client) {
	if (params.empty()) {
		std::string err_461 = ERR_NEEDMOREPARAMS(client.getNick(), "BOT");
		sendToClient(err_461, client);
		return;
	}
	if (channels.find(params[0]) == channels.end()) {
		std::string err_403= ERR_NOSUCHCHANNEL(client.getNick(), params[0]);
		sendToClient(err_403, client);
		return;
	}
	if (!channels[params[0]].isMember(client)) {
		std::string err_442 = ERR_NOTONCHANNEL(client.getNick(), "BOT");
		sendToClient(err_442, client);
		return;
	}
	channels[params[0]].toogleBotState();
	std::string bot_yes = ":🤖 PRIVMSG " + channels[params[0]].getName() + " :BOT TURNED ON!\r\n";
	std::string bot_no = ":🤖 PRIVMSG " + channels[params[0]].getName() + " :BOT TURNED OFF!\r\n";
	if (!channels[params[0]].getBotState())
		channels[params[0]].broadcast(bot_no, -1);
	else
		channels[params[0]].broadcast(bot_yes, -1);
}

std::string Server::getClientsOn() const {
	if (this->clients.empty())
		return "There are no clients online.\n";
	std::string response;
	response += "Clients online:\n";
	for (size_t i = 0; i < this->clients.size(); ++i) {
		response += this->clients[i]->getNick();
		if (i != this->clients.size() - 1)
			response += ", ";
	}
	return response;
}

std::string Server::getChannelsOn() const {
	if (this->channels.empty())
		return "There are no active channels.";
	std::string response;
	response += "Active channels:";
	for (std::map<std::string, Channel>::const_iterator i = this->channels.begin(); i != this->channels.end(); ++i) {
		response += i->first;
		if (i != --this->channels.end())
			response += ", ";
	}
	return response;
}

void Server::printChannelMembers() const {
	if (this->channels.empty()) {
		std::cout << "There are no active channels." << std::endl;
		return;
	}
	std::string response;
	response += "Active members:\r\n";
	for (std::map<std::string, Channel>::const_iterator i = channels.begin(); i != channels.end(); ++i) {
		response += i->first + ": ";
		std::map<std::string, Client> members = i->second.getMembers();
		for (std::map<std::string, Client>::const_iterator j = members.begin(); j != members.end(); ++j) {
			response += j->second.getNick();
			if (j != --members.end()) {
				response += ", ";
			}
		}
		response += "\r\n";
	}
	std::cout << response << std::endl;
}
