
NAME = webserv

CFLAGS = -Wall -Wextra -Werror

CC = c++

FIL_ = *.cpp


OBJ = $(FIL:.c=.o)

FIL = $(addprefix ./, $(FIL_))

all: $(NAME)

$(NAME): $(FIL)
	$(CC)  $(FIL) -o $(NAME)

run1: $(NAME)
	./webserv 1.config

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