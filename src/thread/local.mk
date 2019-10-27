

noinst_LIBRARIES += src/thread/libthread.a
nodist_src_thread_libthread_a_SOURCES = \
	src/thread/call_once.c \
	src/thread/cond_broadcast.c \
	src/thread/cond_destroy.c \
	src/thread/cond_init.c \
	src/thread/cond_signal.c \
	src/thread/cond_timedwait.c	\
	src/thread/cond_wait.c \
	src/thread/mutex_destroy.c \
	src/thread/mutex_init.c \
	src/thread/mutex_lock.c \
	src/thread/mutex_timedlock.c \
	src/thread/mutex_trylock.c \
	src/thread/mutex_unlock.c \
	src/thread/tss_create.c \
	src/thread/tss_delete.c \
	src/thread/tss_get.c \
	src/thread/tss_set.c
