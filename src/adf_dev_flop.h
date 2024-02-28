
#ifndef ADF_DEV_FLOP_H
#define ADF_DEV_FLOP_H

#include "adf_dev.h"
#include "adf_err.h"
#include "adf_prefix.h"

ADF_RETCODE adfMountFlop ( struct AdfDevice * dev );

PREFIX ADF_RETCODE adfCreateFlop ( struct AdfDevice * const dev,
                                   const char * const       volName,
                                   const uint8_t            volType );
#endif  /* ADF_DEV_FLOP_H */
