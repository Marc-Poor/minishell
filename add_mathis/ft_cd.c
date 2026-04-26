/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 22:44:38 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/26 21:11:19 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <unistd.h>

char	**change_pwd(char *cwd, char **env)
{
	int		x;
	char	*new;

	if (!cwd || !env)
		return (env);
	x = find_in_env("PWD=", env);
	new = malloc(sizeof(char) * (ft_strlen(cwd) + 5));
	if (!new)
		return (env);
	strcpy(new, "PWD=");
	strcat(new, cwd);
	if (x >= 0)
	{
		free(env[x]);
		env[x] = new;
	}
	else
	{
		env = ft_export(new, env);
		free(new);
	}
	return (env);
}

static int	av_error_handling(char **av)
{
	if (!av[1])
	{
		perror("cd: missing argument\n");
		return (0);
	}
	if (av[2])
	{
		perror("cd: too many arguments\n");
		return (0);
	}
	return (1);
}

char	**ft_cd(char **av, char **env)
{
	char	*cwd;

	if (!av_error_handling(av))
		return (NULL);
	if (chdir(av[1]) != 0)
	{
		perror("cd");
		return (NULL);
	}
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("getcwd");
		return (NULL);
	}
	env = change_pwd(cwd, env);
	free(cwd);
	return (env);
}
