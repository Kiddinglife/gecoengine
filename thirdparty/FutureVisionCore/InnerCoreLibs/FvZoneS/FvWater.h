//{future header message}
#ifndef __FvWater_H__
#define __FvWater_H__

#include <FvDebug.h>
#include <FvProvider.h>
#include <FvRefList.h>

#ifndef FV_EDITOR_ENABLED
#define FV_SPLASH_ENABLED
#endif //FV_EDITOR_ENABLED

#include "FvWaterSimulation.h"
#include "FvEnvironmentCollider.h"

#define FV_SIM_BORDER_SIZE 3

struct FvWaterVertexType
{
	FvVector3 m_kPos;
	FvUInt32 m_uiColor;
	FvVector2 m_kUV;
};

class FvWaterCell;
typedef FvSmartPointer<FvWaterCell> FvWaterCellPtr;
typedef FvSmartPointer<FvProvider2<FvVector3, FvVector4>> FvWaterProviderPtr;

class FvWaterRefractionListener
{
public:
	virtual void UpdateRefractionTexture(const FvString& kText) = 0;
};
class FvWaterDepthListener
{
public:
	virtual void UpdateDepthTexture(const FvString& kText) = 0;
};

class FvWaterCell : public FvSimulationCell,
	public Ogre::Renderable, public FvSafeReferenceCount,
	public FvWaterRefractionListener,
	public FvWaterDepthListener
{
public:
	typedef std::list< FvSimulationCell* > SimulationCellPtrList;
	typedef std::list< FvWaterCell* > WaterCellPtrList;
	typedef std::vector< FvWaterCellPtr > WaterCellVector;

	FvWaterCell();
	~FvWaterCell();

	bool Init( FvWater *pkWater, FvVector2 kStart, FvVector2 kSize );
	void InitEdge( int iIndex, FvWaterCell *pkCell );
	void CreateManagedIndices();
	void DeleteManaged();

	void Draw( int iNumVerts );

	int IndexCount() const;
	virtual void Deactivate();
	bool SimulationActive() const;
	void InitSimulation( int iSize, float fCellSize );
	virtual void BindNeighbour( Ogre::Technique *pkTechnique, int iEdge );
	void CheckEdgeActivity( SimulationCellPtrList &kActiveList );
	FvRenderTargetPtr SimTexture();
	void CheckActivity( SimulationCellPtrList &kActiveList, WaterCellPtrList &kEdgeList );
	FvUInt32 BufferIndex() const { return m_uiBufferIndex; }

	FvWaterCell *EdgeCell( unsigned int iIndex ) const { FV_ASSERT_DEBUG(iIndex<4); return m_apkEdgeCells[iIndex]; }

	const FvVector2 &Min();
	const FvVector2 &Max();
	const FvVector2 &Size();

	float CellDistance() const { return m_fCellDistance; }
	void UpdateDistance(const FvVector3 &kCamPos);

	const Ogre::MaterialPtr &getMaterial(void) const;
	void getRenderOperation(Ogre::RenderOperation &kOP);
	virtual Ogre::Technique* getTechnique(void) const;
	void getWorldTransforms(Ogre::Matrix4 *pkXForm) const;
	Ogre::Real getSquaredViewDepth(const Ogre::Camera *pkCam) const;
	const Ogre::LightList &getLights(void) const;

private:
	template< class T >
	void BuildIndices();

	void ActivateEdgeCell( unsigned int index );

	virtual void UpdateRefractionTexture(const FvString& kText);
	virtual void UpdateDepthTexture(const FvString& kText);

	FvVector2 m_kMin; 
	FvVector2 m_kMax;
	FvVector2 m_kSize;
	FvWaterCell *m_apkEdgeCells[4];
	FvUInt32 m_uiNumIndices;
	FvUInt32 m_uiBufferIndex;
	FvWater *m_pkWater;
	int m_iXMax;
	int m_iYMax;
	int m_iXMin;
	int m_iYMin;

	float m_fCellDistance;

	Ogre::VertexData *m_pkVertexData;
	Ogre::IndexData *m_pkIndexData;
	Ogre::MaterialPtr m_spMaterial;

	static FvUInt32 ms_uiWaterCellIdentify;

	friend class FvWater;
};

class FvWater : public FvReferenceCount
{
public:
	enum
	{
		ALWAYS_VISIBLE,
		INSIDE_ONLY,
		OUTSIDE_ONLY
	};

