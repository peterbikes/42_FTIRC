#ifndef MACROS_HPP
#define MACROS_HPP
#include <string>

#define HOSTNAME std::string("FT_IRC")

// 001 RPL_WELCOME
#define RPL_WELCOME(NICK) \
	(std::string(":" + HOSTNAME + " 001 " + (NICK) + " :Welcome to ft_irc, " + (NICK) + "!\r\n"))

// 002 RPL_YOURHOST
#define RPL_YOURHOST(NICK) \
	(std::string(":" + HOSTNAME + " 002 " + (NICK) + " :Your host is " + HOSTNAME + ", running version 1.0!\r\n"))

// 003 RPL_CREATED
#define RPL_CREATED(NICK, STARTTIME) \
	(std::string(":" + HOSTNAME + " 003 " + (NICK) + " :This server was created " + (STARTTIME) + "\r\n"))

// 004 RPL_MYINFO
#define RPL_MYINFO(NICK) \
	(std::string(":" + HOSTNAME + " 004 " + (NICK) + " " + HOSTNAME + " 1.0 itkol\r\n"))

// 315 RPL_ENDOFWHO
#define RPL_ENDOFWHO(NICK, TARGET) \
	(std::string(":" + HOSTNAME + " 315 " + (NICK) + " " + (TARGET) + " :End of WHO list\r\n"))

// 324 RPL_CHANNELMODEIS
#define RPL_CHANNELMODEIS(NICK, CHANNEL, MODE) \
	(std::string(":" + HOSTNAME + " 324 " + (NICK) + " " + (CHANNEL) + " " + (MODE) + "\r\n"))

// 329 RPL_CREATIONTIME
#define RPL_CREATIONTIME(NICK, CHANNEL, TIME) \
	(std::string(":" + HOSTNAME + " 329 " + setter.getNick() + " " + this->name + " " + this->startTime + "\r\n"))

// 331 RPL_NOTOPIC
#define RPL_NOTOPIC(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 331 " + (NICK) + " " + (CHANNEL) + " :No topic is set\r\n"))

// 332 RPL_TOPIC
#define RPL_TOPIC(NICK, CHANNEL, TOPIC) \
	(std::string(":" + HOSTNAME + " 332 " + (NICK) + " " + (CHANNEL) + " :" + (TOPIC) + "\r\n"))

// 333 RPL_TOPICWHOTIME
#define RPL_TOPICWHOTIME(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 333 " + (NICK) + " " + (CHANNEL) + "\r\n"))

// 341 RPL_INVITING
#define RPL_INVITING(NICK, CHANNEL, INVITEE) \
	(std::string(":" + HOSTNAME + " 341 " + (NICK) + " " + (INVITEE) + " " + (CHANNEL) + "\r\n"))

// 352 RPL_WHOREPLY
#define RPL_WHOREPLY(NICK, TARGET, SECOND_USERNAME, USER, SECOND_REALNAME, FLAG) \
	(std::string(":" + HOSTNAME + " 352 " + (NICK) +                             \
				 " " + (TARGET) + " " + (SECOND_USERNAME) + " " + HOSTNAME +     \
				 " irc " + (USER) + (FLAG) + " :2 " + (SECOND_REALNAME) + "\r\n"))

// 353 RPL_NAMREPLY
#define RPL_NAMREPLY(NICK, CHANNEL, MEMBER) \
	(std::string(":" + HOSTNAME + " 353 " + (NICK) + " = " + (CHANNEL) + " :" + (MEMBER) + "\r\n"))

// 366 RPL_ENDOFNAMES
#define RPL_ENDOFNAMES(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 366 " + (NICK) + " " + (CHANNEL) + " :End of /NAMES list\r\n"))

// 372 RPL_MOTD
#define RPL_MOTD(NICK) \
	(std::string(":" + HOSTNAME + " 372 " + (NICK) + " :Welcome to psotto and heda's irc\r\n"))

// 375 RPL_MOTDSTART
#define RPL_MOTDSTART(NICK) \
	(std::string(":" + HOSTNAME + " 375 " + (NICK) + " :- " + HOSTNAME + " Message of the Day\r\n"))

