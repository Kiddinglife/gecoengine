#include "BalanceApp.h"
#include "stdio.h"
#include "Draw2D.h"
#include <FvTimestamp.h>

#include "demo.h"

#define FONT_PIXEL_SIZE 12

extern void ConvertWorldToScreen(FvVector2& kPoint, int& iX, int& iY);

float UnitRandomf()
{
	return ((float)rand()) * (1.0f / ((float)RAND_MAX));
}

float Clip(float Min,float Max,float Cur)
{
	float Mod = Max - Min;

	if(Cur >= Min && Cur <= Max)
		return Cur;

	if(Cur < Min)
		return Max - fabs(fmod(fabs(Cur - Min),Mod));

	if(Cur > Max)
		return Min + fabs(fmod(fabs(Cur - Min),Mod));

	assert(false);
	return 0.f;
}

bool HasStringAbbr(const char* pcName, const char* pcPart)
{
	if( *pcName )
	{
		if( !*pcPart )
			return false;

		for(;;)
		{
			if( !*pcPart )
				return true;
			else if( !*pcName )
				return false;
			else if( tolower( *pcName ) != tolower( *pcPart ) )
				return false;
			++pcName; ++pcPart;
		}
	}

	return true;
}

FvVector2 RandomInSpace(Space *pkSpace)
{
	float fGridMinX, fGridMinY, fGridMaxX, fGridMaxY;
	fGridMinX = pkSpace->m_kRect.x;
	fGridMaxX = pkSpace->m_kRect.y;
	fGridMinY = pkSpace->m_kRect.z;
	fGridMaxY = pkSpace->m_kRect.w;
	return FvVector2((rand() * (fGridMaxX - fGridMinX) / (RAND_MAX+1) + fGridMinX),
		(rand() * (fGridMaxY - fGridMinY) / (RAND_MAX+1) + fGridMinY));
}

FvVector2 RandomCircle(const FvVector2& kPos, float fRadius)
{
	float fT = ((float)rand() / (float)RAND_MAX) * FV_MATH_2PI_F;
	float fR = ((float)rand() / (float)RAND_MAX) * fRadius;
	return kPos + FvVector2(fR * cosf(fT),fR * sinf(fT));
};

std::vector<Entity*> Entity::ms_kEntitys;

Entity::Entity(Space *pkSpace, bool bFocus) :
m_uiID(0),
m_uiPower(0),
m_pkCell(NULL),
m_pkSpace(pkSpace),
m_fVision(0.f),
m_uiFocusID(0),
m_bFocus(bFocus),
m_bFocusDirty(false)
{
	
}

Entity::~Entity()
{
	if(m_pkCell)
		m_pkCell->RemoveEntity(this);
}

void Entity::Draw()
{
	DrawPoint(m_kPos, CLR_RED);
	if(m_fVision > 0.f)
		DrawCircle(m_kPos, 80);
	//int iX, iY;
	//ConvertWorldToScreen(m_kPos, iX, iY);
	//DrawString(iX,iY,CLR_BLUE,"Entity ID:%d",m_uiID);
}

void Entity::Tick(float fTime)
{
	if(m_bFocus)
	SelectFocus();

	if((m_kPos - m_kTarget).Length() < m_kSpeed.Length())
	{
		m_kSpeed = FvVector2::ZERO;
		m_kAcceleration = FvVector2::ZERO;
	}
	else
	{
		m_kPos += fTime*(m_kSpeed + m_kAcceleration*fTime);
		m_kPos = m_pkSpace->ClipPos(m_kPos);
		Cell* pkCell = m_pkSpace->GetCellFromPosition(m_kPos);
		if(pkCell && m_pkCell != pkCell)
		{
			if(m_pkCell)
				m_pkCell->RemoveEntity(this);
			pkCell->AddEntity(this);
			m_pkCell = pkCell;
		}
	}
}