	typedef struct _WaterState
	{
		FvVector3 m_kPosition;
		FvVector2 m_kSize;
		float m_fOrientation;		
		float m_fTessellation;
		float m_fTextureTessellation;
		float m_fConsistency;
		float m_fFresnelConstant;
		float m_fFresnelExponent;
		float m_fReflectionScale;
		float m_fRefractionScale;
		FvVector2 m_kScrollSpeed1;
		FvVector2 m_kScrollSpeed2;
		FvVector2 m_kWaveScale;
		float m_fSunPower;
		float m_fSunScale;
		float m_fWindVelocity;
		float m_fDepth;
		FvVector4 m_kReflectionTint;
		FvVector4 m_kRefractionTint;
		float m_fSimCellSize;
		float m_fSmoothness;
		bool m_bUseEdgeAlpha;
		bool m_bUseSimulation;
		bool m_bUseCubeMap;
		FvUInt32 m_uiVisibility;

		FvVector4 m_kDeepColour;
		float m_fFadeDepth;
		float m_fFoamIntersection;
		float m_fFoamMultiplier;
		float m_fFoamTiling;

		FvString m_kFoamTexture;
		FvString m_kWaveTexture;
		FvString m_kReflectionTexture;
		FvString m_kTransparencyTable;
	} WaterState;

	FvWater( const WaterState& config, FvEnvironmentColliderPtr pCollider = NULL );

	void Rebuild( const WaterState &kConfig );
	void DeleteManagedObjects( );
	void CreateManagedObjects( );
	void DeleteUnmanagedObjects( );
	void CreateUnmanagedObjects( );	
	FvUInt32 CreateIndices( );
	int	 CreateWaveIndices( );	
	void CreateCells( );
	void Tick(FvCamera *pkCamera);

	static void DeleteWater(FvWater *pkWater);
	static bool StillValid(FvWater *pkWater);

	FvUInt32 RemapVertex( FvUInt32 iIndex );
	template <class T>
	FvUInt32 Remap(std::vector<T> &kDestIndices,
					const std::vector<FvUInt32> &kSrcIndices);
	std::vector< std::map<FvUInt32, FvUInt32> > m_kRemappedVerts;


#if FV_UMBRA_ENABLE
	void EnableOccluders() const;
	void DisableOccluders() const;
	void AddTerrainItem( FvZoneTerrain *pkItem );
	void EraseTerrainItem( FvZoneTerrain *pkItem );
#endif // FV_UMBRA_ENABLE

#ifdef FV_EDITOR_ENABLED
	void DeleteData( );
	void SaveTransparencyTable( );

	void DrawRed(bool bValue) { m_bDrawRed = bValue; }
	bool DrawRed() const { return m_bDrawRed; }

	void Highlight(bool bValue) { m_bHighlight = bValue; }
	bool Highlight() const { return m_bHighlight; }
#endif // FV_EDITOR_ENABLED

	bool AddMovement( const FvVector3 &kFrom, const FvVector3 &kTo, const float fDiameter );

	FvUInt32 DrawMark() const { return m_uiDrawMark; }
	void DrawMark( FvUInt32 uiMask )	{ m_uiDrawMark = uiMask; }

	FvVector4 GetFogColour() const { return m_kConfig.m_kRefractionTint; }
	float GetDepth() const { return m_kConfig.m_fDepth; }

	void ClearRT();
	void DrawMask();
	void UpdateSimulation( float fTime );
	void VisitRenderable( FvWaters &kGroup, float fTime,
		Ogre::RenderQueue *pkQueue,FvCamera *pkCamera,
		Ogre::Texture *pkReflectionTexture = NULL );
	
	bool ShouldDraw() const;
	bool NeedSceneUpdate() const;

	void UpdateVisibility( FvCamera *pkCamera );
	bool Visible() const { return m_bVisible; }	

	const FvVector3 &Position() const;
	const FvVector2 &Size() const;
	float Orientation() const;

	const FvVector3 &CameraPos() const { return m_kCamPos; }

	const FvBoundingBox &BB() const { return  m_kBBActual; }
	
	bool CheckVolume();
	bool IsInside( FvWaterProviderPtr spMat );
	bool IsInsideBoundary( FvMatrix m );
	float SurfaceHeight() const { return this->Position().z; }

	static Ogre::VertexData *ms_pkVertexData;
	static Ogre::HardwareVertexBufferSharedPtr ms_spQuadVertexBuffer;
	static float ms_fSceneCullDistance;	
	static float ms_fSceneCullFadeDistance;	

