#ifndef OLEDFLUSHTHREAD_H
#define OLEDFLUSHTHREAD_H

#include <QThread>
#include <QObject>


class OledFlushThread : public QThread
{
    Q_OBJECT
public:
    explicit OledFlushThread(QObject *parent = nullptr);

protected:
    void run();

signals:
    void oledFlush();
};

#endif // OLEDFLUSHTHREAD_H
