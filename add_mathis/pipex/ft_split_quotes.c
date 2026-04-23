/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:52:53 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/12 16:33:29 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	free_all(char	**tab, int size)
{
	int	count;

	count = 0;
	while (count <= size)
	{
		free(tab[count]);
		count++;
	}
	free(tab);
}

static int	count_quoted_parts(const char *s, char quote)
{
	int	count;
	int	in_quote;
	int	i;

	count = 0;
	in_quote = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == quote)
		{
			if (in_quote == 0)
				in_quote = 1;
			else
			{
				in_quote = 0;
				count++;
			}
		}
		i++;
	}
	return (count);
}

char	**loop(const char *s, char quote, char **tab, int i)
{
	int		start;
	int		k;
	int		len;

	k = 0;
	while (s[i])
	{
		if (s[i] == quote)
		{
			start = ++i;
			while (s[i] && s[i] != quote)
				i++;
			len = i - start;
			tab[k] = malloc(sizof(len) + 1);
			if (!tab[k])
				return (free_all(tab, k), NULL);
			ft_strncpy(tab[k], s + start, len);
			k++;
		}
		if (s[i])
			i++;
	}
	tab[k] = NULL;
	return (tab);
}

char	**ft_split_quotes(const char *s, char quote)
{
	char	**tab;
	int		parts;

	if (!s)
		return (NULL);
	parts = count_quoted_parts(s, quote);
	tab = malloc(sizeof(char *) * (parts + 1));
	if (!tab)
		return (NULL);
	tab = loop(s, quote, tab, 0);
	return (tab);
}

/*

static int	count_quoted_parts(const char *s, char quote)
{
	int	count;
	int	in_quote;

	count = 0;
	in_quote = 0;
	while (*s)
	{
		if (*s == quote)
		{
			if (in_quote == 0)
			{
				in_quote = 1;
			}
			else
			{
				in_quote = 0;
				count++;
			}
		}
		s++;
	}
	return (count);
}
*/