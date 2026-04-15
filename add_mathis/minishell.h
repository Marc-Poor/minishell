/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 16:02:18 by mseguin           #+#    #+#             */
/*   Updated: 2026/04/12 02:10:25 by mathissegui      ###   ########.fr       */
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

typedef struct s_redir
{
	t_tokentype		type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

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

int					check_syntax(t_token *lst);

t_cmd				*parse_commands(t_token *tokens);
void				free_cmds(t_cmd **cmds);
void				print_cmds(t_cmd *cmds);

t_redir				*redir_new(t_tokentype type, char *file);
void				redir_add_back(t_redir **lst, t_redir *new_redir);

#endif