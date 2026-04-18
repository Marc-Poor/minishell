/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_all.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 16:02:18 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/18 20:26:17 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	close_all(int pipefd[2], int infile, int outfile)
{
	if (pipefd[0])
		close(pipefd[0]);
	if (pipefd[1])
		close(pipefd[1]);
	if (infile != -1)
		close(infile);
	if (outfile != -1)
		close(outfile);
}

void	close_fd(int pipefd[2])
{
	if (pipefd[0])
		close(pipefd[0]);
	if (pipefd[1])
		close(pipefd[1]);
}
