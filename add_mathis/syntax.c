/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 16:55:55 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/26 23:13:07 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>

static int	is_redir(t_tokentype type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT || type == T_REDIR_APPEND
		|| type == T_HEREDOC);
}

static int	print_pipe_error(void)
{
	printf("syntax error near unexpected token `|'\n");
	return (1);
}

static int	check_pipe_syntax(t_token *cur)
{
	if (cur->type != T_PIPE)
		return (0);
	if (!cur->next)
		return (print_pipe_error());
	if (cur->next->type == T_PIPE)
		return (print_pipe_error());
	return (0);
}

static int	check_redir_syntax(t_token *cur)
{
	if (!is_redir(cur->type))
		return (0);
	if (!cur->next || cur->next->type != T_WORD)
	{
		printf("syntax error near redirection\n");
		return (1);
	}
	return (0);
}

int	check_syntax(t_token *lst)
{
	t_token	*cur;

	if (!lst)
		return (0);
	cur = lst;
	if (cur->type == T_PIPE)
		return (print_pipe_error());
	while (cur)
	{
		if (check_pipe_syntax(cur))
			return (1);
		if (check_redir_syntax(cur))
			return (1);
		cur = cur->next;
	}
	return (0);
}

/*
static int	is_redir(t_tokentype type)
{
	return (type == T_REDIR_IN
		|| type == T_REDIR_OUT
		|| type == T_REDIR_APPEND
		|| type == T_HEREDOC);
}

int	check_syntax(t_token *lst)
{
	t_token	*cur;

	if (!lst)
		return (0);
	cur = lst;
	if (cur->type == T_PIPE)
	{
		printf("syntax error near unexpected token `|'\n");
		return (1);
	}
	while (cur)
	{
		if (cur->type == T_PIPE && !cur->next)
		{
			printf("syntax error near unexpected token `|'\n");
			return (1);
		}
		if (cur->type == T_PIPE && cur->next
			&& cur->next->type == T_PIPE)
		{
			printf("syntax error near unexpected token `|'\n");
			return (1);
		}
		if (is_redir(cur->type))
		{
			if (!cur->next || cur->next->type != T_WORD)
			{
				printf("syntax error near redirection\n");
				return (1);
			}
		}
		cur = cur->next;
	}
	return (0);
}
*/