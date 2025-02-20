/*
 * adf_show_metadata
 *
 * an utility for displaying Amiga disk images (ADF) metadata
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

#include <stdio.h>

#include "adf_show_metadata_common.h"

void show_hashtable ( const uint32_t hashtable[ADF_HT_SIZE] )
{
    printf ( "\nHashtable (non-zero):\n" );
    for ( unsigned i = 0 ; i < ADF_HT_SIZE ; ++i ) {
        uint32_t hash_i = hashtable [ i ];
        if ( hash_i )
            printf ( "  hashtable [ %2u ]:\t\t0x%x\t\t%u\n",
                     i, hash_i, hash_i );
    }
}
