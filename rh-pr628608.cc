#include <boost/lexical_cast.hpp>

#include <exception>
#include <new>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

namespace {

struct Error : std::exception
{
    char buf_[1024];

    Error(char const* fmt, ...) __attribute__ ((format(printf,2,3)))
    {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf_, sizeof buf_, fmt, ap);
        va_end(ap);
    }

    char const* what() const throw() { return buf_; }
};

void initialize(pthread_mutex_t& mtx)
{
    pthread_mutexattr_t mtxa;
    if(int err = pthread_mutexattr_init(&mtxa))
        throw Error("pthread_mutexattr_init: (%d)%s", err, strerror(err));
    if(int err = pthread_mutexattr_setpshared(&mtxa, PTHREAD_PROCESS_SHARED))
        throw Error("pthread_mutexattr_setpshared: (%d)%s", err, strerror(err));
    // often one process is a real-time and another is not. protect from priority inversion.
    if(int err = pthread_mutexattr_setprotocol(&mtxa, PTHREAD_PRIO_INHERIT))
        throw Error("pthread_mutexattr_setprotocol: (%d)%s", err, strerror(err));
    // if another process is killed while holding the mutex, the other process must be unaffected
    if(int err = pthread_mutexattr_setrobust(&mtxa, PTHREAD_MUTEX_ROBUST))
        throw Error("pthread_mutexattr_setrobust: (%d)%s", err, strerror(err));
    if(int err = pthread_mutex_init(&mtx, &mtxa))
        throw Error("pthread_mutex_init: (%d)%s", err, strerror(err));
    pthread_mutexattr_destroy(&mtxa);
}

void initialize(pthread_cond_t& cnd)
{
    pthread_condattr_t cnda;
    if(int err = pthread_condattr_init(&cnda))
        throw Error("pthread_condattr_init: (%d)%s", err, strerror(err));
    if(int err = pthread_condattr_setpshared(&cnda, PTHREAD_PROCESS_SHARED))
        throw Error("pthread_condattr_setpshared: (%d)%s", err, strerror(err));
    if(int err = pthread_cond_init(&cnd, &cnda))
        throw Error("pthread_cond_init: (%d)%s", err, strerror(err));
    pthread_condattr_destroy(&cnda);
}


bool set_consistent(pthread_mutex_t& mtx)
{
    if(int err = pthread_mutex_consistent(&mtx))
        throw Error("pthread_mutex_consistent: (%d)%s", err, strerror(err));
    // the mutex is locked, this thread owns it now
    return true;
}

void lock(pthread_mutex_t& mtx, bool* abandoned)
{
    if(int err = pthread_mutex_lock(&mtx)) {
        if(EOWNERDEAD == err) // handle abandoned mutex
            *abandoned = set_consistent(mtx);
        else
            throw Error("pthread_mutex_lock: (%d)%s", err, strerror(err));
    }
    else {
        *abandoned = false;
    }
}

void unlock(pthread_mutex_t& mtx)
{
    if(int err = pthread_mutex_unlock(&mtx))
        throw Error("pthread_mutex_unlock: (%d)%s", err, strerror(err));
}

void signal(pthread_cond_t& cnd)
{
    if(int err = pthread_cond_signal(&cnd))
        throw Error("pthread_cond_signal: (%d)%s", err, strerror(err));
}

void wait(pthread_cond_t& cnd, pthread_mutex_t& mtx, bool* abandoned)
{
    if(int err = pthread_cond_wait(&cnd, &mtx)) {
        if(EOWNERDEAD == err) // handle abandoned mutex
            *abandoned = set_consistent(mtx);
        else
            throw Error("pthread_cond_wait: (%d)%s", err, strerror(err));
    }
    else {
        *abandoned = false;
    }
}

struct SharedData
{
    pthread_mutex_t mtx_1;
    pthread_mutex_t mtx_2;
    pthread_cond_t cnd;
    unsigned long event;

