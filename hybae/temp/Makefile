NAME			=	server

COMPILER		=	clang++
FLAG			=	-Wall -Werror -Wextra -std=c++98 -g -fsanitize=address
LIBRARYHEADER	=	-I./HTTPMessageController \
					-I./ErrorHandler \
					-I./KernelQueueController \
					-I./ParsingController \
					-I./ServerProcess \
					-I./SocketController \
					-I./ClassController \

SRC 			=	server.cpp\
					./ParsingController/Parser.cpp \
					./ParsingController/NginxParser.cpp \
					./ParsingController/ConfigGlobal.cpp \
					./ParsingController/ConfigHttp.cpp \
					./ParsingController/ConfigLocation.cpp \
					./ParsingController/ConfigNginx.cpp \
					./ParsingController/ConfigServer.cpp \
					./ParsingController/ConfigType.cpp \
					./KernelQueueController/KernelQueueController.cpp \
					./SocketController/SocketController.cpp \
					./HTTPMessageController/RequestMessage.cpp \
					./HTTPMessageController/ResponseMessage.cpp

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
