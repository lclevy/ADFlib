
#ifndef __ADF_DEV_FLOP_H__
#define __ADF_DEV_FLOP_H__

#include "adf_defs.h"
#include "adf_dev.h"
#include "prefix.h"

RETCODE adfMountFlop ( struct Device * dev );

PREFIX RETCODE adfCreateFlop ( struct Device * dev,
                               char *          volName,
                               int             volType );
#endif
