
FV_INLINE 
void FvSimulation::Size(int iSize)
{
	m_iSize = iSize;
}

FV_INLINE 
int FvSimulation::Size() const
{
	return m_iSize;
}


FV_INLINE 
void FvSimulation::AddMovement( const FvVector3 &kFrom, const FvVector3 &kTo, const float fDiameter )
{
#ifdef _DEBUG
	if (!FvSimulationManager::ms_bPaused)
#endif
	{
		m_kMovements.push_back( new FvMovement( kFrom, kTo, fDiameter, this ));
	}
}


FV_INLINE 
FvMovements &FvSimulation::Movements()
{ 
	return m_kMovements;
}

FV_INLINE 
void FvSimulation::ResetIdleTimer()
{ 
	m_fIdleTime = 0.f;
}


FV_INLINE 
float FvSimulation::IdleTime() const
{
	return m_fIdleTime;
}


FV_INLINE 
bool FvSimulation::HasMovements() const
{
	return ( m_kMovements.size() > 0 );
}


FV_INLINE 
void FvSimulation::Activate()
{
	m_bActive = true;
}


FV_INLINE 
void FvSimulation::Deactivate() 
{ 
#ifdef _DEBUG
	if (!FvSimulationManager::ms_bPaused)
#endif
		m_bActive = false;
}


FV_INLINE 
bool FvSimulation::IsActive() const
{ 
	return m_bActive;
}


FV_INLINE 
bool FvSimulation::ShouldActivate() const
{
#ifdef _DEBUG
	if (FvSimulationManager::ms_bPaused)
		return false;
#endif

	return !m_bActive && m_bPerturb;
}


FV_INLINE 
bool FvSimulation::ShouldDeactivate() const
{
#ifdef _DEBUG
	if (FvSimulationManager::ms_bPaused)
		return false;
#endif
	if( m_bActive && m_bPerturb == false)
		return (m_fIdleTime > FvSimulationManager::Instance().MaxIdleTime());
	else
		return false;
}


FV_INLINE 
bool FvSimulation::Perturbed() const
{
	return m_bPerturb;
}


FV_INLINE 
void FvSimulation::Perturbed( bool bVal )
{
	m_bPerturb = bVal;
}


FV_INLINE 
void FvSimulation::UpdateTime( float fTime )
{
#ifdef _DEBUG
	if (!FvSimulationManager::ms_bPaused)
#endif
	{
		m_fIdleTime += fTime;
		m_fLastSimTime += fTime;
	}
}


FV_INLINE 
float FvSimulation::LastUpdateTime() const
{
	return m_fLastSimTime;
}


FV_INLINE 
void FvSimulation::ClearMovements()
{
	m_kMovements.clear();
}


FV_INLINE 
void FvSimulationCell::Tick()
{ 
	if ( m_pkSimulationTarget
#ifdef _DEBUG
		&& !FvSimulationManager::ms_bPaused 
#endif
	)
	{
		m_pkSimulationTarget->TickSimulation();
	}
}


FV_INLINE 
void FvSimulationCell::Mark( FvUInt32 uiMark )
{
	m_uiTickMark = uiMark;
}


FV_INLINE 
FvUInt32 FvSimulationCell::Mark( ) const
{
	return m_uiTickMark;
}


FV_INLINE 
FvSimulationTextureBlock *FvSimulationCell::SimulationBlock() const
{
	return m_pkSimulationTarget;
}


FV_INLINE 
void FvSimulationCell::SimulationBlock( FvSimulationTextureBlock *pkVal)
{
	m_pkSimulationTarget = pkVal;
}


FV_INLINE 
bool FvSimulationCell::IsActive() const
{ 
	return ( m_pkSimulationTarget != 0) && FvSimulation::IsActive();
}


FV_INLINE 
void FvSimulationCell::Activate()
{
#ifdef _DEBUG
	if (!FvSimulationManager::ms_bPaused)
#endif
	{
		FvSimulation::Activate();
		m_pkSimulationTarget = FvSimulationManager::Instance().RequestSimulationTextureBlock( this );
	}
}


FV_INLINE 
void FvSimulationCell::Deactivate() 
{ 
#ifdef _DEBUG
	if (!FvSimulationManager::ms_bPaused)
#endif
	{
		FvSimulation::Deactivate();
		if ( m_pkSimulationTarget )
		{
			FvSimulationManager::Instance().ReleaseSimulationTextureBlock( m_pkSimulationTarget );
			m_pkSimulationTarget = NULL;
		}
		Edge(0);
	}
}

FV_INLINE 
int FvSimulationCell::GetActiveEdge( float fThreshold )
{ 
	float fUpper = 1.f - fThreshold;
	int iEdges = 0;	
	for (unsigned int i = 0; i < Movements().size(); i++)
	{
		const FvMovement &kMovement = *Movements()[i];
		const FvVector3 &kMove = kMovement.m_kTo;

		if ( kMove.x <= fThreshold )
			iEdges |= 2;

		if ( kMove.x >= fUpper )
			iEdges |= 8;

		if ( kMove.z <= fThreshold )
			iEdges |= 1;

		if ( kMove.z >= fUpper )
			iEdges |= 4;
	}
	return iEdges;
}


FV_INLINE 
void FvSimulationTextureBlock::TickSimulation() 
{
#ifdef _DEBUG
	if (!FvSimulationManager::ms_bPaused)
#endif
		m_iSimulationIndex++;
}


FV_INLINE 
void FvRainSimulation::Tick()
{ 
	m_kSimulationTarget.TickSimulation();
}


FV_INLINE 
void FvRainSimulation::Clear()
{
	m_kSimulationTarget.Clear();
}


FV_INLINE 
void FvRainSimulation::Recreate()
{
	m_kSimulationTarget.Recreate();
}


FV_INLINE 
void FvRainSimulation::Init(int iSize)
{
	m_kSimulationTarget.Init(iSize,iSize);
}


FV_INLINE 
void FvRainSimulation::Fini()
{
	m_kSimulationTarget.Fini();
}

FV_INLINE 
FvRenderTargetPtr FvRainSimulation::Result()
{ 
	return m_kSimulationTarget.Result();
}


FV_INLINE 
FvRenderTargetPtr FvSimulationManager::RainTexture()
{ 
	return m_kRainSimulation.Result();
}
