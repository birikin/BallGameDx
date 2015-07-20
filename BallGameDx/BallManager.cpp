#include "BallManager.h"
#include <time.h>

BallManager* BallManager::s_pBallManager=NULL;

BallManager& BallManager::GetInstance()
{
	if(s_pBallManager==NULL)
	{
		s_pBallManager=new BallManager();
	}
	return *s_pBallManager;
}

void BallManager::Release()
{
	if(s_pBallManager!=NULL)
	{
		delete s_pBallManager;
	}
}

BallManager::BallManager()
{
}

BallManager::~BallManager()
{
}

BOOL BallManager::Init(LPCWSTR pBallSrcFile,int oriBalls)
{
	m_Rows=World::GetInstance().GetRows();
	m_Columns=World::GetInstance().GetColumns();
	int maxBalls=m_Rows * m_Columns;

	if(oriBalls<=0 || oriBalls>=maxBalls)
	{
		return FALSE;
	}
	m_MovingBall=NULL;
	m_pBallPtrArray=new Ball*[maxBalls];
	BOOL needRealloc=FALSE;
	do
	{
		for(int i=0;i<maxBalls;i++)
		{
			if(needRealloc && m_pBallPtrArray[i]!=NULL)
			{
				delete m_pBallPtrArray[i];
			}
			m_pBallPtrArray[i]=NULL;
		}

		if(!RandomAllocateBalls(oriBalls))
		{
			return FALSE;
		}

		needRealloc=FALSE;

		for(UINT i=0;i<m_NewBallsPos.size();i++)
		{
			if(CancelBalls(m_NewBallsPos[i].x,m_NewBallsPos[i].y,4))
			{
				needRealloc=TRUE;
				break;
			}
		}
	}while(needRealloc);
	
	UINT ballSize=World::GetInstance().GetCellSize();
	D3DXCreateTextureFromFileEx(Application::GetInstance().GetDxDevicePtr(), pBallSrcFile, 
		ballSize, ballSize, 1, //width, height, mipLevel
		0, D3DFMT_UNKNOWN, //usage, format
		D3DPOOL_DEFAULT, D3DX_FILTER_POINT, D3DX_DEFAULT, //pool, filter, mipFilter
		0, NULL, NULL, &m_pBallTex);//ColorKey ,pSrcInfo ,pPalette ,ppTexture 

	return TRUE;
}

void BallManager::ProcessLogic(float elapsedTime)
{
	State state=Application::GetInstance().GetState();
	switch(state)
	{
		case S_Idle:
			if(GetKeyState(VK_LBUTTON) & 0x8000)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(Application::GetInstance().GetHWND(), &pt);
				const LPPOINT worldCoord=World::GetInstance().GetWorldCoordPtr(pt.x,pt.y);
				if(worldCoord!=NULL)
				{
					if(m_pBallPtrArray[worldCoord->x+worldCoord->y*m_Rows]!=NULL)//目的位置有球
					{
						m_SelectedBall.x=worldCoord->x;
						m_SelectedBall.y=worldCoord->y;
						Application::GetInstance().SetState(S_SelectedBall);
					}
				}
			}
			break;
		case S_SelectedBall:
			if(GetKeyState(VK_LBUTTON) & 0x8000)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(Application::GetInstance().GetHWND(), &pt);
				const LPPOINT worldCoord=World::GetInstance().GetWorldCoordPtr(pt.x,pt.y);
				if(worldCoord!=NULL)
				{
					if(m_pBallPtrArray[worldCoord->x+worldCoord->y*m_Rows]!=NULL)//目的位置有球
					{
						m_SelectedBall.x=worldCoord->x;
						m_SelectedBall.y=worldCoord->y;
					}
					else//目的位置没有球
					{
						if(AstarSearch(m_SelectedBall.x,m_SelectedBall.y,worldCoord->x,worldCoord->y))
						{
							Application::GetInstance().SetState(S_BallMoving);
						}
						else
						{
							Application::GetInstance().SetState(S_Idle);
						}
					}
				}
			}
			break;
		case S_BallMoving:
			if(m_pBallPtrArray[m_SelectedBall.x+m_SelectedBall.y*m_Rows]!=NULL)
			{
				m_MovingBall=m_pBallPtrArray[m_SelectedBall.x+m_SelectedBall.y*m_Rows];
				m_pBallPtrArray[m_SelectedBall.x+m_SelectedBall.y*m_Rows]=NULL;
			}
			if(m_MovingBall->move(m_PathPos.back().x, m_PathPos.back().y, 0.1f,elapsedTime))
			{
				m_PathPos.pop_back();
				if(m_PathPos.empty())
				{
					m_pBallPtrArray[(int)m_MovingBall->x+(int)m_MovingBall->y*m_Rows]=m_MovingBall;
					if(CancelBalls((int)m_MovingBall->x,(int)m_MovingBall->y,5))
					{
						Application::GetInstance().SetState(S_Idle);
					}
					else if(RandomAllocateBalls(4))
					{
						Application::GetInstance().SetState(S_AfterAlloc);
					}
					else
					{
						Application::GetInstance().SetState(S_GameOver);
					}
					m_MovingBall=NULL;
				}
			}
			break;
		case S_AfterAlloc:
			for(UINT i=0;i<m_NewBallsPos.size();i++)
			{
				if(CancelBalls(m_NewBallsPos[i].x,m_NewBallsPos[i].y,4))
				{
					Sleep(150);
				}
			}
			Application::GetInstance().SetState(S_Idle);
			break;
	}
}

