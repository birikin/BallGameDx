#pragma once
#include "Application.h"
#include <vector>
//#include <stack>
#include <algorithm>
using namespace std;

#define CL_BROWN  0xFF753726
#define CL_BLUE   0xFF0000FF
#define CL_GREEN  0xFF00FF00
#define CL_PURPLE 0xFFB23AEE
#define CL_ORANGE 0xFFEE7621
#define CL_RED    0xFFFF0000

class Node
{
public:
	friend class BallManager;

private:
	Node(int _x,int _y,int _g,int _h,Node* _parent):x(_x),y(_y),g(_g),h(_h),f(_g+_h),parent(_parent){}

private:
	class HeapComp_f 
	{
		public:
			bool operator() ( const Node *rhs, const Node *lhs ) const
			{
				return rhs->f > lhs->f;
			}
	};

	class VectorComp_xy
	{
		public:
			VectorComp_xy(int _x,int _y):mx(_x),my(_y){}
			bool operator()(const Node* node)
			{
				return (node->x==mx && node->y==my);
			}
		private:
			int mx,my;
	};

private:
	int x;
	int y;
	int g;
	int h;
	int f;
	Node* parent;
};

class Ball
{
public:
	friend class BallManager;

private:
	Ball(int _x,int _y,DWORD _color):x((float)_x),y((float)_y),/*lastx((float)_x),lasty((float)_y),*/color(_color){}
	BOOL move(int dx,int dy,float howMuchSecond,float elapsedTime)
	{
		float deltax=(float)dx-x<0?-0.9999f:0.9999f;
		float deltay=(float)dy-y<0?-0.9999f:0.9999f;
		float newx=x+((int)(dx-x+deltax))*(elapsedTime/howMuchSecond);
		float newy=y+((int)(dy-y+deltay))*(elapsedTime/howMuchSecond);
		if(((float)dx-newx)*((float)dx-x)<0 || ((float)dy-newy)*((float)dy-y)<0)
		{
			x=(float)(int)(x+0.5);
			y=(float)(int)(y+0.5);
			/*lastx=x;
			lasty=y;*/
			return TRUE;
		}
		x=newx;
		y=newy;
		return FALSE;
	}
private:
	//float lastx,lasty;
	float x;
	float y;
	DWORD color;
};

class BallManager
{
public:
	static BallManager& GetInstance();
	static void Release();

	BOOL Init(LPCWSTR pBallSrcFile,int oriBalls);
	void ProcessLogic(float elapsedTime);
	void Render();

private:
	BallManager();
	~BallManager();

	BOOL RandomAllocateBalls(UINT nBalls);
	BOOL CancelBalls(int x,int y,int nBalls);
	DWORD GetRandomColor();
	BOOL AstarSearch(int sx,int sy,int dx,int dy);

private:
	static BallManager* s_pBallManager;

	LPDIRECT3DTEXTURE9	m_pBallTex;
	int  m_Rows;
	int  m_Columns;
	POINT  m_SelectedBall;
	Ball** m_pBallPtrArray;
	vector<POINT> m_NewBallsPos;
	vector<POINT> m_PathPos;
	Ball*  m_MovingBall;
};

