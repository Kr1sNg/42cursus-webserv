# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/26 17:42:09 by tat-nguy          #+#    #+#              #
#    Updated: 2025/10/29 11:24:26 by tat-nguy         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ***** Color Definition ***** #

RED = \033[91m
YELLOW = \033[33m
GREEN = \033[92m
RESET = \033[0m

# **************************** #

NAME 		= webserv

# Source Files
SRCS     	= src/main.cpp src/exception.cpp  \
				src/server/Server.cpp src/server/PollLoop.cpp \
				src/server/Connection.cpp src/server/Listener.cpp \
				src/config/Block.cpp src/config/Config.cpp \
				src/config/Directive.cpp src/config/fillConfig.cpp \
				src/config/Locationconfig.cpp src/config/parsing.cpp \
				src/config/tokenization.cpp src/config/Serverconfig.cpp\
				src/HTTP/Request.cpp src/HTTP/Response.cpp #src/HTTP/ParserHTTP.cpp \
# 				src/HTTP/ErrorHTTP.cpp src/server/cgiHandle.cpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Compiler and Flags
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g -gdwarf-4
RM			= rm -rf

# Targets
all:	$(NAME)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
	@echo "$(YELLOW) 🛠️  waiting for compilation...$(RESET)"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN) 🤖  Compiled successful ✴️  $(NAME) ✴️$(RESET)"

# Clean
clean:
	@$(RM) $(OBJS)
	@echo "$(RED) 🗑️  Deleted object files.$(RESET)"

fclean:	clean
	@$(RM) $(NAME)
	@find . -name "*.dSYM" -type d -exec $(RM) {} +
	@echo "$(RED) 🚽  Deleted ❄️  $(NAME) ❄️$(RESET)"

re: fclean all

# Debug & Memory leaks (iOs)
ASANFLAGS = -fsanitize=address -g
asan: fclean
	$(CXX) $(CXXFLAGS) $(ASANFLAGS) -o $(NAME) $(SRCS)

.PHONY: all clean fclean re asan
