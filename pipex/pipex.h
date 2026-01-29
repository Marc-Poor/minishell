/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 17:13:46 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/12 17:45:11 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

# include <fcntl.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

typedef struct s_pipex
{
	pid_t	pid1;
	pid_t	pid2;
	int		infile;
	int		outfile;
	int		pipefd[2];
	int		wstatus;
	int		wstatus2;

}			t_pipex;

char		*ft_realloc_str(char *str, size_t new_len);
char		*get_next_line(int fd);
size_t		ft_strcat(char *dst, const char *src);
int			ft_strlen(char *str);
char		**ft_split(char const *s, char c);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
void		close_all(int pipefd[2], int infile, int outfile);
char		*find_path(char **env, char *ftname, int i);
void		free_tab(char **tab);
void		ft_putstr(char *s);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_strncpy(char *dest, char const *src, unsigned int n);
char		*remove_quotes(char *str, int i);

#endif