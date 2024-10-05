#ifndef ALL_HPP
# define ALL_HPP

// Standard library / UNIX Headers
# include <algorithm>
# include <arpa/inet.h>
# include <cerrno>
# include <csignal>
# include <cstdlib>
# include <cstring>
# include <ctime>
# include <iostream>
# include <map>
# include <netinet/in.h>
# include <sstream>
# include <string>
# include <sys/socket.h>
# include <termios.h>
# include <unistd.h>
# include <vector>

//  -- STRING MESSAGES
// strings
# define HELP			"<users> to see who is online\n<channels> to see active channels\n"
# define BAD_ARGS		"Wrong usage, run like ./ft_irc <port> <password>\n"
# define BAD_PORT		"Invalid port, provide a numeric port value between 1024 and 65535.\n"
# define SERV_INIT_ERROR "Failed to start the server. Exiting...\n"
# define SELECT_ERR		"Closing server. \n"
# define RECV_ERR		"Closing server. \n"
# define ACCEPT_ERR		"Acceptance failed. \n"

// RPL MACROS
# include "Macros.hpp"
# include "Server.hpp"

// title
# define ASCII_TITLE "\
   __ _     _            \n\
  / _| |   (_)           \n\
 | |_| |_   _ _ __ ___   \n\
 |  _| __| | | '__/ __|  \n\
 | | | |_  | | | | (__   \n\
 |_|  \\__| |_|_|  \\___|\n\
       ______            \n\
      |______|           \n"

// -- GLOBALS
static volatile sig_atomic_t stop_requested = 0;

// -- STRUCTS
struct message {
	std::string				 cmd;
	std::vector<std::string> params;
};

std::string						   getCurrentDateTime();
std::vector<std::string>		   split(const std::string &str, char delimiter);
void							   printServerInfo(Server *server);
bool							   isValidChannelName(const std::string &channelName);
bool							   isValidNickName(const std::string &nickName);
std::map<std::string, std::string> create_channel_map(std::string const &channels, std::string const &keys);
struct message					   parseInput(std::string const &buffer);
void							   debugServer(Server &server, std::vector<std::string> params);
void							   sendToClient(const std::string &msg, const Client &client);

#endif // ALL_HPP
