#include "FvZoneSerializerXMLImpl.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"

#include <FvWatcher.h>
#include <FvDebug.h>

#include <OgreResourceGroupManager.h>
#include <FvOcclusionQueryRenderable.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvZoneItemXMLFactory::FvZoneItemXMLFactory(
								   const FvString &kSection,
								   int iPriority,
								   Creator pfCreator ) :
m_iPriority( iPriority ),
m_pfCreator( pfCreator )
{
	//FV_GUARD;
	FvZoneSerializerXMLImpl::RegisterFactory( kSection, *this );
}

FvZoneItemXMLFactory::Result FvZoneItemXMLFactory::Create( FvZone *pkZone,
												  FvXMLSectionPtr spSection ) const
{
	//FV_GUARD;
	if (m_pfCreator == NULL)
	{	
		FvString kErrorStr = "No item factory found for section ";
		if ( spSection )
		{
			kErrorStr += "'" + spSection->SectionName() + "'";
		}
		else
		{
			kErrorStr += "<unknown>";
		}
		return FvZoneItemXMLFactory::Result( NULL, kErrorStr );
	}
	return (*m_pfCreator)( pkZone, spSection );
}

FvZoneSerializerXMLImpl::Factories * FvZoneSerializerXMLImpl::ms_pkFactories = NULL;

FvZoneSerializerXMLImpl::FvZoneSerializerXMLImpl()
{

}

FvZoneSerializerXMLImpl::~FvZoneSerializerXMLImpl()
{

}

bool FvZoneSerializerXMLImpl::ExportZone(const FvZone *pkZone,const FvString& kFileName)
{
	return true;
}

bool FvZoneSerializerXMLImpl::ImportZone(Ogre::DataStreamPtr& kStream, FvZone *pkDest)
{
	FvXMLSectionPtr spSection = FvXMLSection::OpenSection(kStream);

	bool bGood = true;
	bool bSkipBoundaryAndIncludes = false;

	pkDest->m_kLabel = spSection->AsString();

	if( !pkDest->IsOutsideZone() )
	{
		pkDest->m_kTransform = spSection->ReadMatrix34("transform",FvMatrix::IDENTITY);
		pkDest->m_kTransform.PostMultiply( pkDest->m_pkMapping->Mapper() );
		pkDest->m_kTransformInverse.Invert( pkDest->m_kTransform );

		FvXMLSectionPtr spShellSection = spSection->OpenSection( "shell" );
		if( !spShellSection )
			spShellSection = spSection->OpenSection( "model" );
		if( !spShellSection )
		{
			bGood = false;
		}
		else
		{
			bGood &= bool( this->LoadItem( spShellSection, pkDest ) );
		}
		if( !bGood )
		{
			pkDest->m_kLocalBB = FvBoundingBox( FvVector3( 0.f, 0.f, 0.f ), FvVector3( 1.f, 1.f, 1.f ) );
			pkDest->m_kBoundingBox = pkDest->m_kLocalBB;
#ifndef FV_SERVER
			pkDest->m_kVisibilityBox = pkDest->m_kLocalBB;
#endif // FV_SERVER
			pkDest->m_kBoundingBox.TransformBy( pkDest->m_kTransform );

#ifndef FV_SERVER
			if(pkDest->m_pkOcclusion)
			{
				pkDest->m_pkOcclusion->Update(pkDest->m_kBoundingBox.MaxBounds() - pkDest->m_kBoundingBox.MinBounds(), pkDest->m_kCentre);
			}
#endif

			FV_ERROR_MSG( "FvZoneSerializerXMLImpl::ImportZone: Failed to load shell model for zone %s\n", 
				pkDest->m_kIdentifier.c_str() );
			bSkipBoundaryAndIncludes = true;
		}
	}

	if (!bSkipBoundaryAndIncludes)
	{
		if ( !this->ReadBoundaries( spSection, pkDest ) )
		{
			bGood = false;
			FV_ERROR_MSG( "FvZoneSerializerXMLImpl::ImportZone: Failed to load zone %s boundaries\n", 
				pkDest->m_kIdentifier.c_str() );
		}

		FvString kErrorStr;
		if ( !this->LoadInclude( spSection, FvMatrix::IDENTITY, &kErrorStr, pkDest ) )
		{
			bGood = false;
			FV_ERROR_MSG( "FvZoneSerializerXMLImpl::ImportZone: Failed to load zone %s: %s\n", 
				pkDest->m_kIdentifier.c_str(), kErrorStr.c_str() );
		}
	}

	return bGood;
}