void Entity::SelectFocus()
{
	if(m_bFocusDirty)
	{
		m_uiFocusID = 0;
		SetTarget(RandomInSpace(m_pkSpace));
		m_bFocusDirty = false;
	}

	if(m_uiFocusID != 0)
	{
		FocusPoint *pkFocus = FocusPoint::GetFocus(m_uiFocusID);
		if(!pkFocus)
		{
			m_uiFocusID = 0;
			SetTarget(RandomInSpace(m_pkSpace));
		}
		else
		{
			if(FvAlmostEqual(m_kSpeed,FvVector2::ZERO))
				SetTarget(RandomCircle(pkFocus->m_kPos,pkFocus->m_fRadius));
			return;
		}
	}

	FocusPoint *pkFocus = FocusPoint::GetRandomFocus();
	if(pkFocus)
	{
		m_uiFocusID = pkFocus->m_uiID;
		SetTarget(RandomCircle(pkFocus->m_kPos,pkFocus->m_fRadius));
	}

	if(m_uiFocusID == 0 &&
		FvAlmostEqual(m_kSpeed,FvVector2::ZERO))
		SetTarget(RandomInSpace(m_pkSpace));
}

void Entity::SetTarget(const FvVector2& kTarget)
{
	m_kTarget = kTarget;
	m_kSpeed = m_kTarget - m_kPos;
	m_kSpeed.Normalise();
	float fLength = (m_kTarget - m_kPos).Length() * 0.01f;
	m_kSpeed *= (10.f + 20.f * UnitRandomf()) * fLength;
}

Entity* Entity::CreateEntity(Space* pkSpace, const FvVector2& kPos, 
					 const FvVector2& kSpeed, const FvVector2& kAcceleration)
{
	Entity* pkEntity = new Entity(pkSpace,UnitRandomf() > 0.5f);
	pkEntity->m_uiID = ms_kEntitys.size();
	pkEntity->m_kPos = kPos;
	pkEntity->m_kSpeed = kSpeed;
	pkEntity->m_kAcceleration = kAcceleration;
	ms_kEntitys.push_back(pkEntity);
	return pkEntity;
}

void Entity::DirtyFocus()
{
	for(std::vector<Entity*>::iterator kIt = ms_kEntitys.begin();
		kIt != ms_kEntitys.end(); ++kIt)
		(*kIt)->m_bFocusDirty = true;
}

void Entity::ResizeNumber(Space *pkSpace, FvUInt32 uiNum)
{
	if(ms_kEntitys.size() == uiNum)
		return;

	if(ms_kEntitys.size() < uiNum)
	{
		int iInsertNum = int(uiNum - ms_kEntitys.size());
		for(int i = 0; i < iInsertNum; i++)
		{
			float fSpeedX = ((rand() +1)%137-68) * 50.f / 68;
			float fSpeedY = ((rand() +1)%137-68) * 50.f / 68;
			CreateEntity(pkSpace,RandomInSpace(pkSpace),FvVector2(fSpeedX,fSpeedY),FvVector2(0.f,0.f));
		}
	}
	else
	{
		std::vector<Entity*>::iterator kIt = ms_kEntitys.begin();
		int iEraseNum = int(ms_kEntitys.size() - uiNum);
		for(int i = 0; i < iEraseNum; i++)
		{
			delete (*kIt);
			ms_kEntitys.erase(kIt);
		}
	}
}

void Entity::EntitysDraw()
{
	for(std::vector<Entity*>::iterator kIt = ms_kEntitys.begin();
		kIt != ms_kEntitys.end(); ++kIt)
	{
		(*kIt)->Draw();
	}
}

void Entity::EntitysTick(float fTime)
{
	for(std::vector<Entity*>::iterator kIt = ms_kEntitys.begin();
		kIt != ms_kEntitys.end(); ++kIt)
	{
		(*kIt)->Tick(fTime);
	}
}

