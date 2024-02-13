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
#include "adf_prefix.h"

#include "adf_types.h"
#include "adf_version.h"

/* util */
#include "adf_util.h"

/* dir */
#include "adf_dir.h"

/* file */
#include "adf_file.h"
#include "adf_file_block.h"

/* volume */
#include "adf_vol.h"

/* device */
#include "adf_dev.h"
#include "adf_dev_flop.h"
#include "adf_dev_hd.h"

/* device drivers */
#include "adf_dev_driver_dump.h"
#include "adf_dev_driver_ramdisk.h"

/* env */
#include "adf_env.h"

/* link */
#include "adf_link.h"

/* salv */
#include "adf_salv.h"

/* middle level API */

/* low level API */

#include "adf_bitm.h"
#include "adf_raw.h"

#ifdef __cplusplus
}
#endif

#endif /* ADFLIB_H */
/*##########################################################################*/
