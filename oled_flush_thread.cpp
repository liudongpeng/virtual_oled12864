#include "oled_flush_thread.h"

#include <QEvent>
#include <QDebug>

OledFlushThread::OledFlushThread(QObject *parent)
    : QThread{parent}
{

}

void OledFlushThread::run()
{
    static int count;

    while (true)
    {
        emit oledFlush();

        QThread::msleep(30);
        qDebug() << "in thread, " << count++;
    }
}
