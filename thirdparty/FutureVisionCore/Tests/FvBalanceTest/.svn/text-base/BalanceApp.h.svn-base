#include "App.h"
#include <set>
#include <vector>

struct Cell;
struct Space;
struct Entity
{
	FvUInt32 m_uiID;
	FvVector2 m_kAcceleration;
	FvVector2 m_kSpeed;
	FvVector2 m_kPos;
	FvVector2 m_kTarget;
	FvUInt32 m_uiPower;
	Cell *m_pkCell;
	Space *m_pkSpace;
	float m_fVision;
	FvUInt32 m_uiFocusID;
	bool m_bFocus;
	bool m_bFocusDirty;
	Entity(Space *pkSpace, bool bFocus = false);
	~Entity();
	void Draw();
	void Tick(float fTime);
	void SelectFocus();
	void SetTarget(const FvVector2& kTarget);
	
	static Entity* CreateEntity(Space* pkSpace, const FvVector2& kPos, 
		const FvVector2& kSpeed, const FvVector2& kAcceleration);
	static void DirtyFocus();
	static void ResizeNumber(Space *pkSpace, FvUInt32 uiNum);
	static void EntitysDraw();
	static void EntitysTick(float fTime);
	static std::vector<Entity*> ms_kEntitys;
};

struct Cell
{
	FvUInt32 m_uiID;
	Space *m_pkSpace;
	FvVector4 m_kRect;
	std::set<Entity*> m_kEntitys;
	Cell(Space *pkSpace, FvUInt32 uiID, const FvVector4& kRect);
	void AddEntity(Entity* pkEntity)
	{
		m_kEntitys.insert(pkEntity);
	}
	void RemoveEntity(Entity* pkEntity)
	{
		m_kEntitys.erase(pkEntity);
	}
	bool IsInCell(const FvVector2& kPos);
	void Draw();
};

struct Space
{
	Space(const FvVector4& kRect);
	Cell* CreateCell(FvVector4 kRect);
	bool IsInSpace(const FvVector2& kPos);
	FvVector2 ClipPos(const FvVector2& kPos);
	Cell* GetCellFromPosition(const FvVector2& kPos);
	void Draw();
	FvVector4 m_kRect;
	std::set<Cell*> m_kCells;
};

struct FocusPoint
{
	FvUInt32 m_uiID;
	FvVector2 m_kPos;
	float m_fTime;
	float m_fRadius;
	float m_fDrawRadius;
	FocusPoint();
	bool Tick(float fTime);
	void Draw();

	static bool HasFocusID(FvUInt32 uiID);
	static FocusPoint *GetRandomFocus();
	static FocusPoint *GetFocus(FvUInt32 uiID);
	static FocusPoint *GetFocus(const FvVector2& kPos);
	static FocusPoint *CreateFocus(const FvVector2& kPos, float fRadius, float fTime);
	static void FocusTick(float fTime);
	static void FocusDraw();
	static std::vector<FocusPoint*> ms_kFocus;
	static FvUInt32 ms_uiIdentify;
};

struct Command;

class BalanceApp : public App
{
public:
	BalanceApp();
	virtual~BalanceApp();

	virtual	bool	Init(int argc, char* argv[]);
	virtual	void	Tick(double detaTime);
	virtual	void	Draw();

	virtual void	OnMouseUp(int button, FvVector2& pos);
	virtual	void	OnMouseWheel(int button, int direction, const FvVector2& pos);
	virtual	void	OnCmd(const char* pcCmd);

	bool HandleSetEntityNumberCommand(const char *pcArgs);
	
protected:

	bool ExecuteCommandInTable(Command *pkTable, const char* pcText, const std::string& kFullCmd);

	Space *m_pkSpace;
};


App* CreateApp();
void DestroyApp();