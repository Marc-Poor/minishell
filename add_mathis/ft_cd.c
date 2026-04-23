#include <unistd.h>
#include <stdio.h>

int	ft_cd(char **av)
{
	printf("cd\n");
	if (!av[1])
	{
		printf("cd: missing argument\n");
		return (1);
	}
	if (av[2])
	{
		printf("cd: too many arguments\n");
		return (1);
	}
	if (chdir(av[1]) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}