#ifndef PIPE_H
#define	PIPE_H

#include "types.h"

pipe_t* pipe_create(uint8_t size);      //Retorna ponteiro para novo pipe ou NULL
void pipe_destroy(pipe_t *p);           //Libera um pipe

//void create_pipe(pipe_t *p);          // Como estava antes

void read_pipe(pipe_t *p, char *buffer);
void write_pipe(pipe_t *p, char buffer);

#endif	/* PIPE_H */

