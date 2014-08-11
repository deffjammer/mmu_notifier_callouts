#!/bin/bash

# Default to sles11sp2
BUILD_DIST=${BUILD_DIST:-"sles11sp2"}

USE_CHROOT_KERNEL=0
while getopts "C" c; do
  case $c in
    C)	USE_CHROOT_KERNEL=1; shift; break;
  esac
done

KVER_sles11="2.6.32.12-0.7"
KVER_sles11sp2="3.0.13-0.27"
KVER_sles11sp3="3.0.76-0.11"
KVER_sles12="3.0.76-0.11"
KVER_rhel6="2.6.32-71.el6.x86_64"
KVER_rhel7="3.5.0-0.24.el7"

if [ ${USE_CHROOT_KERNEL} -ne 0 ]; then
  BLDROOT=${BLDROOT:-${WORKAREA}/bldroot.${BUILD_DIST}}
  BDKVER=$(eval echo \${KVER_${BUILD_DIST}})
  KVER=${KVER:-${BDKVER}}
  KVARIANT=${KVARIANT:-"default"}
  if [ -e ${BLDROOT}/usr/src/linux-${KVER}-obj/x86_64/${KVARIANT}/ ]; then
    KREL=${KREL:-"${KVER}-${KVARIANT}"}
    KSRC=${KSRC:-"${BLDROOT}/usr/src/linux-${KVER}-obj/x86_64/${KVARIANT}/"}
  elif [ -e ${BLDROOT}/lib/modules/${KVER}/build ]; then
    KREL=${KREL:-"${KVER}"}
  fi
else
  if [ -f /etc/SuSE-release ]; then
    HOST_BUILD_DIST=$(awk '/VERSION/ {ver=$3};/PATCHLEVEL/ {print "sles" ver "sp" $3}' </etc/SuSE-release)
    HOST_KVER=$(eval echo \${KVER_${HOST_BUILD_DIST}})
  fi
fi

KREL=${KREL:-`uname -r`}
BLDROOT=${BLDROOT:-"/"}
KSRC=${KSRC:-"${BLDROOT}/lib/modules/${KREL}/build"}
MRKR="!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

if [ $# -eq 0 ]; then
  set -- modules
fi

echo \
make -j -C ${KSRC} EXTRA_CFLAGS="${EXTRA_CFLAGS} -I${PWD}/../include/ -Werror" M=${PWD} $@
make -j -C ${KSRC} EXTRA_CFLAGS="${EXTRA_CFLAGS} -I${PWD}/../include/ -Werror" M=${PWD} $@ && \
  echo -en ${MSG}

[ -d linux/uv ] && rm -rf linux

if [ ${USE_CHROOT_KERNEL} -ne 0 ]; then
  GCC_VERSION=$(gcc --version | head -1 | sed -e 's/^.*) //' -e 's/\([0-9]*\.[0-9]*\).*/\1/')
  GCC_VER_sles11="4.3"
  GCC_VER_sles11sp2="4.3"
  GCC_VER_sles11sp3="4.3"
  GCC_VER_sles12="4.3"
  GCC_VER_rhel6="4.4"
  GCC_VER_rhel7="4.7"
  EXPECTED_GCC_VERSION=$(eval echo \${GCC_VER_${BUILD_DIST}})
  if [ ${GCC_VERSION} != ${EXPECTED_GCC_VERSION} ]; then
    echo "GCC Version mismatch.  Used ${GCC_VERSION}, expected ${EXPECTED_GCC_VERSION}"
  fi
fi

# vi: sts=2
