/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */

#include "Client.hpp"

Client::Client(int socketFd)
: fd(socketFd), passOK(false), registered(false) {
}
