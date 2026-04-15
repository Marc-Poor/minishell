/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 16:55:55 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/11 18:15:32 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>

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
