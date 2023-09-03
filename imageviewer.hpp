#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QImage>
#include <QSharedPointer>
#include <QWidget>

class GlobalStorage;

class ImageViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void setGlobalStorage(QSharedPointer<GlobalStorage> globalStorage);

public slots:
    void setImageUrl(const QString &imageUrl);
    void scaleImage(const QSize &toSize);

signals:
    void imageLoaded();

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QImage _originalImage;
    QImage _scaledImage;
    QSharedPointer<GlobalStorage> _globalStorage = nullptr;
};

#endif // IMAGEVIEWER_HPP