    SharedData()
        : event()
    {
        initialize(mtx_1);
        initialize(mtx_2);
        initialize(cnd);
    }

    static SharedData* map(char const* filename, unsigned pid)
    {
        int fd = open(filename, O_CREAT | O_RDWR, mode_t(0666));
        if(fd < 0)
            throw Error("open('%s'): (%d)%s", filename, errno, strerror(errno));
        struct stat st;
        if(fstat(fd, &st))
            throw Error("fstat('%s'): (%d)%s", filename, errno, strerror(errno));
        bool new_file = !st.st_size;
        if(new_file) {
            if(ftruncate(fd, sizeof(SharedData)))
                throw Error("ftruncate('%s'): (%d)%s", filename, errno, strerror(errno));
        }
        void* mem = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        if(MAP_FAILED == mem)
            throw Error("mmap('%s'): (%d)%s", filename, errno, strerror(errno));
        if(new_file)
            new (mem) SharedData;
        printf("%u: opened %s file\n", pid, new_file ? "new" : "existing");
        return static_cast<SharedData*>(mem);
    }
};

char const shared_file[] = "shared_file_test~";

pid_t spawn(int(*fn)())
{
    // fork a child process
    pid_t pid = fork();
    switch(pid) {
    case 0:
        exit(fn());
    case -1:
        abort();
    default:
        return pid;
    }
}

int process_1()
{
    unsigned pid = getpid();
    printf("%u: process 1\n", pid);

    SharedData* shared_data = SharedData::map(shared_file, pid);
    bool abandoned_not_used;
    lock(shared_data->mtx_1, &abandoned_not_used); // abandon this mutex, EOWNERDEAD in pthread_cond_wait
    lock(shared_data->mtx_2, &abandoned_not_used); // abandon this mutex, EOWNERDEAD in pthread_mutex_lock
    shared_data->event = 1;
    signal(shared_data->cnd);
    printf("%u: terminated\n", pid);

    return 0;
}

int process_2()
{
    unsigned pid = getpid();
    printf("%u: process 2\n", pid);

    SharedData* shared_data = SharedData::map(shared_file, pid);
    printf("%u: locking shared data\n", pid);

    bool abandoned;
    int failures = 0;

    printf("%u: locking mtx_1...\n", pid);
    lock(shared_data->mtx_1, &abandoned);
    failures += !abandoned;
    if(!abandoned)
        fprintf(stderr, "check failed: mtx_1 is not reported as abandoned\n");
    printf("%u: mtx_1 locked\n", pid);

    while(1 != shared_data->event)
        wait(shared_data->cnd, shared_data->mtx_1, &abandoned);
    // if(!abandoned)
    //     fprintf("check failed: mtx_1 is not reported as abandoned");

    printf("%u: locking mtx_2...\n", pid);
    lock(shared_data->mtx_2, &abandoned);
    failures += !abandoned;
    if(!abandoned)
        fprintf(stderr, "check failed: mtx_1 is not reported as abandoned\n");
    printf("%u: mtx_2 locked\n", pid);

    unlock(shared_data->mtx_1);
    unlock(shared_data->mtx_2);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}

}

int main(int ac, char** av)
{
    unsigned mode = ac > 1 ? boost::lexical_cast<unsigned>(1[av]) : 0;
    switch(mode) {
    case 0: {
        // start with a new file
        unlink(shared_file);
        // fork process_1 and wait till it terminates
        pid_t child = spawn(process_1);
        int child_status;
        if(-1 == waitpid(child, &child_status, 0))
            throw Error("waitpid: (%d)%s", errno, strerror(errno));

        // now do process_2
        return process_2();
    }

    case 1:
        // start with a new file
        unlink(shared_file);
        // run only process 1
        return process_1();

    case 2:
        // start with an existing file with abandoned mutexes
        // run only process 2
        return process_2();
    }
}
