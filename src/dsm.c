/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dsm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/14 21:37:21 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/15 01:03:46 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//gcc -I inc src/dsm.c src/endian.c src/op.c -g3 -o dsm

#include "corewar.h"

/*
** dsm is a small disassembler for 42's corewar program, it read .cor files and
** try to produce a coherent .s file
** in label mode, it will try to create labels for jumps and forks, labels will
** have a generic name
** label mode is not implemented yet, but will be soon
** this program is a POC, and by no mean should be trusted
*/

int	usage(void)
{
	printf("usage: [-l] <file.cor>\n");
	return (0);
}

int	read_cor(int fd, header_t *head, char code[CHAMP_MAX_SIZE])
{
	if (read(fd, head, sizeof(header_t)) != sizeof(header_t))
		return (printf("file header of wrong size\n"));
	rev_endian_mem((unsigned int*)head, sizeof(header_t));
	if (head->magic != COREWAR_EXEC_MAGIC)
		return (printf("wrong file format\n"));
	if (head->prog_size > CHAMP_MAX_SIZE)
		return (printf("wrong code size\n"));
	if (read(fd, code, head->prog_size) != head->prog_size)
		return (printf("wrong file size\n"));
	// rev_endian_mem((unsigned int*)code, head->prog_size);
	return (0);
}

int	get_bytes(char code[CHAMP_MAX_SIZE], int *i, int size)
{
	int	out;

	out = 0;
	while (size--)
		out |= code[++(*i)] << (size << 3);
	return (out);
}

int	collen(char *str, int tab_len)
{
	int i;

	i = 0;
	while (*str != 0)
		if (*str++ == '\t')
			i += tab_len - i % tab_len;
		else
			++i;
	return (i);
}

int	treat(header_t head, char code[CHAMP_MAX_SIZE], int label_mode, char *file_name)
{
	int				fd;
	int				i;
	t_op			op;
	unsigned char	btarg;
	char			buff[80];
	int				t;

	printf("file: '%s'\n", file_name);
	if ((fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		return (printf("can't create/use '%s'\n", file_name));
	rev_endian_mem((unsigned int*)&head, sizeof(header_t));
	write(fd, ".name\t\t\"", 8);
	write(fd, head.prog_name, strlen(head.prog_name));
	write(fd, "\"\n.comment\t\"", 12);
	write(fd, head.comment, strlen(head.comment));
	write(fd, "\"\n\n", 3);
	head.prog_size = rev_endian(head.prog_size);
	if (label_mode)
		write(fd, "start: ", 7);
	i = -1;
	while (++i < head.prog_size)
	{
		t = i;
		bzero(buff, 80);
		op = get_op(code[i]);
		sprintf(buff, "%s%.*s", op.label, 2 - (strlen(op.label) > 3), "\t\t");
		if (op.need_argument_type_byte)
			btarg = code[++i];
		else
			btarg = DIR_CODE << 6;
		while (btarg & 0b11000000)
		{
			if ((btarg >> 6) == REG_CODE)
				sprintf(buff + strlen(buff), "r%hhu", get_bytes(code, &i, 1));
			if ((btarg >> 6) == DIR_CODE)
			{
				if (op.two_byte_dir)
					sprintf(buff + strlen(buff), "%c%hu", DIRECT_CHAR, get_bytes(code, &i, 2));
				else
					sprintf(buff + strlen(buff), "%c%u", DIRECT_CHAR, get_bytes(code, &i, 4));
			}
			if ((btarg >> 6) == IND_CODE)
				sprintf(buff + strlen(buff), "%u", get_bytes(code, &i, 4));
			if ((btarg <<= 2) && 0b11000000)
				sprintf(buff + strlen(buff), "%c ", SEPARATOR_CHAR);
		}
		sprintf(buff + strlen(buff), "%.*s%c %d\n", 12 - (collen(buff, 4) / 4), "\t\t\t\t\t\t\t\t\t\t\t\t", COMMENT_CHAR, t);
		write(fd, buff, strlen(buff));
	}
}

char	*swap_extension(char *path, char *ext)
{
	unsigned int	l;
	char			*t;

	l = strlen(path);
	if ((t = rindex(path, '.')) != NULL && (unsigned int)(t - path) != l)
	{
		l = (unsigned int)(t - path);
		if ((t = rindex(path, '/')) != NULL && (unsigned int)(t - path) > l)
			l = strlen(path);
	}
	if ((t = strndup(path, l + strlen(ext))) == NULL)
		return (NULL);
	strcpy(t + l, ext);
	return (t);
}

int	main(int argc, char **argv)
{
	int			label_mode;
	int			fd;
	header_t	head;
	char		code[CHAMP_MAX_SIZE];
	char		*t;

	if (argc < 2)
		return (usage());
	label_mode = 0;
	if (argc > 2)
	{
		if (!(argv[1][0] == '-' && argv[1][1] == 'l' && argv[1][2] == '\0'))
			return (usage());
		label_mode = 1;
	}
	if ((fd = open(argv[argc - 1], O_RDONLY)) == -1)
		return (printf("can't open file\n"), 0);
	if (read_cor(fd, &head, code))
		return (close(fd), 0);
	close(fd);
	if ((t = swap_extension(argv[argc - 1], ".s")) == NULL)
		return (printf("memory allocation error\n"), -1);
	treat(head, code, label_mode, t);
	free(t);
	return (0);
}
