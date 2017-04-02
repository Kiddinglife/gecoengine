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

 /**
  * singleton_base.h
  *
  *  Created on: 15Jul.,2016
  *      Author: jackiez
  */

#ifndef SRC_COMMON_ULTILS_SINGLETON_BASE_H_
#define SRC_COMMON_ULTILS_SINGLETON_BASE_H_

#include "../debugging/debug.h"

  /**
   *	This class is used to implement singletons. Generally singletons should be
   *	avoided. If they _need_ to be used, try to use this as the base class.
   *
   *	If creating a singleton class MyApp:
   *	class MyApp : public Singleton< MyApp >	{};
   *
   *	In cpp file,
   *	GECO_SINGLETON_DEFI( MyApp )
   *
   *	To use:
   *	MyApp app; // This will be registered as the singleton
   *	MyApp * pApp = MyApp::instance_ptr();
   *	MyApp & app = MyApp::instance_ref();
   */
template<class T>
struct  GECOAPI base_singleton_t
{
	static T * s_pInstance;
	base_singleton_t()
	{
		GECO_ASSERT(NULL == s_pInstance);
		s_pInstance = static_cast<T *>(this);
	}

	~base_singleton_t()
	{
		GECO_ASSERT(this == s_pInstance);
		s_pInstance = NULL;
	}

	static T & instance_ref()
	{
		GECO_ASSERT(s_pInstance != NULL);
		return *s_pInstance;
	}

	static T * instance_ptr()
	{
		GECO_ASSERT(s_pInstance != NULL);
		return s_pInstance;
	}
};

#define GECO_SINGLETON_DEFI( TYPE ) template <> TYPE * base_singleton_t< TYPE >::s_pInstance = NULL;

// another imple of singleton using static methods instead of inhertance
#define GECO_STATIC_FACTORY_DELC(TYPE) \
static TYPE* get_instance(void); \
static void reclaim_instance(TYPE *i);

#define GECO_STATIC_FACTORY_DEFIS(FATHER_TYPE, CHILD_TYPE) \
FATHER_TYPE* FATHER_TYPE::get_instance(void){ return new CHILD_TYPE;} \
void FATHER_TYPE::reclaim_instance(FATHER_TYPE* i){ delete i;}

#endif /* SRC_COMMON_ULTILS_SINGLETON_BASE_H_ */
