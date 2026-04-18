/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 16:52:58 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/18 20:34:28 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex/pipex.h"
#include "add_mathis/minishell.h"

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
	exit(1);
}

void	open_p(t_pipex *p, t_cmd *cmds, char **env)
{
	t_redir *r;

	p->piped = 0;
	if (!cmds || !env || !env[0])
		exit(1);
	if (!cmds->redirs)
		return ;
	r = cmds->redirs;
	while (r)
	{
		if (r->type == T_REDIR_IN)
		{
			p->infile = open(r->file, O_RDONLY);
			if (p->infile < 0)
			{
				perror("infile");
				exit(1);
			}
		}
		else if (r->type == T_REDIR_OUT)
		{
			p->outfile = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (p->outfile < 0)
			{
				perror("outfile");
				exit(1);
			}
		}
		r = r->next;
	}
}

/*.
	if (cmds->redirs->type == T_REDIR_IN)
	{
		p->infile = open(cmds->redirs->file, O_RDONLY);
		if (p->infile < 0)
		{
			perror("infile or outfile");
			exit(1);
		}
	}
	if (cmds->redirs->type == T_REDIR_OUT)
	{
		p->outfile = open(cmds->redirs->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (p->outfile < 0)
		{
			close(p->infile);
			perror("infile or outfile");
			exit(1);
		}
	}
*/

void	ft_dup(t_pipex p, t_cmd *cmds)
{
	if (cmds->redirs->type == T_REDIR_IN) {
		dup2(p.infile, STDIN_FILENO);
	}
	if (cmds->redirs->type == T_REDIR_OUT)
		dup2(p.outfile, STDOUT_FILENO);
	if (cmds->next)
	{
		dup2(p.pipefd[1], STDOUT_FILENO);
		p.piped = 1;
		return ;
	}
	if (p.piped == 1) 
	{
		dup2(p.pipefd[0], STDIN_FILENO);
		p.piped = 0;
	}
}

int	execute(t_cmd *cmds, t_cmd *cmds2, char **env)
{
	t_pipex	p;

	p.infile = -1;
	p.outfile = -1;
	open_p(&p, cmds, env);
	if (pipe(p.pipefd) == -1)
		exit(1);
	p.pid1 = fork();
	if (p.pid1 == 0)
	{
		if (cmds->redirs)
			ft_dup(p, cmds);
		close_all(p.pipefd, p.infile, p.outfile);
		exec_cmd(cmds->argv, env);
	}
	open_p(&p, cmds2, env);
	printf("inside\n");
	p.pid2 = fork();
	if (p.pid2 == 0)
	{
		ft_dup(p, cmds2);
		close_all(p.pipefd, p.infile, p.outfile);
		printf("inside\n");
		exec_cmd(cmds2->argv, env);
	}
	close_all(p.pipefd, p.infile, p.outfile);
	waitpid(p.pid1, &p.wstatus, 0);
	if (!WIFEXITED(p.wstatus))
		return (-1);
	//waitpid(p.pid2, NULL, 0);
	if (cmds2 == NULL)
		return 1;
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