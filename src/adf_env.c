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
 *  along with ADFLib; if not, write to the Free Software
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

static void Changed ( ADF_SECTNUM nSect,
                      int         changedType );

static void rwHeadAccess ( const ADF_SECTNUM physical,
                           const ADF_SECTNUM logical,
                           const bool        write );

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
    adfEnv.ignoreChecksumErrors = false;
    adfEnv.quiet          = false;

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
 * adfEnvSetProperty
 *
 */
ADF_RETCODE adfEnvSetProperty ( const ADF_ENV_PROPERTY property,
                                const intptr_t         newval )
{
    switch ( property ) {
    case ADF_PR_VFCT:
        adfEnv.vFct = (AdfLogFct) newval;
        break;
    case ADF_PR_WFCT:
        adfEnv.wFct = (AdfLogFct) newval;
        break;
    case ADF_PR_EFCT:
        adfEnv.eFct = (AdfLogFct) newval;
        break;
    case ADF_PR_NOTFCT:
        adfEnv.notifyFct = (AdfNotifyFct) newval;
        break;
    case ADF_PR_USE_NOTFCT:
        adfEnv.useNotify = (bool) newval;
        break;
    case ADF_PR_PROGBAR:
        adfEnv.progressBar = (AdfProgressBarFct) newval;
        break;
    case ADF_PR_USE_PROGBAR:
        adfEnv.useProgressBar = (bool) newval;
        break;
    case ADF_PR_USE_RWACCESS:
        adfEnv.useRWAccess = (bool) newval;
        break;
    case ADF_PR_RWACCESS:
        adfEnv.rwhAccess = (AdfRwhAccessFct) newval;
        break;
    case ADF_PR_USEDIRC:
        adfEnv.useDirCache = (bool) newval;
        break;
    case ADF_PR_IGNORE_CHECKSUM_ERRORS:
        adfEnv.ignoreChecksumErrors =  (bool) newval;
        break;
    case ADF_PR_QUIET:
        adfEnv.quiet =  (bool) newval;
        break;
    default:
        adfEnv.eFct ( "adfEnvSetProp: invalid property %d", property );
        return ADF_RC_ERROR;
    }
    return ADF_RC_OK;
}


intptr_t adfEnvGetProperty ( const ADF_ENV_PROPERTY property )
{
    switch ( property ) {
    case ADF_PR_VFCT:                    return (intptr_t) adfEnv.vFct;
    case ADF_PR_WFCT:                    return (intptr_t) adfEnv.wFct;
    case ADF_PR_EFCT:                    return (intptr_t) adfEnv.eFct;
    case ADF_PR_NOTFCT:                  return (intptr_t) adfEnv.notifyFct;
    case ADF_PR_USE_NOTFCT:              return (intptr_t) adfEnv.useNotify;
    case ADF_PR_PROGBAR:                 return (intptr_t) adfEnv.progressBar;
    case ADF_PR_USE_PROGBAR:             return (intptr_t) adfEnv.useProgressBar;
    case ADF_PR_USE_RWACCESS:            return (intptr_t) adfEnv.useRWAccess;
    case ADF_PR_RWACCESS:                return (intptr_t) adfEnv.rwhAccess;
    case ADF_PR_USEDIRC:                 return (intptr_t) adfEnv.useDirCache;
    case ADF_PR_IGNORE_CHECKSUM_ERRORS:  return (intptr_t) adfEnv.ignoreChecksumErrors;
    case ADF_PR_QUIET:                   return (intptr_t) adfEnv.quiet;
    default:
        adfEnv.eFct ( "adfEnvGetProp: invalid property %d", property );
    }
    return 0;
}


/*
 *  adfEnvSetFct
 *
 */
void adfEnvSetFct ( const AdfLogFct    eFct,
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

static void rwHeadAccess ( const ADF_SECTNUM physical,
                           const ADF_SECTNUM logical,
                           const bool        write )
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
    if ( adfEnv.quiet )
        return;

    va_list ap;
    va_start ( ap, format );

    fprintf ( stderr, "Warning <" );
    vfprintf ( stderr, format, ap );
    fprintf ( stderr, ">\n" );
}


static void Errorf ( const char * const format, ... )
{
    if ( adfEnv.quiet )
        return;

    va_list ap;
    va_start ( ap, format );

    fprintf ( stderr, "Error <" );
    vfprintf ( stderr, format, ap );
    fprintf ( stderr, ">\n" );
/*    exit(1);*/
}


static void Verbosef ( const char * const format, ... )
{
    if ( adfEnv.quiet )
        return;

    va_list ap;
    va_start ( ap, format );

    fprintf ( stderr, "Verbose <" );
    vfprintf ( stderr, format, ap );
    fprintf ( stderr, ">\n" );
}


static void Changed ( const ADF_SECTNUM nSect,
                      const int         changedType )
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
