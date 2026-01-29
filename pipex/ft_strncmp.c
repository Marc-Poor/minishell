/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:10:23 by mfaure            #+#    #+#             */
/*   Updated: 2025/05/21 21:57:16 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdio.h>

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	count;

	count = 0;
	while (count < n && (s1[count] || s2[count]))
	{
		if (s1[count] != s2[count])
			return ((unsigned char) s1[count] - (unsigned char) s2[count]);
		count++;
	}
	return (0);
}
/*
int main()
{
	printf("%i\n", ft_strncmp("test\200", "test\0", 6));
	return 0;
}
*/