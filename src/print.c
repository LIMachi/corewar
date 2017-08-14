/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/14 21:33:42 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/14 21:34:53 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar.h"

void	effect(t_mem_case c)
{
	if (c.status & E_PC)
		printf("\e[1m");
	if (!(c.status & E_TAKEN))
	{
		printf("\e[2m");
		return ;
	}
	if (c.owner == 1)
		printf("\e[32m");
	return ;
}

void	print(t_processor *env)
{
	int	i;

	i = -1;
	while (++i < MEM_SIZE)
	{
		if (i && !(i % MEM_WIDTH))
			printf("\n");
		effect(env->mem[i]);
		printf("%.2hhX\e[0m ", env->mem[i].code);
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

	if (e > 0)
		printf("%s\n", ep[e]);
	return (0);
}

int	help(void)
{
	printf("usage: [-dump nbr_cycles] [[-n number] champion1.cor] ...\n");
	return (-1);
}
