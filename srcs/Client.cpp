#include "../includes/Client.hpp"
#include "../includes/All.hpp"

Client::Client(int client_socket, struct sockaddr_in client_address, std::string const &hostname)
	: socket(client_socket),
	  address(client_address),
	  hostname(hostname),
	  validNick(false),
	  validPass(false),
	  connectionReady(false),
	  connected(true),
	  modSent(false),
	  channels(),
	  messageReady(false) {
}

Client::Client()
	: socket(),
	  address(),
	  hostname(),
	  validNick(false),
	  validPass(false),
	  connectionReady(false),
	  connected(true),
	  modSent(false),
	  channels(),
	  messageReady(false) {
}

Client::Client(const Client &other) {
	*this = other;
}

bool Client::checkDuplicatedNick(std::string nick, std::vector<Client *> &clients) {
	for (std::size_t i = 0; i < clients.size(); ++i) {
		if (clients[i]->getSocket() == this->getSocket()) {
			continue;
		}
		if (clients[i]->nick == nick) {
			if (!clients[i]->modSent)
				continue;
			return true;
		}
	}
	return false;
}

void Client::setConnectionReady(bool connectionReady) {this->connectionReady = connectionReady;} 

void Client::setConnected(bool connected) {this->connected = connected;} 

void Client::setModSent(bool modSent) {this->modSent = modSent;} 


Client	&Client::operator=(const Client &other) {
	if (this == &other) {
		return *this;
	}
	this->socket = other.socket;
	this->address = other.address;
	this->pass = other.pass;
	this->nick = other.nick;
	this->realname = other.realname;
	this->hostname = other.hostname;
	this->validNick = other.validNick;
	this->validPass = other.validPass;
	this->connectionReady = other.connectionReady;
	this->connected = other.connected;
	this->modSent = other.modSent;
	this->buffer = other.buffer;
	this->message = other.message;
	this->messageReady = other.messageReady;
	this->username = other.username;
	this->channels = other.channels;
	return *this;
}

Client::~Client() {
}

const std::string& Client::getHostname() const {return this->hostname;}

bool Client::getValidPass() const {return this->validPass;}

bool Client::getValidNick() const {return this->validNick;}

bool Client::getconnectionReady() const {return this->connectionReady;}

bool Client::getConnected() const {return this->connected;}

bool Client::getModSent() const {return this->modSent;}

int	Client::getSocket() const {return socket;}

const std::string& Client::getRealname() const {return this->realname;}

const std::string& Client::getUsername() const {return this->username;}

const std::string& Client::getNick() const {return this->nick;}

const std::string& Client::getPass() const {return this->pass;}

std::string Client::getAddress() const {
	char ip[INET_ADDRSTRLEN];
	strcpy(ip, inet_ntoa(address.sin_addr)); // inet_ntoa converts binary IP address to text
	std::stringstream ss;
	ss << ip << ":" << ntohs(address.sin_port); // ntohs converts port number to host byte order
	return ss.str();
}

void Client::setPass(std::string buffer, std::string server_password) {
	std::string		   pass;
	std::istringstream stream(buffer);
	std::string		   line;

	while (std::getline(stream, line)) {
		std::istringstream lineStream(line);
		std::string		   key;
		lineStream >> key;
		if (key == "PASS") {
			lineStream >> pass;
		}
	}
	if (pass == server_password) {
		this->validPass = true;
	} else {
		std::string err_464 = ERR_PASSWDMISMATCH(this->getNick());
		send(this->getSocket(), err_464.c_str(), err_464.size(), 0);
	}
	this->pass = pass;
}

void Client::setUser(std::string buffer) {
	std::string		   user;
	std::string		   tmp;
	std::istringstream stream(buffer);
	std::string		   line;

	while (std::getline(stream, line)) {
		std::istringstream lineStream(line);
		std::string		   key;
		lineStream >> key;
		if (key == "USER") {
			std::string skip;
			lineStream >> tmp;	// 1 wrd
			lineStream >> skip; // 2 wrd
			lineStream >> skip; // 3 wrd
			std::getline(lineStream, user, ':');
			std::getline(lineStream, user);
			break;
		}
	}
	this->username = tmp;
	this->realname = user;
}

