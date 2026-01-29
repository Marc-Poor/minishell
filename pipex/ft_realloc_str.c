/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc_str.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:46:32 by mfaure            #+#    #+#             */
/*   Updated: 2025/10/17 18:11:06 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*ft_realloc_str(char *str, size_t new_len)
{
	char	*new_str;
	size_t	i;

	i = 0;
	new_str = malloc(new_len + 1);
	if (!new_str)
		return (NULL);
	if (str)
	{
		while (str[i] && i < new_len)
		{
			new_str[i] = str[i];
			i++;
		}
		free(str);
	}
	new_str[i] = '\0';
	return (new_str);
}
