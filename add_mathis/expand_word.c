/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 00:41:39 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/27 01:01:04 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft/libft.h"

char	*append_str(char *s, char *add)
{
	char	*new_s;

	new_s = ft_strjoin(s, add);
	free(s);
	return (new_s);
}

static char	*expand_single_quote(const char *s, int *i, int end, char *res)
{
	char	quote;

	quote = s[(*i)++];
	while (*i < end && s[*i] != quote)
		res = append_char(res, s[(*i)++]);
	if (*i < end)
		(*i)++;
	return (res);
}

static char	*expand_dollar_part(const char *s, int *i,
		t_shell *shell, char *res)
{
	char	*tmp;

	tmp = expand_dollar(s, i, shell);
	res = append_str(res, tmp);
	free(tmp);
	return (res);
}

static char	*expand_double_quote(const char *s, int *i, int end, t_shell *shell)
{
	char	*res;
	char	quote;

	res = ft_strdup("");
	quote = s[(*i)++];
	while (*i < end && s[*i] != quote)
	{
		if (s[*i] == '$')
			res = expand_dollar_part(s, i, shell, res);
		else
			res = append_char(res, s[(*i)++]);
	}
	if (*i < end)
		(*i)++;
	return (res);
}

char	*expand_word(const char *s, int start, int end, t_shell *shell)
{
	char	*res;
	char	*tmp;

	res = ft_strdup("");
	while (start < end)
	{
		if (s[start] == '\'')
			res = expand_single_quote(s, &start, end, res);
		else if (s[start] == '"')
		{
			tmp = expand_double_quote(s, &start, end, shell);
			res = append_str(res, tmp);
			free(tmp);
		}
		else if (s[start] == '$')
			res = expand_dollar_part(s, &start, shell, res);
		else
			res = append_char(res, s[start++]);
	}
	return (res);
}