void FvZoneSerializerXMLImpl::RegisterFactory( const FvString &kSection,
							const FvZoneItemXMLFactory &kFactory )
{
	//FV_GUARD;
	FV_INFO_MSG( "Registering kFactory for %s\n", kSection.c_str() );

	if (ms_pkFactories == NULL)
	{
		ms_pkFactories = new Factories();
	}

	const FvZoneItemXMLFactory *&pkEntry = (*ms_pkFactories)[ kSection ];

	if (pkEntry == NULL || pkEntry->Priority() < kFactory.Priority())
	{
		pkEntry = &kFactory;
	}
}

void FvZoneSerializerXMLImpl::Fini()
{
	delete ms_pkFactories;
	ms_pkFactories = NULL;
}

FvZoneItemXMLFactory::Result FvZoneSerializerXMLImpl::LoadItem( FvXMLSectionPtr spSection,
															FvZone *pkDest )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( ms_pkFactories != NULL )
	{
		return FvZoneItemXMLFactory::SucceededWithoutItem();
	}

	Factories::iterator found = ms_pkFactories->find( spSection->SectionName() );
	if (found != ms_pkFactories->end())
		return found->second->Create( pkDest, spSection );

	return FvZoneItemXMLFactory::SucceededWithoutItem();
}

bool FvZoneSerializerXMLImpl::LoadInclude( FvXMLSectionPtr spSection, const FvMatrix & kFlatten, 
				 FvString* kErrorStr,FvZone *pkDest)
{
	if (!spSection) return false;

	bool bGood = true;
	bool bLGood;
	int iNIncludes = 0;

	FvXMLSectionIterator kEnd = spSection->End();
	bool bNeedShell = !pkDest->IsOutsideZone() && !spSection->OpenSection( "shell" );
	bool bGotFirstModel = false;
	for (FvXMLSectionIterator it = spSection->Begin(); it != kEnd; it++ )
	{
		const FvString kSType = (*it)->SectionName();

		if( kSType == "shell" )
			continue;

		if( bNeedShell && kSType == "model" && !bGotFirstModel )
		{
			bGotFirstModel = true;
			continue;
		}

		FvString kItemError;

		if (kSType == "include")
		{
			FvMatrix kMLevel;
			kMLevel = (*it)->ReadMatrix34( "transform" ,FvMatrix::IDENTITY);
			kMLevel.PostMultiply( kFlatten );

			bLGood = this->LoadInclude(
				/*FvXMLSection::OpenSection( (*/*it/*)->ReadString( "resource" ) )*/,
				kMLevel,
				kErrorStr,
				pkDest);
			bGood &= bLGood;
			if (!bLGood && kErrorStr)
			{
				std::stringstream ss;
				ss << "bad include section index " << iNIncludes;
				kItemError += ss.str();
			}

			iNIncludes++;
		}
		else
		{
			FvZoneItemXMLFactory::Result res = this->LoadItem( *it, pkDest);
			bGood &= bool(res);
			if (!bool(res) && kErrorStr)
			{
				if ( !res.ErrorString().empty() )
				{
					kItemError = res.ErrorString();
				}
				else
				{
					kItemError = "unknown error in item '" + (*it)->SectionName() + "'";
				}
			}
		}
		if ( !kItemError.empty() && kErrorStr )
		{
			if ( !kErrorStr->empty() )
			{
				*kErrorStr += ", ";
			}
			*kErrorStr += kItemError;
		}
	}

	return bGood;
}

