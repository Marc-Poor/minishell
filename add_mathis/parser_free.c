/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 02:36:53 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/12 02:38:21 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirs(t_redir **redirs)
{
	t_redir	*tmp;

	if (!redirs)
		return ;
	while (*redirs)
	{
		tmp = (*redirs)->next;
		free((*redirs)->file);
		free(*redirs);
		*redirs = tmp;
	}
}

void	free_cmds(t_cmd **cmds)
{
	t_cmd	*tmp;
	int		i;

	if (!cmds)
		return ;
	while (*cmds)
	{
		tmp = (*cmds)->next;
		if ((*cmds)->argv)
		{
			i = 0;
			while ((*cmds)->argv[i])
			{
				free((*cmds)->argv[i]);
				i++;
			}
			free((*cmds)->argv);
		}
		free_redirs(&(*cmds)->redirs);
		free(*cmds);
		*cmds = tmp;
	}
}