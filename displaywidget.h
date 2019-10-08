#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QtGui>
#include <QLabel>
#include <QRubberBand>

class DisplayWidget : public QLabel
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = 0);

private:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

    QPoint origin;
    QRubberBand *rubberBand;

signals:
    void rectangleSelected(QRect rect);

public slots:

};

#endif // DISPLAYWIDGET_H