bool FvZoneSerializerXMLImpl::ReadBoundaries( FvXMLSectionPtr spSection,FvZone *pkDest )
{
	std::vector<FvXMLSectionPtr> kBSects;
	this->CreateBoundaries( spSection, pkDest, kBSects );
	bool bGood = true;

	for (unsigned int i = 0; i < kBSects.size(); i++)
	{
		FvZoneBoundary *pkCB = new FvZoneBoundary;
		this->ReadBoundary(kBSects[i],pkCB,pkDest);

		if (pkCB->Plane().Normal().Length() == 0.f)
		{
			delete pkCB;
			bGood = false;
			continue;
		}

		bool bIsaBound = false;
		bool bIsaJoint = false;
		if (pkCB->m_kUnboundPortals.size())
		{
			bIsaJoint = true;
			if (!pkCB->m_kUnboundPortals[0]->m_bInternal)
			{
				bIsaBound = true;
			}
		}
		else
		{
			if (pkCB->m_kBoundPortals.size())
			{
				bIsaJoint = true;
			}
			bIsaBound = true;
		}

		if (bIsaBound) pkDest->m_kBounds.push_back( pkCB );
		if (bIsaJoint) pkDest->m_kJoints.push_back( pkCB );
	}

	return bGood && kBSects.size() >= 4;
}

FvXMLSectionPtr FvZoneSerializerXMLImpl::CreateBoundary( FvXMLSectionPtr spBoundarySection, 
							   const FvVector3 &kNormal, float fD )
{
	FvXMLSectionPtr spBoundary = spBoundarySection->NewSection( "boundary" );
	spBoundary->WriteVector3( "normal", kNormal );
	spBoundary->WriteFloat( "distance", fD );
	return spBoundary;
}

void FvZoneSerializerXMLImpl::CreatePortal( FvXMLSectionPtr spBoundary, const FvString &kToZone,
				  const FvVector3 &kUAxis, const FvVector3 &kPT1, const FvVector3 &kPT2,
				  const FvVector3 &kPT3, const FvVector3 &kPT4 )
{
	FvXMLSectionPtr portal = spBoundary->NewSection( "portal" );
	portal->WriteString( "zone", kToZone );
	portal->WriteVector3( "uAxis", kUAxis );
	portal->NewSection( "point" )->SetVector3( kPT1 );
	portal->NewSection( "point" )->SetVector3( kPT2 );
	portal->NewSection( "point" )->SetVector3( kPT3 );
	portal->NewSection( "point" )->SetVector3( kPT4 );
}