// 376 RPL_ENDOFMOTD
#define RPL_ENDOFMOTD(NICK) \
	(std::string(":" + HOSTNAME + " 376 " + (NICK) + " :please be civil\r\n"))

// 401 ERR_NOSUCHNICK
#define ERR_NOSUCHNICK(NICK, CHANNEL) \
	(std::string(":" + (NICK) + " 401 " + (NICK) + " " + (CHANNEL) + " :No such nick\r\n"))

// 403 ERR_NOSUCHCHANNEL
#define ERR_NOSUCHCHANNEL(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 403 " + (NICK) + " " + (CHANNEL) + " :Invalid Channel Name\r\n"))

// 411 ERR_NORECIPIENT
#define ERR_NORECIPIENT(NICK, CMD) \
	(std::string(":" + HOSTNAME + " 411 " + (NICK) + " :No recipient given (" + (CMD) + ")\r\n"))

// 412 ERR_NOTEXTTOSEND
#define ERR_NOTEXTTOSEND(NICK) \
	(std::string(":" + HOSTNAME + " 412 " + (NICK) + " :No text to send\r\n"))

// 431 ERR_NONICKNAMEGIVEN
#define ERR_NONICKNAMEGIVEN(NICK) \
	(std::string(":" + HOSTNAME + " 431 " + (NICK) + " :No nickname given\r\n"))

// 432 ERR_ERRONEUSNICKNAME
#define ERR_ERRONEUSNICKNAME(NICK, OLD_NICK) \
	(std::string(":" + HOSTNAME + " 432 " + (OLD_NICK) + " " + (NICK) + " :Erroneus nickname\r\n"))

// 433 ERR_NICKNAMEINUSE
#define ERR_NICKNAMEINUSE(NICK, OLD_NICK) \
	(std::string(":" + HOSTNAME + " 433 " + (OLD_NICK) + " " + (NICK) + " :Nickname is already in use\r\n"))

// 442 ERR_NOTONCHANNEL
#define ERR_NOTONCHANNEL(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 442 " + (NICK) + " " + (CHANNEL) + " :You're not on that channel\r\n"))

// 443 ERR_USERONCHANNEL
#define ERR_USERONCHANNEL(NICK, CHANNEL, INVITEE) \
	(std::string(":" + HOSTNAME + " 443 " + (NICK) + " " + (INVITEE) + " " + (CHANNEL) + " :is already on channel\r\n"))

// 461 ERR_NEEDMOREPARAMS
#define ERR_NEEDMOREPARAMS(NICK, CMD) \
	(std::string(":" + HOSTNAME + " 461 " + (NICK) + " " + (CMD) + " :Not enough parameters\r\n"))

// 464 ERR_PASSWDMISMATCH
#define ERR_PASSWDMISMATCH(NICK) \
	(std::string(":" + HOSTNAME + " 464 " + (NICK) + " :Password incorrect\r\n"))

// 471 ERR_CHANNELISFULL
#define ERR_CHANNELISFULL(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 471 " + (NICK) + " " + (CHANNEL) + " :Cannot join channel (+l)\r\n"))

// 472 ERR_UNKNOWNMODE
#define ERR_UNKNOWNMODE(NICK, CHANNEL, MODE) \
	(std::string(":" + HOSTNAME + " 472 " + (NICK) + " " + (CHANNEL) + " " + (MODE) + " :is unknown mode char to me\r\n"))

// 473 ERR_INVITEONLYCHAN
#define ERR_INVITEONLYCHAN(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 473 " + (NICK) + " " + (CHANNEL) + " :Cannot join channel (+i)\r\n"))

// 475 ERR_BADCHANNELKEY
#define ERR_BADCHANNELKEY(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 475 " + (NICK) + " " + (CHANNEL) + " :Cannot join channel (+k)\r\n"))

// 476 ERR_BADCHANMASK
#define ERR_BADCHANMASK(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 476 " + (NICK) + " " + (CHANNEL) + " :Bad Channel Mask\r\n"))

// 482 ERR_CHANOPRIVSNEEDED
#define ERR_CHANOPRIVSNEEDED(NICK, CHANNEL) \
	(std::string(":" + HOSTNAME + " 482 " + (NICK) + " " + (CHANNEL) + " :You're not channel operator\r\n"))

#endif // MACROS_HPP
