#include "maze.h"
#include <QTime>
#include <QDebug>
#include <queue>
point::point(){}

point::point(int i,int j, int state):i(i),j(j),state(state){}

bool point::operator==(const point& t)
{
    if(this->i == t.i&&j == t.j && t.state == state)
        return true;
    else return false;
}

Maze::Maze()
{
    matrix = NULL;
    recordMatrix = NULL;

    height = width = 0;
    X = Y = 1;

    //初始化随机数种子
    QTime time=QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);

    //创建两个栈
    MazeStack= new QStack<point>;

    //初始化方向Move[4]
    move[0].i = -1; move[0].j=0;
    move[1].i = 0 ; move[1].j=1;
    move[2].i = 1 ; move[2].j=0;
    move[3].i = 0 ; move[3].j=-1;

}

void Maze::initMaze(int h,int w) //创建一个大小为h*w的空白迷宫和record矩阵
{
    //让使用到的两个栈清空
    MazeStack->clear();
    autoPath.clear();

    //获取迷宫边界大小和人物的初始位置
    height = h;
    width  = w;

    X=height/2;
    Y=width/2;
    if(X%2==0)
        X+=1;
    if(Y%2==0)
        Y+=1;

    //收回之前分配的两个矩阵的空间
    if(matrix != NULL)
    {
        for(int i=0;i<height;i++)
            delete [] matrix[i];
        delete [] matrix;

        for(int i=0; i<height;i++)
            delete [] recordMatrix[i];
        delete [] recordMatrix;
    }
    //重新创建matrix数组和recordMaze数组, 因为height和width发生了改变
    matrix=new point*[height];
    recordMatrix = new point*[height];

    for(int i=0;i<height;i++)
    {
        matrix[i]=new point[width];
        recordMatrix[i] = new point[width];
    }
    //初始化matrix(全0)和recordMatrix的值(全1)
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            matrix[i][j].state=0;//所有节点设置为0
            recordMatrix[i][j].state= 1 ;
            recordMatrix[i][j].i = matrix[i][j].i = i;
            recordMatrix[i][j].j = matrix[i][j].j = j;
        }
    }
}
void Maze::refreshMaze(int h,int w ,int (&map)[31][31]) //创建一个大小为h*w的迷宫和record矩阵
{
    //让使用到的两个栈清空
    MazeStack->clear();
    autoPath.clear();
    height = h;
    width  = w;

    X=height/2;
    Y=width/2;
    if(X%2==0)
        X+=1;
    if(Y%2==0)
        Y+=1;

    //收回之前分配的两个矩阵的空间
    if(matrix != NULL)
    {
        for(int i=0;i<height;i++)
            delete [] matrix[i];
        delete [] matrix;

        for(int i=0; i<height;i++)
            delete [] recordMatrix[i];
        delete [] recordMatrix;
    }
    //重新创建matrix数组和recordMaze数组, 因为height和width发生了改变
    matrix=new point*[height];
    recordMatrix = new point*[height];

    for(int i=0;i<height;i++)
    {
        matrix[i]=new point[width];
        recordMatrix[i] = new point[width];
    }
    //初始化matrix()和recordMatrix的值(全1)
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            matrix[i][j].state=map[i][j];//所有节点设置
            recordMatrix[i][j].state= 1 ;
            recordMatrix[i][j].i = matrix[i][j].i = i;
            recordMatrix[i][j].j = matrix[i][j].j = j;
        }
    }
}
void Maze::setDirFalse(bool& up,bool& down,bool& right,bool& left)
{
    up = down = right = left = false;
}

void Maze::createMaze()
{
    int i=3,j=3;
    matrix[i][j].state=1;

    point temp;
    temp.i=i;
    temp.j=j;
    temp.state=1;
    bool up=false, down=false, right=false, left=false;

    while(true)
    {
        temp.i=i;
        temp.j=j;
        int randNum=qrand()%4;
        switch(randNum)
        {
        case 0://上
            if(!up&&i>2&&matrix[i-2][j].state==0)
            {
                MazeStack->push(temp);
                matrix[i-2][j].state=1;
                matrix[i-1][j].state=1;
                i=i-2;
                setDirFalse(up, down, right, left);
            }
            else
                up=true;
            break;
        case 1://下
            if(!down&&i<height-3&&matrix[i+2][j].state==0)
            {
                MazeStack->push(temp);
                matrix[i+2][j].state=1;
                matrix[i+1][j].state=1;
                i=i+2;
                setDirFalse(up, down, right, left);
            }
            else
                down=true;
            break;
        case 2://左
            if(!left&&j>2&&matrix[i][j-2].state==0)
            {
                MazeStack->push(temp);
                matrix[i][j-2].state=1;
                matrix[i][j-1].state=1;
                j=j-2;
                setDirFalse(up, down, right, left);
            }
            else
                left=true;
            break;
        case 3://右
            if(!right&&j<width-3&&matrix[i][j+2].state==0)
            {
                MazeStack->push(temp);
                matrix[i][j+2].state=1;
                matrix[i][j+1].state=1;
                j=j+2;
                setDirFalse(up, down, right, left);
            }
            else
                right=true;
            break;
        }
        if(up&&down&&right&&left)//如果当前访问节点四个方向都没有可拆的节点，回溯
        {
            if(!MazeStack->empty())
            {
                i=MazeStack->top().i;
                j=MazeStack->top().j;
                MazeStack->pop();
                setDirFalse(up, down, right, left);
            }
            else//如果栈为空的话就返回，此时迷宫矩阵已经创建完毕
            {
                return;
            }
        }
    }
}

