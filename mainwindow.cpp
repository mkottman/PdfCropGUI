#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

    QList<int> pageNumList;
    QList<int> leftList;
    QList<int> topList;
    QList<int> rightList;
    QList<int> bottomList;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        document(NULL),
        workerProcess(NULL)
{
    ui->setupUi(this);
    ui->pager->hide();
    ui->progressBar->hide();
    updateTimer.setSingleShot(true);
    connect(&updateTimer, SIGNAL(timeout()), SLOT(reload_page()));
    connect(ui->display, SIGNAL(rectangleSelected(QRect)), SLOT(rectSelected(QRect)));

    //Crosshair cursor
    setCursor(Qt::CrossCursor);

    workerProcess = new QProcess(this);
    connect(workerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(workerProcess, SIGNAL(readyReadStandardError()), SLOT(readError()));
    connect(workerProcess, SIGNAL(readyReadStandardOutput()), SLOT(readOutput()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    on_actionPageChanged_triggered();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    int p = ui->pageValue->value();
    if (event->delta() < 0) {
        ui->pageValue->setValue(p+1);
    } else {
        ui->pageValue->setValue(p-1);
    }
}

void MainWindow::on_actionLoad_triggered()
{
    if (document)
        delete document;

    while (1) {
        sourceFilename = QFileDialog::getOpenFileName(this, "Select a PDF file", QString(), "*.pdf");
        if (sourceFilename.isEmpty()) {
            return;
        }

        document = Poppler::Document::load(sourceFilename);
        if (document) {
            document->setRenderHint(Poppler::Document::Antialiasing);
            document->setRenderHint(Poppler::Document::TextAntialiasing);
            document->setRenderHint(Poppler::Document::TextHinting);

            int pages = document->numPages();
            ui->pager->setVisible(true);
            ui->pageValue->setSuffix(QString("/%1").arg(pages));
            ui->display->setText("");

            ui->pageSlider->setRange(1, pages);
            ui->pageValue->setRange(1, pages);

            ui->pageValue->setValue(1);
            return;
        } else {
            QMessageBox::critical(this, "PDF load error", "An error occured while loading the PDF, try loading another one.");
        }
    }
}

void MainWindow::on_actionPageChanged_triggered()
{
    updateTimer.stop();
    updateTimer.start(100);
}

void MainWindow::reload_page()
{
    if (document) {
        QLabel *display = ui->display;
        int pageNum = ui->pageValue->value();

        display->clear();
        display->setText(QString("Loading page %1...").arg(pageNum));

        Poppler::Page *page = document->page(pageNum-1);
        if (!page) {
            QMessageBox::critical(this, "PDF load error", "Something awful happened at this page.");
        } else {
            currentPageSize = page->pageSizeF();
            double height = display->height();
            double resolution = 72 * height / currentPageSize.height();
            QImage image = page->renderToImage(resolution, resolution);
            display->setPixmap(QPixmap::fromImage(image));
        }
    }
}

void MainWindow::rectSelected(QRect rect)
{
    const QPixmap *pixmap = ui->display->pixmap();
    if (!pixmap)
        return;

    // Mouse coordinates are inside the QLabel which contains the image of
    // the page. However, the image of the page takes up only a part of
    // the QLabel, so calculate the real coordinates in the page.

    int displayWidth = ui->display->width();
    int displayHeight = ui->display->height();

    int centerX = displayWidth/2;
    int centerY = displayHeight/2;

    int realWidth = pixmap->width();
    int realHeight = pixmap->height();
    int realX = rect.x() - (centerX - realWidth/2);
    int realY = rect.y() - (centerY - realHeight/2);

    double widthRatio = currentPageSize.width() / realWidth;
    double heightRatio = currentPageSize.height() / realHeight;

    int left = (int) (realX * widthRatio);
    int right = (int) ((realX+rect.width()) * widthRatio);
    int top = (int) ((realHeight - realY) * heightRatio);
    int bottom = (int) (((realHeight - realY - rect.height())) * heightRatio);

    ui->cropBox->setText(QString("%1 %2 %3 %4").arg(left).arg(top).arg(right).arg(bottom));
}

void MainWindow::on_btnCrop_clicked()
{
    QString cropBox = ui->cropBox->text();
    QRegExp r("^(\\d+) (\\d+) (\\d+) (\\d+)$");

    // Current page
    int pageNum = ui->pageValue->value();

    if (!r.exactMatch(cropBox)) {
        QMessageBox::warning(this, "Cropbox error", "You typed something wrong in cropbox.\nExpected format: 'left top right bottom' (all integers, no special characters).");
    } else {
        int left = r.cap(1).toInt();
        int top = r.cap(2).toInt();
        int right = r.cap(3).toInt();
        int bottom = r.cap(4).toInt();

	pageNumList << pageNum;
	leftList << left;
	topList << top;
	rightList << right;
	bottomList << bottom;
    }
}

void MainWindow::on_btnExport_clicked()
{
        QFile workFile("cropwork.tex");
        if (workFile.open(QIODevice::WriteOnly)) {
            QFile::copy(sourceFilename, "cropsource.pdf");

            QStringList pageList;
	    int num = pageNumList.size();

            for (int i=0; i<num; i++) {
	      pageList << QString("\\page %1 [%2 %3 %4 %5]").arg(pageNumList.at(i)).arg(leftList.at(i)).arg(topList.at(i)).arg(rightList.at(i)).arg(bottomList.at(i));
            }
            QString pages = pageList.join("\n");

            QFile cropper(":/cropper.tex");
            cropper.open(QIODevice::ReadOnly);

            QByteArray source = cropper.readAll();
            source.replace("~INPUT~", "cropsource.pdf");
            source.replace("~PAGES~", pages.toLocal8Bit());
            workFile.write(source);
            workFile.close();

            ui->progressBar->show();
            ui->progressBar->setRange(0, num);
            ui->progressBar->setValue(0);
            workerProcess->start("pdflatex", QStringList() << "-interaction=nonstopmode" << workFile.fileName());
        } else {
            QMessageBox::warning(this, "File creation error", "Cannot create temporary files.");
        }
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    if (exitStatus == EXIT_SUCCESS) {
        QString newName = sourceFilename;
        newName.replace(".pdf", "_cropped.pdf");
        QFile::rename("cropwork.pdf", newName);
        QFile::remove("cropwork.tex");
        ui->progressBar->hide();
        QMessageBox::warning(this, "Finished", "Finished processing, the resulting file is saved as:\n" + newName);
    } else {
        QString error = workerProcess->readAllStandardError();
        QMessageBox::critical(this, "Worker failed", QString("Something horrible happened:\n") + error);
    }
}

void MainWindow::readError()
{
    QMessageBox::warning(this, "Error", workerProcess->readAllStandardError());
}

void MainWindow::readOutput()
{
    QRegExp re("PAGE(\\d+)");
    QString str = workerProcess->readAllStandardOutput();
    if (re.indexIn(str) != -1) {
        ui->progressBar->setValue(re.cap(1).toInt());
    }
}
