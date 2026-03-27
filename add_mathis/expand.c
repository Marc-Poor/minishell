/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 18:31:40 by mathissegui       #+#    #+#             */
/*   Updated: 2026/03/27 19:04:49 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"

static int	is_var_char(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0'
			&& c <= '9') || c == '_');
}

static char	*get_var_value(char *name)
{
	char	*value;

	if (name[0] == '?' && name[1] == '\0')
		return (ft_strdup("0"));
	value = getenv(name);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

static char	*expand_dollar(const char *s, int *i)
{
	char	var[256];
	int		j;

	j = 0;
	(*i)++;
	if (s[*i] == '?')
	{
		var[j++] = s[*i];
		(*i)++;
	}
	else
	{
		while (s[*i] && is_var_char(s[*i]))
		{
			var[j++] = s[*i];
			(*i)++;
		}
	}
	var[j] = '\0';
	return (get_var_value(var));
}

char	*append_char(char *s, char c)
{
	char	tmp[2];
	char	*new_s;

	tmp[0] = c;
	tmp[1] = '\0';
	new_s = ft_strjoin(s, tmp);
	free(s);
	return (new_s);
}

char	*append_str(char *s, char *add)
{
	char	*new_s;

	new_s = ft_strjoin(s, add);
	free(s);
	return (new_s);
}

char	*expand_word(const char *s, int start, int end)
{
	char *res;
	char *tmp;
	char quote;

	res = ft_strdup("");
	while (start < end)
	{
		if (s[start] == '\'')
		{
			quote = s[start++];
			while (start < end && s[start] != quote)
				res = append_char(res, s[start++]);
			if (start < end)
				start++;
		}
		else if (s[start] == '"')
		{
			quote = s[start++];
			while (start < end && s[start] != quote)
			{
				if (s[start] == '$')
				{
					tmp = expand_dollar(s, &start);
					res = append_str(res, tmp);
					free(tmp);
				}
				else
					res = append_char(res, s[start++]);
			}
			if (start < end)
				start++;
		}
		else if (s[start] == '$')
		{
			tmp = expand_dollar(s, &start);
			res = append_str(res, tmp);
			free(tmp);
		}
		else
			res = append_char(res, s[start++]);
	}
	return (res);
}