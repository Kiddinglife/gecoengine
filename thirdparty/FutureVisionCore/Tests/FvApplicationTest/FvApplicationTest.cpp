#include <FvApplication.h>

#include <windows.h>

#include "vld.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hOtherInst, LPSTR pcCmdLine, INT iMode)
{
	FvApplication::SetInstanceReference(hInst);
	FvApplication::SetAcceleratorReference(hOtherInst);
	FvApplication::SetCommandLine(pcCmdLine);

	FvApplication *pkApp = new FvApplication;
	if(pkApp->Initialize("Config.fvc","F:/SaintsPro/Binaries/Data"))
	{
		pkApp->MainLoop();
	}

	pkApp->Terminate();
	delete pkApp;
}