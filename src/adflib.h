#ifndef ADFLIB_H
#define ADFLIB_H 1

/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 * adflib.h
 *
 *  $Id$
 *
 * general include file
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Windows - a DLL-specific function declaration prefix (to import/export library symbols) */
#include "prefix.h"

#include "adf_defs.h"
#include "adf_str.h"

/* util */
#include "adf_util.h"

/* dir */
#include "adf_dir.h"

/* file */
#include "adf_file.h"

/* volume */
#include "adf_disk.h"

/* device */
#include "adf_dev.h"
#include "adf_dev_flop.h"
#include "adf_dev_hd.h"

/* dump device */
#include "adf_dump.h"

/* env */
#include "adf_env.h"

/* link */
#include "adf_link.h"

/* salv */
#include "adf_salv.h"

/* middle level API */

/* low level API */

#include "adf_bitm.h"


#ifdef __cplusplus
}
#endif

#endif /* ADFLIB_H */
/*##########################################################################*/