void FvZoneSerializerXMLImpl::CreateBoundaries( FvXMLSectionPtr spSection, FvZone *pkZone,
					std::vector<FvXMLSectionPtr> &kBSects)
{	
	if( pkZone->IsOutsideZone() )
	{
		 spSection = new FvXMLSection( "root" );
		 FvString kZoneName = pkZone->Identifier();
		 FvInt16 iX, iY;
		 FvZoneDirMapping::GridFromZoneName( kZoneName, iX, iY );
		 for (int i = 0; i < 6; ++i)
		 {
			 float fMinZf = float( FV_MIN_ZONE_HEIGHT );
			 float fMaxZf = float( FV_MAX_ZONE_HEIGHT );

			 if( i == 0 )
			 {
				 // right
				 FvXMLSectionPtr spBoundary = CreateBoundary( spSection, FvVector3( 1.f, 0.f, 0.f ), 0.f );
				 if (iX != pkZone->m_pkMapping->MinGridX() )
					 CreatePortal( spBoundary, pkZone->m_pkMapping->OutsideZoneIdentifier( iX - 1, iY ), FvVector3( 0.f, 0.f, 1.f ),
					 FvVector3( fMinZf, 0.f, 0.f ),
					 FvVector3( fMaxZf, 0.f, 0.f ),
					 FvVector3( fMaxZf, -FV_GRID_RESOLUTION, 0.f ),
					 FvVector3( fMinZf, -FV_GRID_RESOLUTION, 0.f ) );
			 }
			 else if (i == 1)
			 {
				 // left
				 FvXMLSectionPtr spBoundary = CreateBoundary( spSection, FvVector3( -1.f, 0.f, 0.f ), -FV_GRID_RESOLUTION );
				 if (iX != pkZone->m_pkMapping->MaxGridX() )
					 CreatePortal( spBoundary, pkZone->m_pkMapping->OutsideZoneIdentifier( iX + 1, iY ), FvVector3( 0.f, 1.f, 0.f ),
					 FvVector3( 0.f, fMaxZf, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, fMaxZf, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, fMinZf, 0.f ),
					 FvVector3( 0.f, fMinZf, 0.f ) );

			 }
			 else if (i == 2)
			 {
				 // bottom
				 FvXMLSectionPtr spBoundary = CreateBoundary( spSection, FvVector3( 0.f, 0.f, 1.f ), fMinZf );
				 CreatePortal( spBoundary, "earth", FvVector3( 0.f, 1.f, 0.f ),
					 FvVector3( 0.f, 0.f, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, 0.f, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, -FV_GRID_RESOLUTION, 0.f ),
					 FvVector3( 0.f, -FV_GRID_RESOLUTION, 0.f ) );
			 }
			 else if (i == 3)
			 {
				 // top
				 FvXMLSectionPtr spBoundary = CreateBoundary( spSection, FvVector3( 0.f, 0.f, -1.f ), -fMaxZf );
				 CreatePortal( spBoundary, "heaven", FvVector3( 1.f, 0.f, 0.f ),
					 FvVector3( 0.f, 0.f, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, 0.f, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, -FV_GRID_RESOLUTION, 0.f ),
					 FvVector3( 0.f, -FV_GRID_RESOLUTION, 0.f ) );
			 }
			 else if (i == 4)
			 {
				 // back
				 FvXMLSectionPtr spBoundary = CreateBoundary( spSection, FvVector3( 0.f, 1.f, 0.f ), 0.f );
				 if( iY != pkZone->m_pkMapping->MinGridY() )
					 CreatePortal( spBoundary, pkZone->m_pkMapping->OutsideZoneIdentifier( iX, iY - 1 ), FvVector3( 1.f, 0.f, 0.f ),
					 FvVector3( 0.f, fMaxZf, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, fMaxZf, 0.f ),
					 FvVector3( FV_GRID_RESOLUTION, fMinZf, 0.f ),
					 FvVector3( 0.f, fMinZf, 0.f ) );
			 }
			 else if (i == 5)
			 {
				 // front
				 FvXMLSectionPtr spBoundary = CreateBoundary( spSection, FvVector3( 0.f, -1.f, 0.f ), -FV_GRID_RESOLUTION );
				 if( iY != pkZone->m_pkMapping->MaxGridY() )
					 CreatePortal( spBoundary, pkZone->m_pkMapping->OutsideZoneIdentifier( iX, iY + 1 ), FvVector3( 0.f, 0.f, 1.f ),
					 FvVector3( fMinZf, 0.f, 0.f ),
					 FvVector3( fMaxZf, 0.f, 0.f ),
					 FvVector3( fMaxZf, -FV_GRID_RESOLUTION, 0.f ),
					 FvVector3( fMinZf, -FV_GRID_RESOLUTION, 0.f ) );
			 }
		 }
	}
	else
	{
		FvXMLSectionPtr spModelSection = spSection->OpenSection( "shell" );
		if( !spModelSection )
			spModelSection = spSection->OpenSection( "model" );
		if( spModelSection )
		{
			FvString kResource = spModelSection->ReadString( "resource" );
			if( !kResource.empty() )
			{
				FvString kFullName,kBaseName,kExtention;
				Ogre::StringUtil::splitBaseFilename(kResource,kBaseName,kExtention);
				kFullName = kBaseName + ".node";

				Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
					openResource(kBaseName + ".node",pkZone->Mapping()->Group(),true);

				FvXMLSectionPtr spVisualSection = FvXMLSection::OpenSection(spDataStream);
				if(spDataStream.isNull())
				{
					Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
						openResource(kBaseName + ".static.node",pkZone->Mapping()->Group(),true);
					spVisualSection = FvXMLSection::OpenSection(spDataStream);
				}
				if(spVisualSection)
				{
					std::vector<FvXMLSectionPtr> kBoundarySections;
					spVisualSection->OpenSections( "boundary", kBoundarySections );
					//if( kBoundarySections.empty() )
					//	spVisualSection = CreateBoundarySections( spVisualSection, FvMatrix::IDENTITY );
					spSection->CopySections(spVisualSection,"boundary");
				}
			}
		}
	}
	spSection->OpenSections("boundary",kBSects);
}

