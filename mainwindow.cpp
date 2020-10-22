#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>
#include <QMessageBox>
#include <QStack>
#include <QVector>
#include <QQueue>
#include <QTime>
#include <QTimer>
#include <QKeyEvent>
#include <QMovie>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include "maze.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <fstream>
#include <string>
#define BASIC_LENGTH 40
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    initCharacter();
    initWidget();

}

void MainWindow::refresh(int h,int w ,int (&map)[31][31])
{

    //initWidget();
   //initCharacter();
    ui->spinBox->setValue(h);
    ui->spinBox_2->setValue(w);
    timer->stop();
    group->stop();
    time = 1;
    step = 0;
    isShowPath = false;
    isPlayMode = false;
    isPaintPath = false;

    maze.refreshMaze(h,w,map);
    if(maze.height*30+80 <=320)
        this->setGeometry(40,40,240+maze.width*BASIC_LENGTH,320);
    else
        this->setGeometry(40,40,240+maze.width*BASIC_LENGTH,maze.height*BASIC_LENGTH+80);
    ui->groupBox->move(width()-180,10);

    //重新初始化mazeWidgets和_pMouse
    _pMouse->hide();
    for(int i=0;i<31;i++)
    {
        for(int j=0 ; j<31;j++)
        {
            mazeWidgets[i][j]->setStyleSheet("");
            mazeWidgets[i][j]->hide();
        }
    }
    //maze.autoFindPath();
    xPos = 40 + maze.X*BASIC_LENGTH;
    yPos = 40 + maze.Y*BASIC_LENGTH;

    showMaze();
}

//初始化ui的widgets
void MainWindow::initWidget()
{

    //初始化界面控件
    ui->spinBox->setStyle(QStyleFactory::create("Macintosh"));
    ui->spinBox_2->setStyle(QStyleFactory::create("Macintosh"));
    ui->buttonGenerate->setStyle(QStyleFactory::create("fusion"));
    ui->buttonPlay->setStyle(QStyleFactory::create("fusion"));
    ui->buttonPath->setStyle(QStyleFactory::create("fusion"));
    connect(ui->buttonGenerate,SIGNAL(clicked()),this,SLOT(createMazeBtn()));
    connect(ui->buttonPath,SIGNAL(clicked()),this,SLOT(showAnimation()));
    connect(ui->buttonPlay,SIGNAL(clicked()),this,SLOT(playMode()));
    //connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(showPath()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(allpaths()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(shortpath()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(change()));
    connect(ui->actionfilein,SIGNAL(triggered()),this,SLOT(filein()));
    connect(ui->actionfileout,SIGNAL(triggered()),this,SLOT(fileout()));
    ui->buttonPath->setStyleSheet("QPushButton{border-image: url(:/new/image/button1.png);}"
                                  "QPushButton:hover{border-image: url(:/new/image/button4.png);}"
                                  "QPushButton:pressed{border-image: url(:/new/image/button2.png);}");

    ui->buttonPlay->setEnabled(false);
    ui->buttonPath->setEnabled(false);

    //游戏的状态
    isPlayMode = false;
    isShowPath = false;

    //创建好迷宫砖块
    for(int i=0; i <31 ;i++)
    {
        for(int j=0; j<31; j++)
        {
            mazeWidgets[i][j]=new QWidget(this);
            mazeWidgets[i][j]->hide();
        }
    }

    //人物
    _pMouse = new QWidget(this);
    _pMouse->setStyleSheet(character[0]);
    _pMouse->hide();

    //动画和时间相关的初始化
    group = new QSequentialAnimationGroup();
    isShowPath = false;

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(moveCharacter()));

    //初始化开始界面
    surface = new interface(this);
}

bool MainWindow::checkIsOdd()
{
    if(ui->spinBox->value()%2 == 0)
        return false;
    if(ui->spinBox_2->value()%2 == 0)
        return false;

    return true;
}

