/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 17:37:37 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/21 17:35:42 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
int	find_in_env(char *str, char **env)
{
	int		i;
	int		x;
	char	**new_tab;
	size_t	i;

	i = 0;
	while (env[i])
	{
		x = 0;
		while (env[i][x] != '=' && env[i][x] != '\0')
			if (!is_alpha(env[i][x]))
				-1;
			x++;
		if (ft_strncmp(str, env[i], x) == 0 && str[x] == '\0')
			return (i);
		i++;
	}
	return (-1);
}*/
char	**ft_realloc_tab(char **tab, size_t new_len)
{
	size_t		i;
	char	**new_tab;

	i = 0;
	new_tab = malloc(sizeof(char *) * (new_len + 1));
	if (!new_tab)
		return (NULL);
	while (tab && tab[i] && i < new_len)
	{
		new_tab[i] = ft_strdup(tab[i]);
		i++;
	}
	new_tab[new_len] = NULL;
	i = 0;
	while (tab && tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
	return (new_tab);
}

char	**copy_tab(char **env)
{
	int		i;
	char	**copy;

	i = 0;
	while (env[i])
		i++;
	copy = malloc(sizeof(char *) * (i + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (env[i])
	{
		copy[i] = ft_strdup(env[i]);
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

char	**ft_export(char *str, char **env)
{
	int	i;
	int	x;

	if (!env)
		return (NULL); // potential error message needed
	// if (!find_in_env(str, env)) {
	//	printf("bash: unset: '%s': not a valid identifier\n", str);
	//	return (NULL);
	//}
	i = 0;
	while (env[i])
		i++;
	env = ft_realloc_tab(env, i + 1);
	env[i] = malloc(sizeof(char) * (ft_strlen(str) + 1));
	x = 0;
	while (str[x])
	{
		env[i][x] = str[x];
		x++;
	}
	env[i][x] = '\0';
	// printf("str : %s\n", env[i]);
	return (env);
}

char	**ft_export_main(char **av, char **env)
{
	if (!av[1])
		return (0);
	env = ft_export(av[1], env);
	return (env);
}
