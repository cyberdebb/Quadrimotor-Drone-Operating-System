#ifndef PIPE_H
#define	PIPE_H

#include "types.h"

void create_pipe(pipe_t *p);
void read_pipe(pipe_t *p, char *buffer);
void write_pipe(pipe_t *p, char buffer);

#endif	/* PIPE_H */

