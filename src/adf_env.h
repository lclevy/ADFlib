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

#include "adf_prefix.h"
#include "adf_types.h"

/* ----- ENVIRONMENT ----- */

#define PR_VFCT	        1
#define PR_WFCT	        2
#define PR_EFCT	        3
#define PR_NOTFCT       4
#define PR_USEDIRC      5
#define PR_USE_NOTFCT   6
#define PR_PROGBAR      7
#define PR_USE_PROGBAR  8
#define PR_RWACCESS     9
#define PR_USE_RWACCESS 10
#define PR_IGNORE_CHECKSUM_ERRORS 11

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

PREFIX void adfSetEnvFct ( const AdfLogFct    eFct,
                           const AdfLogFct    wFct,
                           const AdfLogFct    vFct,
                           const AdfNotifyFct notifyFct );

PREFIX void adfEnvCleanUp(void);
PREFIX void adfChgEnvProp(int prop, void *new);
PREFIX char* adfGetVersionNumber(void);
PREFIX char* adfGetVersionDate(void);

PREFIX extern struct AdfEnv adfEnv;

#endif  /* ADF_ENV_H */
