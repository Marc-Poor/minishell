/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 22:44:38 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/26 20:07:56 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include "minishell.h"


char **change_pwd(char *cwd, char **env)
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

char **ft_cd(char **av, char **env)
{
	char *cwd;

	if (!av[1])
	{
		perror("cd: missing argument\n");
		return (NULL);
	}
	if (av[2])
	{
		perror("cd: too many arguments\n");
		return (NULL);
	}
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