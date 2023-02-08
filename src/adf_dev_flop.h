
#ifndef __ADF_DEV_FLOP_H__
#define __ADF_DEV_FLOP_H__

#include "adf_defs.h"
#include "adf_dev.h"
#include "prefix.h"

RETCODE adfMountFlop ( struct adfDevice * dev );

PREFIX RETCODE adfCreateFlop ( struct adfDevice * dev,
                               char *             volName,
                               int                volType );
#endif
