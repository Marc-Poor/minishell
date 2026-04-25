/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 17:38:39 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/24 22:49:05 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	find_in_env(char *str, char **env)
{
	int	i;
	int	x;

	i = 0;
	// while (str[i] && str[i] != '=')
	//{
	//	if (!ft_isalpha(str[i]))
	//		return (-2);
	//	i++;
	//}
	i = 0;
	while (env[i])
	{
		x = 0;
		while (env[i][x] != '=' && env[i][x] != '\0')
			x++;
		if (ft_strncmp(str, env[i], x) == 0)
			return (i);
		i++;
	}
	printf("%s\n", env[i - 1]);
	return (-1);
}

char	**ft_unset(char **av, char **env)
{
	int	x;

	if (!av[1] || !env)
		return (NULL);
	x = find_in_env(av[1], env);
	if (x < 0)
	{
		printf("%i\n", x);
		printf("bash: unset: '%s': not a valid identifier\n", av[1]);
		return (env);
	}
	free(env[x]);
	while (env[x])
	{
		env[x] = env[x + 1];
		x++;
	}
	return (env);
}
