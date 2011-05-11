#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <poppler-qt4.h>

#include "displaywidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Poppler::Document *document;
    QRubberBand *rubberBand;
    QTimer updateTimer;
    QSizeF currentPageSize;
    QString sourceFilename;
    QProcess *workerProcess;

    void resizeEvent ( QResizeEvent * event );
    void wheelEvent ( QWheelEvent * event );

private slots:
    void on_btnCrop_clicked();
    void on_actionPageChanged_triggered();
    void on_actionLoad_triggered();
    void reload_page();
    void rectSelected(QRect rect);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void readError();
    void readOutput();
};

#endif // MAINWINDOW_H
