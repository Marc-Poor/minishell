/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 22:17:19 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/28 20:21:54 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex/pipex.h"
#include <readline/history.h>
#include <readline/readline.h>

int	handle_heredoc(char *delimiter)
{
	int		fd[2];
	char	*line;
	int		print;

	if (pipe(fd) == -1)
		exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		print = write(fd[1], line, strlen(line));
		print = write(fd[1], "\n", 1);
		if (print == -1)
			printf("fuck Werror\n");
		free(line);
	}
	close(fd[1]);
	return (fd[0]);
}

//  return read end