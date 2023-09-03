#include "imageviewer.hpp"

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>

#include "globalstorage.hpp"

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget{parent}
{
    connect(this, &ImageViewer::imageLoaded, this, [=]() { update(); });
}

void ImageViewer::setGlobalStorage(QSharedPointer<GlobalStorage> globalStorage)
{
    _globalStorage = globalStorage;
}

void ImageViewer::setImageUrl(const QString &imageUrl)
{
    if (imageUrl.toLower().startsWith("http")) {
        if (_globalStorage.isNull() || !_globalStorage->has(imageUrl)) {
            QNetworkRequest request((QUrl(imageUrl)));
            auto networkAccessManager = new QNetworkAccessManager(this);
            auto reply = networkAccessManager->get(request);
            connect(reply, &QNetworkReply::finished, this, [=]() {
                if (reply->error() == QNetworkReply::NoError) {
                    _originalImage = QImage::fromData(reply->readAll());
                    if (!_originalImage.isNull()) {
                        _globalStorage->set(imageUrl, _originalImage);
                    }
                    scaleImage(size());
                    emit imageLoaded();
                } else {
                    qWarning() << reply->errorString();
                }
                reply->deleteLater();
                networkAccessManager->deleteLater();
            });
        } else {
            _originalImage = _globalStorage->get(imageUrl).value<QImage>();
            scaleImage(size());
            emit imageLoaded();
        }
    } else if (QFile(imageUrl).exists()) {
        _originalImage = QImage(imageUrl);
        scaleImage(size());
        emit imageLoaded();
    } else {
        _originalImage = QImage();
        scaleImage(size());
        emit imageLoaded();
    }
}

void ImageViewer::scaleImage(const QSize &toSize)
{
    if (!_originalImage.isNull()) {
        _scaledImage = _originalImage.scaledToHeight(toSize.height(), Qt::SmoothTransformation);
    } else {
        _scaledImage = QImage();
    }
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    scaleImage(event->size());
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    if (_scaledImage.isNull()) {
        return QWidget::paintEvent(event);
    }
    QPainter p(this);
    QRect imageRect = _scaledImage.rect();
    imageRect.moveCenter(rect().center());
    QRect sourceImageRect = imageRect;
    sourceImageRect.moveCenter(_scaledImage.rect().center());
    p.drawImage(imageRect, _scaledImage, sourceImageRect);
}
