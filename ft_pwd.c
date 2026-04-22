/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 12:47:00 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/21 19:11:53 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_str_in_env(char **env, char *str, int i)
{
	while (env[i])
	{
		if (env[i] && ft_strncmp(env[i], "PWD", 3) == 0 && env[i][3] == '=') {
			str = env[i] + 4;
			}
		i++;
	}
	if (str == 0)
			return ("error : missing PWD\n");
	return (str);
}

int ft_pwd(char **env)
{
	if (!env)
		return (-1);
	printf("%s\n", find_str_in_env(env, 0, 0));
	return (0);
}