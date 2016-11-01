#include "FvServerZoneModel.h"
#include "FvZoneModelObstacle.h"
#include "FvZone.h"
#include "FvZoneSpace.h"

#include <FvDebug.h>
#include <FvModel.h>

int iServerZoneModelToken;

FV_DECLARE_DEBUG_COMPONENT2( "Zone", 0 )

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone)
FV_IMPLEMENT_ZONE_ITEM_XML( FvServerZoneModel, model, 0 )
FV_IMPLEMENT_ZONE_ITEM_XML_ALIAS( FvServerZoneModel, tree, 0 )
FV_IMPLEMENT_ZONE_ITEM_XML_ALIAS( FvServerZoneModel, shell, 0 )

FvServerZoneModel::FvServerZoneModel() :
	FvZoneItem( (WantFlags)(WANTS_VISIT | WANTS_TICK) ),
	m_spModel( NULL ),
	m_kTransform( FvMatrix::IDENTITY )
{
}


FvServerZoneModel::~FvServerZoneModel()
{
	m_spModel = NULL;
}

bool FvServerZoneModel::Load( FvXMLSectionPtr spSection, FvZone *pkZone )
{
	m_kLabel = spSection->AsString();
	FvString kResource = spSection->ReadString("resource");
	m_kTransform = spSection->ReadMatrix34("transform");
	m_spModel = new FvModel;
	if (!m_spModel->Load(kResource,
		pkZone->Mapping()->Group(),
		pkZone->IsOutsideZone() ? "" : pkZone->Identifier(),
		m_kTransform))
	{
		m_spModel = NULL;
		return false;
	}

	return true;
}

const char *FvServerZoneModel::Label() const
{
	return m_kLabel.c_str();
}

void FvServerZoneModel::Toss( FvZone *pkZone )
{
	if (m_pkZone != NULL)
	{
		FvZoneModelObstacle::ms_kInstance( *m_pkZone ).DelObstacles( this );
	}

	this->FvZoneItem::Toss( pkZone );

	if ((m_pkZone != NULL) && (m_spModel != NULL))
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( m_kTransform );

		if(m_spModel)
		{
			FvZoneModelObstacle::ms_kInstance( *m_pkZone ).AddModel(
				m_spModel, kWorld, this );
		}
	}
}

FvBoundingBox FvServerZoneModel::LocalBB() const
{
	if(!m_spModel)
		return FvBoundingBox::ms_kInsideOut;

	return m_spModel->BoundingBox();
}