Cell::Cell(Space *pkSpace, FvUInt32 uiID, const FvVector4& kRect) :
m_uiID(uiID),
m_pkSpace(pkSpace),
m_kRect(kRect)
{
}

bool Cell::IsInCell(const FvVector2& kPos)
{
	return (kPos.x >= m_kRect.x && 
		kPos.x < m_kRect.y &&
		kPos.y >= m_kRect.z &&
		kPos.y < m_kRect.w);
}

void Cell::Draw()
{
	DrawRect(m_kRect, CLR_YELLOW);

	int iX, iY;
	ConvertWorldToScreen(
		FvVector2((m_kRect.x + m_kRect.y)/2.f, (m_kRect.z + m_kRect.w)/2.f), iX, iY);
	DrawString(iX,iY,CLR_WHITE,"Cell ID:%d",m_uiID);
	DrawString(0,(FONT_PIXEL_SIZE*(m_uiID + 1)),CLR_WHITE,"Cell [%d] Number:%d",m_uiID,m_kEntitys.size());
}

Space::Space(const FvVector4& kRect) :
m_kRect(kRect)
{

}

Cell* Space::CreateCell(FvVector4 kRect)
{
	Cell *pkCell = new Cell(this,m_kCells.size(),kRect);
	m_kCells.insert(pkCell);
	return pkCell;
}

bool Space::IsInSpace(const FvVector2& kPos)
{
	return kPos.x >= m_kRect.x && kPos.x < m_kRect.y
		&& kPos.y >= m_kRect.z && kPos.y < m_kRect.w;
}

FvVector2 Space::ClipPos(const FvVector2& kPos)
{
	return FvVector2(Clip(m_kRect.x,m_kRect.y,kPos.x),
		Clip(m_kRect.z,m_kRect.w,kPos.y));
}

Cell* Space::GetCellFromPosition(const FvVector2& kPos)
{
	for(std::set<Cell*>::iterator kIt = m_kCells.begin(); 
		kIt != m_kCells.end(); ++kIt)
	{
		if(!(*kIt)->IsInCell(kPos)) continue;
		return *kIt;
	}

	return NULL;
}

void Space::Draw()
{
	DrawRect(m_kRect, CLR_GREEN);
	for(std::set<Cell*>::iterator kIt = m_kCells.begin(); 
		kIt != m_kCells.end(); ++kIt)
		(*kIt)->Draw();
}

FvUInt32 FocusPoint::ms_uiIdentify = 0;
std::vector<FocusPoint*> FocusPoint::ms_kFocus;

FocusPoint::FocusPoint() :
m_fTime(5),
m_fRadius(0.f),
m_fDrawRadius(0.f)
{
	
}

bool FocusPoint::Tick(float fTime)
{
	if(m_fTime < 0.f)
		return false;

	m_fDrawRadius += fTime * m_fRadius;
	if(m_fDrawRadius >= m_fRadius)
		m_fDrawRadius = 0.f;

	m_fTime -= fTime;
	return true;
}

void FocusPoint::Draw()
{
	DrawCircle(m_kPos, m_fDrawRadius, CLR_YELLOW);
	DrawCircle(m_kPos, m_fRadius, CLR_YELLOW);
	int iX, iY;
	ConvertWorldToScreen(m_kPos, iX, iY);
	DrawString(iX,iY,CLR_WHITE,"ID:%d",m_uiID);
	DrawString(iX,iY + FONT_PIXEL_SIZE,CLR_WHITE,"Time:%d",int(m_fTime));
	DrawString(iX,iY + FONT_PIXEL_SIZE * 2,CLR_WHITE,"Radius:%d",int(m_fRadius));
}

bool FocusPoint::HasFocusID(FvUInt32 uiID)
{
	for(std::vector<FocusPoint*>::iterator kIt = ms_kFocus.begin();
		kIt != ms_kFocus.end(); ++kIt)
		if((*kIt)->m_uiID == uiID)
			return true;

	return false;
}

