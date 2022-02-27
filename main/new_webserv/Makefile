NAME			=	server

COMPILER		=	clang++
FLAG			=	-Wall -Werror -Wextra -std=c++98 -g -fsanitize=address
LIBRARYHEADER	=	-I./CGI -I./HTTPMessageController -I./ErrorHandler -I./KernelQueueController -I./ParsingController -I./ServerProcessController -I./SocketController

SRC 			=	server.cpp\
					./CGI/CGIProcess.cpp \
					./HTTPMessageController/RequestMessageController.cpp \
					./ParsingController/Parser.cpp \
					./ParsingController/NginxParser.cpp \
					./ParsingController/ConfigBlocks.cpp \
					./ParsingController/ConfigController.cpp \
					./ParsingController/ConfigGlobal.cpp \
					./ParsingController/ConfigHttp.cpp \
					./ParsingController/ConfigLocation.cpp \
					./ParsingController/ConfigNginx.cpp \
					./ParsingController/ConfigServer.cpp \
					./ParsingController/ConfigType.cpp
OBJ				=	$(SRC:.cpp=.o)

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
