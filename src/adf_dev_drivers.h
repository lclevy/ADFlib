/*
 * adf_dev_drivers.h
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

#ifndef ADF_DEV_DRIVERS_H
#define ADF_DEV_DRIVERS_H

#include "adf_dev_driver.h"

#include "adf_err.h"


RETCODE adfAddDeviceDriver ( const struct AdfDeviceDriver * const driver );

RETCODE adfRemoveDeviceDriver ( const struct AdfDeviceDriver * const driver );

void adfRemoveDeviceDrivers ( void );

const struct AdfDeviceDriver * adfGetDeviceDriverByName ( const char * const driverName );
const struct AdfDeviceDriver * adfGetDeviceDriverByDevName ( const char * const deviceName );

#endif /* ADF_DEV_DRIVERS_H */

/*#######################################################################################*/
