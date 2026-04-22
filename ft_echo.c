/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 14:53:54 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/22 15:25:06 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex/pipex.h"

void ft_echo(char **av, int x, int i)
{
	while(av[x])
	{
		if (i == 0)
		{
			printf("%s", av[x]);
		} else {
			printf("%s", av[x]);
		}
		if (av[x + 1])
			printf(" ");
		x++;
	}
}

int main_ft_echo(char **av)
{
	if (ft_strncmp(av[1], "-n", ft_strlen(av[1])) == 0) {
		ft_echo(av, 2, 0);
		return 0;
	}
	ft_echo(av, 1, 1);
	printf("\n");
	return 0;
}