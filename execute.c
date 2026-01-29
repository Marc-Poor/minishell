/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 16:52:58 by mfaure            #+#    #+#             */
/*   Updated: 2026/01/28 16:35:33 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex/pipex.h"

void	close_fd(int pipefd[2])
{
	if (pipefd[0])
		close(pipefd[0]);
	if (pipefd[1])
		close(pipefd[1]);
}

void	free_command(char **tab)
{
	int	i;

	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

void	open_p(char **env)
{
	if (!env || !env[0])
		exit(1);
}

void	exec_cmd(char **cmd, char **env)
{
	char	*path;

	if (!cmd)
	{
		perror("Invalid command: cmd_str");
		exit(127);
	}
	if (!cmd[0])
	{
		free(cmd);
		exit(127);
	}
	path = find_path(env, cmd[0], 0);
	if (!path || path[0] == '\0')
	{
		ft_putstr(" : Command not found\n");
		free_command(cmd);
		exit(127);
	}
	execve(path, cmd, env);
	perror("execve failed");
	exit(EXIT_FAILURE);
}

int execute(char **av, char **env)
{
	t_pipex	p;

	open_p(env);
	if (pipe(p.pipefd) == -1)
		exit(1);
	p.pid1 = fork();
	if (p.pid1 == 0)
	{
		close_fd(p.pipefd);
		exec_cmd(av, env);
	}
	close_fd(p.pipefd);
	waitpid(p.pid1, &p.wstatus, 0);
	if (!WIFEXITED(p.wstatus))
		return (-1);
	return (0);
}

int main(int ac, char **av, char **env)
{
	if (ac < 2)
		return (0);
	execute(av++, env);
	return (0);
}

/*

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

*/