/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 02:10:00 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/24 22:51:33 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redir	*redir_new(t_tokentype type, char *file)
{
	t_redir	*r;

	r = malloc(sizeof(t_redir));
	if (!r)
		return (NULL);
	r->type = type;
	r->file = file;
	r->next = NULL;
	return (r);
}

void	redir_add_back(t_redir **lst, t_redir *new_redir)
{
	t_redir	*cur;

	if (!lst || !new_redir)
		return ;
	if (!*lst)
	{
		*lst = new_redir;
		return ;
	}
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = new_redir;
}
