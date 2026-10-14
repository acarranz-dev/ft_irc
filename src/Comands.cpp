#include "Server.hpp"
#include <iostream>

//REGISTERED

void Server::tryRegister(int fd)
{
    if (!_clients[fd].registered && _clients[fd].passOK
        && !_clients[fd].nick.empty() && !_clients[fd].user.empty())
    {
        _clients[fd].registered = true;
        std::cout << ":ircserv 001 " << _clients[fd].nick
                  << " :Welcome to the IRC server" << std::endl;
    }
}

// PASS

void Server::cmdPass(int fd, const std::vector<std::string>& args){
	if (args.empty()){
		std::cout << ":ircserv 461 * PASS :Not enough parameters" << std::endl;

	}
    else if (_clients[fd].registered == true){
		std::cout << ":ircserv 462 * :You may not reregister" << std::endl;
	}
	else if (args[0] !=_password){
		std::cout << ":ircserv 464 * :Password incorrect" << std::endl;
	}
	else{
		_clients[fd].passOK = true;
		std::cout << ":ircserv :Correct Password" << std::endl;
    }
    tryRegister(fd);
}


//NICK
void Server::cmdNick(int fd, const std::vector<std::string>& args){
	
	if (args.empty()){
		std::cout << ":ircserv 431 * NICK :Not nickname given" << std::endl;
        return;
	}
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        if(it->first != fd && it->second.nick == args[0]){
		    std::cout << ":ircserv 433 * NICK :Nick already be used" << std::endl;
            return;
        }
    }
    _clients[fd].nick = args[0];
    tryRegister(fd);

}

//USER
void Server::cmdUser(int fd, const std::vector<std::string>& args){
    if (args.empty()){
		std::cout << ":ircserv 461 * USER :Not enough parameters" << std::endl;
        return;
    }
    else if (_clients[fd].registered == true){
		std::cout << ":ircserv 462 * :You may not reregister" << std::endl;
        return;
    }    
    else if( args.size() < 4){
    	std::cout << ":ircserv 461 * USER :Not enough parameters" << std::endl;
        return;
    }
    _clients[fd].user = args[0];
    tryRegister(fd);

}

