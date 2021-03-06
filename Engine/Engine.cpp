// Engine.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Engine.h"
#include "System.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	System* system = new System;
	if( !system )
		return 0;

	bool bResult = system->Initialize();
	if( bResult )
		system->Run();

	system->ShutDown();

	delete system;
	system = NULL;

	return 0;

}