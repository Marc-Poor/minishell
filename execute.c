/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 16:52:58 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/20 16:01:42 by mfaure           ###   ########.fr       */
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
		printf(" : Command not found\n");
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

	if (!env || !env[0]) {
		printf("exit\n");
		exit(1);
	}
	if (!cmds || !cmds->redirs)
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

void ft_dup(t_pipex *p, t_cmd *cmds, int is_first)
{
	// INPUT
	if (!is_first)
		dup2(p->pipefd[0], STDIN_FILENO);

	if (cmds->redirs && cmds->redirs->type == T_REDIR_IN)
		dup2(p->infile, STDIN_FILENO);

	// OUTPUT
	if (cmds->next)
		dup2(p->pipefd[1], STDOUT_FILENO);
	else if (cmds->redirs && cmds->redirs->type == T_REDIR_OUT)
		dup2(p->outfile, STDOUT_FILENO);
}

int end_it(t_pipex *p)
{
	waitpid(p->pid1, &p->wstatus, 0);
	if (!WIFEXITED(p->wstatus))
		return (-1);
	return 0;	
}

static void	close_fds_child(int prev_fd, t_pipex *p, t_cmd *cmd)
{
	if (prev_fd != -1)
		close(prev_fd);

	if (cmd->next)
	{
		close(p->pipefd[0]);
		close(p->pipefd[1]);
	}

	if (p->infile != -1)
		close(p->infile);
	if (p->outfile != -1)
		close(p->outfile);
}

int	execute(t_cmd *cmds, char **env)
{
	t_pipex	p;
	int		prev_fd;
	t_cmd	*curr;

	prev_fd = -1;
	curr = cmds;
	while (curr)
	{
		p.infile = -1;
		p.outfile = -1;
		open_p(&p, curr, env);
		// create pipe only if needed
		if (curr->next)
		{
			if (pipe(p.pipefd) == -1)
				exit(1);
		}
		p.pid = fork();
		if (p.pid == 0)
		{
			// ---------- INPUT ----------
			if (prev_fd != -1)
				dup2(prev_fd, STDIN_FILENO);
			if (curr->redirs && curr->redirs->type == T_REDIR_IN)
				dup2(p.infile, STDIN_FILENO);
			// ---------- OUTPUT ----------
			if (curr->next)
				dup2(p.pipefd[1], STDOUT_FILENO);
			else if (curr->redirs && curr->redirs->type == T_REDIR_OUT)
				dup2(p.outfile, STDOUT_FILENO);
			// ---------- CLOSE FDS ----------
			close_fds_child(prev_fd, &p, cmds);
			exec_cmd(curr->argv, env);
			exit(1);
		}
		// ---------- PARENT ----------
		if (prev_fd != -1)
			close(prev_fd);
		if (curr->next)
		{
			close(p.pipefd[1]);      // parent keeps read end
			prev_fd = p.pipefd[0];
		}
		curr = curr->next;
	}
	// ---------- WAIT ----------
	while (wait(NULL) > 0)
		;
	return (0);
}

/*

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
		return 1;
	return (0);
}

*/