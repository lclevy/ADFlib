/*
 * adf_dev_driver.h
 *
 * $ID$
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

#ifndef ADF_DEV_DRIVER_H
#define ADF_DEV_DRIVER_H

#include "adf_dev.h"

struct AdfDeviceDriver {
    const char * const name;    /* driver name / id */
    void *             data;    /* private driver-specific data */


    /* at least one of these two "factories" is required */

    struct AdfDevice * (*createDev)( const char * const name,
                                     const uint32_t     cylinders,
                                     const uint32_t     heads,
                                     const uint32_t     sectors );

    struct AdfDevice * (*openDev) ( const char * const  name,
                                    const AdfAccessMode mode );

    /* required */

    RETCODE (*closeDev)(struct AdfDevice * const dev);

    RETCODE (*readSector)( struct AdfDevice * const dev,
                           const uint32_t           n,
                           const unsigned           size,
                           uint8_t * const          buf );

    RETCODE (*writeSector)( struct AdfDevice * const dev,
                            const uint32_t           n,
                            const unsigned           size,
                            const uint8_t * const    buf );

    bool (*isNative)( void );   /* should return true only on a native block device driver;
                                   used only in adfMountDev() to determine which method
                                   should be used to mount the device
                                   (check if there is a better way so that this can be removed */


    /* optional (can be NULL); should help to match device string with the driver */

    bool (*isDevice)( const char * const name );
};

#endif  /* ADF_DEV_DRIVER_H */
