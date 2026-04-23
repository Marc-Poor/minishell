/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:11:42 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/21 16:11:43 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

static int	is_numeric(char *str)
{
	int i = 0;

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

static long	ft_atol(const char *str)
{
	long	result = 0;
	int		sign = 1;

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
}

int	ft_exit(char **argv)
{
	long	status;

	printf("exit\n");

	if (!argv[1])
		exit(0);

	if (!is_numeric(argv[1]))
	{
		fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
		exit(2);
	}

	if (argv[2])
	{
		fprintf(stderr, "exit: too many arguments\n");
		return (1);
	}

	status = ft_atol(argv[1]);
	exit((unsigned char)status);
}