FocusPoint *FocusPoint::GetRandomFocus()
{
	if(ms_kFocus.size() == 0)
		return NULL;

	float fRoll = UnitRandomf();
	float fSum = 0.f;
	float fBegin = 0.f;
	for(int i = 0; i < int(ms_kFocus.size()); i++)
		fSum += ms_kFocus[i]->m_fRadius;	

	fRoll *= fSum;

	for(int i = 0; i < int(ms_kFocus.size()); i++)
	{
		FocusPoint *pkFocus = ms_kFocus[i];
		if(fRoll >= fBegin && fRoll < (fBegin + pkFocus->m_fRadius))
			return pkFocus;

		fBegin += pkFocus->m_fRadius;
	}

	return NULL;
}

FocusPoint *FocusPoint::GetFocus(FvUInt32 uiID)
{
	for(std::vector<FocusPoint*>::iterator kIt = ms_kFocus.begin();
		kIt != ms_kFocus.end(); ++kIt)
	{
		if((*kIt)->m_uiID == uiID)
			return *kIt;
	}

	return NULL;
}

FocusPoint *FocusPoint::GetFocus(const FvVector2& kPos)
{
	for(std::vector<FocusPoint*>::iterator kIt = ms_kFocus.begin();
		kIt != ms_kFocus.end(); ++kIt)
	{
		if(((*kIt)->m_kPos - kPos).Length() < (*kIt)->m_fRadius)
			return (*kIt);
	}

	return NULL;
}

FocusPoint *FocusPoint::CreateFocus(const FvVector2& kPos, float fRadius, float fTime)
{
	FocusPoint *pkFocus = new FocusPoint;
	pkFocus->m_uiID = ++ms_uiIdentify;
	pkFocus->m_kPos = kPos;
	pkFocus->m_fRadius = fRadius;
	pkFocus->m_fTime = fTime;
	ms_kFocus.push_back(pkFocus);
	Entity::DirtyFocus();
	return pkFocus;
}

void FocusPoint::FocusTick(float fTime)
{
	for(std::vector<FocusPoint*>::iterator kIt = ms_kFocus.begin();
		kIt != ms_kFocus.end();)
	{
		if(!(*kIt)->Tick(fTime))
		{
			delete (*kIt);
			ms_kFocus.erase(kIt);
			Entity::DirtyFocus();
		}
		else
			++kIt;
	}
}

void FocusPoint::FocusDraw()
{
	for(std::vector<FocusPoint*>::iterator kIt = ms_kFocus.begin();
		kIt != ms_kFocus.end(); ++kIt)
		(*kIt)->Draw();
}

struct Command
{
public:
	const char *m_pcName;
	bool (BalanceApp::*m_pfHandler)(const char* args);
	std::string m_kHelp;
	Command *m_pkChildCommands;
};

Command* GetCommandTable()
{
    static Command s_kCommandTable[] =
    {
        { "SetEntityNumber", &BalanceApp::HandleSetEntityNumberCommand,     "", NULL },
        { NULL,              NULL,                                          "", NULL }
    };

	return s_kCommandTable;
}

BalanceApp* g_pkApp(NULL);

BalanceApp::BalanceApp() :
m_pkSpace(NULL)
{

}

BalanceApp::~BalanceApp()
{

}

bool BalanceApp::Init(int argc, char* argv[])
{
	SetWindowSize(800, 600);

	float fWidth = 7000.0f;
	m_pkSpace = new Space(FvVector4(-fWidth,fWidth,-fWidth,fWidth));
	m_pkSpace->CreateCell(FvVector4(-fWidth,0.f,-fWidth,0.f));
	m_pkSpace->CreateCell(FvVector4(0.f,fWidth,-fWidth,0.f));
	m_pkSpace->CreateCell(FvVector4(-fWidth,0.f,0.f,fWidth));
	m_pkSpace->CreateCell(FvVector4(0.f,fWidth,0.f,fWidth));

	for(int i = 0; i < 10000; i++)
	{
		float fSpeedX = ((rand() +1)%137-68) * 50.f / 68;
		float fSpeedY = ((rand() +1)%137-68) * 50.f / 68;
		Entity::CreateEntity(m_pkSpace,RandomInSpace(m_pkSpace),FvVector2(fSpeedX,fSpeedY),FvVector2(0.f,0.f));
	}

	return true;
}

