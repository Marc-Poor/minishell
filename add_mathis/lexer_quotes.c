/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 19:33:07 by mathissegui       #+#    #+#             */
/*   Updated: 2026/03/19 19:53:54 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	find_word_end(const char *s, int start)
{
	int		i;
	char	quote;

	i = start;
	while (s[i] && !is_space(s[i]) && !is_op(s[i]))
	{
		if (s[i] == '\'' || s[i] == '"')
		{
			quote = s[i];
			i++;
			while (s[i] && s[i] != quote)
				i++;
			if (!s[i])
				return (-1);
		}
		i++;
	}
	return (i);
}

int	word_len_no_quotes(const char *s, int start, int end)
{
	int		len;
	char	quote;

	len = 0;
	while (start < end)
	{
		if (s[start] == '\'' || s[start] == '"')
		{
			quote = s[start];
			start++;
			while (start < end && s[start] != quote)
			{
				len++;
				start++;
			}
			if (start < end)
				start++;
		}
		else
		{
			len++;
			start++;
		}
	}
	return (len);
}

char	*word_dup_no_quotes(const char *s, int start, int end)
{
	char	*word;
	int		i;
	char	quote;

	word = malloc(sizeof(char) * (word_len_no_quotes(s, start, end) + 1));
	if (!word)
		return (NULL);
	i = 0;
	while (start < end)
	{
		if (s[start] == '\'' || s[start] == '"')
		{
			quote = s[start];
			start++;
			while (start < end && s[start] != quote)
			{
				word[i] = s[start];
				i++;
				start++;
			}
			if (start < end)
				start++;
		}
		else
		{
			word[i] = s[start];
			i++;
			start++;
		}
	}
	word[i] = '\0';
	return (word);
}
