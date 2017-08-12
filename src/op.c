/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   op.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/12 20:25:23 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/13 00:49:44 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

t_op    op_tab[17] =
{
	{"live", 1, {T_DIR}, 1, 10, "alive", 0, 0},
	{"ld", 2, {T_DIR | T_IND, T_REG}, 2, 5, "load", 1, 0},
	{"st", 2, {T_REG, T_IND | T_REG}, 3, 5, "store", 1, 0},
	{"add", 3, {T_REG, T_REG, T_REG}, 4, 10, "addition", 1, 0},
	{"sub", 3, {T_REG, T_REG, T_REG}, 5, 10, "soustraction", 1, 0},
	{"and", 3, {T_REG | T_DIR | T_IND, T_REG | T_IND | T_DIR, T_REG}, 6, 6,
		"et (and  r1, r2, r3   r1&r2 -> r3", 1, 0},
	{"or", 3, {T_REG | T_IND | T_DIR, T_REG | T_IND | T_DIR, T_REG}, 7, 6,
		"ou  (or   r1, r2, r3   r1 | r2 -> r3", 1, 0},
	{"xor", 3, {T_REG | T_IND | T_DIR, T_REG | T_IND | T_DIR, T_REG}, 8, 6,
		"ou (xor  r1, r2, r3   r1^r2 -> r3", 1, 0},
	{"zjmp", 1, {T_DIR}, 9, 20, "jump if zero", 0, 1},
	{"ldi", 3, {T_REG | T_DIR | T_IND, T_DIR | T_REG, T_REG}, 10, 25,
		"load index", 1, 1},
	{"sti", 3, {T_REG, T_REG | T_DIR | T_IND, T_DIR | T_REG}, 11, 25,
		"store index", 1, 1},
	{"fork", 1, {T_DIR}, 12, 800, "fork", 0, 1},
	{"lld", 2, {T_DIR | T_IND, T_REG}, 13, 10, "long load", 1, 0},
	{"lldi", 3, {T_REG | T_DIR | T_IND, T_DIR | T_REG, T_REG}, 14, 50,
		"long load index", 1, 1},
	{"lfork", 1, {T_DIR}, 15, 1000, "long fork", 0, 1},
	{"aff", 1, {T_REG}, 16, 2, "aff", 1, 0},
	{"noop", 0, {0}, 0, 0, "no operation", 0, 0}
};

/*
** securized incrementation of the pc to make the memory circular
*/

void	inc_pc(t_process *p)
{
	++p->pc;
	if (p->pc == MEM_SIZE)
		p->pc = 0;
}

/*
** add a new process to the list at the head
*/

int		ft_lst_head(t_processor *env, t_process *p)
{
	t_lst	*n;

	if ((n = malloc(sizeof(t_lst))) == NULL)
		return (-1);
	n->proc = p;
	n->next = env->processes;
	env->processes = n;
	return (0);
}

/*
** add a new process in the processor's memory at specific position and set the
** pc of the new process, register 1 to the creator id, other registers to 0,
** iner cycle to 0 (imediate start), and owner
*/

int		new_process(t_processor	*env, t_champion *champ, int position, int pc)
{
	t_process		*p;
	unsigned int	i;
	t_mem_case		*c;

	if ((p = malloc(sizeof(t_process))) == NULL)
		return (-1);
	i = -1;
	while (++i < champ->header.prog_size)
	{
		c = &env->mem[(position + i) % MEM_SIZE];
		c->code = champ->code[i];
		c->status |= E_TAKEN;
		c->owner = champ->creator;
	}
	pc %= MEM_SIZE;
	p->pc = pc;
	env->mem[pc].status |= E_PC;
	p->carry = 0;
	p->registers[0] = champ->creator;
	i = 0;
	while (++i < REG_NUMBER)
		p->registers[i] = 0;
	p->iner_cycle = 0;
	p->owner = champ->creator;
	return (ft_lst_head(env, p));
}

/*
** stop the processor by freeing all the processes
*/

void	end(t_processor *env)
{
	t_lst	*cur;
	t_lst	*next;

	cur = env->processes;
	while (cur != NULL)
	{
		next = cur->next;
		free(cur->proc);
		free(cur);
		cur = next;
	}
}

/*
** fetch loop, read an instruction pointed by the pc and execute it
** WIP
*/

