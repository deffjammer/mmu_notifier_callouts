# SGI
ifneq ($(shell rpm -qa sgi-mpt),)
        CC_S = icc -D_SGI
        CFLAGS_S = -I. -I$(MPI_ROOT)/include
        LFLAGS_S = -lsma -lmpi
        SUFFIX_S = shmem
endif

# CRAY
ifeq ($(CRAY_SITE_LIST_DIR), /etc/opt/cray/modules)
     CC_S = cc
     CFLAGS_S = -I. -I.. -D_CRAY
     LFLAGS_S =
     SUFFIX_S = shmem
endif


