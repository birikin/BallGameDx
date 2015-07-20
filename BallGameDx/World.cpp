#include "World.h"
#include "Application.h"

World* World::s_pWorld=NULL;

World& World::GetInstance()
{
	if(s_pWorld==NULL)
	{
		s_pWorld=new World();
	}
	return *s_pWorld;
}

void World::Release()
{
	if(s_pWorld!=NULL)
	{
		delete s_pWorld;
	}
}

World::World()
{
	m_pScreenCoord=new D3DXVECTOR2();
	m_pWorldCoord=new POINT();
}

World::~World()
{
	if(m_pScreenCoord!=NULL)
	{
		delete m_pScreenCoord;
	}

	if(m_pWorldCoord!=NULL)
	{
		delete m_pWorldCoord;
	}
}

void World::Init(LPCWSTR pCellSrcFile,int x,int y, int rows,int columns,UINT cellSize,DWORD color,DWORD selectedColor)
{
	m_OffsetX=x;
	m_OffsetY=y;
	m_Rows=rows;
	m_Columns=columns;
	m_CellSize=cellSize;
	m_Color=color;
	m_SelectedColor=selectedColor;
	m_SelectedCell.x=-1;
	m_SelectedCell.y=-1;

	m_Width=m_CellSize*m_Rows;
	m_Height=m_CellSize*m_Columns;
	
	D3DXCreateTextureFromFileEx(Application::GetInstance().GetDxDevicePtr(), pCellSrcFile, 
		m_CellSize, m_CellSize, 1, //width, height, mipLevel
		0, D3DFMT_UNKNOWN, //usage, format
		D3DPOOL_DEFAULT, D3DX_FILTER_POINT, D3DX_DEFAULT, //pool, filter, mipFilter
		0, NULL, NULL, &m_pCellTex);//ColorKey ,pSrcInfo ,pPalette ,ppTexture 
}


void World::ProcessLogic()
{
	State state=Application::GetInstance().GetState();
	switch(state)
	{
		case S_Idle:
		case S_SelectedBall:
			if(GetKeyState(VK_LBUTTON) & 0x8000)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(Application::GetInstance().GetHWND(), &pt);
				const LPPOINT worldCoord=GetWorldCoordPtr(pt.x,pt.y);
				if(worldCoord!=NULL)
				{
					m_SelectedCell.x=worldCoord->x;
					m_SelectedCell.y=worldCoord->y;
				}
			}
			break;
		case S_BallMoving:
			m_SelectedCell.x=-1;
			m_SelectedCell.y=-1;
			break;
	}
}

void World::Render()
{
	DWORD color;

	for(int x=0; x<m_Rows; x++)
	{
		for(int y=0; y<m_Columns; y++)
		{
			if(x==m_SelectedCell.x && y==m_SelectedCell.y)
			{
				color=m_SelectedColor;
			}
			else
			{
				color=m_Color;
			}
			const LPD3DXVECTOR2 sCoold=GetScreenCoordPtr((float)x,(float)y);
			Application::GetInstance().GetDxSpritePtr()->Draw(
				m_pCellTex,NULL,NULL,&D3DXVECTOR3(sCoold->x,sCoold->y,0),color );
		}
	}
}

const LPD3DXVECTOR2 World::GetScreenCoordPtr(float worldX, float worldY)
{
	m_pScreenCoord->x = worldX*m_CellSize+m_OffsetX;
	m_pScreenCoord->y = worldY*m_CellSize+m_OffsetY;
	return m_pScreenCoord;
}

const LPPOINT World::GetWorldCoordPtr(int screenX, int screenY)
{
	if(screenX<m_OffsetX && screenX>(m_OffsetX+m_Width)-1 && screenY<m_OffsetY && screenY>(m_OffsetY+m_Height)-1)
	{
		return NULL;
	}
	m_pWorldCoord->x = (screenX-m_OffsetX)/m_CellSize;
	m_pWorldCoord->y = (screenY-m_OffsetY)/m_CellSize; 
	return m_pWorldCoord;
}




