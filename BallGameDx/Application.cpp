#include "Application.h"
#include <sstream>

Application* Application::s_pApplication=NULL;

Application& Application::GetInstance()
{
	if(s_pApplication==NULL)
	{
		s_pApplication=new Application();
	}
	return *s_pApplication;
}

void Application::Release()
{
	if(s_pApplication!=NULL)
	{
		delete s_pApplication;
	}
}

Application::Application()
{
}

Application::~Application()
{
	World::Release();

	if( m_pDxSprite != NULL )
	{
        m_pDxSprite->Release();
	}

    if( m_pDxDevice != NULL )
	{
        m_pDxDevice->Release();
	}

    if( m_pDx != NULL )
	{
        m_pDx->Release();
	}
}

int Application::Run(LPCWSTR title,UINT screenWidth,UINT screenHeight,int nCmdShow)
{
	m_ScreenWidth=screenWidth;
	m_ScreenHeight=screenHeight;

	LPCWSTR className=L"BallGameDx";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_CLASSDC | CS_DBLCLKS;
	wcex.lpfnWndProc = MsgProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle( NULL );
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = NULL;

    RegisterClassEx( &wcex );

	int windowWidth=m_ScreenWidth + GetSystemMetrics(SM_CXFIXEDFRAME)*2;
	int windowHeight=m_ScreenHeight + GetSystemMetrics(SM_CYFIXEDFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

    // Create the application's window
    HWND hWnd = CreateWindow( L"BallGameDx", title,
                              WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
                              NULL, NULL, wcex.hInstance, NULL );
	m_hWnd=hWnd;

	if(SUCCEEDED(InitAll()))
	{
		ShowWindow( hWnd, nCmdShow );
        UpdateWindow( hWnd );

        // Enter the message loop
		MSG  msg;
		msg.message = WM_NULL;
		PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );
	    
		while( WM_QUIT != msg.message  )
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				float elapsedTime=GlobalTimer::GetInstance().GetElapsedTime();
				ProcessLogic(elapsedTime);
				Render(elapsedTime);
			}
		}
	}
	
	UnregisterClass( className, wcex.hInstance );
    return 0;
}



HRESULT Application::InitAll()
{
	m_pDx = Direct3DCreate9( D3D_SDK_VERSION );
    if( NULL == m_pDx )
        return E_FAIL;
    
    D3DPRESENT_PARAMETERS dxpp;
    ZeroMemory( &dxpp, sizeof( dxpp ) );
    dxpp.Windowed = TRUE;
    dxpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    dxpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	dxpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	dxpp.BackBufferCount=1;
	dxpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    
    if( FAILED( m_pDx->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &dxpp, &m_pDxDevice ) ) )
    {
        return E_FAIL;
    }

	//D3DXCreateFont(m_pDxDevice, 50, 0, 0, 1, FALSE, DEFAULT_CHARSET, 0, 0, 0, L"Î¢ÈíÑÅºÚ", &m_pDxFont);

    D3DXCreateSprite(m_pDxDevice,&m_pDxSprite);

	SetState(S_Idle);
	
	World::GetInstance().Init(L"WorldCell.png",5,5,15,11,50,0xFF6E6E6E,0xFFFF0000);

	if(!BallManager::GetInstance().Init(L"Ball.png",16))
	{
		return E_FAIL;
	}

    return S_OK;
}

void Application::ProcessLogic(float elapsedTime)
{
	World::GetInstance().ProcessLogic();
	BallManager::GetInstance().ProcessLogic(elapsedTime);
}

void Application::Render(float elapsedTime)
{
	m_pDxDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );
    
    m_pDxDevice->BeginScene();
    
    m_pDxSprite->Begin(D3DXSPRITE_ALPHABLEND);

	World::GetInstance().Render();
	BallManager::GetInstance().Render();

	/*static int frame=0;
	static float temp=0.0f;
	static float fps=0.0f;
	temp+=elapsedTime;
	frame++;
	
	if(temp>1)
	{
		fps=(float)frame/temp;
		frame=0;
		temp=0;
	}
	
	std::wostringstream oss;
	oss<<fps;
	RECT rc;
	rc.left=50; rc.top=50; rc.right=300; rc.bottom=300;
    m_pDxFont->DrawText(m_pDxSprite, oss.str().c_str(), -1, &rc, DT_NOCLIP, 0xFFFF0000);*/

	m_pDxSprite->End();
    
    m_pDxDevice->EndScene();
	
	/*LPDIRECT3DSURFACE9 backBuffer= NULL;
	m_pDxDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	HDC hdc;
	HRESULT hr=backBuffer->GetDC(&hdc);
	
	SetBkMode(hdc,TRANSPARENT); 
	TextOut(hdc,500,500,L"helloworld",10);
	backBuffer->ReleaseDC(hdc);
	backBuffer->Release();*/
    
    m_pDxDevice->Present( NULL, NULL, NULL, NULL );
}



LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;

        /*case WM_PAINT:
            Render();
            ValidateRect( hWnd, NULL );
            return 0;*/
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}