void Maze::autoFindPath()
{
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {

            recordMatrix[i][j].state= 1 ;
            recordMatrix[i][j].i = matrix[i][j].i = i;
            recordMatrix[i][j].j = matrix[i][j].j = j;
        }
    }

    pathStack.clear();
    recordMatrix[X][Y].state = 0;  //出口位置标记为已经访问过,0为墙
    point temp(X,Y,0);
    pathStack.push_back(temp);
    QList<point> stack;

    int i , j , d ;
    while(!pathStack.isEmpty())
    {
        point top = pathStack.back();
        pathStack.pop_back();

        if(!autoPath.isEmpty()&& !(top==autoPath.back()))
            autoPath.push_back(top);
        if(autoPath.isEmpty())
            autoPath.push_back(top);

        i = top.i;j=top.j;d=top.state;

        while(d < 4)
        {
            temp.i = i+move[d].i;
            temp.j = j+move[d].j;

            if(temp.i == height-2 &&temp.j ==width-2)
            {
                pathStack.push_back(top);
                pathStack.push_back(temp);
                autoPath.push_back(temp);

                for(int i=0 ; i<pathStack.size();i++)
                {
                    qDebug()<<"("<<pathStack[i].i<<","<<pathStack[i].j<<")";
                }
                return;
            }

            if(matrix[temp.i][temp.j].state == 1 && recordMatrix[temp.i][temp.j].state == 1)
            {

                recordMatrix[temp.i][temp.j].state = 0;
                top.state=d;
                pathStack.push_back(top);
                temp.state=0;
                pathStack.push_back(temp);
                autoPath.push_back(temp);
                break;
            }
            d++;
        }
        if(d==4)
            autoPath.push_back(pathStack.back());
    }
}
void Maze::dfs(int x,int y)
{

    int dir[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};

    if(x==height-2 && y==width-2)//成功---下面处理路径问题
        {

            while(!path.empty())//将path里面的点取出来，放在pa里面
            {//path从栈顶-栈底的方向，路径是从终点-起点的顺序
                point p1 = path.top();
                path.pop();
                pa.push(p1);
            }
            while(!pa.empty())
            {//输出temp里面的路径，这样刚好是从起点到终点的顺序
                point p1 = pa.top();
                pa.pop();
                path.push(p1);//将路径放回path里面，因为后面还要回溯!!!
                point temp(p1.i,p1.j,0);
                allpath.push_back(temp);
                qDebug()<< "(" << p1.i << "," << p1.j << ")";
            }
            return;
        }
    if(x<=0 || x>=height-1 || y<=0 || y>=width-1)//越界
            return;

        //如果到了这一步，说明还没有成功，没有出界
        for(int i=0;i<4;i++)//从4个方向探测
        {
            int nx = x + dir[i][0];
            int ny = y + dir[i][1];//nx,ny：选择一个方向，前进一步之后，新的坐标
            if(0<=nx && nx<height-1 && 0<=ny && ny<width-1 && matrix[nx][ny].state==1 && recordMatrix[nx][ny].state==1)
            {//条件：nx,ny没有出界，maze[nx][ny]=0这个点不是障碍可以走，vis[nx][ny]=0说明(nx,ny)没有访问过，可以访问

                recordMatrix[nx][ny].state=0;//设为访问过
                point p(nx,ny,0);

                path.push(p);//让当前点进栈

                dfs(nx,ny);//进一步探测

                recordMatrix[nx][ny].state=1;//回溯
                path.pop();//由于是回溯，所以当前点属于退回去的点，需要出栈
            }
        }


}
void Maze::initbfs(int x, int y)
{
    int dx[] = { -1,0,1,0 }, dy[] = { 0,1,0,-1 };
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            dis[i][j]=1000;
        }
    }
    dis[X][Y]=0;
    bfs(x,y);
    int m=height-2,n=width-2;
    for(int i=0;i<dis[height-2][width-2];i++)
    {
        point p(m,n,1);
        shortpath.push_front(p);
        for(int j=0;j<4;j++)
        {
            if(dis[m+dx[j]][n+dy[j]]==dis[m][n]-1)
            {
                m+=dx[j];
                n+=dy[j];
                break;
            }
        }
    }
    point p(x,y,1);
    shortpath.push_back(p);
}
void Maze::bfs(int x, int y)
{
     int dx[] = { -1,0,1,0 }, dy[] = { 0,1,0,-1 };
    queue<point>que;
     int sx=x, sy=y;
     point temp(sx,sy,1);
     que.push(temp);
      while (!que.empty())
      {
          point p=que.front();
          que.pop();
          int xx = p.i; int yy = p.j;
          if (xx == height-2 && yy == width-2)  break;
          for (int i = 0; i < 4; i++)
          {
              int nx = xx + dx[i]; int ny = yy + dy[i];
              if (nx > 0 && nx < height-1 && ny > 0 && ny < width-1 && matrix[nx][ny].state== 1 && dis[nx][ny] == 1000)
              {
                  point temp(nx,ny,1);
                  que.push(temp);
                  dis[nx][ny] = dis[xx][yy] + 1;
               }
           }
         }
         return ;
}



void Maze::changePath(int i, int j)
{
    if(i>=height&&j>=width)return;
    if(matrix!=NULL)
    {
        if(matrix[i][j].state==0)
            matrix[i][j].state=1;
        else matrix[i][j].state=0;
    }
}
