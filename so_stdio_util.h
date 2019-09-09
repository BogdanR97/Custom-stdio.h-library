#ifndef SO_STDIO_UTIL_H
#define SO_STDIO_UTIL_H

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define OP_NONE		0
#define OP_READ		1
#define OP_WRITE	2

#define BUFFSIZE	4096

enum io_mode {R, W, U};

typedef struct _so_file {

	int fd;
	int eof;
	int err;
	int buf_size;
	int buf_point;
	int last_read_size;
	enum io_mode mode;
	int last_op;
	pid_t child_pid;
	unsigned char buffer[BUFFSIZE];
} SO_FILE;

#endif
