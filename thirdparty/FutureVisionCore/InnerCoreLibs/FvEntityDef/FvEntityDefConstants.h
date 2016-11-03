//{future header message}
#ifndef __FvEntityDefConstants_H__
#define __FvEntityDefConstants_H__

#include "FvEntityDefDefines.h"

namespace FvEntityDef
{

class FV_ENTITYDEF_API Constants
{
public:
	
	// common constants
	
	inline static const char*
		CommonPath() { return "EntityDef/common"; }

	inline static const char*
		AliasesFile() { return "EntityDef/alias.xml"; }

	
	// database constants

	inline static const char*
		DatabasePath() { return "EntityDef/db"; }

	inline static const char*
		XmlDatabaseFile() { return "EntityDef/db.xml"; }


	// bot scripts constants

	inline static const char*
		BotScriptsPath() { return "EntityDef/bot"; }


	// entities constants

	inline static const char*
		EntitiesFile() { return "EntityDef/entities.xml"; }

	inline static const char*
		EntitiesDefsPath() { return "EntityDef/entity_defs"; }

	inline static const char*
		EntitiesDefsShortPath() { return "entity_defs"; }

	inline static const char*
		EntitiesClientPath() { return "EntityDef/client"; }

	inline static const char*
		EntitiesCellPath() { return "EntityDef/cell"; }

	inline static const char*
		EntitiesBasePath() { return "EntityDef/base"; }

	inline static const char*
		EntitiesEditorPath() { return "EntityDef/editor"; }

	inline static const char*
		EntitiesCapabilitiesFile() { return "EntityDef/common/capabilities.xml"; }


	// user data objects constants

	inline static const char*
		UserDataObjectsFile() { return "EntityDef/user_data_objects.xml"; }

	inline static const char*
		UserDataObjectsDefsPath() { return "EntityDef/user_data_object_defs"; }

	inline static const char*
		UserDataObjectsDefsShortPath() { return "user_data_object_defs"; }

	inline static const char*
		UserDataObjectsClientPath() { return "EntityDef/client"; }

	inline static const char*
		UserDataObjectsCellPath() { return "EntityDef/cell"; }

	inline static const char*
		UserDataObjectsBasePath() { return "EntityDef/base"; }

	inline static const char*
		UserDataObjectsEditorPath() { return "EntityDef/editor"; }


	// marker constants

	inline static const char*
		MarkerCategoriesFile() { return "EntityDef/marker_categories.xml"; }

	inline static const char*
		MarkerEntitiesPath() { return "EntityDef/marker_entities"; }

};

}


#endif // __FvEntityDefConstants_H__
