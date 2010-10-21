#include <QtGui>
#include <QPainter>

#include "imageviewer.h"

ImageViewer::ImageViewer()
{
    imageLabel = new QLabel;
    //imageLabel->
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();

    setWindowTitle(tr("Image Viewer"));
    resize(500, 400);
}

void ImageViewer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        image.load(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                    tr("Cannot load %1.").arg(fileName));
            return;
        }
        // zmieniony kod
        /*QPoint start(0,0);
        QPoint end(0, image.height());
        QLinearGradient gradient(start, end);
        gradient.setColorAt(0.5, Qt::black);
        gradient.setColorAt(0, Qt::white);
        QImage mask = image;
        QPainter painter(&mask);
        painter.fillRect(image.rect(), gradient);
        painter.end();
        
        QImage test = image.mirrored();
        test.setAlphaChannel(mask);*/

        // --------------
        
        imageLabel->setPixmap(QPixmap::fromImage(image));
        scaleFactor = 1.0;

        printAct->setEnabled(true);
        fitToWindowAct->setEnabled(true);
        updateActions();

        if (!fitToWindowAct->isChecked())
            imageLabel->adjustSize();
    }
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateActions();
}

void ImageViewer::greyScale()
{
    for (int x = 0; x < image.width(); x++)
    {
        for (int y = 0; y < image.height(); y++) {
            QRgb value;
            int newValue;
            value = image.pixel(QPoint(x,y));
            newValue = (qRed(value) + qGreen(value) + qBlue(value)) / 3;
            image.setPixel(x,y,qRgb(newValue, newValue, newValue));
        }
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));
}

void ImageViewer::negative()
{
    for (int x = 0; x < image.width(); x++)
    {
        for (int y = 0; y < image.height(); y++) {
            QRgb value;
            value = image.pixel(QPoint(x,y));
            image.setPixel(x,y,qRgb(255 - qRed(value), 255 - qGreen(value), 255 - qBlue(value)));
        }
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About QuTe"),
            tr("<b>QuTe 6.66 alpha-0</b><br/>Version: 0.1.1 21-10-2010"));
}

void ImageViewer::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    greyScaleAct = new QAction(tr("&Greyscale"), this);
    greyScaleAct->setEnabled(false);
    connect(greyScaleAct, SIGNAL(triggered()), this, SLOT(greyScale()));

    negativeAct = new QAction(tr("&Negative"), this);
    negativeAct->setEnabled(false);
    connect(negativeAct, SIGNAL(triggered()), this, SLOT(negative()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    filtersMenu = new QMenu(tr("&Filters"), this);
    filtersMenu->addAction(greyScaleAct);
    filtersMenu->addAction(negativeAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(filtersMenu);
    menuBar()->addMenu(helpMenu);
}

void ImageViewer::updateActions()
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
    greyScaleAct->setEnabled(true);
    negativeAct->setEnabled(true);
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