void BallManager::Render()
{
	for(int i=0;i<m_Rows*m_Columns;i++)
	{
		if(m_pBallPtrArray[i]!=NULL)
		{
			const LPD3DXVECTOR2 sCoold=World::GetInstance().GetScreenCoordPtr(m_pBallPtrArray[i]->x,m_pBallPtrArray[i]->y);
			Application::GetInstance().GetDxSpritePtr()->Draw(
				m_pBallTex,NULL,NULL,&D3DXVECTOR3(sCoold->x,sCoold->y,0),m_pBallPtrArray[i]->color );
		}
	}
	if(m_MovingBall!=NULL)
	{
		const LPD3DXVECTOR2 sCoold=World::GetInstance().GetScreenCoordPtr(m_MovingBall->x,m_MovingBall->y);
		Application::GetInstance().GetDxSpritePtr()->Draw(
				m_pBallTex,NULL,NULL,&D3DXVECTOR3(sCoold->x,sCoold->y,0),m_MovingBall->color );
	}
}

BOOL BallManager::RandomAllocateBalls(UINT nBalls)
{
	srand(time(NULL));

	m_NewBallsPos.resize(nBalls);
	vector<POINT> randomBalls;
	POINT ballPos;
	for(int x=0;x<m_Rows;x++)
	{
		for(int y=0;y<m_Columns;y++)
		{
			if(m_pBallPtrArray[x+y*m_Rows]==NULL)
			{
				ballPos.x=x;
				ballPos.y=y;
				randomBalls.push_back(ballPos);
			}
		}
	}
	if(randomBalls.size()<nBalls)
	{
		return FALSE;
	}
	
	for(UINT i=randomBalls.size();i>randomBalls.size()-nBalls;i--)
	{
		int r=rand() % i;
		Ball* ball=new Ball(randomBalls[r].x, randomBalls[r].y, GetRandomColor());
		m_pBallPtrArray[randomBalls[r].x+randomBalls[r].y * m_Rows]=ball;
		POINT newPos={randomBalls[r].x,randomBalls[r].y};
		m_NewBallsPos[randomBalls.size()-i]=newPos;
		randomBalls[r]=randomBalls[i-1];
	}
	return TRUE;
}

