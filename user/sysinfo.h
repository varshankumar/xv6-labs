#ifndef USER_SYSINFO_H
#define USER_SYSINFO_H

#include "types.h"

struct sysinfo {
    uint64 freemem;
    uint64 nproc;
};

#endif