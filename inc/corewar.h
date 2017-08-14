/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   corewar.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/12 20:30:49 by hmartzol          #+#    #+#             */
/*   Updated: 2017/08/14 23:15:13 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "op.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define INPLACE_CHAR '$'

#define MEM_WIDTH 64

typedef struct		s_op
{
	char			*label;
	int				nb_argument;
	int				argument_types[MAX_ARGS_NUMBER];
	char			code;
	int				cycles;
	char			*extended_name;
	int				need_argument_type_byte;
	int				two_byte_dir;
}					t_op;

typedef struct		s_process
{
	int				pc;
	int				carry;
	int				registers[REG_NUMBER];
	int				iner_cycle;
	int				owner;
}					t_process;

typedef struct		s_lst
{
	t_process		*proc;
	struct s_lst	*next;
}					t_lst;

typedef enum		e_status
{
	E_NULL = 0, E_TAKEN = 1, E_PC = 2
}					t_status;

typedef struct		s_mem_case
{
	char			code;
	t_status		status;
	int				owner;
}					t_mem_case;

typedef struct		s_champion
{
	header_t		header;
	char			*code;
	int				creator;
}					t_champion;

typedef struct		s_processor
{
	t_lst			*processes;
	t_champion		champ[MAX_PLAYERS];
	int				nb_champ;
	t_mem_case		mem[MEM_SIZE];
	int				dump;
}					t_processor;

void				rev_endian_mem(unsigned int *p, unsigned int size);
unsigned int		rev_endian(unsigned int i);

t_op				get_op(char op_code);
