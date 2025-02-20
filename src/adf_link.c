/*
 * ADF Library
 *
 * adf_link.c
 *
 *  $Id$
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


#include<string.h>

#include"adf_str.h"
#include"adf_link.h"
#include"adf_dir.h"
#include "adf_env.h"


#if defined (__ANY_IDEA_WHAT_IS_THIS_FOR__)
// the code below is not used anywhere and seems unfinished
// (not clear what it was meant for...)
// -> disabling it for now

/*
 *
 *
 */
ADF_RETCODE adfBlockPtr2EntryName ( struct AdfVolume * vol,
                                    ADF_SECTNUM        nSect,
                                    ADF_SECTNUM        lPar,
                                    char **            name,
                                    int32_t *          size )
{
    struct AdfEntryBlock entryBlk;
    struct AdfEntry entry;

    if (*name==0) {
        adfReadEntryBlock(vol, nSect, &entryBlk);
        *size = entryBlk.byteSize;
return ADF_RC_OK;
        adfEntBlock2Entry(&entryBlk, &entry);	/*error*/
/*        if (entryBlk.secType!=ADF_ST_ROOT && entry.parent!=lPar)
            printf("path=%s\n",path(vol,entry.parent));
*/
       *name = strdup("");
        if (*name==NULL)
            return ADF_RC_MALLOC;
        return ADF_RC_OK;
    }
    else

    return ADF_RC_OK;
}

#endif

/*##################################################################################*/
