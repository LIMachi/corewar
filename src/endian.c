/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   endian.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/14 21:34:08 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/14 21:34:29 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar.h"

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
