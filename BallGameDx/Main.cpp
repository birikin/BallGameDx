#include "Application.h"

int WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow )
{
	int result = Application::GetInstance().Run(L"Ball Game",800,600,nCmdShow);
	Application::Release();
	return result;
}