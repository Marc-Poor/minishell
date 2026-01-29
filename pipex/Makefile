# ==============================
# so_long Makefile
# ==============================

SRC     =	main.c			\
			ft_strcat.c		\
			ft_strlen.c		\
			ft_putstr.c		\
			ft_strncmp.c	\
			ft_strncpy.c	\
			ft_split.c		\
			close_all.c		\
			find_path.c		\
			remove_quotes.c	\
			ft_realloc_str.c

OBJ		=	$(SRC:.c=.o)

NAME	=	pipex

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
