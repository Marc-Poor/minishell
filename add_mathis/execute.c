/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 16:52:58 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/27 00:50:38 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex/pipex.h"
#include <readline/history.h>
#include <readline/readline.h>

static int	handle_fork(t_cmd *curr, t_shell *shell, t_pipex *p, int prev_fd)
{
	if (curr->next && pipe(p->pipefd) == -1)
		exit(1);
	p->pid = fork();
	if (p->pid == 0)
		exec_child(curr, shell, p, prev_fd);
	return (0);
}

static void	update_parent_fds(t_cmd *curr, t_pipex *p, int *prev_fd)
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (curr->next)
	{
		close(p->pipefd[1]);
		*prev_fd = p->pipefd[0];
	}
}

static void	wait_all(t_shell *shell)
{
	int	status;

	while (wait(&status) > 0)
	{
		if (WIFEXITED(status))
			shell->last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGINT)
				printf("\n");
			else if (WTERMSIG(status) == SIGQUIT)
				printf("Quit: 3\n");
			shell->last_status = 128 + WTERMSIG(status);
		}
	}
	setup_signals();
}

void	setupp(t_pipex *p)
{
	p->infile = -1;
	p->outfile = -1;
}

int	execute(t_cmd *cmds, t_shell *shell)
{
	t_pipex	p;
	t_cmd	*curr;

	shell->prev_fd = -1;
	curr = cmds;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	while (curr)
	{
		setupp(&p);
		if (!curr->next && shell->prev_fd == -1)
		{
			shell->ret = handle_parent_builtin(curr, shell);
			if (shell->ret == 1)
				return (1);
			if (shell->ret == 2)
				return (0);
		}
		open_p(&p, curr, shell->env);
		handle_fork(curr, shell, &p, shell->prev_fd);
		update_parent_fds(curr, &p, &shell->prev_fd);
		curr = curr->next;
	}
	wait_all(shell);
	return (0);
}

/*
int	end_it(t_pipex *p)
{
	waitpid(p->pid1, &p->wstatus, 0);
	if (!WIFEXITED(p->wstatus))
		return (-1);
	return (0);
}

void	ft_dup(t_pipex *p, t_cmd *cmds, int is_first)
{
	if (!is_first)
		dup2(p->pipefd[0], STDIN_FILENO);
	if (cmds->redirs && cmds->redirs->type == T_REDIR_IN)
		dup2(p->infile, STDIN_FILENO);
	if (cmds->next)
		dup2(p->pipefd[1], STDOUT_FILENO);
	else if (cmds->redirs && cmds->redirs->type == T_REDIR_OUT)
		dup2(p->outfile, STDOUT_FILENO);
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
		ft_dup(&p, cmds, 0);
		close_all(p.pipefd, p.infile, p.outfile);
		exec_cmd(cmds->argv, env);
	}
	if (!cmds2)
		return (end_it(&p));
	open_p(&p, cmds2, env);
	p.pid2 = fork();
	if (p.pid2 == 0)
	{
		ft_dup(&p, cmds2, 1);
		close_all(p.pipefd, p.infile, p.outfile);
		//printf("inside\n");
		exec_cmd(cmds2->argv, env);
	}
	close_all(p.pipefd, p.infile, p.outfile);
	waitpid(p.pid1, &p.wstatus, 0);
	if (!WIFEXITED(p.wstatus))
		return (-1);
	waitpid(p.pid2, NULL, 0);
	if (cmds2 == NULL)
		return (1);
	return (0);
}

*/