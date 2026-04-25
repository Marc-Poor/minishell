/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 14:53:54 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/24 22:44:15 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex/pipex.h"

int	is_n_flag(char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] == '\0')
		return (0);
	i = 1;
	while (s[i])
	{
		if (s[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	main_ft_echo(char **av)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (av[i] && is_n_flag(av[i]))
	{
		newline = 0;
		i++;
	}
	while (av[i])
	{
		printf("%s", av[i]);
		if (av[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}