void	fetch(t_processor *env, t_process *p)
{
	t_op	op;
	int		i;
	// int		t;

	i = -1;
	while (++i < (int)sizeof(op_tab))
		if (op_tab[i].code == env->mem[p->pc].code)
			break;
	if (i == sizeof(op_tab))
		return ;
	op = op_tab[i];
	printf("opcode: 0x%2hhX, (%s)\n", op.code, op.label);
	// inc_pc(p);
	// if (op.need_argument_type_byte)
	// {
	// 	i = -1;
	// 	while (++i < op.nb_argument)
	// 	{
	// 		t = (env->mem[p->pc].code & 0b11000000 >> (i << 1)) >> ((4 - i) << 1);
	// 		printf("arg %d: %d\n", i, t);
	// 	}
	// }
}

void	print(t_processor *env)
{
	int	i;

	i = -1;
	while (++i < MEM_SIZE)
	{
		if (i && !(i % MEM_WIDTH))
			printf("\n");
		printf("%.2hhX ", env->mem[i].code);
	}
	printf("\n");
}

int		error(int e)
{
	static const char	ep[6][80] = {
		"",
		"can't open file",
		"invalid file size",
		"invalid magic number",
		"malloc failled",
		"invalid program size"
	};

	printf("%s\n", ep[e]);
	return (0);
}

unsigned int	rev_endian(unsigned int i)
{
	return ((i << 24) | (i >> 24) | ((i << 8) & 0xFF0000) | ((i >> 8) & 65280));
}

void			rev_endian_mem(unsigned int *p, unsigned int size)
{
	size = (size - size % 4) / 4;
	while (size--)
		p[size] = rev_endian(p[size]);
}

int	read_champ(char *file_path, t_champion *c, int id)
{
	int				fd;

	if ((fd = open(file_path, O_RDONLY)) == -1)
		return (1);
	if (read(fd, &c->header, sizeof(header_t)) != sizeof(header_t))
		return (2);
	rev_endian_mem((unsigned int *)&c->header, sizeof(header_t));
	if (c->header.magic != COREWAR_EXEC_MAGIC)
		return (3);
	if (c->header.prog_size >= CHAMP_MAX_SIZE)
		return (5);
	if ((c->code = malloc(c->header.prog_size)) == NULL)
		return (4);
	if (read(fd, c->code, c->header.prog_size) != c->header.prog_size)
	{
		free(c->code);
		c->code = NULL;
		return (2);
	}
	c->creator = id;
	close(fd);
	return (0);
}

int	help(void)
{
	printf("usage: [-dump nbr_cycles] [[-n number] champion1.cor] ...\n");
	return (1);
}

int	isnum(char c)
{
	return (c >= '0' && c <= '9');
}

int	arg(int argc, char **argv, t_processor *env)
{
	int i;
	int	id;
	int	t;
	int	l;

	id = 1;
	l = 0;
	if (argc == 1)
		return (help());
	i = 0;
	while (++i < argc)
		if (argv[i][0] == '-')
		{
			if (!strcmp(argv[i], "-dump"))
			{
				if (++i < argc && argv[i] != NULL && isnum(argv[i][0]))
					env->dump = atoi(argv[i]);
				else
					return (help());
			}
			if (!strcmp(argv[i], "-n"))
			{
				if (++i < argc && argv[i] != NULL && isnum(argv[i][0]))
					t = atoi(argv[i]);
				else
					return (help());
				if (++i < argc && argv[i] != NULL)
				{
					if ((t = read_champ(argv[i], &env->champ[l++], t)))
						return (t);
					++env->nb_champ;
				}
			}
		}
		else
		{
			if ((t = read_champ(argv[i], &env->champ[l++], id++)))
				return (t);
			++env->nb_champ;
		}
	return (0);
}

int	main(int argc, char **argv)
{
	t_processor	env = {0};
	int	i;
	int	m;

	if ((i = arg(argc, argv, &env)))
		return (error(i));
	i = -1;
	while (++i < env.nb_champ)
	{
		m = (MEM_SIZE / env.nb_champ) * i % MEM_SIZE;
		printf("%d\n", m);
		new_process(&env, &env.champ[i], m, m);
	}
	// new_process(&env, &test, 0, 0);
	print(&env);
	return (0);
}
