#include "./includes/All.hpp"
#include "./includes/Client.hpp"
#include "./includes/Server.hpp"

void signal_handler(int signal) {
	if (signal == SIGINT) {
		stop_requested = 1;
	}
}

int main(int ac, char **av) {
	if (ac != 3) {
		std::cout << BAD_ARGS;
		return EXIT_FAILURE;
	}

	int				   port;
	std::string		   password = av[2];
	std::istringstream port_stream(av[1]);

	if (!(port_stream >> port) || !port_stream.eof() || port < 1024 || port > 65535) {
		std::cout << BAD_PORT;
		return EXIT_FAILURE;
	}
	Server ft_irc(port, password);

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);

	if (!ft_irc.start()) {
		std::cout << SERV_INIT_ERROR;
		return EXIT_FAILURE;
	}
	while (!stop_requested)
		ft_irc.monitorConnections();
	return EXIT_SUCCESS;
}
