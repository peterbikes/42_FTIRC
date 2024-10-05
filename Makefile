NAME= ircserv

CXX= c++
CXXFLAGS= -Wall -Werror -Wextra -std=c++98 -pedantic
MEM= valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes

ifdef DEBUG
	CXXFLAGS+= -ggdb
else ifdef SANITIZE
	CXXFLAGS+= -fsanitize=address -ggdb
endif

#DIRS
INCS = ./includes
SRCS_DIR = ./srcs
OBJS_DIR = ./objs


SRCF = Server Client Channel utils

SRCS = $(addprefix $(SRCS_DIR)/, $(addsuffix .cpp, $(SRCF)))
OBJS = $(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(SRCF)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) main.cpp $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

debug:
	make re DEBUG=1

mem:
	make re DEBUG=1
	$(MEM) ./$(NAME) 6667 password

san:
	clear
	make re SANITIZE=1
	./$(NAME) 6667 password

run: fclean all
	clear
	@./$(NAME) 6667 password
