include upc.make
include shmem.make

SRCS = 	mpp_bench.c test_all2all.c test_barrier.c test_bcast.c \
	test_pingpong.c test_reduce.c mpp_bench.h makefile README

OBJS1 = test_all2all.o mpp_bench.o
OBJS2 = test_barrier.o mpp_bench.o
OBJS3 = test_bcast.o mpp_bench.o
OBJS4 = test_pingpong.o mpp_bench.o
#OBJS4 = test_pingpong_jwf.o mpp_bench.o
OBJS5 = test_reduce.o mpp_bench.o

all:	clobber upc my_clean shmem


shmem:
	$(MAKE) CC='$(CC_S)' CFLAGS='$(CFLAGS_S)' LFLAGS='$(LFLAGS_S)' SUFFIX='$(SUFFIX_S)' all_progs


upc:
	$(MAKE) CC='$(CC_U)' CFLAGS='$(CFLAGS_U)' LFLAGS='$(LFLAGS_U)' SUFFIX='$(SUFFIX_U)' all_progs

all_progs:	all2all barrier bcast pingpong reduce

all2all: $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $(OBJS1) $(LFLAGS)
	mv all2all all2all.$(SUFFIX)
	cp all2all.$(SUFFIX) bin/all2all.$(SUFFIX)


barrier: $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2) $(LFLAGS)
	mv barrier barrier.$(SUFFIX)
	cp barrier.$(SUFFIX) bin/barrier.$(SUFFIX)

bcast: $(OBJS3)
	$(CC) $(CFLAGS) -o $@ $(OBJS3) $(LFLAGS)
	mv bcast bcast.$(SUFFIX)
	cp bcast.$(SUFFIX) bin/bcast.$(SUFFIX)

pingpong: $(OBJS4)
	$(CC) $(CFLAGS) -o $@ $(OBJS4) $(LFLAGS)
	mv pingpong pingpong.$(SUFFIX)
	cp pingpong.$(SUFFIX) bin/pingpong.$(SUFFIX)

reduce: $(OBJS5)
	$(CC) $(CFLAGS) -o $@ $(OBJS5) $(LFLAGS)
	mv reduce reduce.$(SUFFIX)
	cp reduce.$(SUFFIX) bin/reduce.$(SUFFIX)


$(OBJS1): makefile mpp_bench.h

$(OBJS2): makefile mpp_bench.h

$(OBJS3): makefile mpp_bench.h

$(OBJS4): makefile mpp_bench.h

$(OBJS5): makefile mpp_bench.h

.c.o:
	$(CC) $(CFLAGS) -c $<

mpp_bench.tgz: $(SRCS)
	tar -zcvf $@ $(SRCS)

clean:
	/bin/rm -f *~ $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) *.w2c.c *.w2c.h bin/* all2all barrier bcast pingpong reduce *.upc.exe *.shmem

clobber:
	/bin/rm -f *~ $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) \
	all2all barrier bcast pingpong reduce *.upc.exe *.shmem *.w2c.c *.w2c.h

my_clean:
	/bin/rm -f *~ $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) *.w2c.c *.w2c.h

run:
	srun 2 bin/all2all.shmem 1 4096 64 4
	srun 2 bin/all2all.upc.exe 1 4096 64 4
	srun 2 bin/barrier.shmem 100000
	srun 2 bin/barrier.upc.exe 100000
	srun 2 bin/bcast.shmem 0 100000 1000
	srun 2 bin/bcast.upc.exe 0 100000 1000
	srun 2 bin/pingpong.shmem 0 4 16
	srun 2 bin/pingpong.upc.exe 0 4 16
	srun 2 bin/reduce.shmem 100000
	srun 2 bin/reduce.upc.exe 100000
