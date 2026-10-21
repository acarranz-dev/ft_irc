# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/04 03:08:30 by sarodrig          #+#    #+#              #
#    Updated: 2026/10/02 23:05:57 by sarodrig         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NO_COLOR		= \e[0m
BLUE			= \e[34m # Blue color for compilation messages
YELLOW			= \e[93m # Yellow color for build messages
WHITE			= \e[97m # White color for project management messages
GREEN			= \e[32m # Green color for test messages
RED				= \e[31m # Red color for error messages
SUCCESS			= $(GREEN)[✔]$(NO_COLOR)
FAIL			= $(RED)[✘]$(NO_COLOR)


# ***** VARIABLES ************************************************************ #

MAKEFLAGS		+= --silent
CXX				= c++
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98 -MMD -MP

NAME 			= ircserv

INC_DIR			= inc/
SRC_DIR			= src/
SRC				= main.cpp Server.cpp Client.cpp Channel.cpp
CMD_SRC			= cmds/pass.cpp cmds/nick.cpp cmds/user.cpp \
				  cmds/join.cpp cmds/part.cpp cmds/topic.cpp \
				  cmds/kick.cpp cmds/invite.cpp cmds/mode.cpp \
				  cmds/privmsg.cpp cmds/quit.cpp

OBJ_DIR			= obj/
OBJ 			= $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC) $(CMD_SRC))
DEP 			= $(OBJ:.o=.d)

TOTAL_FILES		= $(words $(SRC) $(CMD_SRC))
CURRENT			= 0


# ***** FILE TARGETS ********************************************************* #
-include $(DEP)

$(OBJ_DIR)cmds/%.o : $(SRC_DIR)cmds/%.cpp Makefile
	@$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))
	@printf "\r$(BLUE)>>> [%d/%d] Compiling %-20s$(NO_COLOR)" $(CURRENT) $(TOTAL_FILES) "$(@F)"
	@mkdir -p $(OBJ_DIR)cmds
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@ \
	|| (printf "\n$(WHITE)---$(FAIL)$(RED)Compilation of $@ failed$(WHITE)---\n$(NO_COLOR)" && exit 1)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp Makefile
	@$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))
	@printf "\r$(BLUE)>>> [%d/%d] Compiling %-20s$(NO_COLOR)" $(CURRENT) $(TOTAL_FILES) "$(@F)"
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@ \
	|| (printf "\n$(WHITE)---$(FAIL)$(RED)Compilation of $@ failed$(WHITE)---\n$(NO_COLOR)" && exit 1)

$(NAME): $(OBJ) Makefile
	@printf "\n$(WHITE)---$(SUCCESS)$(BLUE)Compilation [$(NAME)] complete$(WHITE)-------$(NO_COLOR)"
	@printf "\n$(YELLOW)>>> $(YELLOW)Building [$(NAME)]$(NO_COLOR)\n"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) \
	|| (printf "$(WHITE)---$(FAIL)$(RED)Build failed$(WHITE)-----------------------\n$(NO_COLOR)" && exit 1)


# ***** PHONY TARGETS ******************************************************** #
.PHONY: start all clean fclean re sanitize test
.DEFAULT_GOAL = all

start: 
	@printf "$(WHITE)------ Building project [$(NAME)] ------------$(NO_COLOR)\n"

all: start ${NAME}
	@printf "$(WHITE)---$(SUCCESS)$(YELLOW)Build [$(NAME)] complete$(WHITE)-------------$(NO_COLOR)\n"

clean:
	@printf "$(WHITE)>>> Removing obj files $(OBJ_DIR)* $(NO_COLOR) \n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(WHITE)>>> Removing bin files $(NAME)$(NO_COLOR) \n"
	@rm -f $(NAME)

re: fclean all


# ***** TEST TARGETS ********************************************************* #
sanitize: fclean
	@printf "$(WHITE)---> Building with AddressSanitizer for testing$(WHITE)---$(NO_COLOR)\n"
	@$(MAKE) CXXFLAGS="$(CXXFLAGS) -g -fsanitize=address" all

PARAM1 			= ""
test: all
	@printf "\n$(GREEN)------------ Running Tests -----------------------$(NO_COLOR)\n"
	@printf "$(GREEN)Command: ./$(NAME) $(PARAM1)$(NO_COLOR)\n"
	@./$(NAME) $(PARAM1)