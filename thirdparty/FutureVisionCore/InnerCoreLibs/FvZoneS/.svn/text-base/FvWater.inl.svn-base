
FV_INLINE 
float FvWaters::MovementImpact() const
{ 
	return m_fMovementImpact;
}

FV_INLINE 
bool FvWaters::DrawReflection() 
{ 
	return ms_bDrawReflection;
}

FV_INLINE 
int FvWaters::SimulationLevel() const
{ 
	return ms_iSimulationLevel; 
}

FV_INLINE 
void FvWaters::DrawWaters( bool bDraw )
{
	ms_bDrawWaters = bDraw;
}


FV_INLINE 
void FvWaters::SimulationEnabled( bool bVal)
{ 
	ms_bSimulationEnabled = bVal;
}


FV_INLINE 
bool FvWaters::SimulationEnabled()
{
	return ((ms_bSimulationEnabled) && (ms_iSimulationLevel != 0));
}

FV_INLINE 
bool FvWaters::InsideVolume() const
{ 
	return m_bInsideWaterVolume;
}

FV_INLINE void FvWaters::InsideVolume( bool bVal )
{ 
	m_bInsideWaterVolume = bVal;
}

FV_INLINE void FvWaters::DrawWireframe( bool bWire )
{
	m_bDrawWireframe = bWire;
}

FV_INLINE void FvWaters::RainAmount( float fRainAmount )
{
	m_fRainAmount = ::max( 0.f, ::min( 1.f, fRainAmount ) );
}

FV_INLINE float FvWaters::RainAmount() const
{ 
	return m_fRainAmount;
}

FV_INLINE void FvWaters::PlayerPos( FvVector3 kPos )
{
	m_kPlayerPos = kPos;
}


#ifdef FV_SPLASH_ENABLED
FV_INLINE 
void FvWaters::AddSplash( const FvVector4 &impact,const float height, bool force )
{ 
	//m_kSplashManager.AddSplash(impact,height,force);
}

FV_INLINE 
bool FvWaters::CheckImpactTimer() 
{
	if ( ms_fAutoImpactInterval >= 0 && (m_fImpactTimer > ms_fAutoImpactInterval) )
	{
		m_fImpactTimer = 0.f;
		return true;
	}
	return false;
}
#endif // FV_SPLASH_ENABLED

FV_INLINE 
bool FvWater::ShouldDraw() const
{
	return ( m_bInitialised && m_bVisible && FvWaters::ms_bDrawWaters );
}


FV_INLINE 
const FvVector3& FvWater::Position() const
{ 
	return m_kConfig.m_kPosition;
}

FV_INLINE 
const FvVector2& FvWater::Size() const
{ 
	return m_kConfig.m_kSize;
}

FV_INLINE 
float FvWater::Orientation() const
{ 
	return m_kConfig.m_fOrientation;
}

FV_INLINE 
FvRenderTargetPtr FvWaterCell::SimTexture()
{
	return Result();
} 

FV_INLINE 
int FvWaterCell::IndexCount() const
{
	return m_uiNumIndices;
}

FV_INLINE
bool FvWaterCell::SimulationActive() const
{
	return IsActive();
}

FV_INLINE 
void FvWaterCell::InitSimulation( int iSize, float fCellSize )
{	
	FvSimulationCell::Init( iSize, fCellSize );
}

FV_INLINE 
void FvWaterCell::Draw( int nVerts )
{ 
	//FV_ASSERT_DEBUG( m_uiNumIndices > 2 );
	//Moo::rc().drawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, nVerts, 0, m_uiNumIndices - 2 );
}

FV_INLINE
void FvWaterCell::InitEdge( int index, FvWaterCell* cell )
{
	m_apkEdgeCells[index] = cell;
}

FV_INLINE 
const FvVector2& FvWaterCell::Min()
{
	return m_kMin;
}

FV_INLINE 
const FvVector2& FvWaterCell::Max()
{
	return m_kMax;
}

FV_INLINE 
const FvVector2& FvWaterCell::Size()
{
	return m_kSize;
}

FV_INLINE void FvWaterCell::Deactivate()
{
	FvSimulationCell::Deactivate();
	Edge(0);
}