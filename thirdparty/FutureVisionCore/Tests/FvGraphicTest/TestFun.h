#include "FvZoneTest.h"
bool Test_VerticalCollision( FvZoneSpace& space, const FvVector3 & oldPos, float& fAdvanceLen, const float fBodyRadius,/*FvBodyScale& kBody,*/ FvVector3 & hitNormal, const FvUInt16 uiIgnoreMask)
{
	// TODO: Should pass in the flat direction.
	const FvVector3 flatDir( 0.f, 1.0f, 0.0f );

	const FvVector3 perpFlatDir(flatDir.y, 0.f, -flatDir.x);
	float fAdvanceMod = abs(fAdvanceLen);

	// Find the corners of the triangle to test.
	FvVector3 corners[4];
	//! 内接矩形
	corners[0] = oldPos - flatDir * fBodyRadius*0.4f - perpFlatDir * fBodyRadius*0.4f;
	corners[1] = oldPos - flatDir * fBodyRadius*0.4f + perpFlatDir * fBodyRadius*0.4f;
	corners[2] = oldPos + flatDir * fBodyRadius*0.4f - perpFlatDir * fBodyRadius*0.4f;
	corners[3] = oldPos + flatDir * fBodyRadius*0.4f + perpFlatDir * fBodyRadius*0.4f;

	// make the object that checks them
	FvVector3 kPickDir( 0.f, 0.0f, -1.0f );
	kPickDir.z = (fAdvanceLen > 0)? 1.0f: -1.0f;

	FvCollisionAdvance kCollisionAdvance( corners[0],
		corners[1] - corners[0], corners[2] - corners[0],
		kPickDir, fAdvanceMod );
	kCollisionAdvance.IgnoreFlags(uiIgnoreMask);
	//kCollisionAdvance.IgnoreFlags( FV_TRIANGLE_NOCOLLIDE );

	// and test the two triangles
	FvWorldTriangle kDropOne( corners[1], corners[0], corners[3] );
	FvWorldTriangle kDropTwo( corners[2], corners[3], corners[0] );
	FvVector3 dest( oldPos.x, oldPos.y, oldPos.z - fAdvanceMod - 0.1f);
	const float collideValue1 = space.Collide( kDropOne, dest, kCollisionAdvance );
	const float collideValue2 = space.Collide( kDropTwo, dest, kCollisionAdvance );

	// make sure we are within the bounds of the chunk space!
	//testPos.x = Math::clamp( pSpace->minCoordX() + 0.05f,
	//	testPos.x,
	//	pSpace->maxCoordX() - 0.05f );
	//testPos.z = Math::clamp( pSpace->minCoordZ() + 0.05f,
	//	testPos.z,
	//	pSpace->maxCoordZ() - 0.05f );

	bool bDropCollided = false;
	if (	fAdvanceMod <= kCollisionAdvance.GetAdvance())
	{
		bDropCollided = false;
	}
	else
	{
		bDropCollided = true;
		fAdvanceMod = kCollisionAdvance.GetAdvance();
		hitNormal = kCollisionAdvance.HitTriangle().Normal();
		hitNormal.Normalise();

		//中心高度纠正
		//const float fZ = hitNormal.z;
		//const float fDropScale = sqrt(1-fZ*fZ)/fZ;
		//const float fDrop = fDropScale*fBodyRadius*0.3f;
		//fAdvanceMod += fDrop;
	}
	fAdvanceLen = (fAdvanceLen > 0)? fAdvanceMod: -fAdvanceMod;
	return bDropCollided;
}
bool Test_CalculateSlide(const FvVector3& kNormal, const float fFriction, FvVector3& kAcc)
{
	const FvVector3 kG(0.0f, 0.0f, -9.8f);
	FvVector3 kSlide = kG - (kG.DotProduct(kNormal))*kNormal;
	const float fSlide = kSlide.Length();
	if(fSlide > fFriction)
	{
		kSlide.Normalise();
		kAcc = fSlide*kSlide;
		return true;
	}
	else
		return false;
}
int Test_CreateSpace()
{
	static int iClientSpaceID = (1 << 30);

	FvZoneManager &kZM = FvZoneManager::Instance();

	int iNewSpaceID = 0;
	do
	{
		iNewSpaceID = iClientSpaceID++;
		if (iClientSpaceID == 0) iClientSpaceID = (1 << 30);
	} while (kZM.Space( iNewSpaceID, false ));

	FvZoneSpacePtr spNewSpace = kZM.Space( iNewSpaceID );
	
	s_kClientSpaces.push_back( spNewSpace );
	return spNewSpace->ID();
}
FvZoneSpacePtr Test_GetClientSpace( int iSpaceID )
{
	FvZoneSpacePtr spSpace = FvZoneManager::Instance().Space( iSpaceID, false );
	if (!spSpace)
	{
		return NULL;
	}

	if (std::find( s_kClientSpaces.begin(), s_kClientSpaces.end(), spSpace ) ==
		s_kClientSpaces.end())
	{
		return NULL;
	}

	return spSpace;
}
unsigned int Test_AddSpaceGeometryMapping( int iSpaceID, const char *pcPath )
{
	FvZoneSpacePtr spSpace = Test_GetClientSpace( iSpaceID );
	if (!spSpace)
	{
		return false;
	}

	FvString kEmptyString;
	
	static FvUInt32 uiNextLowEntryID = 0;
	FvUInt32 uiLowEntryID;

	FvZoneSpaceEntryID kSEID;
	kSEID.m_uiIP = 0;
	do
	{
		uiLowEntryID = uiNextLowEntryID++;
		kSEID.m_uiPort = FvUInt16(uiLowEntryID >> 16);
		kSEID.m_uiSalt = FvUInt16(uiLowEntryID);
	}
	while (spSpace->DataEntry( kSEID, 1, kEmptyString ) == FvUInt16(-1));

	if (!spSpace->AddMapping( kSEID, (float*)&FvMatrix::IDENTITY, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, pcPath ))
	{
		spSpace->DataEntry( kSEID, uint16(-1), kEmptyString );
		return NULL;
	}
	return uiLowEntryID;
}
int Test_CameraSpaceID( int iNewSpaceID )
{
	FV_ASSERT(FvClientCamera::pInstance());
	if (iNewSpaceID != 0)
	{
		FvZoneSpacePtr spSpace =
			FvZoneManager::Instance().Space( iNewSpaceID, false );
		if (!spSpace)
		{
			return NULL;
		}

		FvZoneManager::Instance().Camera( FvClientCamera::Instance().GetMatrix(), spSpace );
	}
	else 
	{ 
		FvZoneManager::Instance().Camera( FvClientCamera::Instance().GetMatrix(), NULL ); 
	} 


	FvZoneSpacePtr spSpace = FvZoneManager::Instance().CameraSpace();
	return spSpace ? spSpace->ID() : 0;
}
float Test_SpaceLoadStatus( float fDistance = -1.f)
{
	FvZoneSpacePtr spSpace = FvZoneManager::Instance().CameraSpace();

	if ( spSpace )
	{
		if (fDistance < 0.f)
		{
			FV_ASSERT(FvClientCamera::Instance().GetCamera());
			fDistance = FvClientCamera::Instance().GetCamera()->getFarClipDistance();
		}
		float fDist = FvZoneManager::Instance().ClosestUnloadedZone( spSpace );

		return std::min( 1.f, fDist / fDistance );
	}
	else
	{
		return 0.0f;
	}
}
void Test_ClearAllSpaces( bool bKeepClientOnlySpaces = false )
{
	FvZoneManager::Instance().ClearAllSpaces( bKeepClientOnlySpaces );
}

