#pragma once
#include "Application.h"

class World
{
public:
	static World& GetInstance();
	static void Release();

	void Init(LPCWSTR pCellSrcFile,int x,int y, int rows,int columns,UINT cellSize,DWORD color,DWORD selectedColor);
	void ProcessLogic();
	void Render();
	const LPD3DXVECTOR2 GetScreenCoordPtr(float worldX,float worldY);
	const LPPOINT GetWorldCoordPtr(int screenX,int screenY);
	int GetRows(){return m_Rows;}
	int GetColumns(){return m_Columns;}
	UINT GetCellSize(){return m_CellSize;}

private:
	World();
	~World();

private:
	static World* s_pWorld;

	LPDIRECT3DTEXTURE9	m_pCellTex;
	int m_OffsetX;
	int m_OffsetY;
	int m_Width;
	int m_Height;
	int m_Rows;
	int m_Columns;
	UINT m_CellSize;
	DWORD m_Color;
	DWORD m_SelectedColor;
	POINT m_SelectedCell;
	LPD3DXVECTOR2 m_pScreenCoord;
	LPPOINT m_pWorldCoord;
};
