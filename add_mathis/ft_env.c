/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 13:36:27 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/26 20:49:32 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex/pipex.h"

int	ft_env(char **env)
{
	int	i;

	if (!env)
		return (-1);
	i = 0;
	while (env[i] != NULL)
	{
		printf("%s\n", env[i]);
		i++;
	}
	return (0);
}

/*
int	main(int ac, char **av, char **env)
{
	ft_env(env);
	return (0);
}
*/