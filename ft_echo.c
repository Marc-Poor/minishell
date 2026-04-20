/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 14:53:54 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/20 18:11:24 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex/pipex.h"

void ft_echo(char *s, int i)
{
	if (i == 0)
	{
		printf("%s%%\n", s);
		return ;
	}
	printf("%s\n", s);
}

int main_ft_echo(char **av)
{
	if (ft_strncmp(av[1], "-n", ft_strlen(av[1])) == 0)
		ft_echo(av[2], 0);
	ft_echo(av[1], 1);
	return 0;
}