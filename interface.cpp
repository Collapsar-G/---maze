#include "interface.h"
#include <QPalette>
#include <QPixmap>
#include <QFont>
#include <QThread>
int tim;
QString msg[] = {"加载中..","加载中...","加载中...."};
interface::interface(QWidget *parent) :
    QWidget(parent)
{

    setGeometry(0,0,720,456);

    initWidgets();

    label = new QLabel(this);
    label->setGeometry(200,80,250,200);
    label->setContentsMargins(0,0,0,0);
    movie=new QMovie(":/new/image/loading.gif");
    label->setMovie(movie);
    label->hide();

    msgLabel = new QLabel(this);
    msgLabel->setGeometry(320,250,100,75);
    msgLabel->setFont(QFont("宋体",13));
    msgLabel->hide();
}

void interface::paintEvent(QPaintEvent* event)
{
    QPainter* painter = new QPainter(this);
    QImage pixmap(":/new/image/fengmian.jpg");
    painter->drawImage(geometry(),pixmap);
}

void interface::initWidgets()
{
    probar = new QProgressBar(this);
    probar->setGeometry(120,300,480,10);
    probar->setTextVisible(true);
    probar->hide();

    this->thread = new QThread(this);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(loading()));

    button= new QPushButton(this);
    button->setGeometry(400,200,200,100);
    button->setStyleSheet("QPushButton{border-image: url(:/new/image/playbutton.png);}"
                          "QPushButton:hover{border-image: url(:/new/image/playbutton1.png);}");
    connect(button,SIGNAL(clicked()),this,SLOT(startButton()));
}

void interface::startButton()
{
    button->hide();

    label->show();
    msgLabel->show();
    movie->start();

    timer->start(100);
    probar->show();
}

void interface::loading()
{
    msgLabel->setText(msg[((tim++)/2)%3]);
    probar->setValue(probar->value()+2);
    if(probar->value() >= 99)
        delete this;
}
