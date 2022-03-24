NAME			=	webserv

COMP			=	clang++
FLAG			=	-Wall -Werror -Wextra -std=c++98 -g -fsanitize=address

CONF			=	
MIME			=	

HEADER			=	-I./HTTPMessageController \
					-I./ErrorHandler \
					-I./KernelQueueController \
					-I./ParsingController \
					-I./ServerProcess \
					-I./SocketController \
					-I./ClassController \
					-I./TimeController \
					-I./CGI \
					-I./FileController \
					-I./AutoindexController

SRCS 			=	./server.cpp\
					./ParsingController/Parser.cpp \
					./ParsingController/NginxParser.cpp \
					./ParsingController/ConfigGlobal.cpp \
					./ParsingController/ConfigHttp.cpp \
					./ParsingController/ConfigLocation.cpp \
					./ParsingController/ConfigNginx.cpp \
					./ParsingController/ConfigServer.cpp \
					./ParsingController/ConfigType.cpp \
					./ParsingController/ConfigMime.cpp \
					./ServerProcess/ServerProcess.cpp \
					./KernelQueueController/KernelQueueController.cpp \
					./SocketController/SocketController.cpp \
					./HTTPMessageController/HTTPConnection.cpp \
					./HTTPMessageController/HTTPData.cpp \
					./HTTPMessageController/RequestMessage.cpp \
					./HTTPMessageController/ResponseMessage.cpp \
					./FileController/FileController.cpp \
					./AutoindexController/AutoindexController.cpp

OBJS			=	$(SRCS:.cpp=.o)

%.o: %.cpp
	@printf "\033[34m[WEBSERV]\033[0m Generating file : "
	@$(COMP) $(FLAG) $(HEADER) -c -o $@ $<
	@sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m. "
	@printf "\033[32mOK\033[0m\n"

$(NAME): $(OBJS)
	@printf "\033[34m[WEBSERV]\033[0m Compiling file  : "
	@$(COMP) $(FLAG) $(HEADER) $(OBJS) -o $(NAME)
	@sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m. "
	@printf "\033[32mOK\033[0m\n"
	@printf "\n\033[34m[WEBSERV]\033[0m Now \033[32mWEBSERV\033[0m is executable.\n"
	@printf "\033[34m[WEBSERV]\033[0m Launch this with \033[33m./webserv [config] [mime setting]\033[0m\n"

help:
	@printf "\033[34m[WEBSERV]\033[0m \033[33m[make]\033[0m :        make ./webserv, delete object files\n"
	@printf "\033[34m[WEBSERV]\033[0m \033[33m[make clean]\033[0m :  delete object files\n"
	@printf "\033[34m[WEBSERV]\033[0m \033[33m[make fclean]\033[0m : delete ./webserv and object files\n"
	@printf "\033[34m[WEBSERV]\033[0m \033[33m[make re]\033[0m :     execute \033[33m[make fclean]\033[0m and \033[33m[make]\033[0m\n"
	@printf "\033[34m[WEBSERV]\033[0m \033[33m[make run]\033[0m :    \033[33m[make]\033[0m and execute ./webserv.\n"
	@printf "\033[34m[WEBSERV]\033[0m \033[33m          \033[0m      \033[3m\033[4m./webserv CONF=\"nginx.conf\" MIME=\"mime.types\"\033[0m\n"

all:
	@make clean
	@$(NAME)

run:
	@make
	@./webserv $(CONF) $(MIME)

clean:
	@printf "\033[34m[WEBSERV]\033[0m Removing object : "
	@rm -rf $(OBJS)
	@sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m. "
	@printf "\033[32mOK\033[0m\n"

fclean:
	@make clean
	@printf "\033[34m[WEBSERV]\033[0m Removing result : "
	@rm -rf $(NAME)
	@sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m." && sleep 0.05 && printf "\033[33m. "
	@printf "\033[32mOK\033[0m\n"

re:
	@make fclean
	@make
