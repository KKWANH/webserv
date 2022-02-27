NAME = server

COMPILER = clang++
FLAG = -Wall -Werror -Wextra -std=c++98 -g -fsanitize=address
LIBRARYHEADER = -I./

SRC =	server.cpp \
		srcs/parse/Parser.cpp \
		srcs/parse/NginxParser.cpp \
		srcs/parse/Config_Global.cpp \
		srcs/parse/Config_Http.cpp \
		srcs/parse/Config_Location.cpp \
		srcs/parse/Config_Nginx.cpp \
		srcs/parse/Config_Server.cpp \
		srcs/parse/Config_Type.cpp
		

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