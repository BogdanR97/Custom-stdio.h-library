#include "so_stdio.h"
#include "so_stdio_util.h"

static void set_buffpoint(SO_FILE *stream, int op)
{
	if (stream->last_op != OP_NONE)
		return;

	if (stream->mode == R) {
		stream->buf_point = stream->buf_size;
		stream->last_read_size = stream->buf_size;

	} else if (stream->mode == W) {
		stream->buf_point = 0;

	} else if (stream->mode == U) {
		if (op == OP_READ) {
			stream->buf_point = stream->buf_size;
			stream->last_read_size = stream->buf_size;
		} else {
			stream->buf_point = 0;
		}
	}
}

int so_fgetc(SO_FILE *stream)
{
	size_t bytes_read;

	set_buffpoint(stream, OP_READ);

	if (stream->buf_point >= stream->last_read_size) {
		bytes_read = read(stream->fd, stream->buffer, stream->buf_size);
		if (bytes_read == -1) {
			stream->err = 1;
			return SO_EOF;
		} else if (bytes_read == 0) {
			stream->eof = 1;
			return SO_EOF;
		}

		stream->last_read_size = bytes_read;
		stream->buf_point = 0;
	}
	stream->last_op = OP_READ;
	return (int)stream->buffer[stream->buf_point++];
}

int so_fputc(int c, SO_FILE *stream)
{
	size_t bytes_written;

	set_buffpoint(stream, OP_WRITE);

	if (stream->buf_point >= stream->buf_size) {
		bytes_written = write(stream->fd, stream->buffer,
								stream->buf_size);
		if (bytes_written == -1) {
			stream->err = 1;
			return SO_EOF;
		}

		stream->buf_point = 0;
	}

	stream->buffer[stream->buf_point++] = (unsigned char)c;
	stream->last_op = OP_WRITE;

	return c;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, j;
	unsigned char *ptr_elem;
	int temp;

	ptr_elem = malloc(size * sizeof(unsigned char));
	if (ptr_elem == NULL)
		return 0;

	for (i = 0; i < nmemb; i++) {
		for (j = 0; j < size; j++) {
			temp = so_fgetc(stream);
			/* Checks if there was an error */
			if (temp == SO_EOF) {
				free(ptr_elem);
				ptr_elem = NULL;
				/* Checks if the end of file was reached */
				if (so_feof(stream) != 0)
					return i;
				else
					return 0;
			}
			ptr_elem[j] = (unsigned char)temp;
		}
		memcpy((ptr + i), ptr_elem, size);
	}

	free(ptr_elem);
	ptr_elem = NULL;
	return i;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, j;
	unsigned char *ptr_elem;
	int temp;

	ptr_elem = malloc(size * sizeof(unsigned char));
	if (ptr_elem == NULL)
		return 0;

	for (i = 0; i < nmemb; i++) {
		memcpy(ptr_elem, (ptr + i), size);

		for (j = 0; j < size; j++) {
			temp = so_fputc((int)ptr_elem[j], stream);
			if (temp == SO_EOF) {
				free(ptr_elem);
				ptr_elem = NULL;
				return 0;
			}
		}
	}

	free(ptr_elem);
	ptr_elem = NULL;
	return i;
}
