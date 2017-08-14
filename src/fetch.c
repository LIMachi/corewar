/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fetch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/14 21:32:46 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/14 21:32:59 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar.h"

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
