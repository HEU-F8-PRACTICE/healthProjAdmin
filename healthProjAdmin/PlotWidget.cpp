#include "PlotWidget.h"

PlotWidget::PlotWidget(QWidget *parent,
                       QColor color_, double y0_, double yScale_) : QWidget(parent),
                        color(color_), y0(y0_), yScale(yScale_)
{
    painter = new QPainter;
    pix = new QPixmap(this->width(), this->height());
    pix->fill(Qt::black);
    label.setParent(this);
    label.move(0,0);
    label.setStyleSheet("font: 10pt \"黑体\"; background-color: black; color: white");
}

void PlotWidget::sendData(QList<int> d)
{
    data.clear();
    for(int i = 0;i<d.size();i++)
    {
        data.append(pix->height() - (d[i]-y0)*pix->height()/(yScale-y0) + label.height());
    }
    this->update();
}

void PlotWidget::paintEvent(QPaintEvent *event)
{
    label.setText(text);
    label.adjustSize();
    Q_UNUSED(event);
    paintData();
    painter->begin(this);
    painter->drawPixmap(0, 0, this->width(), this->height(),*pix);
    painter->end();
}

void PlotWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    *pix = pix->scaled(this->width(), this->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //data.clear();
    pix->fill(Qt::black);
}

void PlotWidget::paintData()
{
    pix->fill(Qt::black);
    painter->begin(pix);
    painter->setPen(color);
    painter->setRenderHint(QPainter::Antialiasing,true);
    for(int i=1;i<data.size();i++)
    {
        painter->drawLine((i-1)*this->width()/data.size(), data.at(i-1),
                              i*this->width()/data.size(), data.at(i));
    }
    painter->end();
}
