# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/03 20:11:23 by mathissegui       #+#    #+#              #
#    Updated: 2026/04/21 19:10:17 by mfaure           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC     =	main.c			\
			lexer.c			\
			lexer_quotes.c	\
			expand.c		\
			parser_debug.c	\
			parser_free.c	\
			parser_utils.c	\
			parser.c		\
			proces.c		\
			syntax.c		\
			token_debug.c	\
			token.c			\
			../pipex/close_all.c	\
			../execute.c			\
			../pipex/ft_split.c		\
			../pipex/find_path.c		\
			../pipex/ft_strcat.c		\
			../pipex/ft_strncmp.c	\
			../pipex/ft_strncpy.c	\
			../ft_cd.c	\
			../ft_export.c	\
			../ft_unset.c	\
			../ft_exit.c	\
			../ft_env.c		\
			../ft_pwd.c		\
			../ft_echo.c

OBJ		=	$(SRC:.c=.o)

NAME	=	minishell

LIBFTDIR	= 	libft
LIBFT		=	$(LIBFTDIR)/libft.a
LDFLAGS		=	-L$(LIBFTDIR)
LDLIBS		=	-lreadline -lncurses

all:	$(NAME)

$(NAME): $(LIBFT) $(OBJ)
	gcc $(OBJ) $(LIBFT) $(LDLIBS) -o $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

%.o:	%.c
		gcc -g3 -std=gnu17 -Wall -Wextra -Werror -O3 -c $< -o $@


clean:
	rm -f $(OBJ)
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all bonus clean fclean re