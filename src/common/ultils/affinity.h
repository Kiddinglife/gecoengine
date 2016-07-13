/*
 * Geco Gaming Company
 * All Rights Reserved.
 * Copyright (c)  2016 GECOEngine.
 *
 * GECOEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GECOEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// created on 03-July-2016 by Jackie Zhang
#ifndef _SRC_GECO_ENGINE_AFFINITY
#define _SRC_GECO_ENGINE_AFFINITY
#include "../plateform.h"

namespace geco
{
namespace ultils
{
#if !defined( _XBOX360) && defined(_WIN32)
/**
 * These methods aid with setting the processor affinity of the main thread
 * This has to be done so that the timing can be done as accurately as possible
 */

/**
 * @breif this sets the affinity for the app to the given processors.
 * @param [in] processor the processor to set the affinity to, please
 * note that this is only a hint, if the processor is not
 * available the first available processor will be chosen
 */
void affinity_set(uint32 processorHint = 0);
/**@brief this gets the processor mask that the application is running on.*/
uint32 affinity_get();
/**@brief this makes sure that the process is running on the right processor.*/
void affinity_update();
#endif
}
}

#endif
