# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: skim <skim@student.42seoul.kr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/11 22:31:50 by hybae             #+#    #+#              #
#    Updated: 2022/02/19 10:31:15 by skim             ###   ########.fr        #
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

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean:	clean
	rm -rf $(NAME)

re: fclean all