void FvZoneSerializerXMLImpl::ReadBoundary( FvXMLSectionPtr spSection,
				  FvZoneBoundary *pkBoundary,FvZone *pkDest )
{
	FvVector3 kNormal = spSection->ReadVector3( "normal" );
	float fD = spSection->ReadFloat( "distance" );
	float fNormalLen = kNormal.Length();
	kNormal /= fNormalLen;
	fD /= fNormalLen;
	pkBoundary->m_kPlane = FvPlane( kNormal, fD );

	bool bSomeInternal = false;
	bool bSomeExternal = false;

	FvXMLSectionIterator kEnd = spSection->End();
	for (FvXMLSectionIterator it = spSection->Begin(); it != kEnd; it++)
	{
		if ((*it)->SectionName() != "portal") continue;

		FvZoneBoundary::Portal *pkNewPortal = new FvZoneBoundary::Portal(pkBoundary);
		this->ReadBoundaryPortal(*it,pkNewPortal,pkBoundary,pkDest);

		if (pkNewPortal->IsHeaven() || pkNewPortal->IsEarth())
			pkBoundary->m_kBoundPortals.push_back( pkNewPortal );
		else
			pkBoundary->m_kUnboundPortals.push_back( pkNewPortal );

		if (pkNewPortal->m_bInternal)
			bSomeInternal = true;
		else
			bSomeExternal = true;
	}
	FV_ASSERT_DEV(!(bSomeInternal && bSomeExternal));
}

