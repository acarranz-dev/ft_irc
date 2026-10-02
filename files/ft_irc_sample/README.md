This project has been created as part of the 42 curriculum by `ncampo-f` and `gpico-co`.

# ft_irc

## Description

`ft_irc` is a custom **Internet Relay Chat (IRC) server** written from scratch in **C++98**, developed as part of the 42 Common Core.

The goal of the project is to implement the core of the IRC protocol: a TCP server able to handle multiple clients at the same time using a single `poll()` loop, with non-blocking I/O and no forking. The server allows real IRC clients to connect, authenticate, choose a nickname and username, join channels, exchange public and private messages, and manage channels through operator commands and modes.

The project does **not** implement an IRC client and does **not** implement server-to-server communication, as required by the subject.

## Features

- Non-blocking TCP server handled with a single `poll()` loop.
- Support for multiple simultaneous clients.
- Authentication via server password (`PASS`).
- User registration (`NICK`, `USER`).
- Channel management (creation, joining, members, operators).
- Public (channel) and private messaging (`PRIVMSG`).
- Topic management with operator restrictions (`TOPIC`).
- Kicking users from a channel (`KICK`).
- Inviting users to a channel (`INVITE`).
- Channel modes (`MODE`): `i`, `t`, `k`, `o`, `l`.
- Partial/fragmented message handling (buffered input, reassembled on `\n`).

## Architecture overview

- **`Server`**: Creates and configures the listening socket, runs the main `poll()` event loop, accepts new connections, reads and buffers client input, splits it into complete lines, and dispatches commands to the relevant handler.
- **`Client`**: Represents a connected user (file descriptor, nickname, username, input buffer, registration state).
- **`Channel`**: Represents an IRC channel (name, topic, key, user limit, members, operators, invite list, and broadcasting to members).

### Supported commands

| Command   | Description |
|-----------|-------------|
| `PASS`    | Authenticates the client with the server password. |
| `NICK`    | Sets or changes the client's nickname. |
| `USER`    | Sets the client's username as part of the registration process. |
| `JOIN`    | Joins (or creates) a channel. |
| `PRIVMSG` | Sends a message to a user or to a channel. |
| `KICK`    | Kicks a user from a channel (operators only). |
| `INVITE`  | Invites a user to a channel. |
| `TOPIC`   | Views or changes a channel's topic. |
| `MODE`    | Changes channel modes: `i`, `t`, `k`, `o`, `l`. |

### Channel modes (`MODE`)

| Mode | Meaning |
|------|---------|
| `i`  | Set/remove invite-only channel. |
| `t`  | Restrict/unrestrict the `TOPIC` command to channel operators. |
| `k`  | Set/remove the channel key (password). |
| `o`  | Give/take channel operator privilege. |
| `l`  | Set/remove the user limit of the channel. |


## Instructions

### Compilation

```bash
make
```

This compiles the project with `c++` using the flags `-Wall -Wextra -Werror -std=c++98` and produces the `ircserv` executable.

Other available rules:

```bash
make clean
make fclean
make re
```
### Usage

```bash
./ircserv <port> <password>
```

- `port`: the TCP port the server will listen on (1024–65535).
- `password`: the connection password that any IRC client must provide via `PASS` before registering.

Example:

```bash
./ircserv 6667 mypassword
```

### Connecting & Testing

#### 1. Testing with Netcat (`nc`)
Once your server is running, open a new terminal window to connect. You can keep an eye on the server's terminal to monitor the logs and live connection traffic.

Run the following command to connect via Netcat:
```bash
nc -C 127.0.0.1 6667
```

Once connected, authenticate and register your user by typing the following commands **in raw format** (press Enter after each line):
```text
PASS mypassword
NICK noemi_nick
USER noemi_user 0 * :noemi_real
```

After a successful registration, you can test channel management and messaging:
```text
JOIN #channel1
PRIVMSG #channel1 :Hello everyone
```

* **Testing Channel Messages:** Open another terminal, run `nc` again, and register a second user. Join the same channel (`JOIN #channel1`) to see how messages are broadcasted and received between users.
* **Testing Private Messages:** To send a direct message to a specific user, ensure you have a second user connected from another terminal. Then use the following syntax:
  ```text
  PRIVMSG noemi_nick :Hola Noemi
  ```

* **Other Features:** You can also test the rest of the channel modes (`+i`, `+t`, `+k`, `+o`, `+l`) and operators commands (`KICK`, `INVITE`, `TOPIC`) following their standard IRC syntax.

#### 2. Testing with Irssi
To test the server using a real, robust CLI client like **Irssi**, open a new terminal window and follow these steps:

1. Launch Irssi in your terminal:
   ```bash
   irssi
   ```

2. **Connect and Authenticate:** Connect by supplying the server IP, port, and password **in that exact order** as parameters. Irssi will automatically parse them and transmit the `PASS`, `NICK`, and `USER` registration commands behind the scenes to your server:
   ```text
   /connect 127.0.0.1 6667 mypassword
   ```

3. **Testing Channels & Communication:**
   * **Join a channel (`JOIN`):** Type `/join #channel1` to create or enter a channel.
   * **Public Message (`PRIVMSG` to channel):** Simply type your text and press Enter inside the channel window to send a message to everyone.
   * **Private Message (`PRIVMSG` to user):** To open a private chat window with another connected user, use:
     ```text
     /msg noemi_nick Hi!This is a private message
     ```

Irssi uses separate windows for the server, channels, and private conversations.
The active window number is displayed in Irssi's status bar.
Press `Esc`, release it, and then press the number of the window you want to open.

## Project structure

```
ft_irc/
├── include/
│   ├── Server.hpp
│   ├── Client.hpp
│   └── Channel.hpp
├── src/
│   ├── Channel.cpp
│   ├── Client.cpp
│   ├── main.cpp
│   ├── Server.cpp
│   └── commands/
│       ├── Pass.cpp
│       ├── Nick.cpp
│       ├── User.cpp
│       ├── Join.cpp
│       ├── Privmsg.cpp
│       ├── Topic.cpp
│       ├── Kick.cpp
│       ├── Invite.cpp
│       └── Mode.cpp
├── Makefile
└── README.md
```

## Resources

### IRC protocol references

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Internet Relay Chat Protocol Documentation ](https://dd.ircdocs.horse/)
- [modern.ircdocs.info — Modern IRC Client Protocol](https://modern.ircdocs.info/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- `man poll`, `man fcntl`, `man socket`


### AI Usage Disclosure
Artificial Intelligence (Gemini) was utilized during the development of this project as an auxiliary tool for the following tasks:

* **Code Debugging & Troubleshooting:** Assisting in identifying the root causes of logical errors, handling edge cases regarding socket disconnections, and unexpected program behaviors.
* **Testing Concepts:** Providing conceptual ideas and methodologies to structure testing scenarios for validating channel behaviors and edge cases.
* **Documentation & Formatting:** Structuring and translating the content for this `README.md` file.