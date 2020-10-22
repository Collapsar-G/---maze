#ifndef MAZE_H
#define MAZE_H
#include <QStack>
#include <QList>
#include <stack>
using namespace std;
class point
{
public:
    int i;
    int j;
    int state;

    point();
    point(int i,int j, int state);

    bool operator==(const point& maze);
};

class Maze
{
private:

    QStack<point> *MazeStack;
    point move[4]; //人移动的四个方向
public:
    point **recordMatrix;//地图
    point **matrix;

    int height;//迷宫矩阵的行
    int width;//迷宫矩阵的列

    int X;//人的位置
    int Y;
    QList<point> pathStack,allpath,shortpath;
    QList<point> autoPath;   //存放自动寻路的结果
    int dis[31][31];
    stack<point> path,pa;
    Maze();
    void refreshMaze(int h,int w,int (&map)[31][31]);
    void initMaze(int h,int w);
    void createMaze();
    void autoFindPath();
    void changePath(int i, int j);
    void bfs(int x,int y);
    void dfs(int x,int y);
    void initbfs(int x,int y);
private:
    void setDirFalse(bool& up,bool& down,bool& right,bool& left);
};

#endif // MAZE_H
