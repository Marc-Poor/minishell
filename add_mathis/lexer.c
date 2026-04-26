/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:36:18 by mathissegui       #+#    #+#             */
/*   Updated: 2026/04/26 23:16:21 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_space(char c)
{
	if (c == ' ' || c == '\t')
		return (1);
	return (0);
}

int	is_op(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

static void	add_op_token(t_token **lst, const char *s, int *i)
{
	if (s[*i] == '|')
	{
		add_token(lst, new_token(T_PIPE, NULL));
		(*i)++;
	}
	else if (s[*i] == '<' && s[*i + 1] == '<')
	{
		add_token(lst, new_token(T_HEREDOC, NULL));
		(*i) += 2;
	}
	else if (s[*i] == '>' && s[*i + 1] == '>')
	{
		add_token(lst, new_token(T_REDIR_APPEND, NULL));
		(*i) += 2;
	}
	else if (s[*i] == '<')
	{
		add_token(lst, new_token(T_REDIR_IN, NULL));
		(*i)++;
	}
	else if (s[*i] == '>')
	{
		add_token(lst, new_token(T_REDIR_OUT, NULL));
		(*i)++;
	}
}

static int	add_word_token(t_token **lst, const char *s, int *i, t_shell *shell)
{
	int		end;
	char	*word;

	end = find_word_end(s, *i);
	if (end == -1)
	{
		clear_token(lst);
		return (0);
	}
	word = word_dup_no_quotes(s, *i, end);
	free(word);
	add_token(lst, new_token(T_WORD, expand_word(s, *i, end, shell)));
	*i = end;
	return (1);
}

t_token	*tokenize_line(const char *s, t_shell *shell)
{
	t_token	*lst;
	int		i;

	lst = NULL;
	i = 0;
	while (s[i])
	{
		while (s[i] && is_space(s[i]))
			i++;
		if (!s[i])
			break ;
		if (is_op(s[i]))
		{
			add_op_token(&lst, s, &i);
			continue ;
		}
		if (!add_word_token(&lst, s, &i, shell))
			return (NULL);
	}
	return (lst);
}

/*
t_token	*tokenize_line(const char *s, t_shell *shell)
{
	t_token	*lst;
	int		i;
	int		end;
	char	*word;

	lst = NULL;
	i = 0;
	while (s[i])
	{
		while (s[i] && is_space(s[i]))
			i++;
		if (!s[i])
			break ;
		if (is_op(s[i]))
		{
			add_op_token(&lst, s, &i);
			continue ;
		}
		end = find_word_end(s, i);
		if (end == -1)
		{
			clear_token(&lst);
			return (NULL);
		}
		word = word_dup_no_quotes(s, i, end);
		free(word);
		add_token(&lst, new_token(T_WORD, expand_word(s, i, end, shell)));
		i = end;
	}
	return (lst);
}
*/