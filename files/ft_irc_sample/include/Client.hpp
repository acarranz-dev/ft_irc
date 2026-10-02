#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client 
{
	private:
		int			_fd;
		std::string	_nickname;
		std::string	_username;
		std::string	_buffer;
		std::string	_outBuffer;
		bool		_isRegistered;
		bool		_hasValidPassword;

	public:
		Client(int fd);
		~Client();
		int getFd() const;
		std::string getNickname() const;
		void setNickname(const std::string& nick);
		std::string getUsername() const;
		void setUsername(const std::string& user);
		std::string& getBuffer();
		void appendToBuffer(const std::string& str);
		void clearBuffer();
		std::string& getOutBuffer();
		void appendToOutBuffer(const std::string& str);
		bool isRegistered() const;
		void setRegistered(bool value);
		bool hasValidPassword() const;
		void setValidPassword(bool value);
};

#endif
