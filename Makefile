NAME			=	webserv

COMP			=	c++
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
					-I./CGIProcess \
					-I./FileController \
					-I./AutoindexController \
					-I./ErrorPageController \
					-I./TimeController

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
					./ParsingController/ConfigStatus.cpp \
					./ServerProcess/ServerProcess.cpp \
					./KernelQueueController/KernelQueueController.cpp \
					./SocketController/SocketController.cpp \
					./HTTPMessageController/HTTPConnection.cpp \
					./HTTPMessageController/HTTPData.cpp \
					./HTTPMessageController/RequestMessage.cpp \
					./HTTPMessageController/ResponseMessage.cpp \
					./FileController/FileController.cpp \
					./AutoindexController/AutoindexController.cpp \
					./ErrorPageController/ErrorPageController.cpp \
					./CGIProcess/CGIProcess.cpp \
					./TimeController/TimeController.cpp \
					./ErrorHandler/ErrorHandler.cpp

OBJS			=	$(SRCS:.cpp=.o)

%.o: %.cpp
	@printf "\033[34m[WEBSERV]\033[0m Compiling file  : "
	@printf "\033[35m[ %-47s ] \033[0m" $<
	@$(COMP) $(FLAG) $(HEADER) -c -o $@ $<
	@sleep 0.08 && printf "." && sleep 0.08 && printf "." && sleep 0.08 && printf ". "
	@printf "\033[32mOK\033[0m\n"

$(NAME): $(OBJS)
	@printf "\033[34m[WEBSERV]\033[0m\n"
	@printf "\033[34m[WEBSERV]\033[0m Linking         : \033[36m[ %-47s ]\033[0m " "obj file (*.o) to $(NAME) binary"
	@$(COMP) $(FLAG) $(HEADER) $(OBJS) -o $(NAME)
	@sleep 0.08 && printf "." && sleep 0.08 && printf "." && sleep 0.08 && printf ". "
	@printf "\033[32mOK\033[0m\n"
	@printf "\033[34m[WEBSERV]\033[0m"
	@printf "\n\033[34m[WEBSERV]\033[0m Now \033[32mWEBSERV\033[0m is executable."
	@printf "\n\033[34m[WEBSERV]\033[0m"
	@printf "\n\033[34m[WEBSERV]\033[0m \033[33mmake help\033[0m to get help.\n"
	@printf "\033[34m[WEBSERV]\033[0m Launch with ./webserv [config] [mime setting] [status_code][\033[0m\n"

help:
	@printf "\033[34m[WEBSERV]\033[0m \033[35m[%-11s]\033[0m : make ./webserv, delete object files\n" "make"
	@printf "\033[34m[WEBSERV]\033[0m \033[35m[%-11s]\033[0m : delete object files\n" "make clean"
	@printf "\033[34m[WEBSERV]\033[0m \033[35m[%-11s]\033[0m : delete ./webserv and object files\n" "make fclean"
	@printf "\033[34m[WEBSERV]\033[0m \033[35m[%-11s]\033[0m : execute \033[33m[make fclean]\033[0m and \033[33m[make]\033[0m\n" "make re"
	@printf "\033[34m[WEBSERV]\033[0m \033[35m[%-11s]\033[0m : \033[33m[make]\033[0m and execute ./webserv.\n" "make run"
	@printf "\033[34m[WEBSERV]\033[0m               : \033[0m\033[4m./webserv nginx.conf mime.types status_code.txt\033[0m\n"

all:
	@$(NAME)

run:
	@make
	@./webserv $(CONF) $(MIME)

clean:
	@printf "\033[34m[WEBSERV]\033[0m Removing object : "
	@rm -rf $(OBJS)
	@sleep 0.08 && printf "." && sleep 0.08 && printf "." && sleep 0.08 && printf ". "
	@printf "\033[32mOK\033[0m\n"

fclean:
	@make clean
	@printf "\033[34m[WEBSERV]\033[0m Removing result : "
	@rm -rf $(NAME)
	@sleep 0.08 && printf "." && sleep 0.08 && printf "." && sleep 0.08 && printf ". "
	@printf "\033[32mOK\033[0m\n"

re:
	@make fclean
	@printf "\033[34m[WEBSERV]\033[0m\n"
	@make
