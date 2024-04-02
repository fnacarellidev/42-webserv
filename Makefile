NAME = webserv

PATH_INCS = ./
PATH_SRCS = ./
PATH_OBJS = ./objects/

FILES = main
SRCS = ${FILES:%=$(PATH_OBJS)%.cpp}
OBJS = ${FILES:%=$(PATH_OBJS)%.o}
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

all: $(NAME)

$(NAME): $(OBJS)
	c++ $(FLAGS) $(OBJS) -o $(NAME)

$(PATH_OBJS)%.o: $(PATH_SRCS)%.cpp | $(PATH_OBJS)
	c++ $(FLAGS) -c $< -o $@

$(PATH_OBJS):
	mkdir -p $(PATH_OBJS)

clean:
	rm -rf $(PATH_OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
