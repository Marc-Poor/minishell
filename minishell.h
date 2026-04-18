/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 19:04:00 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/18 15:54:32 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H


# include <fcntl.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#include "pipex/pipex.h"

int	ft_isdigit(int c);
int	ft_isalpha(int c);
char	*ft_strdup(char *src);
void ft_pwd(char **env);
void ft_env(char **env);
char **ft_unset(char *str, char **env);
char **ft_realloc_tab(char **tab, size_t new_len);

#endif