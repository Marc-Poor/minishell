/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 14:51:40 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/28 18:18:24 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup_and_exit(t_shell *shell, t_cmd *cmds, int status)
{
	cleanup(shell, cmds);
	exit(status);
}