void MainWindow::createMazeBtn()
{
    //停止之前可能进行的所有工作
    timer->stop();
    group->stop();
    time = 1;
    step = 0;
    isShowPath = false;
    isPlayMode = false;
    isPaintPath = false;

    if(!checkIsOdd())
    {
         QMessageBox::information(this,"警告","输入的数必须是奇数",QMessageBox::Yes);
         return;
    }

    //窗口形状变形
    maze.height = ui->spinBox->value();
    maze.width  = ui->spinBox_2->value();
    if(maze.height*30+80 <=320)
        this->setGeometry(40,40,240+maze.width*BASIC_LENGTH,320);
    else
        this->setGeometry(40,40,240+maze.width*BASIC_LENGTH,maze.height*BASIC_LENGTH+80);
    ui->groupBox->move(width()-180,10);

    //重新初始化mazeWidgets和_pMouse
    _pMouse->hide();
    for(int i=0;i<31;i++)
    {
        for(int j=0 ; j<31;j++)
        {
            mazeWidgets[i][j]->setStyleSheet("");
            mazeWidgets[i][j]->hide();
        }
    }

    //创建新的迷宫
    maze.initMaze(ui->spinBox->value(),ui->spinBox_2->value());
    maze.createMaze();
    maze.autoFindPath();
    xPos = 40 + maze.X*BASIC_LENGTH;
    yPos = 40 + maze.Y*BASIC_LENGTH;

    showMaze();
}

void MainWindow::showMaze()
{
    _pMouse->setGeometry(40+maze.Y*BASIC_LENGTH,40+maze.X*BASIC_LENGTH,BASIC_LENGTH,BASIC_LENGTH);
    _pMouse->show();
    for(int i=0 ; i<maze.height ; i++)
    {
        for(int j=0 ; j<maze.width; j++)
        {
            mazeWidgets[i][j]->show();
            mazeWidgets[i][j]->setGeometry(40+j*BASIC_LENGTH,40+i*BASIC_LENGTH,BASIC_LENGTH,BASIC_LENGTH);

            if(maze.matrix[i][j].state == 0)
                mazeWidgets[i][j]->setStyleSheet("border-image: url(:/new/image/Wall.png);");

            if(maze.matrix[i][j].state == 1)
                mazeWidgets[i][j]->setStyleSheet("border-image: url(:/new/image/Load.png);");
        }
    }
    mazeWidgets[maze.height-2][maze.width-2]->setStyleSheet("border-image: url(:/new/image/box.png);");

    //更新按钮状态
    ui->buttonPath->setEnabled(true);
    ui->buttonPlay->setEnabled(true);
}

void MainWindow::showAnimation()
{
    maze.autoFindPath();
    if(isShowPath == true)
    {
        if(timer->isActive())
            timer->stop();
        group->stop();
        isShowPath = false;
        ui->buttonPlay->setEnabled(true);
        ui->buttonPath->setStyleSheet("QPushButton{border-image: url(:/new/image/button1.png);}"
                                      "QPushButton:hover{border-image: url(:/new/image/button4.png);}"
                                      "QPushButton:pressed{border-image: url(:/new/image/button2.png);}");
        return;
    }
    else
    {
        ui->buttonPath->setEnabled(true);
        ui->buttonPath->setStyleSheet("QPushButton{border-image: url(:/new/image/button3.png);}"
                                      "QPushButton:hover{border-image: url(:/new/image/button4.png);}"
                                      "QPushButton:pressed{border-image: url(:/new/image/button2.png);}");
        time = 1;
        ui->buttonPlay->setEnabled(false);
        isPlayMode = false;
        isShowPath =true;
        time = 1;
        group->clear();

        for(int i=0; i<maze.autoPath.size()-1;i++)
        {
            QPropertyAnimation* animation = new QPropertyAnimation(_pMouse, "pos");
            animation->setDuration(400);
            animation->setStartValue(QPoint(40+maze.autoPath[i].j*BASIC_LENGTH,40+maze.autoPath[i].i*BASIC_LENGTH));
            animation->setEndValue(QPoint(40+maze.autoPath[i+1].j*BASIC_LENGTH,40+maze.autoPath[i+1].i*BASIC_LENGTH));
            animation->setEasingCurve(QEasingCurve::Linear);
            group->addAnimation(animation);
        }
        timer->start(400);
        group->start();
    }
     ui->buttonPlay->setEnabled(true);
}
void MainWindow::warn()
{
   time1->stop();
   isPlayMode = false;
   QMessageBox::warning(this,"TimeOver","You haven't moved the character to the box at the specified time, the system will automatically generate the path for you!");
  // QTimer *t;
   //t=new QTimer(this);
   //t->start(6000);
   //connect(t,SIGNAL(timeout()),this,SLOT(showAnimation()));
   //t->stop();
   showAnimation();

}
bool MainWindow::checkadd()
{
    if(xPos/40<=maze.height&&yPos/40<=maze.width&&xPos/40>=maze.height-1&&yPos/40>=maze.width-1)
    {
        QMessageBox::critical(this,"Win","You win!");
        isPlayMode = false;
        showPath();
        playMode();
        return true;
    }
    return false;
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(isPlayMode == false)
        return;
    if(checkadd())
    {
        return;
    }
    else{
    step++;
    switch(event->key())
    {
    case Qt::Key_W:
        if(maze.matrix[(xPos-8-40)/40][maze.Y].state  == 0)
        {
            return;
        }
        xPos-=8;
        maze.X=(xPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[3+(step/5)%3]);
        break;
    case Qt::Key_S:
        if(maze.matrix[(xPos)/40][maze.Y].state  == 0)
        {
           return;
        }
        xPos+=8;
        maze.X=(xPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[0+(step/5)%3]);
        break;
    case Qt::Key_A:
        if(maze.matrix[maze.X][(yPos-8-40)/40].state  == 0)
        {
            return;
        }
        yPos-=8;
        maze.Y=(yPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[6+(step/5)%3]);
        break;
    case Qt::Key_D:
        if(maze.matrix[maze.X][(yPos)/40].state  == 0)
        {
            return;
        }
        yPos+=8;
        maze.Y=(yPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[9+(step/5)%3]);
        break;}
    }
}

