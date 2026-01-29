# ==============================
# so_long Makefile
# ==============================

SRC     =	execute.c			\
			pipex/ft_split.c		\
			pipex/find_path.c		\
			pipex/ft_putstr.c	\
			pipex/ft_strcat.c		\
			pipex/ft_strlen.c		\
			pipex/ft_strncmp.c	\
			pipex/ft_strncpy.c	\

OBJ		=	$(SRC:.c=.o)

NAME	=	minishell

all:	$(NAME)

$(NAME):	$(OBJ)
			cc $(OBJ) -o $(NAME)

%.o:	%.c
		cc -g -std=gnu17 -Wall -Wextra -Werror -O3 -c $< -o $@


clean:
	rm -f $(OBJ)
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all bonus clean fclean re
