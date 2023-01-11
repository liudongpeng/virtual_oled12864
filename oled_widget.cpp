#include "oled_widget.h"
#include "qcoreevent.h"
#include <QPainter>
#include <QDebug>


OledWidget::OledWidget(QWidget *parent, int pixelsize)
    : QWidget(parent), pixelSize(pixelsize)
{

    oled = new QWidget(this);
    memset(cache, 0x00, sizeof(cache));

    oled->installEventFilter(this);

    /**
     * 计算控件的宽高
     * #|#|#|#|#|#|#|#|#|#|#|#
     * - - - - - - - - - - - -
     * #|#|#|#|#|#|#|#|#|#|#|#
     * 宽度为: (屏幕宽度*像素大小) + ((屏幕宽度-1)*像素间距)
     * 高度为: (屏幕高度*像素大小) + ((屏幕高度-1)*像素间距)
     */
    oled->resize(oledWidth * pixelSize + (oledWidth - 1) * pixelSpace, \
                 oledHeight * pixelSize + (oledHeight - 1) * pixelSpace);
    oled->setStyleSheet(tr("background-color: rgb(255, 255, 255);"));

    oledSize.setWidth(oled->width());
    oledSize.setHeight(oled->height());


    this->setAttribute(Qt::WA_StyledBackground, true);
}


OledWidget::~OledWidget()
{
    if (oled != nullptr)
        delete oled;
}

bool OledWidget::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Paint:
        if (watched == oled)
        {
            flush_screen();
        }
        break;

    default:
        break;
    }

    return QWidget::eventFilter(watched, event);
}


/**
 * @brief 清屏
 */
void OledWidget::clear_screen()
{
    memset(cache, 0, sizeof(cache));
    oled->update();
}

/**
 * @brief 刷新屏幕
 */
void OledWidget::flush_screen()
{
    int x = 0, y = 0, page = 0, col = 0;
    uint8_t byte;

    /* 遍历缓存, 绘制128x64个像素点 */
    for (int i = 0; i < sizeof(cache); i++)
    {
        /* 计算当前像素坐标 */
        page = i / oledWidth;

        col = i % oledWidth;
        x = col * (pixelSize + 1);

        y = page * ((pixelSize + 1) * 8);

        /* 遍历每个字节的每一位, 由低位到高位 */
        byte = cache[i];
        for (int j = 0; j < 8; j++)
        {
            if ((byte >> j) & 0x01)
                draw_pixel(x, y + j * (pixelSize + 1), PixelColorWhite);
            else
                draw_pixel(x, y + j * (pixelSize + 1), PixelColorBlack);
        }
    }
}

void OledWidget::clear_area(int x, int y, int width, int height)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            clear_point(x + i, y + j);
        }
    }
}

void OledWidget::clear_point(int x, int y)
{
    int pageIndex, offset;
    uint8_t srcByte, tar;

    pageIndex = y / 8;
    offset = y % 8;

    /* 获取目标字节 */
    srcByte = cache[(pageIndex * oledWidth) + x];

    /* 熄灭指定的像素点 */
    tar = (~(0x01 << offset)) & srcByte;

    /* 处理完的包含目标像素的字节重新赋值 */
    cache[(pageIndex * oledWidth) + x] = tar;
}

/**
 * @brief 获取屏幕尺寸
 * @return
 */
QSize OledWidget::get_screen_size() const
{
    return oledSize;
}

/**
 * @brief 绘制6x8大小的ASCII字符
 * @param x
 * @param y
 * @param ch
 */
void OledWidget::draw_ascii_char_6x8(int x, int y, char ch)
{
    int page = y / 8 % 8;

    for (int i = 0; i < 6; i++)
    {
        cache[(page * 128) + (x + i)] |= Font6x8[ch - 32][i];
    }
}

/**
 * @brief 绘制6x8大小的ASCII字符串
 * @param x
 * @param y
 * @param str
 */
void OledWidget::draw_ascii_str_6x8(int x, int y, const char *str)
{
    while (*str)
    {
        draw_ascii_char_6x8(x, y, *str++);
        x += 6;
    }
}

void OledWidget::draw_bmp_128x64(int x, int y, const uint8_t *bmp)
{
    memcpy(cache, bmp, sizeof(cache));
}

/**
 * @brief 绘制单个像素点
 * @param x
 * @param y
 * @param color
 */
void OledWidget::draw_pixel(int x, int y, bool color)
{
    QPainter painter(oled);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(color ? QColor(255, 255, 255) : QColor(0, 0, 0));
    painter.setPen(pen);

    /* 开始绘制 */
    for (int i = 0; i < pixelSize; i++)
    {
        for (int j = 0; j < pixelSize; j++)
        {
            painter.drawPoint(x + i, y + j);
        }
    }
}
