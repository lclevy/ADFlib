/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_env.h
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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ADF_ENV_H
#define ADF_ENV_H

#include "adf_err.h"
#include "adf_prefix.h"
#include "adf_types.h"

/* ----- ENVIRONMENT ----- */

typedef enum {
    ADF_PR_VFCT                   = 1,
    ADF_PR_WFCT                   = 2,
    ADF_PR_EFCT                   = 3,
    ADF_PR_NOTFCT                 = 4,
    ADF_PR_USEDIRC                = 5,
    ADF_PR_USE_NOTFCT             = 6,
    ADF_PR_PROGBAR                = 7,
    ADF_PR_USE_PROGBAR            = 8,
    ADF_PR_RWACCESS               = 9,
    ADF_PR_USE_RWACCESS           = 10,
    ADF_PR_IGNORE_CHECKSUM_ERRORS = 11
} ADF_ENV_PROPERTY;

//typedef void (*AdfLogFct)(const char * const txt);
typedef void (*AdfLogFct)(const char * const format, ...);
//typedef void (*AdfLogFileFct)(FILE * file, const char * const format, ...);

typedef void (*AdfNotifyFct)(SECTNUM, int);
typedef void (*AdfRwhAccessFct)(SECTNUM, SECTNUM, bool);
typedef void (*AdfProgressBarFct)(int);

struct AdfEnv {
    AdfLogFct vFct;       /* verbose callback function */
    AdfLogFct wFct;       /* warning callback function */
    AdfLogFct eFct;       /* error callback function */

    AdfNotifyFct notifyFct;
    bool useNotify;

    AdfRwhAccessFct rwhAccess;
    bool useRWAccess;

    AdfProgressBarFct progressBar;
    bool useProgressBar;

    bool useDirCache;

    bool ignoreChecksumErrors;
};


PREFIX void adfEnvInitDefault(void);
PREFIX void adfEnvCleanUp(void);

PREFIX void adfEnvSetFct ( const AdfLogFct    eFct,
                           const AdfLogFct    wFct,
                           const AdfLogFct    vFct,
                           const AdfNotifyFct notifyFct );

PREFIX ADF_RETCODE adfEnvChgProp ( const ADF_ENV_PROPERTY property,
                                   const intptr_t         newValue );

PREFIX intptr_t adfEnvGetProp ( const ADF_ENV_PROPERTY property );

PREFIX char* adfGetVersionNumber(void);
PREFIX char* adfGetVersionDate(void);

PREFIX extern struct AdfEnv adfEnv;

#endif  /* ADF_ENV_H */
