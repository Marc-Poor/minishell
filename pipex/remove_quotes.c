/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   remove_quotes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 17:40:47 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/12 18:54:35 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	check_quotes(char *str)
{
	int	i;
	int	quotes;

	i = 0;
	quotes = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			quotes++;
		i++;
	}
	return (quotes);
}

char	*ft_strdup(char *src)
{
	char	*dup;
	int		i;

	dup = malloc(sizeof(char) * ft_strlen(src) + 1);
	i = 0;
	if (dup == NULL)
		return (NULL);
	while (src[i] != '\0')
	{
		dup[i] = src[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	*remove_quotes(char *str, int i)
{
	int		j;
	char	*new_str;

	if (!str)
		return (NULL);
	if (check_quotes(str) == 0)
		return (ft_strdup(str));
	new_str = malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	j = 0;
	while (str[i])
	{
		if (str[i] != '\'')
		{
			new_str[j] = str[i];
			j++;
		}
		i++;
	}
	if (!new_str)
		return (NULL);
	new_str[j] = '\0';
	return (new_str);
}
