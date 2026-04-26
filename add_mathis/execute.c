/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 16:52:58 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/26 20:13:55 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex/pipex.h"
#include <readline/history.h>
#include <readline/readline.h>

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

int	handle_heredoc(char *delimiter)
{
	int		fd[2];
	char	*line;
	int		print;

	if (pipe(fd) == -1)
		exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		print = write(fd[1], line, strlen(line));
		print = write(fd[1], "\n", 1);
		if (print == -1)
			printf("fuck Werror\n");
		free(line);
	}
	close(fd[1]);// important
	return (fd[0]);
}// return read end

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
	if (strchr(cmd[0], '/'))
	{
		execve(cmd[0], cmd, env);
		perror(cmd[0]);
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
	t_redir	*r;

	if (!env || !env[0])
	{
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
		if (r->type == T_HEREDOC)
		{
			if (p->infile != -1)
				close(p->infile);
			p->infile = handle_heredoc(r->file);
		}
		else if (r->type == T_REDIR_OUT || r->type == T_REDIR_APPEND)
		{
			if (r->type == T_REDIR_OUT)
				p->outfile = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			else
				p->outfile = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
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
		p->outfile = open(cmds->redirs->file, O_WRONLY | O_CREAT | O_TRUNC,
				0644);
		if (p->outfile < 0)
		{
			close(p->infile);
			perror("infile or outfile");
			exit(1);
		}
	}
*/

void	ft_dup(t_pipex *p, t_cmd *cmds, int is_first)
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

int	end_it(t_pipex *p)
{
	waitpid(p->pid1, &p->wstatus, 0);
	if (!WIFEXITED(p->wstatus))
		return (-1);
	return (0);
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

char	**exec_parent_built_in(t_cmd *curr, char **env)
{
	if (strcmp(curr->argv[0], "cd") == 0)
		return (ft_cd(curr->argv, env));
	if (strcmp(curr->argv[0], "export") == 0)
		return (ft_export_main(curr->argv, env));
	if (strcmp(curr->argv[0], "unset") == 0)
		return (ft_unset(curr->argv, env));
	if (strcmp(curr->argv[0], "exit") == 0)
		ft_exit(curr->argv);
	return (env);
}

int	exec_child_builtin(t_cmd *curr, char **env)
{
	if (!curr->argv[0])
		return (0);
	if (strcmp(curr->argv[0], "echo") == 0)
		return (main_ft_echo(curr->argv));
	if (strcmp(curr->argv[0], "pwd") == 0)
		return (ft_pwd(env));
	if (strcmp(curr->argv[0], "env") == 0)
		return (ft_env(env));
	return (-1);
}

int	execute(t_cmd *cmds, t_shell *shell)
{
	t_pipex	p;
	int		prev_fd;
	t_cmd	*curr;
	char	**tmp;
	int		ret;
	int		status;

	prev_fd = -1;
	curr = cmds;
	while (curr)
	{
		p.infile = -1;
		p.outfile = -1;
		if (curr && !curr->next && prev_fd == -1 && curr->argv[0])
		{
			if (strcmp(curr->argv[0], "cd") == 0 || strcmp(curr->argv[0],
					"export") == 0 || strcmp(curr->argv[0], "unset") == 0
				|| strcmp(curr->argv[0], "exit") == 0)
			{
				tmp = exec_parent_built_in(curr, shell->env);
				if (tmp == NULL)
					return (0);
				shell->env = tmp;
				return (0);
			}
		}
		open_p(&p, curr, shell->env);
		// create pipe only if needed
		if (curr->next)
		{
			if (pipe(p.pipefd) == -1)
				exit(1);
		}
		p.pid = fork();
		if (p.pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			// ---------- INPUT ----------
			if (prev_fd != -1)
				dup2(prev_fd, STDIN_FILENO);
			if (curr->redirs && (curr->redirs->type == T_REDIR_IN
					|| curr->redirs->type == T_HEREDOC))
				dup2(p.infile, STDIN_FILENO);
			// ---------- OUTPUT ----------
			if (curr->next)
				dup2(p.pipefd[1], STDOUT_FILENO);
			else if (curr->redirs && curr->redirs->type == T_REDIR_OUT)
				dup2(p.outfile, STDOUT_FILENO);
			else if (curr->redirs && curr->redirs->type == T_REDIR_APPEND)
				dup2(p.outfile, STDOUT_FILENO);
			// ---------- CLOSE FDS ----------
			close_fds_child(prev_fd, &p, curr);
			if ((ret = exec_child_builtin(curr, shell->env)) != -1)
				exit(ret);
			exec_cmd(curr->argv, shell->env);
			exit(1);
		}
		// ---------- PARENT ----------
		if (prev_fd != -1)
			close(prev_fd);
		if (curr->next)
		{
			close(p.pipefd[1]); // parent keeps read end
			prev_fd = p.pipefd[0];
		}
		curr = curr->next;
	}
	// ---------- WAIT ----------
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
		return (1);
	return (0);
}

*/