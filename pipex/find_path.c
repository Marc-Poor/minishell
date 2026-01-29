/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 16:57:28 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/04 10:49:52 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

char	**find_in_env(char **env, char **tab, int i)
{
	while (env[i])
	{
		if (env[i] && ft_strncmp(env[i], "PATH", 4) == 0 && env[i][4] == '=')
			tab = ft_split(env[i] + 5, ':');
		i++;
	}
	return (tab);
}

char	*find_path(char **env, char *ftname, int i)
{
	char	**tab;
	char	*full_path;

	tab = find_in_env(env, NULL, 0);
	while (tab && tab[i])
	{
		full_path = malloc(ft_strlen(tab[i]) + ft_strlen(ftname) + 2);
		if (!full_path)
			continue ;
		full_path[0] = '\0';
		ft_strcat(full_path, tab[i]);
		ft_strcat(full_path, "/");
		ft_strcat(full_path, ftname);
		if (access(full_path, X_OK) == 0)
		{
			free_tab(tab);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	ft_putstr(ftname);
	free_tab(tab);
	return (NULL);
}
