#include "so_stdio.h"
#include "so_stdio_util.h"

int so_fseek(SO_FILE *stream, long offset, int whence)
{
	long buff_pos, real_pos, pos_diff = 0;
	int rc;

	/* Invalidate buffer if the last operation was READ */
	if (stream->last_op == OP_READ)
		stream->buf_point = stream->buf_size;
	/* Flush the buffer if the last operation was WRITE */
	else if (stream->last_op == OP_WRITE) {
		rc = so_fflush(stream);
		if (rc == SO_EOF)
			return -1;
	}
	stream->last_op = OP_NONE;

	/* Gets the difference between current position in the file
	 * and current position in the buffer
	 */
	if (whence == SEEK_CUR) {
		buff_pos = so_ftell(stream);
		if (buff_pos == -1)
			return -1;

		real_pos = lseek(stream->fd, 0, SEEK_CUR);
		if (real_pos == -1) {
			stream->err = 1;
			return -1;
		}
		pos_diff = real_pos - buff_pos;
	}

	buff_pos = lseek(stream->fd, offset - pos_diff, whence);
	if (buff_pos == -1) {
		stream->err = 1;
		return -1;
	}

	return 0;
}

long so_ftell(SO_FILE *stream)
{
	long file_pos = lseek(stream->fd, 0, SEEK_CUR);

	if (file_pos == -1) {
		stream->err = 1;
		return -1;
	}

	/* Sets the "file pointer" according to last operation on the buffer */
	if (stream->last_op == OP_READ)
		return file_pos - stream->last_read_size + stream->buf_point;
	else if (stream->last_op == OP_WRITE)
		return file_pos + stream->buf_point;

	return file_pos;
}

int so_fflush(SO_FILE *stream)
{
	size_t bytes_written;

	bytes_written = write(stream->fd, stream->buffer, stream->buf_point);
	if (bytes_written == -1) {
		stream->err = 1;
		return SO_EOF;
	}

	stream->buf_point = 0;

	return 0;
}

int so_fileno(SO_FILE *stream)
{
	return stream->fd;
}

int so_feof(SO_FILE *stream)
{
	return stream->eof;
}

int so_ferror(SO_FILE *stream)
{
	return stream->err;
}