void MainWindow::playMode()
{
    time1=new QTimer();
    connect(time1,SIGNAL(timeout()),this,SLOT(warn()));

    time1->start(45000);
    if(isPlayMode == false)
    {
        isPlayMode =true;
        ui->buttonPath->setEnabled(false);

    }
    else
    {
        isPlayMode = false;
        ui->buttonPath->setEnabled(true);
        time1->stop();
    }

}

void MainWindow::moveCharacter()
{
     if(maze.autoPath[time+1].i < maze.autoPath[time].i)
     {
         _pMouse->setStyleSheet(character[3+time%3]);
     }
     if(maze.autoPath[time+1].i > maze.autoPath[time].i)
     {
         _pMouse->setStyleSheet(character[0+time%3]);
     }
     if(maze.autoPath[time+1].j > maze.autoPath[time].j)
     {
         _pMouse->setStyleSheet(character[9+time%3]);
     }
     if(maze.autoPath[time+1].j < maze.autoPath[time].j)
     {
         _pMouse->setStyleSheet(character[6+time%3]);
     }
     time++;
     if(time == maze.autoPath.size()-1)
     {
         timer->stop();
         time = 0;
     }
}

void MainWindow::initCharacter()
{
    character[0] = "border-image: url(:/new/image/down1.png);";
    character[1] = "border-image: url(:/new/image/down2.png);";
    character[2] = "border-image: url(:/new/image/down3.png);";
    character[3] = "border-image: url(:/new/image/up1.png);";
    character[4] = "border-image: url(:/new/image/up2.png);";
    character[5] = "border-image: url(:/new/image/up3.png);";
    character[6] = "border-image: url(:/new/image/left1.png);";
    character[7] = "border-image: url(:/new/image/left2.png);";
    character[8] = "border-image: url(:/new/image/left3.png);";
    character[9] = "border-image: url(:/new/image/right1.png);";
    character[10]= "border-image: url(:/new/image/right2.png);";
    character[11]= "border-image: url(:/new/image/right3.png);";
}

void MainWindow::showPath()
{
    //maze.autoFindPath();
    for(int i=0; i<maze.pathStack.size();i++)
    {
        point temp = maze.pathStack[i];
        int dir = maze.pathStack[i].state;
        switch (dir) {
        case 1:
            dir = 3;
            break;
        case 2:
            dir =9;
            break;
        case 3:
            dir = 0;
            break;
        case 4:
            dir = 6;
            break;
        default:
            break;
        }
        if(!isPaintPath)
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet(character[dir+i%3]);
        }
        else
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet("border-image: url(:/new/image/Load.png);");
            mazeWidgets[maze.height-2][maze.width-2]->setStyleSheet("border-image: url(:/new/image/box.png);");

        }
    }
    isPaintPath = !isPaintPath;
}

void MainWindow::change()
{

    int dx=ui->spinBox_3->value();
    int dy=ui->spinBox_4->value();
    maze.changePath(dx,dy);

    isPlayMode = false;
    isShowPath = false;
    showMaze();
    return;
}

