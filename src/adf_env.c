/*
 * ADF Library
 *
 * adf_env.c
 *
 *  $Id$
 *
 * library context and customization code
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

#include "adf_env.h"

#include "adf_blk.h"
#include "adf_byteorder.h"
#include "adf_dev_drivers.h"
#include "adf_dev_driver_dump.h"
#include "adf_dev_driver_ramdisk.h"
#include "adf_version.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


struct AdfEnv adfEnv;


static void Warningf ( const char * const format, ... );
static void Errorf ( const char * const format, ... );
static void Verbosef ( const char * const format, ... );

static void Changed ( SECTNUM nSect,
                      int     changedType );

static void rwHeadAccess ( SECTNUM physical,
                           SECTNUM logical,
                           bool    write );

static void progressBar ( int perCentDone );

static void checkInternals(void);


/*
 * adfInitEnv
 *
 */
void adfEnvInitDefault(void)
{
    checkInternals();

    adfEnv.wFct = Warningf;
    adfEnv.eFct = Errorf;
    adfEnv.vFct = Verbosef;

    adfEnv.notifyFct = Changed;
    adfEnv.rwhAccess = rwHeadAccess;
    adfEnv.progressBar = progressBar;

    adfEnv.useDirCache    = false;
    adfEnv.useRWAccess    = false;
    adfEnv.useNotify      = false;
    adfEnv.useProgressBar = false;

/*    sprintf(str,"ADFlib %s (%s)",adfGetVersionNumber(),adfGetVersionDate());
    (*adfEnv.vFct)(str);
*/
    adfAddDeviceDriver ( &adfDeviceDriverDump );
    adfAddDeviceDriver ( &adfDeviceDriverRamdisk );
}


/*
 * adfEnvCleanUp
 *
 */
void adfEnvCleanUp(void)
{
    adfRemoveDeviceDrivers();
}


/*
 * adfChgEnvProp
 *
 */
void adfChgEnvProp(int prop, void *newval)
{
    bool *newBool;
/*    int *newInt;*/

    switch(prop) {
    case PR_VFCT:
        adfEnv.vFct = (AdfLogFct) newval;
        break;
    case PR_WFCT:
        adfEnv.wFct = (AdfLogFct) newval;
        break;
    case PR_EFCT:
        adfEnv.eFct = (AdfLogFct) newval;
        break;
    case PR_NOTFCT:
        adfEnv.notifyFct = (AdfNotifyFct) newval;
        break;
    case PR_USE_NOTFCT:
        newBool = (bool *) newval;
        adfEnv.useNotify = *newBool;
        break;
    case PR_PROGBAR:
        adfEnv.progressBar = (AdfProgressBarFct) newval;
        break;
    case PR_USE_PROGBAR:
        newBool = (bool *) newval;
        adfEnv.useProgressBar = *newBool;
        break;
    case PR_USE_RWACCESS:
        newBool = (bool *) newval;
        adfEnv.useRWAccess = *newBool;
        break;
    case PR_RWACCESS:
        adfEnv.rwhAccess = (AdfRwhAccessFct) newval;
        break;
    case PR_USEDIRC:
        newBool = (bool *) newval;
        adfEnv.useDirCache = *newBool;
        break;
    }
}

/*
 *  adfSetEnv
 *
 */
void adfSetEnvFct ( const AdfLogFct    eFct,
                    const AdfLogFct    wFct,
                    const AdfLogFct    vFct,
                    const AdfNotifyFct notifyFct )
{
    if ( eFct != NULL )
        adfEnv.eFct = eFct;
    if ( wFct != NULL )
        adfEnv.wFct = wFct;
    if ( vFct != NULL )
        adfEnv.vFct = vFct;
    if ( notifyFct != NULL )
        adfEnv.notifyFct = notifyFct;
}


/*
 * adfGetVersionNumber
 *
 */
char* adfGetVersionNumber(void)
{
	return(ADFLIB_VERSION);
}


/*
 * adfGetVersionDate
 *
 */
char* adfGetVersionDate(void)
{
	return(ADFLIB_DATE);
}

/*##################################################################################*/

static void rwHeadAccess ( SECTNUM physical,
                           SECTNUM logical,
                           bool    write )
{
    /* display the physical sector, the logical block, and if the access is read or write */
    fprintf(stderr, "phy %d / log %d : %c\n", physical, logical, write ? 'W' : 'R');
}

static void progressBar ( int perCentDone )
{
    fprintf(stderr,"%d %% done\n",perCentDone);
}

