/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/14 21:28:44 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/14 23:14:57 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar.h"

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

int		new_process(t_processor	*env, t_champion *champ, int position)
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
	p->pc = position;
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

void	cw_read_args(t_processor *env, t_process *p)
{

}

void	cw_op_add(t_processor *env, t_process *p)
{

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
		new_process(&env, &env.champ[i], m, m);
	}
	// new_process(&env, &test, 0, 0);
	print(&env);
	return (0);
}
