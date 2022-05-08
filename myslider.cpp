#include "myslider.h"
#include <QMouseEvent>
#include <QStyle>

MySlider::MySlider(QWidget *parent) : QSlider(parent)
{

}

void MySlider::mousePressEvent(QMouseEvent *ev)
{
//    int value = (int)(minimum() + ((ev->pos().x() * 1.0 / width()) * (maximum() - minimum())));
    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), ev->pos().x(), width());
    setValue(value);
    //
    QSlider::mousePressEvent(ev);
}
