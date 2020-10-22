#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QMovie>
#include <QPainter>
#include <QStyleOption>
#include <QThread>
class interface : public QWidget
{
    Q_OBJECT
public:
    explicit interface(QWidget *parent = 0);

    QLabel* msgLabel;
    QPushButton* button;
    QProgressBar* probar;
    QTimer* timer;
    QLabel* label;
    QMovie* movie;

    QThread* thread;
protected:
    void paintEvent(QPaintEvent* event);

private:
    void initWidgets();
signals:

public slots:
    void startButton();
    void loading();
};

#endif // INTERFACE_H
