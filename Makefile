NAME = webserv

PATH_INCS = ./includes/
PATH_SRCS = ./srcs/
PATH_OBJS = ./objects/

CONFIGURATION_FILES = $(addprefix configs/, WebServer validateConfig RouteConfig ServerConfig staticFunctions addRoutes addServers)
UTILS_FILES = $(addprefix utils/, fileInfo pathInfo split strdup strEndsWith time trim)
FILES = main Response Request $(CONFIGURATION_FILES) $(UTILS_FILES)
SRCS = ${FILES:%=$(PATH_OBJS)%.cpp}
OBJS = ${FILES:%=$(PATH_OBJS)%.o}
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3
VAL_FLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=val 

all: $(NAME)

$(NAME): $(OBJS)
	c++ $(FLAGS) $(OBJS) -o $(NAME)

$(PATH_OBJS)%.o: $(PATH_SRCS)%.cpp $(wildcard $(PATH_INCS)*.hpp) | $(PATH_OBJS)
	@mkdir -p $(@D)
	c++ $(FLAGS) -c $< -o $@

$(PATH_OBJS):
	mkdir -p $(PATH_OBJS)

clean:
	rm -rf $(PATH_OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug re_debug

val: $(NAME)
	valgrind $(VAL_FLAGS) ./$(NAME) ./arquivo.conf
.PHONY: val
