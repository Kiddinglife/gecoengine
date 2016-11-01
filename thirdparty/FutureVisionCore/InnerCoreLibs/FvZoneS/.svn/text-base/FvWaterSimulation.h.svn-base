//{future header message}
#ifndef __FvWaterSimulation_H__
#define __FvWaterSimulation_H__

#include <FvVector3.h>
#include <FvSmartPointer.h>
#include <OgreTexture.h>
#include <OgreMaterial.h>
#include <OgreRenderTarget.h>
#include <OgreRenderQueueListener.h>
#include <OgreRenderTargetListener.h>

#include "FvCamera.h"

class FvWater;
typedef FvSmartPointer< FvWater > FvWaterPtr;
class FvWaters;
class FvBackgroundTask;

struct FvRenderTarget : public FvSafeReferenceCount,
	public Ogre::RenderTargetListener
{
	FvRenderTarget();
	~FvRenderTarget();

	void preRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent);
	void postRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent);

	void Create(int iWidth, int iHeight, bool bReuseMainZBuffer = false, 
		Ogre::PixelFormat ePixelFormat = Ogre::PF_A8R8G8B8);

	FvCamera *m_pkCamera;
	Ogre::Viewport *m_pkViewport;
	Ogre::RenderTarget *m_pkRenderTarget;
	Ogre::TexturePtr m_spTexture;
	Ogre::SceneManager::IlluminationRenderStage m_eSavedStage;
	static FvUInt32 ms_uiRenderTargetIdentify;
};
typedef FvSmartPointer<FvRenderTarget> FvRenderTargetPtr;

class FvSimulationTextureBlock : 
	public Ogre::Renderable
{
public:
	FvSimulationTextureBlock();
	FvSimulationTextureBlock( int iWidth, int iHeight );

	void Init( int iWidth, int iHeight );
	void Fini();
	void Recreate();
	void Clear();
	void TickSimulation();
	bool Lock();
	void Unlock() { m_bLocked = false; }

	FvRenderTargetPtr &operator()(int iIndex) { return m_aspSimTexture[iIndex]; }
	FvRenderTargetPtr operator()(int iIndex) const { return m_aspSimTexture[iIndex]; }

	void Render(FvCamera::VisibleObjectFinder *pkFinder);

	FvRenderTargetPtr Previous() { return m_aspSimTexture[ (m_iSimulationIndex + 1) % 3 ]; } 
	FvRenderTargetPtr Current() { return m_aspSimTexture[ (m_iSimulationIndex + 2) % 3 ]; } 
	FvRenderTargetPtr Result() { return (m_iSimulationIndex>0) ? m_aspSimTexture[(m_iSimulationIndex-1) % 3] : m_aspSimTexture[0]; }

	int Width() { return m_iWidth; }
	int Height() { return m_iHeight; }

	void SetTechnique(int iTech) { m_iTechinqueIndex = iTech; }

	void UpdateHeightTexture();

	bool preRender(Ogre::SceneManager *pkSceneManager, Ogre::RenderSystem *pkRender);

	const Ogre::MaterialPtr &getMaterial(void) const;

	void getRenderOperation(Ogre::RenderOperation &kOP);

	Ogre::Technique* getTechnique(void) const;

	void getWorldTransforms(Ogre::Matrix4 *pkXForm) const;

	Ogre::Real getSquaredViewDepth(const Ogre::Camera *pkCam) const;

	const Ogre::LightList &getLights(void) const;

	void _updateCustomGpuParameter(
		const Ogre::GpuProgramParameters::AutoConstantEntry &kConstantEntry,
		Ogre::GpuProgramParameters *pkParams) const;

	float m_fConsistency;
	float m_fTime;

private:

	FvRenderTargetPtr m_aspSimTexture[3];
	int m_iWidth;
	int	m_iHeight;
	int m_iSimulationIndex;
	int m_iTechinqueIndex;
	bool m_bLocked;
};

class FvSimulation;

struct FvMovement : 
	public Ogre::Renderable,
	public FvSafeReferenceCount
{
	FvMovement(const FvVector3 &kFrom,
		const FvVector3 &kTo, 
		float fDiameter,FvSimulation *pkSimulation):
	m_kFrom(kFrom),
	m_kTo(kTo),
	m_fDiameter(fDiameter),
	m_pkSimulation(pkSimulation){}

	const Ogre::MaterialPtr &getMaterial(void) const;

	void getRenderOperation(Ogre::RenderOperation &kOP);

	Ogre::Technique* getTechnique(void) const;

	void getWorldTransforms(Ogre::Matrix4 *pkXForm) const;

	Ogre::Real getSquaredViewDepth(const Ogre::Camera *pkCam) const;

	const Ogre::LightList &getLights(void) const;

	void _updateCustomGpuParameter(
		const Ogre::GpuProgramParameters::AutoConstantEntry &kConstantEntry,
		Ogre::GpuProgramParameters *pkParams) const;

	FvVector3 m_kFrom;
	FvVector3 m_kTo;
	float m_fDiameter;

	FvSimulation *m_pkSimulation;

	static const int TECHNIQUE_INDEX = 0;
};
typedef FvSmartPointer< FvMovement > FvMovementPtr;
typedef std::vector< FvMovementPtr > FvMovements;

class FvSimulation : public FvCamera::VisibleObjectFinder
{
public:
	FvSimulation();	
	virtual ~FvSimulation();

