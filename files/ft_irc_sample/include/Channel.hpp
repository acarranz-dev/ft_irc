#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"
#include <algorithm>

class Channel
{
	private:
		std::string		_name;
		std::string		_topic;
		std::string		_key;
		size_t			_userLimit;
		bool			_inviteOnly;
		bool			_topicRestricted;
		std::map<int, Client*> _members;
		std::map<int, Client*> _operators;
		std::vector<std::string> _invitedUsers;

	public:
		Channel(const std::string& name);
		~Channel();

		std::string getName() const;
		std::string getTopic() const;
		void setTopic(const std::string& topic);
		void addMember(Client* client);
		void removeMember(int fd);
		bool isMember(int fd) const;
		void addOperator(Client* client);
		void removeOperator(int fd);
		bool isOperator(int fd) const;
		std::vector<int> getMemberFds() const;
		std::string getNamesList() const;
		bool isInviteOnly() const;
		bool isTopicRestricted() const;
		std::string getKey() const;
		size_t getUserLimit() const;
		size_t getMemberCount() const;
		void addInvitation(const std::string& nick);
		bool isInvited(const std::string& nick) const;
		void removeInvitation(const std::string& nick);
		void renameInvitation(const std::string& oldNick, const std::string& newNick);
		void setInviteOnly(bool value);
		void setTopicRestricted(bool value);
		void setKey(const std::string& key);
		void setUserLimit(size_t limit);
};

#endif
