/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 16:02:18 by mseguin           #+#    #+#             */
/*   Updated: 2026/04/27 00:48:16 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "pipex/pipex.h"
# include <fcntl.h>
# include <signal.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

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
}					t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_shell
{
	char			**env;
	int				last_status;
}					t_shell;

typedef struct s_dup
{
	char			*word;
	int				i;
	int				end;
}					t_dup;

int					ft_isdigit(int c);
int					ft_isalpha(int c);
char				*ft_strdup(const char *src);
char				**ft_unset(char **av, char **env);
char				**ft_realloc_tab(char **tab, size_t new_len);
int					execute(t_cmd *cmds, t_shell *shell);
char				**ft_export_main(char **av, char **env);
void				close_all(int pipefd[2], int infile, int outfile);
void				close_fd(int pipefd[2]);
int					ft_cd(char **av, char **env);
int					ft_exit(char **argv);
char				**copy_tab(char **env);
int					main_ft_echo(char **av);
int					ft_pwd(char **env);
int					ft_env(char **env);

t_token				*new_token(t_tokentype type, char *str);
void				add_token(t_token **lst, t_token *new_tok);
void				clear_token(t_token **lst);

void				print_tokens(t_token *lst);

int					is_space(char c);
int					is_op(char c);
char				*word_dup(const char *s, int start, int end);
t_token				*tokenize_line(const char *s, t_shell *shell);

int					word_len_no_quotes(const char *s, int start, int end);
char				*word_dup_no_quotes(const char *s, int start, int end);
int					find_word_end(const char *s, int start);

char				*expand_variables(char *str);

char				*append_char(char *s, char c);
char				*append_str(char *s, char *add);
char				*expand_word(const char *s, int start, int end,
						t_shell *shell);
char				*env_get_value(t_shell *shell, char *key);

int					is_var_char(char c);
char				*get_var_value(char *name, t_shell *shell);
char				*get_var_value(char *name, t_shell *shell);

int					check_syntax(t_token *lst);

t_cmd				*parse_commands(t_token *tokens);
void				free_cmds(t_cmd **cmds);
void				print_cmds(t_cmd *cmds);

t_redir				*redir_new(t_tokentype type, char *file);
void				redir_add_back(t_redir **lst, t_redir *new_redir);

void				setup_signals(void);

#endif