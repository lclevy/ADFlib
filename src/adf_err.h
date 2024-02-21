#ifndef ADF_ERR_H
#define ADF_ERR_H

/*
 * adf_err.h
 *
 *  $Id$
 *
 *  error codes
 *
 *  This file is part of ADFLib.
 *
 *  ADFLib is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  ADFLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "adf_types.h"

typedef enum {
    RC_OK              = 0,
    RC_ERROR           = -1,

    RC_MALLOC          = 1,
    RC_VOLFULL         = 2,

    RC_FOPEN           = 1 << 10,
    RC_NULLPTR         = 1 << 12,

/* adfRead*Block() */

    RC_BLOCKTYPE       = 1,
    RC_BLOCKSTYPE      = 1 << 1,
    RC_BLOCKSUM        = 1 << 2,
    RC_HEADERKEY       = 1 << 3,
    RC_BLOCKREAD       = 1 << 4,

/* adfWrite*Block */
    RC_BLOCKWRITE      = 1 << 4,

/* adfVolReadBlock() */
    RC_BLOCKOUTOFRANGE = 1,
    RC_BLOCKNATREAD    = 1 << 1,

/* adfVolWriteBlock() */
/* RC_BLOCKOUTOFRANGE */
    RC_BLOCKNATWRITE   = 1 << 1,
    RC_BLOCKREADONLY   = 1 << 2,

/* adfInitDumpDevice() */
/* RC_FOPEN */
/* RC_MALLOC */

/* adfNativeReadBlock(), adfReadDumpSector() */

    RC_BLOCKSHORTREAD  = 1,
    RC_BLOCKFSEEK      = 1 << 1,

/* adfNativeWriteBlock(), adfWriteDumpSector() */

    RC_BLOCKSHORTWRITE = 1,
/* RC_BLOCKFSEEK */


/*-- adfReadRDSKblock --*/
    RC_BLOCKID         = 1 << 5,

/*-- adfWriteRDSKblock() --*/
/*RC_BLOCKREADONLY*/
} RETCODE;

//#define hasRC(rc,c) ((rc)&(c))
static inline bool adfHasRC ( RETCODE mask, RETCODE code ) {
    return ( mask & code );
}
#endif /* ADF_ERR_H */

/*############################################################################*/
