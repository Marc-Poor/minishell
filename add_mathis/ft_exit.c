/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:11:42 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/28 20:02:54 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "./libft/libft.h"

static int	is_numeric(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}
/*
static long	ft_atol(const char *str)
{
	long	result;
	int		sign;

	result = 0;
	sign = 1;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}*/

int	ft_exit(char **argv)
{
	int	status;

	if (!argv[1])
		exit(0);
	if (!is_numeric(argv[1]))
	{
		status = write(2, argv[1], ft_strlen(argv[1]));
		status = write(2, ": exit: numeric argument required\n", 35);
		exit(2);
	}
	if (argv[2])
	{
		perror("exit: too many arguments\n");
		return (1);
	}
	status = ft_atoi(argv[1]);
	exit(status);
}
