/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 19:17:54 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/12 15:52:26 by mfaure           ###   ########.fr       */
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

static int	find_size_of(char const *str, char separation, int type)
{
	int	count;

	count = 0;
	if (type == 1)
		while (str[count] != '\0' && str[count] != separation)
			count++;
	if (type == 2)
		while (str[count] != '\0' && str[count] == separation)
			count++;
	return (count);
}

static int	count_words(char const	*str, char separation)
{
	int	separator;
	int	count;
	int	words;

	separator = 0;
	count = 0;
	words = 0;
	if (!str)
		return (0);
	while (str[count] != '\0')
	{
		if (str[count] != separation && words == 0)
			separator++;
		words++;
		if (str[count] == separation)
			words = 0;
		count++;
	}
	return (separator);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;
	int		find_size;
	int		size_str;
	int		size_tab;

	tab = malloc(sizeof(char *) * (count_words(s, c) + 1));
	find_size = 0;
	size_str = find_size_of(s, c, 2);
	if (!tab)
		return (NULL);
	while (find_size < count_words(s, c))
	{
		size_tab = find_size_of(s + size_str, c, 1);
		tab[find_size] = malloc(sizeof(char) * (size_tab + 1));
		if (!tab[find_size])
		{
			free_all(tab, find_size);
			return (NULL);
		}
		ft_strncpy(tab[find_size], s + size_str, size_tab);
		size_str += size_tab + find_size_of(s + size_str + size_tab, c, 2);
		find_size++;
	}
	tab[count_words(s, c)] = NULL;
	return (tab);
}

/*
int	main()
{
	int i = 0;
	char **tab = ft_split("   lorem ", ' ');

	while(tab[i] != NULL)
	{
		printf("%s\n", tab[i]);
		i++;
	}

}*/