    static bool BackgroundLoad();
    static void BackgroundLoad(bool bBackground);

private:
	typedef std::vector< FvUInt32 > WaterAlphas;
	typedef std::vector< bool > WaterRigid;
	typedef std::vector< FvInt32 > WaterIndices;

	~FvWater();

	void Visible(bool bVal) { m_bVisible=bVal; }

	WaterState m_kConfig;

	FvMatrix m_kTransform;
	FvMatrix m_kInvTransform;

	FvVector3 m_kCamPos;

	FvUInt32 m_uiGridSizeX;
	FvUInt32 m_uiGridSizeY;

	float m_fTime;
	float m_fLastDTime;

	WaterRigid m_kWaterRigid;
	WaterAlphas	m_kWaterAlpha;
	WaterIndices m_kWaterIndex;

	std::vector<Ogre::HardwareVertexBufferSharedPtr> m_kVertexBuffers;
	FvUInt32 m_uiNumVertices;

	FvWaterCell::WaterCellVector m_kCells;
	FvWaterCell::SimulationCellPtrList m_kActiveSimulations;

#ifdef FV_EDITOR_ENABLED
	bool m_bDrawRed;
	bool m_bHighlight;
#endif // FV_EDITOR_ENABLED

	bool m_bVisible;
	bool m_bRaining;
	bool m_bDrawSelf;
	bool m_bInitialised;
	bool m_bEnableSim;
	bool m_bReflectionCleared;
	bool m_bCreatedCells;
	float m_fTexAnim;
	float m_fSimpleReflection; 

#ifdef FV_DEBUG_WATER
	static int ms_iDebugCell;
	static int ms_iDebugCell2;
	static bool ms_bDebugSim;
#endif // FV_DEBUG_WATER
	static bool ms_bBackgroundLoad;
	static bool ms_bCullCells;
	static float ms_fCullDistance;
	static float ms_fMaxSimDistance;

	FvBoundingBox m_kBB;
	FvBoundingBox m_kBBDeep;
	FvBoundingBox m_kBBActual;

	FvUInt32 m_uiDrawMark;
	bool m_bVisited;

	Ogre::TexturePtr m_spNormalTexture;
	Ogre::TexturePtr m_spScreenFadeTexture;
	Ogre::TexturePtr m_spFoamTexture;
	Ogre::TexturePtr m_spReflectionTexture;
	FvEnvironmentColliderPtr m_spCollider;	
	//PyObjectPtr m_spPyVolume;

#if FV_UMBRA_ENABLE
	typedef std::vector<FvZoneTerrainPtr> TerrainVector;
	TerrainVector m_kTerrainItems;
#endif // FV_UMBRA_ENABLE	

	FvWater(const FvWater&);
	FvWater &operator=(const FvWater&);

	friend class FvWaters;
	friend class FvWaterCell;

	void BuildTransparencyTable( );
	bool LoadTransparencyTable( );
	void Setup2ndPhase();
	void StartResLoader();
	void RenderSimulation(float fTime);
	void ResetSimulation();
	void ReleaseWaterScene();

	void SetupGPUParameters( FvWaterCell *pkCell, Ogre::Texture *pkReflectionTexture = NULL );
	bool Init();

	static void DoCreateTables( void *pkSelf );
	static void OnCreateTablesComplete( void *pkSelf );
	static void DoLoadResources( void *pkSelf );
	static void OnLoadResourcesComplete( void *pkSelf );
};

class FV_ZONE_API FvWaters : public std::vector< FvWater* >
{
	typedef FvWaters This;
public:
	~FvWaters();
	static FvWaters &Instance();

	void Init( FvCamera *pkCamera, bool bHDR);
	void Fini();

	void FindWatersList( float fTime,
		Ogre::RenderQueue *pkQueue,
		FvCamera *pkCamera );
	void CheckVolumes( FvCamera *pkCamera );

	void UpdateSimulations( float fTime );

	void Tick( float fTime );

	void DrawMasks();

	unsigned int DrawCount() const;
	
	int SimulationLevel() const;
	void PlayerPos( FvVector3 kPos );

	float RainAmount() const;
	void RainAmount( float fRainAmount );

	//class VolumeCallback
	//{
	//public:
	//	virtual void Callback(bool bEntering, const FvWater *pkWater) = 0;
	//};
	//static FvUInt32 AddWaterVolumeListener( FvWaterProviderPtr spDynamicSource,
	//	VolumeCallback *pkCallback );

	class VolumeListener: public FvRefNode1<VolumeListener*>
	{
	public:
		VolumeListener():m_pkWater( NULL ){}

