
NAME = webserv

CFLAGS = -Wall -Wextra -Werror 

CC = c++

FIL_ = webserv.cpp request.cpp response.cpp Client.cpp Statucode.cpp Cgi.cpp Header.cpp


OBJ = $(FIL:.c=.o)

FIL = $(addprefix srcs/, $(FIL_))

all: $(NAME)

$(NAME): $(FIL)
	$(CC) $(CFLAGS)  $(FIL) parse_confile/*.cpp -o $(NAME)

run1: $(NAME)
	./webserv ./conf_file/1.config

run: $(NAME)
	./webserv dflt.config

run2: $(NAME)
	./webserv 2.config

fclean:
	rm -f $(NAME)

re: fclean all

me:
	@echo $(NAME)
	@echo "#    #  ####   ####  #####   #    #  ####   ####  #####  "
	@echo "##   # #    # #    # #    #  ##   # #    # #    # #    # "
	@echo "# #  # #    # #    # #####   # #  # #    # #    # #####  "
	@echo "#  # # #    # #    # #    #  #  # # #    # #    # #    # "
	@echo "#   ## #    # #    # #    #  #   ## #    # #    # #    # "
	@echo "#    #  ####   ####  #####   #    #  ####   ####  #####  "
