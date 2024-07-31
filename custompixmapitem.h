#ifndef CUSTOMPIXMAPITEM_H
#define CUSTOMPIXMAPITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QLabel>

class CustomPixmapItem : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    static int GlobalItemId;
    CustomPixmapItem(const QPixmap &pixmap);
    CustomPixmapItem(const CustomPixmapItem& other)    : QObject(), QGraphicsItemGroup(), ItemId(other.ItemId + 1),
        IsStartConnected(other.IsStartConnected),
        IsEndConnected(other.IsEndConnected),
        ContainerWidget(nullptr), TextLabel(new QLabel()),
        PixmapLabel(new QLabel()),
        StartCircle(new QGraphicsEllipseItem()), EndCircle(new QGraphicsEllipseItem())
    {

    }
    CustomPixmapItem* clone() const {
        return new CustomPixmapItem(*this);
    }
    void SetText(const QString &text);
    QString GetText() const;
    void write(QDataStream &out) const;
    void read(QDataStream &in);
    void SetStartConnected(bool connected);
    void SetEndConnected(bool connected);
    bool GetStartConnected();
    bool GetEndConnected();
    void SetItemId(int itemId);
    int GetItemId();
    void HideLabelIfNeeded();

    int pixmapWidth() const { return PixmapLabel->pixmap()->width(); }
    int pixmapHeight() const { return PixmapLabel->pixmap()->height(); }

    QGraphicsEllipseItem *GetStartCircle() const;
    QGraphicsEllipseItem *GetEndCircle() const;

signals:
    void positionChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void AddEndCircles();

    QPointF DragStartPosition;
    bool IsDraggingInProgress;
    QWidget* ContainerWidget;
    QLabel* TextLabel;
    QLabel* PixmapLabel;
    QGraphicsProxyWidget* ProxyWid;
    QGraphicsEllipseItem *StartCircle;
    QGraphicsEllipseItem *EndCircle;
    int ItemId;
    bool IsStartConnected;
    bool IsEndConnected;
};

#endif // CUSTOMPIXMAPITEM_H
