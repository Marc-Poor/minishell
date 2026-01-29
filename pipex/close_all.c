/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_all.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 16:02:18 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/01 17:00:46 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	close_all(int pipefd[2], int infile, int outfile)
{
	if (pipefd[0])
		close(pipefd[0]);
	if (pipefd[1])
		close(pipefd[1]);
	close(infile);
	close(outfile);
}
