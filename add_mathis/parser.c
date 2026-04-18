/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 02:11:01 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/15 18:10:46 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft/libft.h"

static int	count_args_until_pipe(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != T_PIPE)
	{
		if (tok->type == T_WORD)
			count++;
		else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_REDIR_APPEND || tok->type == T_HEREDOC)
			tok = tok->next;
		tok = tok->next;
	}
	return (count);
}

static t_cmd	*cmd_new(int argc)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = malloc(sizeof(char *) * (argc + 1));
	if (!cmd->argv)
		return (free(cmd), NULL);
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	fill_cmd(t_cmd *cmd, t_token *tok)
{
	int	i;

	i = 0;
	while (tok && tok->type != T_PIPE)
	{
		if (tok->type == T_WORD)
			cmd->argv[i++] = ft_strdup(tok->str);
		else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_REDIR_APPEND || tok->type == T_HEREDOC)
		{
			if (tok->next && tok->next->type == T_WORD)
				redir_add_back(&cmd->redirs,
					redir_new(tok->type, ft_strdup(tok->next->str)));
			tok = tok->next;
		}
		tok = tok->next;
	}
	cmd->argv[i] = NULL;
}

static void	cmd_add_back(t_cmd **lst, t_cmd *new_cmd)
{
	t_cmd	*cur;

	if (!lst || !new_cmd)
		return ;
	if (!*lst)
	{
		*lst = new_cmd;
		return ;
	}
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = new_cmd;
}

t_cmd	*parse_commands(t_token *tokens)
{
	t_cmd	*cmds;
	t_cmd	*new_cmd;
	t_token	*start;
	int		argc;

	cmds = NULL;
	start = tokens;
	while (start)
	{
		argc = count_args_until_pipe(start);
		new_cmd = cmd_new(argc);
		if (!new_cmd)
			return (NULL);
		fill_cmd(new_cmd, start);
		cmd_add_back(&cmds, new_cmd);
		while (start && start->type != T_PIPE)
			start = start->next;
		if (start && start->type == T_PIPE)
			start = start->next;
	}
	return (cmds);
}