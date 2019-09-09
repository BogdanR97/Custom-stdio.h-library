#include "so_stdio.h"
#include "so_stdio_util.h"

static int get_fd(const char *pathname, const char *mode, enum io_mode *type)
{
	int fd;

	if (strcmp(mode, "r") == 0) {
		*type = R;
		fd = open(pathname, O_RDONLY);

	} else if (strcmp(mode, "r+") == 0) {
		*type = U;
		fd = open(pathname, O_RDWR);

	} else if (strcmp(mode, "w") == 0) {
		*type = W;
		fd = open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0644);

	} else if (strcmp(mode, "w+") == 0) {
		*type = U;
		fd = open(pathname, O_RDWR | O_TRUNC | O_CREAT, 0644);

	} else if (strcmp(mode, "a") == 0) {
		*type = W;
		fd = open(pathname, O_WRONLY | O_APPEND | O_CREAT, 0644);

	} else if (strcmp(mode, "a+") == 0) {
		*type = U;
		fd = open(pathname, O_RDWR | O_APPEND | O_CREAT, 0644);

	} else
		/* Could not match against any valid mode */
		return -1;

	return fd;
}

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *stream;
	enum io_mode type;
	int fd;

	fd = get_fd(pathname, mode, &type);
	if (fd < 0)
		return NULL;

	stream = malloc(sizeof(SO_FILE));
	if (stream == NULL)
		return NULL;

	stream->fd = fd;
	stream->eof = 0;
	stream->err = 0;
	stream->buf_size = BUFFSIZE;
	stream->mode = type;
	stream->last_op = OP_NONE;

	return stream;
}

int so_fclose(SO_FILE *stream)
{
	int rc1 = 0, rc2;

	if (stream->last_op == OP_WRITE)
		rc1 = so_fflush(stream);

	rc2 = close(stream->fd);

	free(stream);
	stream = NULL;

	if (rc1 == SO_EOF || rc2 < 0)
		return SO_EOF;

	return 0;
}
