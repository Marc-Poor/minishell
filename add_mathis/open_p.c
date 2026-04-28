/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_p.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 22:21:01 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/28 17:44:00 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex/pipex.h"
#include <readline/history.h>
#include <readline/readline.h>

static void	handle_input(t_pipex *p, t_redir *r)
{
	if (r->type == T_REDIR_IN)
	{
		if (p->infile != -1)
			close(p->infile);
		p->infile = open(r->file, O_RDONLY);
		if (p->infile < 0)
		{
			perror("infile");
			exit(1);
		}
	}
	else if (r->type == T_HEREDOC)
	{
		if (p->infile != -1)
			close(p->infile);
		p->infile = handle_heredoc(r->file);
	}
}

static void	handle_output(t_pipex *p, t_redir *r)
{
	if (r->type != T_REDIR_OUT && r->type != T_REDIR_APPEND)
		return ;
	if (p->outfile != -1)
		close(p->outfile);
	if (r->type == T_REDIR_OUT)
		p->outfile = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (r->type == T_REDIR_APPEND)
		p->outfile = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return ;
	if (p->outfile < 0)
	{
		perror("outfile");
		exit(1);
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
		handle_input(p, r);
		handle_output(p, r);
		r = r->next;
	}
}

/*
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
}*/