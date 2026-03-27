/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 16:02:18 by mseguin           #+#    #+#             */
/*   Updated: 2026/03/27 19:01:23 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>

typedef enum e_tokentype
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_HEREDOC
}					t_tokentype;

typedef struct s_token
{
	t_tokentype		type;
	char			*str;
	struct s_token	*next;
}					t_token;

t_token				*new_token(t_tokentype type, char *str);
void				add_token(t_token **lst, t_token *new_tok);
void				clear_token(t_token **lst);

void				print_tokens(t_token *lst);

int					is_space(char c);
int					is_op(char c);
char				*word_dup(const char *s, int start, int end);
t_token				*tokenize_line(const char *s);

int					word_len_no_quotes(const char *s, int start, int end);
char				*word_dup_no_quotes(const char *s, int start, int end);
int					find_word_end(const char *s, int start);

char				*expand_variables(char *str);

char				*append_char(char *s, char c);
char				*append_str(char *s, char *add);
char				*expand_word(const char *s, int start, int end);

#endif