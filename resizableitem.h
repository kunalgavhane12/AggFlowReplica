#ifndef RESIZABLEITEM_H
#define RESIZABLEITEM_H

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>

class ResizableItem : public QGraphicsItem
{
public:
    explicit ResizableItem(QGraphicsItem *item = nullptr);

    void setShapeItem(QGraphicsItem *item);

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void updateHandles();
    void resizeShape(const QPointF &newPos);
    void createHandle(const QPointF &pos);
    bool isHandle(const QPointF &pos) const;

    QGraphicsItem *shapeItem;
    QList<QGraphicsEllipseItem *> handles;
    bool isResizing = false;
    QPointF resizeStartPoint;
    QRectF initialRect;
};

#endif // RESIZABLEITEM_H