BOOL BallManager::CancelBalls(int x, int y, int nBalls)
{
	if(m_pBallPtrArray[x+y*m_Rows]==NULL)
	{
		return FALSE;
	}
	BOOL result=FALSE;
	int ox,oy;
	const int direction[4][2][2]={ {{-1,-1},{1,1}}, {{-1,0},{1,0}}, {{0,-1},{0,1}}, {{-1,1},{1,-1}} };

	for(int i=0;i<4;i++)
	{
		int count[2]={0,0};
		for(int j=0;j<2;j++)
		{
			for(int k=1;;k++)
			{
				ox=x+direction[i][j][0]*k;
				oy=y+direction[i][j][1]*k;
				if(ox<0 || ox>=m_Rows || oy<0 || oy>=m_Columns || m_pBallPtrArray[ox+oy*m_Rows]==NULL || m_pBallPtrArray[ox+oy*m_Rows]->color!=m_pBallPtrArray[x+y*m_Rows]->color)
				{
					break;
				}
				count[j]++;
			}
		}
		if(count[0]+count[1]+1 >= nBalls)
		{
			for(int j=0;j<2;j++)
			{
				for(int k=1;k<=count[j];k++)
				{
					ox=x+direction[i][j][0]*k;
					oy=y+direction[i][j][1]*k;
					delete m_pBallPtrArray[ox+oy*m_Rows];
					m_pBallPtrArray[ox+oy*m_Rows]=NULL;
				}
			}
			result=TRUE;
		}
	}
	if(result==TRUE)
	{
		delete m_pBallPtrArray[x+y*m_Rows];
		m_pBallPtrArray[x+y*m_Rows]=NULL;
	}
	return result;
}

DWORD BallManager::GetRandomColor()
{
	DWORD colors[]={CL_BROWN,CL_GREEN,CL_BLUE,CL_PURPLE,CL_ORANGE,CL_RED};
	return colors[rand()%6];
}

#define DIST(x1,y1,x2,y2) (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)
BOOL BallManager::AstarSearch(int sx,int sy,int dx,int dy)
{
	if(m_pBallPtrArray[sx+sy*m_Rows]==NULL)
	{
		return FALSE;
	}
	
	int direction[4][2]={{-1,0},{0,-1},{1,0},{0,1}};
	Node* goal=NULL;
	vector<Node*> closeList;
	vector<Node*> openList;
	Node* start=new Node(sx,sy,0,DIST(sx,sy,dx,dy),NULL);
	openList.push_back(start);
	while(!openList.empty())
	{
		make_heap(openList.begin(),openList.end(),Node::HeapComp_f());
		Node* current=openList.front();
		//pop_heap(openList.begin(),openList.end(),Node::HeapComp_f());
		
		if(current->x==dx && current->y==dy)
		{
			goal=current;
			break;
		}
		openList.front()=openList.back();
		openList.pop_back();
		closeList.push_back(current);
		for(int i=0;i<4;i++)
		{
			int ox=current->x+direction[i][0];
			int oy=current->y+direction[i][1];
			if(ox<0 || ox>=m_Rows || oy<0 || oy>=m_Columns || m_pBallPtrArray[ox+oy*m_Rows]!=NULL)
			{
				continue;
			}
			//在关闭列表
			if(find_if(closeList.begin(),closeList.end(),Node::VectorComp_xy(ox,oy))!=closeList.end())
			{
				continue;
			}
			vector<Node*>::iterator opit=find_if(openList.begin(),openList.end(),Node::VectorComp_xy(ox,oy));
			//没在打开列表
			if(opit == openList.end())
			{
				Node* node=new Node(ox,oy,current->g+1,DIST(ox,oy,dx,dy),current);
				openList.push_back(node);
			}
			//当前G值小于以前的
			else if( current->g+1 < (*opit)->g )
			{
				(*opit)->g=current->g +1;
				(*opit)->f=(*opit)->g+(*opit)->h;
			}
		}
		//make_heap(openList.begin(),openList.end(),Node::HeapComp_f());
	}
	if(goal==NULL)//没找到路径
	{
		return FALSE;
	}
	m_PathPos.clear();
	Node* temp=goal;
	while(temp->parent!=NULL)
	{
		POINT pos={temp->x,temp->y};
		m_PathPos.push_back(pos);
		temp=temp->parent;
	}
	for(UINT i=0;i<openList.size();i++)
	{
		delete openList[i];
	}
	for(UINT i=0;i<closeList.size();i++)
	{
		delete closeList[i];
	}
	return TRUE;
}