	void ResetIdleTimer();
	int Size() const;
	void Size(int iSize);
	void ClearMovements();
	float LastUpdateTime() const;
	float IdleTime() const;
	bool Perturbed() const;
	void Perturbed( bool val );	
	void UpdateTime( float dTime );
	bool HasMovements() const;
	bool ShouldActivate() const;
	bool ShouldDeactivate() const;	
	FvMovements &Movements();
	FvCamera *SimulationCamera();

	virtual void Tick() = 0;
	virtual void Activate();
	virtual void Deactivate();
	virtual bool IsActive() const;
	virtual void UpdateMovements();
	virtual void AddMovement( const FvVector3 &kFrom, const FvVector3 &kTo, const float fDiameter );
	virtual void FindVisibleObject( 
		Ogre::RenderQueue *pkQueue,
		FvCamera *pkCamera, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
		bool bOnlyShadowCasters ){}

private:

	int m_iSize;
	float m_fIdleTime;
	float m_fLastSimTime;
	FvMovements m_kMovements;
	FvUInt32 m_uiLastMovementFrame;
	bool m_bPerturb;
	bool m_bActive;
};

class FvRainSimulation : public FvSimulation
{
public:
	FvRainSimulation();

	void Fini();
	void Clear();
	void Recreate();
	void Init(int size);
	FvRenderTargetPtr Result();
	void Simulate( float fTime, float fAmount, float fConsistency );

	void FindVisibleObject( 
		Ogre::RenderQueue *pkQueue,
		FvCamera *pkCamera, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
		bool bOnlyShadowCasters );

	virtual void Tick();

private:
	FvSimulationTextureBlock m_kSimulationTarget;
};

class FvSimulationCell;

class FvSimulationManager
{
public:
	static void Fini();	
	static void Init();

	void LoadResources();
	void RecreateBlocks();
	void TickSimulations();
	FvRenderTargetPtr RainTexture();
	float MaxIdleTime() { return ms_fMaxIdleTime; }
	bool RainActive() { return !m_kRainSimulation.ShouldDeactivate(); }
	void VisitMovements( FvSimulation *pkSim, Ogre::RenderQueue *pkQueue );
	void SetMaxTextures( int value ) { ms_iMaxTextureBlocks = value; }	
	void SimulateRain( float fTime, float fAmount, float fConsistency );
	void ReleaseSimulationTextureBlock( FvSimulationTextureBlock* block );
	FvSimulationTextureBlock *RequestSimulationTextureBlock( FvSimulationCell* cell );

	Ogre::TexturePtr &CookieTexture() { return m_spCookieTexture; }
	Ogre::TexturePtr &NullSim() { return m_spNullSim; }
	int SimulationSize() const { return m_iSimulationSize; }

#ifdef _DEBUG
	static bool ms_bPaused;
#endif

	static FvSimulationManager &Instance( ) { return *ms_pkInstance; }
private:
	typedef std::vector< FvSimulationTextureBlock > TextureBlocks;
	TextureBlocks m_kBlocks;
	int m_iAvailableBlocks;
	int m_iSimulationSize;

	Ogre::TexturePtr m_spCookieTexture;
	Ogre::TexturePtr m_spNullSim;

	static FvSimulationManager *ms_pkInstance;
	static float ms_fMaxIdleTime;
	static int ms_iMaxTextureBlocks;

	FvRainSimulation m_kRainSimulation;

	void InitInternal();
	void FiniInternal();

	FvSimulationManager();
	~FvSimulationManager();
};

class FvSimulationCell : public FvSimulation
{
public:
	FvSimulationCell();

	void Init( int iTexSize, float fCellSize );	
	virtual void Clear();

	virtual void Tick();
	virtual void Activate();
	virtual void Deactivate();
	virtual bool IsActive() const;
	void Simulate( float fTime, FvWaters &kGroup, float fConsistency );

	void StitchEdges( float fTime, FvWaters &kGroup );

	void Edge( int iEdge ) { m_iEdge = iEdge; }
	int Edge( ) { return m_iEdge; }
	void AddEdge( int iEdge ){ m_iEdge |= (1<<iEdge); }	
	int GetActiveEdge( float fThreshold );
	void BindAsNeighbour( Ogre::Technique *pkTechnique, int kBinding );
	virtual void BindNeighbour( Ogre::Technique *pkTechnique, int iEdge ) { }

	void Mark( FvUInt32 uiMark );
	FvUInt32 Mark() const;

	FvSimulationTextureBlock *SimulationBlock() const;
	void SimulationBlock( FvSimulationTextureBlock *pkVal);

	static void CreateUnmanaged();

	void FindVisibleObject( 
		Ogre::RenderQueue *pkQueue,
		FvCamera *pkCamera, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
		bool bOnlyShadowCasters );

	FvRenderTargetPtr Previous() { return m_pkSimulationTarget->Previous(); } 
	FvRenderTargetPtr Current() { return m_pkSimulationTarget->Current(); } 
	FvRenderTargetPtr Result() { return m_pkSimulationTarget->Result(); }

	static float ms_fHitTime;
	static float ms_fWaveSpeedSquared;
	static float ms_fLandScaleX;
	static float ms_fLandScaleY;
	static float ms_fLandScaleZ;

	const bool EdgeActivation() const { return m_bEdgeActivation; }
	void EdgeActivation(bool bVal) { m_bEdgeActivation = bVal; }

protected:
	bool m_bEdgeActivation;

	FvUInt32 m_uiTickMark;

private:
	int m_iEdge;
	float m_fCellSize;
	FvSimulationTextureBlock *m_pkSimulationTarget;
};

#include "FvWaterSimulation.inl"

#endif // __FvWaterSimulation_H__