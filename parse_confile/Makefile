NAME = webserver

SRC = cgi.cpp  location.cpp  main.cpp  config_file.cpp  server.cpp

OBJECT = $(SRC:.cpp=.o)


all : $(NAME)

$(NAME):
	@c++ -std=c++98 -Wall -Wextra -Werror $(SRC) -o $(NAME)


clean:
	@rm -f $(OBJECT)

fclean: clean
	@rm -f $(NAME)

re: fclean all
