////#include "SplashScreen.h"
//
#include "FvZoneTest.h"
#include <FvCommand.h>
#include "FvFlora.h"
void ToPos(char* ch)
{
	int i=0;
	while (ch[i]!='\0'&& i<127)
	{
		if (ch[i]=='/')
		{
			ch[i]=' ';
		}
		if (ch[i]=='+')
		{
			ch[i]='-';
		}
		i++;
	}
}
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hOtherInst, LPSTR pcCmdLine, INT iMode)
{
	FvCommand kCommand(pcCmdLine);
	char acT[128] = {0};
	char Pos[128] = {0};
	float Time = 10.0f;
	FvVector3 KPos(0.0f,0.0f,30.0f);
	kCommand.String("WorkMap",acT,128);
	kCommand.String("XYZ",Pos,128);
	kCommand.Float("Time",Time);
	ToPos(Pos);
	sscanf( Pos, "%f%f%f", &KPos.x, &KPos.y, &KPos.z );
	
	FILE* pkFile = fopen("GraphicTest.xml","rb");
	if(pkFile)
	{
		Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
		FvXMLSectionPtr spAvatarSec = FvXMLSection::OpenSection(spDataStream);
		
		
		TestApplication::SetInstanceReference(hInst);
		TestApplication::SetAcceleratorReference(hOtherInst);
		TestApplication::SetCommandLine(pcCmdLine);
		FvGameTimer::Create(1);

		TestApplication *pkApp = new TestApplication;
		FvString PathConfigFile = spAvatarSec->GetAttributeString(FvString("ConfigFile"),FvString("Path"));
		FvString PathData       = spAvatarSec->GetAttributeString(FvString("DataPath"),FvString("Path"));
		FvString PathCursorFile = spAvatarSec->GetAttributeString(FvString("CursorFile"),FvString("Path"));
		FvString PathAnimShell  = spAvatarSec->GetAttributeString(FvString("AnimShell"),FvString("Path"));
		FvString PathAvatarRes  = spAvatarSec->GetAttributeString(FvString("AvatarRes"),FvString("Path"));
		pkApp->m_GameUnit.m_spd = spAvatarSec->GetAttributeFloat(FvString("Spd"),FvString("Value"),10.0f);
		if(pkApp->Initialize(PathConfigFile.c_str(),PathData.c_str(),PathCursorFile.c_str()))//初始化
		{
			pkApp->MouseSpd=10.0f;
			pkApp->kPos=Vector3(KPos.x,KPos.y,KPos.z);
			pkApp->CreateSpace(PathAnimShell.c_str(),acT);//创建场景，WorkMap表示场景相对路径
			pkApp->RefreshPlayerAvatar(1,1,1,1,1);//创建人物 参数是人物一些脸型或者头发的ID
			pkApp->m_GameUnit.SetAvatar(PathAvatarRes.c_str(),PathAnimShell.c_str());//人物动作
			pkApp->m_GameUnit.Initialize();//进行物理等一些初始化工作
			pkApp->m_GameUnit.SetPos(KPos);
			pkApp->CreateController();//创建控制器
			pkApp->m_Time = Time;
			pkApp->FreeCamera=false;
			FvGraphicsCommon::SetClock(Time);//设置时间
			FvFlora::Enabled(false);
			pkApp->pkAttachment->SetPosition(KPos);
			pkApp->MainLoop();
		}

		pkApp->Terminate();
		delete pkApp;
	}
	
}
