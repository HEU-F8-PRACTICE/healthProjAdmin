#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QLabel>
#include <QDebug>

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr, QColor color_ = Qt::white, double y0_ = 0, double yScale_ = 4096);
    QColor color = Qt::white; //笔刷颜色
    double y0 = 0; //y起始值
    double yScale = 4096; //y最大值
    QString text = "";

    void paintEvent(QPaintEvent *event) override; //重写重绘函数
    void resizeEvent(QResizeEvent *event) override;

private:
    QList<int> data; //数据
    QLabel label;
    QPainter *painter; //主painter
    QPixmap *pix;

    void paintData(); //绘制数据

public:
    void sendData(QList<int> d);
};

#endif
