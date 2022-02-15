# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: server <server@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/11 22:31:50 by hybae             #+#    #+#              #
#    Updated: 2022/02/15 19:44:09 by juhpark          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = server

COMPILER = clang++
FLAG = -Wall -Werror -Wextra -std=c++98 -g -fsanitize=address
LIBRARYHEADER = -I./

SRC = server.cpp ErrorHandler/ErrorHandler.cpp
OBJ = $(SRC:.cpp=.o)

%.o: %.cpp
	$(COMPILER) $(FLAG) $(LIBRARYHEADER) -c -o $@ $<

$(NAME): $(OBJ)
	$(COMPILER) $(FLAG) $(LIBRARYHEADER) $(OBJ) -o $(NAME)
	rm -f $(OBJ)

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean:	clean
	rm -rf $(NAME)

re: fclean all
