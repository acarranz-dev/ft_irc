/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */

#include "Client.hpp"

Client::Client(int socketFd): 
	fd(socketFd), passOK(false), registered(false) {
}

Client::~Client() {
}

Client::Client(const Client& other): 
	fd(other.fd),
	nick(other.nick),
	user(other.user),
	input(other.input),
	output(other.output),
	passOK(other.passOK),
	registered(other.registered) {
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		fd = other.fd;
		nick = other.nick;
		user = other.user;
		input = other.input;
		output = other.output;
		passOK = other.passOK;
		registered = other.registered;
	}
	return *this;
}
