#include "displaywidget.h"

DisplayWidget::DisplayWidget(QWidget *parent) :
        QLabel(parent),
        rubberBand(NULL)
{
}

void DisplayWidget::mousePressEvent(QMouseEvent *event)
{
    origin = event->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void DisplayWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QRect rubberRect = QRect(origin, event->pos()).normalized();
    emit rectangleSelected(rubberRect);
}


void DisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
    QRect rubberRect = QRect(origin, event->pos()).normalized();
    rubberBand->setGeometry(rubberRect);
}