void FvZoneSerializerXMLImpl::ReadBoundaryPortal( FvXMLSectionPtr spSection,
						FvZoneBoundary::Portal *pkPortal,FvZoneBoundary *pkBoundary,FvZone *pkDest )
{
	pkPortal->m_kPlane = pkBoundary->m_kPlane;

	pkPortal->m_kLabel = spSection->SectionName();

	pkPortal->m_bInternal = spSection->ReadBool( "internal", pkPortal->m_bInternal );
	pkPortal->m_bPermissive = spSection->ReadBool( "permissive", pkPortal->m_bPermissive );

	FvString kZoneName = spSection->ReadString( "zone" );
	if (kZoneName == "")
	{
		pkPortal->m_pkZone = NULL;
	}
	else if (kZoneName == "heaven")
	{
		pkPortal->m_pkZone = (FvZone*)FvZoneBoundary::Portal::HEAVEN;
	}
	else if (kZoneName == "earth")
	{
		pkPortal->m_pkZone = (FvZone*)FvZoneBoundary::Portal::EARTH;
	}
	else if (kZoneName == "invasive")
	{
		pkPortal->m_pkZone = (FvZone*)FvZoneBoundary::Portal::INVASIVE;
	}
	else if (kZoneName == "extern")
	{
		pkPortal->m_pkZone = (FvZone*)FvZoneBoundary::Portal::EXTERN;
	}
	else
	{
		pkPortal->m_pkZone = new FvZone( kZoneName, pkDest->m_pkMapping );
	}

	FvVector2 kAvg(0,0);

	std::vector<FvVector3> kV3Points;
	spSection->ReadVector3s( "point", kV3Points );
	for (unsigned int i = 0; i < kV3Points.size(); i++)
	{
		FvVector2 kNext( kV3Points[i].x, kV3Points[i].y );

		kAvg += kNext;

		pkPortal->m_kPoints.push_back( kNext );
	}

	pkPortal->m_kUAxis = spSection->ReadVector3( "uAxis" );
	pkPortal->m_kVAxis = pkPortal->m_kPlane.Normal().CrossProduct( pkPortal->m_kUAxis );
	pkPortal->m_kOrigin = pkPortal->m_kPlane.Normal() * pkPortal->m_kPlane.Distance() / pkPortal->m_kPlane.Normal().LengthSquared();

#ifdef FV_EDITOR_ENABLED
	const float EPSILON = 0.5;
	if( !kZoneName.empty() && *kZoneName.rbegin() == 'o' &&
		!pkDest->Identifier().empty() && *pkDest->Identifier().rbegin() == 'o' &&
		pkPortal->m_kPoints.size() == 4 )
	{
		if( pkPortal->m_kUAxis[ 1 ] >= EPSILON || pkPortal->m_kUAxis[ 1 ] <= -EPSILON )
		{
			for( int i = 0; i < 4; ++i )
			{
				if( pkPortal->m_kPoints[ i ][ 0 ] > EPSILON )
					pkPortal->m_kPoints[ i ][ 0 ] = FV_MAX_ZONE_HEIGHT;
				else if( pkPortal->m_kPoints[ i ][ 0 ] < -EPSILON )
					pkPortal->m_kPoints[ i ][ 0 ] = FV_MIN_ZONE_HEIGHT;
			}
		}
		else
		{
			for( int i = 0; i < 4; ++i )
			{
				if( pkPortal->m_kPoints[ i ][ 1 ] > EPSILON )
					pkPortal->m_kPoints[ i ][ 1 ] = FV_MAX_ZONE_HEIGHT;
				else if( pkPortal->m_kPoints[ i ][ 1 ] < -EPSILON )
					pkPortal->m_kPoints[ i ][ 1 ] = FV_MIN_ZONE_HEIGHT;
			}
		}
	}
#endif // FV_EDITOR_ENABLED

	kAvg /= float(pkPortal->m_kPoints.size());
	pkPortal->m_kLCentre = pkPortal->m_kUAxis * kAvg.x + pkPortal->m_kVAxis * kAvg.y + pkPortal->m_kOrigin;
	pkPortal->m_kCentre = pkPortal->m_kLCentre;

	FvPlane kTestPlane(
		pkPortal->m_kPoints[0][0] * pkPortal->m_kUAxis + pkPortal->m_kPoints[0][1] * pkPortal->m_kVAxis + pkPortal->m_kOrigin,
		pkPortal->m_kPoints[1][0] * pkPortal->m_kUAxis + pkPortal->m_kPoints[1][1] * pkPortal->m_kVAxis + pkPortal->m_kOrigin,
		pkPortal->m_kPoints[2][0] * pkPortal->m_kUAxis + pkPortal->m_kPoints[2][1] * pkPortal->m_kVAxis + pkPortal->m_kOrigin );
	FvVector3 kN1 = pkPortal->m_kPlane.Normal();
	FvVector3 kN2 = kTestPlane.Normal();
	kN1.Normalise();	
	kN2.Normalise();
	if ((kN1 + kN2).Length() < 1.f)	
	{
		std::reverse( pkPortal->m_kPoints.begin(), pkPortal->m_kPoints.end() );
	}
}