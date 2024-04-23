NAME = webserv

PATH_INCS = ./includes/
PATH_SRCS = ./srcs/
PATH_OBJS = ./objects/

CONFIGURATION_FILES = $(addprefix configs/, WebServer validateConfig RouteConfig ServerConfig staticFunctions addRoutes addServers)
FILES = main Response $(CONFIGURATION_FILES) utils/fileInfo utils/time utils/split utils/trim utils/strEndsWith utils/pathInfo Request
SRCS = ${FILES:%=$(PATH_OBJS)%.cpp}
OBJS = ${FILES:%=$(PATH_OBJS)%.o}
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

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

debug:
	c++ $(FLAGS) $(wildcard srcs/configs/*.cpp) $(wildcard srcs/utils/*.cpp) srcs/Request.cpp srcs/main_test.cpp -o debug_server

re_debug:
	rm -rf debug_server
	make debug
