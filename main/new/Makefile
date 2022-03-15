NAME			=	webserv

COMP			=	clang++
FLAG			=	-Wall -Werror -Wextra -std=c++98 -g -fsanitize=address

HEADER			=	-I./HTTPMessageController \
					-I./ErrorHandler \
					-I./KernelQueueController \
					-I./ParsingController \
					-I./ServerProcess \
					-I./SocketController \
					-I./ClassController \
					-I./TimeController \
					-I./CGI

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
					./KernelQueueController/KernelQueueController.cpp \
					./SocketController/SocketController.cpp \
					./HTTPMessageController/RequestMessage.cpp \
					./HTTPMessageController/ResponseMessage.cpp

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
	@make clean
	@printf "\n\033[34m[WEBSERV]\033[0m Now \033[32mWEBSERV\033[0m is executable.\n"
	@printf "\033[34m[WEBSERV]\033[0m Launch this with \033[33m./webserv [config] [mime setting]\033[0m\n"

all:
	@make fclean
	@$(NAME)

run:
	@make
	@./webserv

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
