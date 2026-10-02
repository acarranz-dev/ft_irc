#include "../include/Channel.hpp"

Channel::Channel(const std::string& name) 
	: _name(name), _topic(""), _key(""), _userLimit(0), 
	  _inviteOnly(false), _topicRestricted(true) {}

Channel::~Channel() {}

std::string Channel::getName() const { return _name; }
std::string Channel::getTopic() const { return _topic; }
void Channel::setTopic(const std::string& topic) { _topic = topic; }

void Channel::addMember(Client* client) 
{
	_members[client->getFd()] = client;
}

void Channel::removeMember(int fd) 
{
	_members.erase(fd);
	_operators.erase(fd);
}

bool Channel::isMember(int fd) const {
	return _members.find(fd) != _members.end();
}

void Channel::addOperator(Client* client) {
	_operators[client->getFd()] = client;
}

void Channel::removeOperator(int fd) {
	_operators.erase(fd);
}

bool Channel::isOperator(int fd) const {
	return _operators.find(fd) != _operators.end();
}

std::vector<int> Channel::getMemberFds() const
{
	std::vector<int> fds;
	std::map<int, Client*>::const_iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
		fds.push_back(it->first);
	return fds;
}

std::string Channel::getNamesList() const
{
	std::string names;
	std::map<int, Client*>::const_iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (!names.empty())
			names += " ";
		if (isOperator(it->first))
			names += "@";
		names += it->second->getNickname();
	}
	return names;
}

size_t Channel::getMemberCount() const { return _members.size(); }

bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
std::string Channel::getKey() const { return _key; }
size_t Channel::getUserLimit() const { return _userLimit; }

void Channel::addInvitation(const std::string& nick) 
{
	if (std::find(_invitedUsers.begin(), _invitedUsers.end(), nick) == _invitedUsers.end())
		_invitedUsers.push_back(nick);
}

bool Channel::isInvited(const std::string& nick) const {
	return std::find(_invitedUsers.begin(), _invitedUsers.end(), nick) != _invitedUsers.end();
}

void Channel::removeInvitation(const std::string& nick)
{
	std::vector<std::string>::iterator it = std::find(_invitedUsers.begin(), _invitedUsers.end(), nick);
	if (it != _invitedUsers.end())
		_invitedUsers.erase(it);
}

void Channel::renameInvitation(const std::string& oldNick, const std::string& newNick)
{
	std::vector<std::string>::iterator it = std::find(_invitedUsers.begin(), _invitedUsers.end(), oldNick);
	if (it != _invitedUsers.end())
		*it = newNick;
}

void Channel::setInviteOnly(bool value) { _inviteOnly = value; }
void Channel::setTopicRestricted(bool value) { _topicRestricted = value; }
void Channel::setKey(const std::string& key) { _key = key; }
void Channel::setUserLimit(size_t limit) { _userLimit = limit; }
