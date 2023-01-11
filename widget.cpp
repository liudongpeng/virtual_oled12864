#include "widget.h"
#include "./ui_widget.h"

#include <windows.h>
#include <QTimer>
#include <QThread>
#include <QDebug>




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    oled = new OledWidget(this);


    //    setMinimumSize(oled->get_screen_size());
    //    setMaximumSize(oled->get_screen_size());


    oled->draw_ascii_str_6x8(0, 0, "Hello World");

    oledFlushThread = new OledFlushThread(this);
    connect(oledFlushThread, &OledFlushThread::oledFlush, this, [=](){oled->update();});

    oledFlushThread->start();





    // 清屏按钮
    connect(ui->btnClearScreen, &QPushButton::clicked, this, [=](){oled->clear_screen();});

    // 文本框
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [=](){
        oled->draw_ascii_str_6x8(0, 0, ui->lineEdit->text().toStdString().c_str());
    });
}

Widget::~Widget()
{
    delete ui;
    if (oled)
        delete oled;
}

