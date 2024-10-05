#include "../includes/All.hpp"

// loose functions that do not beling to any class

std::string	getCurrentDateTime() {
	std::time_t now = std::time(0);
	std::tm	*localTime = std::localtime(&now);
	char		buffer[100];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
	return std::string(buffer);
}

std::vector<std::string>	split(const std::string &str, char delimiter) {
	std::vector<std::string> tokens;
	std::string				 token;
	std::istringstream		 tokenStream(str);

	while (std::getline(tokenStream, token, delimiter))
		tokens.push_back(token);
	return tokens;
}

void	printServerInfo(Server* server) {
	std::cout << ASCII_TITLE;
	std::cout << "<------------------->\n";
	std::cout << "Hostname   : " << (*server).getHostname() << std::endl;
	std::cout << "Port       : " << (*server).getPort() << std::endl;
	std::cout << "Password   : " << (*server).getPassword() << std::endl;
	std::cout << "Start time : " << (*server).getStartTime() << std::endl;
	std::cout << "-------------------\n";
}

bool	isValidChannelName(const std::string &channelName) {
	if (channelName.empty())
		return false;
	if (channelName[0] != '#' && channelName[0] != '&')
		return false; // Check prefix
	if (channelName.length() > 50)
		return false;									// Check length
	for (size_t i = 1; i < channelName.length(); ++i) { // Check invalid characters
		char c = channelName[i];
		if (c == ' ' || c == ',' || c == '\0' || c == 7)
			return false; // 7 is ASCII for ^G (bell)
	}
	return true;
}

bool	isValidNickName(const std::string &nickName) {
	if (nickName.empty()) {
		return false;
	}
	if (nickName[0] == '#' || nickName[0] == '&') {
		return false;
	} // Check prefix
	if (nickName.length() > 50)
		return false;
	for (size_t i = 1; i < nickName.length(); ++i) { // Check invalid characters
		char c = nickName[i];
		if (c == ':' || c == ' ' || c == ',' || c == '\0' || c == 7)
			return false; // 7 is ASCII for ^G (bell)
	}
	return true;
}

std::map<std::string, std::string>	create_channel_map(std::string const &channels, std::string const &keys) {
	std::vector<std::string>           channel_list;
	std::vector<std::string>           key_list;
	std::map<std::string, std::string> channel_key;

	std::istringstream channel_stream(channels);
	std::string        channel;
	while (std::getline(channel_stream, channel, ',')) {
		channel.erase(std::remove_if(channel.begin(), channel.end(),
									 ::isspace), channel.end());
		channel_list.push_back(channel);
	}

	std::istringstream key_stream(keys);
	std::string        key;

	while (std::getline(key_stream, key, ',')) {
		key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
		key_list.push_back(key);
	}

	// Create the map, ensuring each channel has a corresponding key or an empty string
	for (size_t i = 0, j = 0; i < channel_list.size(); ++i) {
		if (channel_list[i].empty()) {
			continue;
		} // Skip empty channels
		if (j < key_list.size())
			channel_key[channel_list[i]] = key_list[j++];
		else
			channel_key[channel_list[i]] = "";
	}
	return channel_key;
}

struct message	parseInput(std::string const &buffer) {
	struct message        	input;
	std::string::size_type	command_end = buffer.find(' ');
	std::string           	parameters;
	std::string           	trailing;

	if (command_end != std::string::npos) {
		parameters = buffer.substr(command_end + 1);
	}

	size_t trailingStart = parameters.find(':');
	input.cmd = buffer.substr(0, command_end);
	if (trailingStart != std::string::npos) {
		trailing   = parameters.substr(trailingStart + 1);
		parameters = parameters.substr(0, trailingStart);
		input.params = split(parameters, ' ');
		input.params.push_back(trailing);
	} else {
		input.params = split(parameters, ' ');
	}
	return input;
}

void	debugServer(Server& server, std::vector<std::string> params) {
	if (params.empty()) {
		std::cout << "Debuging - run with HELP to see options\n";
		return;
	}
	if (params[0] == "help") std::cout << HELP;
	if (params[0] == "users") std::cout << server.getClientsOn() << std::endl;
	if (params[0] == "channels") std::cout << server.getChannelsOn() << std::endl;
	if (params[0] == "members") server.printChannelMembers();
}

void sendToClient(const std::string &msg, const Client &client) {
	if (!client.getModSent()) {
		return ;
	}
	send(client.getSocket(), msg.c_str(), msg.size(), 0);
}
