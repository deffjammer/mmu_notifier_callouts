#SGI
ifneq ($(shell rpm -qa sgi-mpt),)
        CC_U = /opt/sgi/upc/upc-1.10/bin/sgiupc -w
        CFLAGS_U = -I. -D__UPC__
        LFLAGS_U =
        SUFFIX_U = upc.exe
endif

# CRAY
ifeq ($(CRAY_SITE_LIST_DIR), /etc/opt/cray/modules)
     ifeq ($(PE_ENV), CRAY)
       CC_U = cc
     else
       CC_U = not_cray_compiler
     endif
     CFLAGS_U = -h upc -D__UPC__ -I. -I..
     LFLAGS_U =
     SUFFIX_U = upc.exe
endif

