#include "so_stdio.h"
#include "so_stdio_util.h"

SO_FILE *so_popen(const char *command, const char *type)
{
	int rc, pipefd[2];
	pid_t child_pid;
	SO_FILE *stream;

	char *const argvec[] = {"sh", "-c", (char *)command, NULL};

	/* Creates a pipe so that the child and parent process
	 * can communicate
	 */
	rc = pipe(pipefd);
	if (rc < 0)
		return NULL;

	child_pid = fork();
	if (child_pid == -1) {
		return NULL;

	/* Child process will execute the command */
	} else if (child_pid == 0) {

		/* According to the open mode, the child process
		 * will get the file descriptor for
		 * the read-end of the pipe(pipefd[0]) or
		 * the write-end(pipefd[1])
		 */
		if (strcmp(type, "r") == 0) {
			rc = dup2(pipefd[1], 1);
			if (rc < 0)
				return NULL;

			rc = close(pipefd[1]);
			if (rc < 0)
				return NULL;

			execv("/bin/sh", argvec);

		} else if (strcmp(type, "w") == 0) {
			dup2(pipefd[0], 0);
			if (rc < 0)
				return NULL;

			rc = close(pipefd[1]);
			if (rc < 0)
				return NULL;

			execv("/bin/sh", argvec);
		} else
			/* Could not match against any valid mode */
			return NULL;
	}

	enum io_mode mode;
	int fd;

	/* According to the open mode, the parent process
	 * will get the file descriptor for
	 * the read-end of the pipe(pipefd[0]) or
	 * the write-end(pipefd[1])
	 */

	if (strcmp(type, "r") == 0) {
		mode = R;
		fd = pipefd[0];
		rc = close(pipefd[1]);
		if (rc < 0)
			return NULL;

	} else if (strcmp(type, "w") == 0) {
		mode = W;
		fd = pipefd[1];
		rc = close(pipefd[0]);
		if (rc < 0)
			return NULL;

	} else
		/* Could not match against any valid mode */
		return NULL;

	stream = malloc(sizeof(SO_FILE));
	if (stream == NULL)
		return NULL;

	stream->fd = fd;
	stream->eof = 0;
	stream->err = 0;
	stream->buf_size = BUFFSIZE;
	stream->mode = mode;
	stream->last_op = OP_NONE;
	stream->child_pid = child_pid;

	return stream;
}

int so_pclose(SO_FILE *stream)
{
	int status, rc;
	pid_t child_pid = stream->child_pid;

	rc = so_fclose(stream);
	if (rc == SO_EOF)
		return SO_EOF;

	waitpid(child_pid, &status, 0);

	return status;
}
