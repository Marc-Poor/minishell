
NAME 		= 	libft.a

SRCS 		=	ft_case.c \
				ft_char_utils.c \
				ft_convert.c \
				ft_mem_basic.c \
       			ft_mem_extra.c \
				ft_print_fd.c \
				ft_split.c \
				ft_string_tools_basic.c \
				ft_string_tools_extra.c \
       			ft_strn_utils.c 

BONUS_SRCS	=	ft_lst_basics_bonus.c \
				ft_lst_add_bonus.c \
				ft_lst_delete_bonus.c \
				ft_lst_apply_bonus.c

ifneq ($(filter bonus,$(MAKECMDGOALS)),)
SRCS := $(SRCS) $(BONUS_SRCS)
endif

OBJS		=	$(SRCS:.c=.o)

BONUS_OBJS	= 	$(BONUS_SRCS:.c=.o)

CC 			= 	cc
CFLAGS 		= 	-Wall -Wextra -Werror
AR 			=	ar rcs

all: $(NAME)

bonus: $(NAME)

$(NAME): $(OBJS)
	$(AR) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS) $(BONUS_OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re 