//static void Warning(char* msg) {
//    fprintf(stderr,"Warning <%s>\n",msg);
//}

//static void Error(char* msg) {
//    fprintf(stderr,"Error <%s>\n",msg);
/*    exit(1);*/
//}

//static void Verbose(char* msg) {
//    fprintf(stderr,"Verbose <%s>\n",msg);
//}


static void Warningf ( const char * const format, ... )
{
    va_list ap;
    va_start ( ap, format );

    fprintf ( stderr, "Warning <" );
    vfprintf ( stderr, format, ap );
    fprintf ( stderr, ">\n" );
}


static void Errorf ( const char * const format, ... )
{
    va_list ap;
    va_start ( ap, format );

    fprintf ( stderr, "Error <" );
    vfprintf ( stderr, format, ap );
    fprintf ( stderr, ">\n" );
/*    exit(1);*/
}


static void Verbosef ( const char * const format, ... )
{
    va_list ap;
    va_start ( ap, format );

    fprintf ( stderr, "Verbose <" );
    vfprintf ( stderr, format, ap );
    fprintf ( stderr, ">\n" );
}


static void Changed(SECTNUM nSect, int changedType)
{
    (void) nSect, (void) changedType;
/*    switch(changedType) {
    case ADF_ST_FILE:
        fprintf(stderr,"Notification : sector %ld (FILE)\n",nSect);
        break;
    case ADF_ST_DIR:
        fprintf(stderr,"Notification : sector %ld (DIR)\n",nSect);
        break;
    case ADF_ST_ROOT:
        fprintf(stderr,"Notification : sector %ld (ROOT)\n",nSect);
        break;
    default:
        fprintf(stderr,"Notification : sector %ld (???)\n",nSect);
    }
*/}


union u {
    int32_t l;
    char    c[4];
};

static void assertInternal ( bool cnd, const char * const msg )
{
    if ( ! cnd ) {
        fputs ( msg, stderr );
        exit(1);
    }
}

static void checkInternals(void)
{
/*    char str[80];*/
    union u val;

    /* internal checking */

    assertInternal ( sizeof(short) == 2,
                     "Compilation error : sizeof(short)!=2\n" );

    assertInternal ( sizeof(int32_t) == 4,
                     "Compilation error : sizeof(int32_t) != 4\n" );

    assertInternal ( sizeof(struct AdfEntryBlock) == 512,
                     "Internal error : sizeof(struct AdfEntryBlock) != 512\n");

    assertInternal ( sizeof(struct AdfRootBlock) == 512,
                     "Internal error : sizeof(struct AdfRootBlock) != 512\n");

    assertInternal ( sizeof(struct AdfDirBlock) == 512,
                     "Internal error : sizeof(struct AdfDirBlock) != 512\n");

    assertInternal ( sizeof(struct AdfBootBlock) == 1024,
                     "Internal error : sizeof(struct AdfBootBlock) != 1024\n" );

    assertInternal ( sizeof(struct AdfFileHeaderBlock) == 512,
                     "Internal error : sizeof(struct AdfFileHeaderBlock) != 512\n" );

    assertInternal ( sizeof(struct AdfFileExtBlock) == 512,
                     "Internal error : sizeof(struct AdfFileExtBlock) != 512\n" );

    assertInternal ( sizeof(struct AdfOFSDataBlock) == 512,
                     "Internal error : sizeof(struct AdfOFSDataBlock) != 512\n" );

    assertInternal ( sizeof(struct AdfBitmapBlock) == 512,
                     "Internal error : sizeof(struct AdfBitmapBlock) != 512\n" );

    assertInternal ( sizeof(struct AdfBitmapExtBlock) == 512,
                     "Internal error : sizeof(struct AdfBitmapExtBlock) != 512\n" );

    assertInternal ( sizeof(struct AdfLinkBlock) == 512,
                     "Internal error : sizeof(struct AdfLinkBlock) != 512\n" );

    val.l = 1L;
/* if LITT_ENDIAN not defined : must be BIG endian */
#ifndef LITT_ENDIAN
    assertInternal ( val.c[3] == 1, /* little endian : LITT_ENDIAN must be defined ! */
                     "Compilation error : #define LITT_ENDIAN must exist\n" );
#else
    assertInternal ( val.c[3] != 1, /* big endian : LITT_ENDIAN must not be defined ! */
                     "Compilation error : #define LITT_ENDIAN must not exist\n" );
#endif
}
