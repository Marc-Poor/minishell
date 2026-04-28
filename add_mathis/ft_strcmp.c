/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 20:18:21 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/28 20:24:10 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	count;

	count = 0;
	while (s1[count] || s2[count])
	{
		if (s1[count] != s2[count])
			return ((unsigned char) s1[count] - (unsigned char) s2[count]);
		count++;
	}
	return (0);
}
