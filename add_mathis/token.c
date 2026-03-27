/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 15:57:12 by mseguin           #+#    #+#             */
/*   Updated: 2026/03/17 18:56:24 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*new_token(t_tokentype type, char *str)
{
	t_token	*t;
	
	t = malloc(sizeof(t_token));
	if(!t)
		return (NULL);
	t->type = type;
	t->str = str;
	t->next = NULL;
	return (t);
}

void add_token(t_token **list, t_token *new)
{
	t_token	*cursor;
	
	if (!list || !new)
		return ;
	if (!*list)
	{
		*list = new;
		return	;
	}
	cursor = *list;
	while (cursor->next)
		cursor = cursor->next;
	cursor->next = new;	
}

void clear_token(t_token **list)
{
	t_token *temporary;
	
	if (!list)
		return ;
	while (*list)
	{
		temporary = (*list)->next;
		free((*list)->str);
		free(*list);
		*list = temporary;
	}
}