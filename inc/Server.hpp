/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 22:08:08 by sarodrig         #+#    #+#             */
/*   Updated: 2026/10/20 19:32:48 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>
# include <set>
# include <vector>
# include <poll.h>

# include "Client.hpp"
# include "Channel.hpp"

class Server {
	public:
		Server(int port, const std::string& password);
		~Server();

	void	init();
	void	run();

private:
	int						_port;
	std::string				_password;
	int						_socketFd;
	bool					_running;
	std::vector<pollfd>		_pollFds;
	std::map<int, Client>	_clients;
	std::map<std::string, Channel> _channels;

	void	setNonBlocking(int fd); 
	void	addPollFd(int fd, short events);
	void	removePollFd(int fd);
	void	updatePollFd(int fd);
	void	handleNewClient();
	void	handleReadClient(int fd);
	void	handleWriteClient(int fd);
	void	handleCloseClient(int fd);
	void	processInput(int fd);
	void	handleCommand(int fd, const std::string& line);
	void	sendNumericReply(int fd, const std::string& code, const std::string& msg);
	void	queueMessage(int fd, const std::string& message);

	void	cmdPass(Client&, const std::vector<std::string>&);
	void	cmdNick(Client&, const std::vector<std::string>&);
	void	cmdUser(Client&, const std::vector<std::string>&);
	void	cmdJoin(Client&, const std::vector<std::string>&);
	void	cmdKick(Client&, const std::vector<std::string>&);
	void	cmdInvite(Client&, const std::vector<std::string>&);
	void	cmdTopic(Client&, const std::vector<std::string>&);
	void	cmdPart(Client&, const std::vector<std::string>&);
	void	cmdMode(Client&, const std::vector<std::string>&);
	void	cmdPrivmsg(Client&, const std::vector<std::string>&);
	void	cmdQuit(Client&, const std::vector<std::string>&);

};

#endif