void Test_DirectionToQuaternion(const FvVector3& kDir, FvQuaternion& kQuat, bool bCamera = false)
{
	FvMatrix kMatrix;
	kMatrix.SetIdentity();
	const double sya = sin( kDir.z );
	const double cya = cos( kDir.z );
	const double sxa = sin( kDir.y );
	const double cxa = cos( kDir.y );
	const double sza = sin( kDir.x );
	const double cza = cos( kDir.x );

	kMatrix.m[0][0] =  cya * cza;
	kMatrix.m[0][1] =  cxa * sza;
	kMatrix.m[0][2] = -sya * cza + cya * sza * sxa;
	kMatrix.m[0][3] =  0.f;

	kMatrix.m[1][0] = -cya * sza;
	kMatrix.m[1][1] =  cxa * cza;
	kMatrix.m[1][2] =  sya * sza + cya * cza * sxa;
	kMatrix.m[1][3] =  0.f;

	kMatrix.m[2][0] =  sya * cxa;
	kMatrix.m[2][1] = -sxa;
	kMatrix.m[2][2] =  cxa * cya;
	kMatrix.m[2][3] =  0.f;

	kMatrix.m[3][0] = 0.f;
	kMatrix.m[3][1] = 0.f;
	kMatrix.m[3][2] = 0.f;
	kMatrix.m[3][3] = 1.f;

	FvMatrix kNewMatrix(
		kMatrix._11,kMatrix._13,kMatrix._12,kMatrix._14,
		kMatrix._31,kMatrix._33,kMatrix._32,kMatrix._24,
		kMatrix._21,kMatrix._23,kMatrix._22,kMatrix._34,
		kMatrix._41,kMatrix._43,kMatrix._42,kMatrix._44);

	FvVector3 kVec;
	FvQuaternion kQuatDir;
	FvMatrixDecomposeWorld4f((float*)&kQuatDir,(float*)&kVec,(float*)&kVec,
		(float*)&kNewMatrix);

	if(bCamera)
	{
		FvQuaternion kRotX;
		kRotX.FromAngleAxis(FV_MATH_PI_2_F, FvVector3::UNIT_X);
		kQuat = kRotX * kQuatDir;
	}
	else
	{
		kQuat = kQuatDir;
	}
}

void Test_DirectionToQuaternion(const FvVector3& kDir, Ogre::Quaternion& kQuat, bool bCamera = false)
{
	FvQuaternion kQuatNew;
	Test_DirectionToQuaternion(kDir,kQuatNew,bCamera);
	kQuat.w = kQuatNew.w;
	kQuat.x = kQuatNew.x;
	kQuat.y = kQuatNew.y;
	kQuat.z = kQuatNew.z;
}