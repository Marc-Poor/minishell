/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_debug.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 02:19:38 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/12 02:47:17 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>

static const char	*redir_name(t_tokentype type)
{
	if (type == T_REDIR_IN)
		return ("<");
	if (type == T_REDIR_OUT)
		return (">");
	if (type == T_REDIR_APPEND)
		return (">>");
	if (type == T_HEREDOC)
		return ("<<");
	return ("?");
}

void	print_cmds(t_cmd *cmds)
{
	t_redir	*r;
	int		i;

	while (cmds)
	{
		printf("=== CMD ===\n");
		i = 0;
		while (cmds->argv && cmds->argv[i])
		{
			printf("argv[%d] = \"%s\"\n", i, cmds->argv[i]);
			i++;
		}
		r = cmds->redirs;
		while (r)
		{
			printf("redir %s \"%s\"\n", redir_name(r->type), r->file);
			r = r->next;
		}
		cmds = cmds->next;
	}
}