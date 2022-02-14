# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: server <server@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/11 22:31:50 by hybae             #+#    #+#              #
#    Updated: 2022/01/28 14:43:00 by server           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = server

COMPILER = clang++
FLAG = -Wall -Werror -Wextra -std=c++98 -g -fsanitize=address
LIBRARYHEADER = -I./

SRC = server.cpp
OBJ = $(SRC:.cpp=.o)

%.o: %.cpp
	$(COMPILER) $(FLAG) $(LIBRARYHEADER) -c -o $@ $<

$(NAME): $(OBJ)
	$(COMPILER) $(FLAG) $(LIBRARYHEADER) $(OBJ) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean:	clean
	rm -rf $(NAME)

re: fclean all