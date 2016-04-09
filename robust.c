#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_mutex_t *gl;

static void
child(void)
{
	int error;

	for (;;) {
		error = pthread_mutex_lock(gl);
		switch (error) {
		case 0:
			break;
		case EOWNERDEAD:
			error = pthread_mutex_consistent(gl);
			if (error != 0) {
				fprintf(stderr, "pthread_mutex_consistent %s\n",
				    strerror(errno));
				exit(1);
			}
			/* XXX */
			break;
		case ENOTRECOVERABLE:
			/* XXX */
			break;
		default:
			fprintf(stderr, "pthread_mutex_lock %s\n",
			    strerror(error));
			exit(1);
		}
		error = pthread_mutex_unlock(gl);
		if (error != 0) {
			fprintf(stderr, "pthread_mutex_unlock %s\n",
			    strerror(error));
			exit(1);
		}
	}
}

int
main(void)
{
	pthread_mutexattr_t ma;
	int error;

	error = pthread_mutexattr_init(&ma);
	if (error != 0) {
		fprintf(stderr, "pthread_mutexattr_init %s\n", strerror(error));
		exit(1);
	}
	error = pthread_mutexattr_setpshared(&ma, PTHREAD_PROCESS_SHARED);
	if (error != 0) {
		fprintf(stderr, "pthread_mutexattr_setpshared %s\n",
		    strerror(error));
		exit(1);
	}
	error = pthread_mutexattr_setrobust(&ma, PTHREAD_MUTEX_ROBUST);
	if (error != 0) {
		fprintf(stderr, "pthread_mutexattr_setrobust %s\n",
		    strerror(error));
		exit(1);
	}
	gl = mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
	    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (gl == MAP_FAILED) {
		fprintf(stderr, "mmap gl: %s\n", strerror(errno));
		exit(1);
	}
	error = pthread_mutex_init(gl, &ma);
	if (error != 0) {
		fprintf(stderr, "pthread_mutex_init %s\n", strerror(error));
		exit(1);
	}
	return (0);
}
