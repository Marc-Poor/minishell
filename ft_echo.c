/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 14:53:54 by mfaure            #+#    #+#             */
/*   Updated: 2026/01/29 14:59:13 by mfaure           ###   ########.fr       */
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

int main(int ac, char **av, char **env)
{
	if (ac == 3)
		ft_echo(av[2], 0);
	ft_echo(av[1], 1);
	return 0;
}