/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 15:53:06 by mfaure            #+#    #+#             */
/*   Updated: 2025/11/12 19:12:09 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	free_command(char **tab, char *str)
{
	int	i;

	if (str)
		free(str);
	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

void	exec_cmd(char *cmd_str, char **env)
{
	char	**cmd;
	char	*path;

	cmd = ft_split(cmd_str, ' ');
	if (!cmd)
	{
		perror("Invalid command: cmd_str");
		exit(127);
	}
	if (!cmd[0])
	{
		free(cmd);
		exit(127);
	}
	path = find_path(env, cmd[0], 0);
	if (!path || path[0] == '\0')
	{
		ft_putstr(" : Command not found\n");
		free_command(cmd, cmd_str);
		exit(127);
	}
	execve(path, cmd, env);
	perror("execve failed");
	exit(1);
}

void	open_p(t_pipex *p, int ac, char **av, char **env)
{
	if (ac != 5 || !env || !env[0])
		exit(1);
	p->infile = open(av[1], O_RDONLY);
	if (p->infile < 0)
	{
		perror("infile or outfile");
		exit(1);
	}
	p->outfile = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (p->outfile < 0)
	{
		close(p->infile);
		perror("infile or outfile");
		exit(1);
	}
}

void	ft_dup(t_pipex p, int i)
{
	if (i == 0)
	{
		dup2(p.infile, STDIN_FILENO);
		dup2(p.pipefd[1], STDOUT_FILENO);
	}
	if (i == 1)
	{
		dup2(p.pipefd[0], STDIN_FILENO);
		dup2(p.outfile, STDOUT_FILENO);
	}
}

int	main(int ac, char **av, char **env)
{
	t_pipex	p;

	open_p(&p, ac, av, env);
	if (pipe(p.pipefd) == -1)
		exit(1);
	p.pid1 = fork();
	if (p.pid1 == 0)
	{
		ft_dup(p, 0);
		close_all(p.pipefd, p.infile, p.outfile);
		exec_cmd(remove_quotes(av[2], 1), env);
	}
	p.pid2 = fork();
	if (p.pid2 == 0)
	{
		ft_dup(p, 1);
		close_all(p.pipefd, p.infile, p.outfile);
		exec_cmd(remove_quotes(av[3], 1), env);
	}
	close_all(p.pipefd, p.infile, p.outfile);
	waitpid(p.pid1, &p.wstatus, 0);
	if (!WIFEXITED(p.wstatus))
		return (-1);
	waitpid(p.pid2, NULL, 0);
	return (0);
}

/*
char *read_all(char *av)
{
	char	*line;
	char	*str;
	int		fd;

	fd = open(av, O_RDONLY);
	if (fd < 0)
		return (NULL);
	line = get_next_line(fd);
	str = ft_strdup("");
	if (line == NULL || str == NULL)
		return (0);
	while (line != NULL)
	{
		str = ft_realloc_str(str, ft_strlen(str)
				+ ft_strlen(line));
		ft_strcat(str, line);
		free(line);
		line = get_next_line(fd);
	}
	return (str);
}*/

/*

# Basic functionality
./pipex infile "cat" "wc -l" outfile
< infile cat | wc -l > outfile

# Grep example
echo -e "apple\nbanana\napple\n" > infile
./pipex infile "grep apple" "wc -l" outfile
< infile grep apple | wc -l > outfile

# Invalid command (first fails)
./pipex infile "invalidcmd" "wc -l" outfile
invalidcmd | wc -l

# Invalid command (second fails)
./pipex infile "cat" "fakecmd" outfile
< infile cat | fakecmd > outfile

# Missing infile
./pipex nofile "cat" "wc -l" outfile

# Permission denied infile
touch infile && chmod 000 infile
./pipex infile "cat" "wc -l" outfile
chmod 644 infile

# Execve permission denied (non-executable file)
echo "echo hello" > script.sh
./pipex infile "./script.sh" "wc -l" outfile
chmod +x script.sh

# Execve on directory
mkdir testdir
./pipex infile "testdir" "wc -l" outfile
rmdir testdir

# Empty infile
> infile
./pipex infile "cat" "wc -l" outfile

# Valgrind test
valgrind --track-fds=yes --leak-check=full ./pipex infile "ls -l" "wc -l" outfile
*/