void MainWindow::filein()
{
    QString path=QFileDialog::getOpenFileName(this,"打开文件","C:\\");
    string pa=path.toStdString();
    char*p=(char*)pa.data();
    ifstream in;
    in.open(p, ios::in) ;
   // QFile file(path);
    //file.open(QIODevice::ReadOnly);
    //QTextStream stream(&file);
    int buf;
    if (! in.is_open())
           { qDebug() << "Error opening file"; exit (1); }
    in>>buf;
    int h=buf;
    in>>buf;
    int w=buf;


    //QStringList list=stream.readAll().split(' ');
    //QListIterator<QString> li(list);
    //buf=li.next().toInt();
    //ui->spinBox->setValue(buf);
    //buf=li.next().toInt();
    //ui->spinBox_2->setValue(buf);


    int i=0,j=0;
    int map[31][31];

    while (!in.eof() )

     //while(li.hasNext())
     {
       // buf=li.next().toInt();
        in>>buf;
        map[i][j]=buf;
        j++;
        if(j==w)
        {
               j=0;
               i++;
           }
        }

       qDebug()<<h<<'z'<<w<<'z'<<i<<'z'<<j;
       for (int i=0;i<h;i++)
       {
           for(int j=0;j<w;j++)
           {
               qDebug()<<map[i][j]<<' ';
           }
       }
       in.close();
       //initWidget();
       refresh(h,w,map);

        return;

}

void MainWindow::fileout()
{
    //QString path=QFileDialog::getOpenFileName(this,"打开文件夹","C:\\Users\\ASUS\\Desktop");
    QString path = QFileDialog::getExistingDirectory(this, tr("选择保存路径"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    path+="\\data.txt";
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    QString d = QString::number(maze.height);
    file.write(d.toLatin1().data());
    file.write(" ");
    d=QString::number(maze.width);
    file.write(d.toLatin1().data());
    file.write(" ");
    for (int i=0;i<maze.height;i++)
    {
        for(int j=0;j<maze.width;j++)
        {
            d=QString::number(maze.matrix[i][j].state);
            file.write(d.toLatin1().data());
            file.write(" ");
        }
    }
    file.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::allpaths()

{
    qDebug()<<"suoyoulujing";
    maze.allpath.clear();

    for(int i=0;i<maze.height;i++)
    {
        for(int j=0;j<maze.width;j++)
        {

            maze.recordMatrix[i][j].state= 1 ;
            maze.recordMatrix[i][j].i = maze.matrix[i][j].i = i;
            maze.recordMatrix[i][j].j = maze.matrix[i][j].j = j;
        }
    }
    maze.X=maze.height/2;
    maze.Y=maze.width/2;
    if(maze.X%2==0)
        maze.X+=1;
    if(maze.Y%2==0)
        maze.Y+=1;
    point d(maze.X,maze.Y,0);
    maze.allpath.push_back(d);
    maze.dfs(maze.X,maze.Y);
    for(int i=0; i<maze.allpath.size();i++)
    {
        point temp = maze.allpath[i];
        int dir = maze.allpath[i].state;
        switch (dir) {
        case 1:
            dir = 3;
            break;
        case 2:
            dir =9;
            break;
        case 3:
            dir = 0;
            break;
        case 4:
            dir = 6;
            break;
        default:
            break;
        }
        if(!isPaintPath)
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet(character[dir+i%3]);
        }
        else
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet("border-image: url(:/new/image/Load.png);");
            mazeWidgets[maze.height-2][maze.width-2]->setStyleSheet("border-image: url(:/new/image/box.png);");

        }
    }
    isPaintPath = !isPaintPath;

}
void MainWindow::shortpath()
{
    qDebug()<<"zuiduanlujing";
    maze.shortpath.clear();
    maze.X=maze.height/2;
    maze.Y=maze.width/2;
    if(maze.X%2==0)
        maze.X+=1;
    if(maze.Y%2==0)
        maze.Y+=1;
    maze.initbfs(maze.X,maze.Y);
    for(int i=0; i<maze.shortpath.size();i++)
    {
        point temp = maze.shortpath[i];
        int dir = maze.shortpath[i].state;
        switch (dir) {
        case 1:
            dir = 3;
            break;
        case 2:
            dir =9;
            break;
        case 3:
            dir = 0;
            break;
        case 4:
            dir = 6;
            break;
        default:
            break;
        }
        if(!isPaintPath)
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet(character[dir+i%3]);
        }
        else
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet("border-image: url(:/new/image/Load.png);");
            mazeWidgets[maze.height-2][maze.width-2]->setStyleSheet("border-image: url(:/new/image/box.png);");

        }
    }
    isPaintPath = !isPaintPath;
}
