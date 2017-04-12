/*
 * test-app-msg-handlers.h
 *
 *  Created on: 12Apr.,2017
 *      Author: jackiez
 */

// UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP was defined before
// here undef it so that definitions can be included in the followings
#if defined(DEFINE_INTERFACE_HERE) || defined( DEFINE_SERVER_HERE )
    #undef UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP
#endif

#ifndef UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP
#define UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP

#include "network/msg-parser-generator.h"

#undef INAME // in ase it has been defined in other module
#define INAME BaseAppInternalInterface

// Prevent multi-declarations when DEFINE_INTERFACE_HERE or DEFINE_SERVER_HERE
#ifndef UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP_GUARD
#define UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP_GUARD
// These constants apply to the BaseAppIntInterface::logOnAttempt message.
namespace INAME
{
    const uint8 LOG_ON_ATTEMPT_REJECTED = 0;
}
#endif // UNITTEST_FAKED_BASE_APP_EXTERNAL_INTERFACE_HPP_GUARD

// put your msg macros here...
BEGIN_GECO_INTERFACE(CELLAPP)
BEGIN_STRUCT_MESSAGE(change_health, change_health_handler)
uint health;
END_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(change_health)
os.Write(args.health);
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(change_health)
is.Read(args.health);
END_GECO_ISTREAM()
END_GECO_INTERFACE()

#endif /* UNITTEST_TEST_APP_MSG_HANDLERS_H_ */


#if defined(DEFINE_INTERFACE_HERE) || defined( DEFINE_SERVER_HERE )
    #undef UNITTEST_FAKED_CLIENT_APP_INTERFACE_HPP
#endif

#ifndef UNITTEST_FAKED_CLIENT_APP_INTERFACE_HPP
#define UNITTEST_FAKED_CLIENT_APP_INTERFACE_HPP
#include "network/msg-parser-generator.h"

#endif /* UNITTEST_TEST_APP_MSG_HANDLERS_H_ */
