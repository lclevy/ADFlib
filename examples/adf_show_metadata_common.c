
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