void Client::setNick(std::string buffer, std::vector<Client *> clients) {
	std::string		   nick;
	std::istringstream stream(buffer);
	std::string		   line;

	while (std::getline(stream, line)) {
		if (line.compare(0, 4, "NICK") == 0) {
			std::istringstream lineStream(line);
			std::string		   key;
			lineStream >> key;
			if (key == "NICK") {
				lineStream >> nick;
				break;
			}
		}
	}
	if (nick.empty()) {
		return;
	}

	this->nick = nick;
	if (!checkDuplicatedNick(nick, clients) && isValidNickName(nick)) {
		this->validNick = true;
		return;
	}
}

void Client::cmdNick(const std::string &newNick, std::vector<Client *> &clients) {
	if (newNick.empty()) {
		std::string err_431 = ERR_NONICKNAMEGIVEN(this->nick);
		sendToClient(err_431, *this);
	}
	if (!isValidNickName(newNick)) {
		std::string err_432 = ERR_ERRONEUSNICKNAME(newNick, this->getNick());
		sendToClient(err_432, *this);
		return;
	}
	for (std::size_t i = 0; i < clients.size(); ++i) {
		if (clients[i]->getSocket() == this->getSocket()) {
			continue;
		}
		if (clients[i]->nick == newNick && this->validNick) {
			if (!clients[i]->modSent)
				continue;
			std::string err_433 = ERR_NICKNAMEINUSE(newNick, this->getNick());
			sendToClient(err_433, *this);
			return;
		}
	}
	std::string oldNick = this->nick;
	this->nick = newNick;
	this->validNick = true;
	if (!this->modSent) {
		return;
	}
	std::string nickChangeMsg = ":" + oldNick + "!" + this->username + "@" +
								this->hostname + " NICK " + newNick + "\r\n";

	for (std::size_t k = 0; k < clients.size(); ++k) {
		sendToClient(nickChangeMsg, *clients[k]);
	}
	if (this->channels.empty()) {
		return;
	}
	for (std::vector<Channel *>::iterator channel_it = channels.begin();
		 channel_it != channels.end(); ++channel_it)
		(*channel_it)->updateMap(oldNick, newNick, *this);
}

void Client::cmdQuit(std::vector<Client *> &clients, std::string const &reason) {
	std::string quitReason = "Quit: ";
	quitReason += reason;
	std::string quittingMessage = ":" + this->nick + " QUIT " + quitReason + "\r\n";
	for (std::size_t i = 0; i < clients.size(); ++i) {
		if (clients[i]->connected) {
			sendToClient(quittingMessage, *clients[i]);
		}
	}
}

void Client::printInfo() const {
	std::cout << "Nickname  : " << this->nick << std::endl;
	std::cout << "Realname  : " << this->realname << std::endl;
	std::cout << "Socket    : " << this->socket << std::endl;
	std::cout << "Pass used : " << this->pass << std::endl;
	std::cout << "Hostname  : " << this->hostname << std::endl;
	std::cout << "Connected : " << this->connected << std::endl;
	std::cout << "ValidNick : " << this->validNick << std::endl;
	std::cout << "ValidPass : " << this->validPass << std::endl;
	std::cout << "Connection ready : " << this->connectionReady << std::endl;
	std::cout << "Mod sent : " << this->modSent << std::endl;
	for (std::vector<Channel *>::const_iterator it = this->channels.begin(); it != this->channels.end(); ++it) {
		std::cout << "Channel: " << (*it)->getName() << " at " << *it << std::endl;
	}
}

bool Client::parseBuffer() {
	std::size_t pos = this->buffer.find("\r\n");
	if (pos != std::string::npos) {
		this->message = this->buffer.substr(0, pos + 2); // +2 to include "\r\n"
		this->buffer.erase(0, pos + 2);					 // +2 "\r\n"
		this->messageReady = true;
		return true;
	}
	return false;
}
