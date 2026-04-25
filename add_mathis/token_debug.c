/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 14:40:07 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/24 22:52:58 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>

static const char	*type_name(t_tokentype t)
{
	if (t == T_WORD)
		return ("WORD");
	if (t == T_PIPE)
		return ("PIPE");
	if (t == T_REDIR_IN)
		return ("<");
	if (t == T_REDIR_OUT)
		return (">");
	if (t == T_REDIR_APPEND)
		return (">>");
	return ("<<");
}

void	print_tokens(t_token *lst)
{
	while (lst)
	{
		if (lst->str)
			printf("[%s] \"%s\"\n", type_name(lst->type), lst->str);
		else
			printf("[%s]\n", type_name(lst->type));
		lst = lst->next;
	}
}
