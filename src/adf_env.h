#ifndef ADF_ENV_H
#define ADF_ENV_H 1

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
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include"prefix.h"

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

struct AdfEnv {
    void (*vFct)(char*);       /* verbose callback function */
    void (*wFct)(char*);       /* warning callback function */
    void (*eFct)(char*);       /* error callback function */
    void (*vFctf)(const char * const format, ...); /* verbose cb formatted */
    void (*wFctf)(const char * const format, ...); /* warning cb formatted */
    void (*eFctf)(const char * const format, ...); /* error cb formatted */

    void (*notifyFct)(SECTNUM, int);
    BOOL useNotify;

    void (*rwhAccess)(SECTNUM,SECTNUM,BOOL);
    BOOL useRWAccess;

    void (*progressBar)(int);
    BOOL useProgressBar;

    BOOL useDirCache;

    void *nativeFct;
};


PREFIX void adfEnvInitDefault();
PREFIX void adfSetEnvFct( void(*e)(char*), void(*w)(char*), void(*v)(char*),
	void(*n)(SECTNUM,int) );
PREFIX void adfEnvCleanUp();
PREFIX void adfChgEnvProp(int prop, void *new);
PREFIX char* adfGetVersionNumber();
PREFIX char* adfGetVersionDate();

extern struct AdfEnv adfEnv;

#endif /* ADF_ENV_H */
/*##########################################################################*/
