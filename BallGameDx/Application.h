#pragma once
#include <windows.h>
#include <d3d9.h>
#include <D3dx9core.h>

#include "GlobalTimer.h"
#include "World.h"
#include "BallManager.h"

enum State
{
	S_Idle,
	S_SelectedBall,
	S_BallMoving,
	S_AfterAlloc,
	S_GameOver
};

class Application
{
public:
	static Application& GetInstance();
	static void Release();
	
	int Run(LPCWSTR title,UINT screenWidth,UINT screenHeight,int nCmdShow);
	HWND GetHWND(){return m_hWnd;}
	int GetScreenWidth(){return m_ScreenWidth;}
	int GetScreenHeight(){return m_ScreenHeight;}
	LPDIRECT3DDEVICE9 GetDxDevicePtr(){return m_pDxDevice;}
	LPD3DXSPRITE GetDxSpritePtr(){return m_pDxSprite;}
	State GetState(){return m_eState;}
	void SetState(State state){m_eState=state;}
	
private:
	Application();
	~Application();

	HRESULT InitAll();
	void ProcessLogic(float elapsedTime);
	void Render(float elapsedTime);
	

private:
	static Application* s_pApplication;
	int m_ScreenWidth;
	int m_ScreenHeight;
	HWND m_hWnd;
	LPD3DXSPRITE m_pDxSprite;
	LPDIRECT3DDEVICE9 m_pDxDevice;
	LPDIRECT3D9 m_pDx;
	//LPD3DXFONT m_pDxFont;
	State m_eState;
};

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