void BalanceApp::Tick(double detaTime)
{
	Entity::EntitysTick((float)detaTime);
	FocusPoint::FocusTick((float) detaTime);
}

void BalanceApp::Draw()
{
	Entity::EntitysDraw();
	if(m_pkSpace)
		m_pkSpace->Draw();
	FocusPoint::FocusDraw();
}

void BalanceApp::OnMouseUp(int button, FvVector2& pos)
{
	FocusPoint *pkFocus = FocusPoint::GetFocus(pos);
	if(!pkFocus && m_pkSpace && 
		m_pkSpace->IsInSpace(pos)
		&& button == GLUT_LEFT_BUTTON)
		FocusPoint::CreateFocus(pos,500,100);
}

void BalanceApp::OnMouseWheel(int button, int direction, const FvVector2& pos)
{
	FocusPoint *pkFocus = FocusPoint::GetFocus(pos);
	if(pkFocus)
	{
		if(button == GLUT_LEFT_BUTTON)
			pkFocus->m_fRadius += float(direction) * 10.f;
		else if(button == GLUT_RIGHT_BUTTON)
			pkFocus->m_fTime += float(direction) * 10.f;

		Entity::DirtyFocus();
	}
}

void BalanceApp::OnCmd(const char* pcCmd)
{
	if(pcCmd == NULL)
		return;

	if (pcCmd[0] == '!' || pcCmd[0] == '.')
		++pcCmd;
	
	std::string kFullCmd = pcCmd;

	ExecuteCommandInTable(GetCommandTable(),pcCmd,kFullCmd);
}

bool BalanceApp::ExecuteCommandInTable(Command *pkTable, const char* pcText, const std::string& kFullCmd)
{
	char const* pcOldText = pcText;
	std::string kCmd = "";

	while (*pcText != ' ' && *pcText != '\0')
	{
		kCmd += *pcText;
		++pcText;
	}

	while (*pcText == ' ') ++pcText;

    for(FvUInt32 i = 0; pkTable[i].m_pcName != NULL; ++i)
    {
        if( !HasStringAbbr(pkTable[i].m_pcName, kCmd.c_str()) )
            continue;

		if(pkTable[i].m_pkChildCommands != NULL)
		{
			if(!ExecuteCommandInTable(pkTable[i].m_pkChildCommands, pcText, kFullCmd))
			{
				
			}

			return true;
		}

        if(!pkTable[i].m_pfHandler)
            continue;

		if(!(this->*(pkTable[i].m_pfHandler))(strlen(pkTable[i].m_pcName)!=0 ? pcText : pcOldText))
		{
			
		}
	}

	return false;
}

bool BalanceApp::HandleSetEntityNumberCommand(const char *pcArgs)
{
    if (!*pcArgs)
        return false;

    char* pcNumber = strtok ((char*)pcArgs, " ");
    if (!pcNumber)
        return false;

    //std::string nameOrIP = cnameOrIP;

    //char* duration = strtok (NULL," ");
    //if(!duration || !atoi(duration))
    //    return false;

    //char* reason = strtok (NULL,"");
    //if(!reason)
    //    return false;
	Entity::ResizeNumber(m_pkSpace,atoi(pcNumber));

	return true;
}

App* CreateApp()
{
	if(!g_pkApp)
		g_pkApp = new BalanceApp();
	return g_pkApp;
}

void DestroyApp()
{
	if(g_pkApp)
	{
		delete g_pkApp;
		g_pkApp = NULL;
	}
}
