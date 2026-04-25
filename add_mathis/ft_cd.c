/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 22:44:38 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/24 22:44:39 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include "minishell.h"

int	ft_cd(char **av, char **env)
{
	if (!av[1])
	{
		printf("cd: missing argument\n");
		return (1);
	}
	if (av[2])
	{
		printf("cd: too many arguments\n");
		return (1);
	}
	if (strcmp(av[1], "-") == 0)
	{
		ft_pwd(env);
		return (0);
	}
	if (chdir(av[1]) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}
