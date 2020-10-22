#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "maze.h"
#include "interface.h"
#include <QStack>
#include <QVector>
#include <QQueue>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QSequentialAnimationGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    interface* surface;
    Maze maze;

    QTimer *timer;
    QTimer *time1;
    int time;

    QWidget* mazeWidgets[31][31];
    QWidget* _pMouse;
    QSequentialAnimationGroup* group;

    bool isPaintPath;
    bool isShowPath;
    bool isPlayMode;
    int xPos;
    int yPos;
    int step;

    QString character[12];

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool checkadd();
    void initWidget();
    bool checkIsOdd();
    void showMaze();
    void initCharacter();
    void refresh(int h,int w ,int (&map)[31][31] );

protected:

    void keyPressEvent(QKeyEvent* event);

private slots:
    void shortpath();
    void allpaths();
    void showAnimation();
    void warn();

    void createMazeBtn();

    void playMode();

    void moveCharacter();

    void showPath();
    void change();

    void filein();
    void fileout();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