		  const FvWaterProviderPtr& Source()const{return m_spSource;}
		  void SetSource(const FvWaterProviderPtr& spSource){m_spSource = spSource;}

		  void Water( FvWater *pkWater ) { m_pkWater = pkWater; }
		  FvWater *Water() const { return m_pkWater; }

		  bool Inside() const { return (m_pkWater != NULL); }
		  virtual void Callback(bool bEntering) = 0;

	private:
		FvWaterProviderPtr m_spSource;
		FvWater *m_pkWater;
	};
	static void AddWaterVolumeListener(VolumeListener& kListener);
	static void UpdateVolumeListener(VolumeListener& kListener);
	//static void DelWaterVolumeListener( FvUInt32 uiID );

	void CheckAllListeners();
	bool InsideVolume() const;
	void DrawWireframe( bool bWire );

	void SetQualityOption(int iOptionIndex);
	void SetSimulationOption(int iOptionIndex);
	virtual void OnSelectPSVersionCap(int iPSVerCap);

	float MovementImpact() const;
	void AddMovement( const FvVector3 &kFrom, const FvVector3 &kTo, const float fDiameter );

#ifdef FV_SPLASH_ENABLED
	void AddSplash( const FvVector4 &kImpact, const float fHeight, bool bForce = false );
	bool CheckImpactTimer();
#endif //FV_SPLASH_ENABLED

	void CreateSplashSystem();

	static void SetEnableHDR( bool bHDR );

	static void DrawWaters( bool bDraw );
	static void AddToDrawList( FvWater *pkWater );	

	static bool SimulationEnabled();
	static void SimulationEnabled( bool bVal);

	static void LoadResources( void *pkSelf );
	static void LoadResourcesComplete( void *pkSelf );

	static bool DrawReflection();
	static void DrawReflection( bool bVal );

	static FvUInt32 NextDrawMark() { return ms_uiNextMark; }

	static FvUInt32 LastDrawMark() { return ms_uiLastDrawMark; }

	static bool	HighQualityMode() { return ms_bHighQuality; }

#if FV_EDITOR_ENABLED
	static void ProjectView( bool bVal ) { ms_bProjectView = bVal; }
	static bool ProjectView( ) { return ms_bProjectView; }
	static bool ms_bProjectView;
#endif

	static void AddRefractionListener(FvWaterRefractionListener *pkListener);
	static void RemoveRefractionListener(FvWaterRefractionListener *pkListener);
	static void SetRefractionTexture(const FvString &kTex);
	static void AddDepthListener(FvWaterDepthListener *pkListener);
	static void RemoveDepthListener(FvWaterDepthListener *pkListener);
	static void SetDepthTexture(const FvString &kTex);

	static Ogre::MaterialPtr ms_spMaterial;
	static Ogre::MaterialPtr ms_spSimulationMaterial;
	static Ogre::Vector3 ms_kSunDirection;
	static Ogre::Vector3 ms_kSunColor;

private:
	FvWaters();

	void InsideVolume( bool val );

#ifdef FV_SPLASH_ENABLED
	//SplashManager m_kSplashManager;
#endif

	bool m_bInsideWaterVolume;	
	bool m_bDrawWireframe;
	float m_fMovementImpact;
	float m_fRainAmount;
	float m_fImpactTimer;
	FvVector3 m_kPlayerPos;

	//GraphicsSettingPtr m_spQualitySettings;
	//GraphicsSettingPtr m_spSimSettings;
	int m_iShaderCap;

	FvBackgroundTask *m_pkLoadingTask;  

	//typedef std::vector<VolumeListener>	VolumeListeners;
	typedef FvRefList<VolumeListener*, FvRefNode1> VolumeListeners;
	VolumeListeners m_kListeners;

	static std::set<FvWaterRefractionListener*> ms_kRefractionListeners;
	static FvString ms_kRefractionTextureName;
	static std::set<FvWaterDepthListener*> ms_kDepthListeners;
	static FvString ms_kDepthTextureName;

	static bool ms_bEnableHDR;
	static bool ms_bHighQuality;
	static bool ms_bDrawWaters;
	static bool ms_bSimulationEnabled;
	static bool ms_bDrawReflection;
	static int ms_iSimulationLevel;
	static float ms_fAutoImpactInterval;
	static FvUInt32	ms_uiNextMark;
	static FvUInt32	ms_uiLastDrawMark;	

	friend class FvWater;
	friend class FvSimulationCell;
	friend class FvRenderManager;
};

#include "FvWater.inl"

#endif // __FvWater_H__