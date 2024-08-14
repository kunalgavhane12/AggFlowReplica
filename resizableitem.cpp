#include "resizableitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>

// Constructor
ResizableItem::ResizableItem(QGraphicsItem *item)
    : shapeItem(item), isResizing(false)
{
    if (shapeItem)
    {
        setFlag(ItemIsMovable);
        setFlag(ItemIsSelectable);
        updateHandles();
    }
}

void ResizableItem::setShapeItem(QGraphicsItem *item)
{
    shapeItem = item;
    updateHandles();
}

// Bounding rectangle
QRectF ResizableItem::boundingRect() const
{
    if (shapeItem)
        return shapeItem->boundingRect();
    return QRectF();
}

// Paint method
void ResizableItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (shapeItem)
    {
        shapeItem->paint(painter, option, widget);
        painter->setPen(QPen(Qt::black));
        painter->setBrush(QBrush(Qt::red));
        foreach (QGraphicsEllipseItem *handle, handles)
        {
            painter->drawEllipse(handle->rect());
        }
    }
}

// Mouse press event
void ResizableItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (isHandle(event->scenePos()))
    {
        isResizing = true;
        resizeStartPoint = event->scenePos();
        if (shapeItem)
        {
            QRectF rect = shapeItem->boundingRect();
            initialRect = rect;
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

// Mouse move event
void ResizableItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isResizing)
    {
        resizeShape(event->scenePos());
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

// Mouse release event
void ResizableItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isResizing)
    {
        isResizing = false;
    }
    else
    {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

// Update handles
void ResizableItem::updateHandles()
{
    // Remove existing handles
    foreach (QGraphicsEllipseItem *handle, handles)
    {
        if (scene())
        {
            scene()->removeItem(handle);
        }
        delete handle;
    }
    handles.clear();

    if (shapeItem)
    {
        QRectF rect = shapeItem->boundingRect();
        qreal handleSize = 10.0;

        handles << new QGraphicsEllipseItem(rect.topLeft().x() - handleSize / 2,
                                            rect.topLeft().y() - handleSize / 2,
                                            handleSize, handleSize, this);
        handles << new QGraphicsEllipseItem(rect.topRight().x() - handleSize / 2,
                                            rect.topRight().y() - handleSize / 2,
                                            handleSize, handleSize, this);
        handles << new QGraphicsEllipseItem(rect.bottomLeft().x() - handleSize / 2,
                                            rect.bottomLeft().y() - handleSize / 2,
                                            handleSize, handleSize, this);
        handles << new QGraphicsEllipseItem(rect.bottomRight().x() - handleSize / 2,
                                            rect.bottomRight().y() - handleSize / 2,
                                            handleSize, handleSize, this);

        foreach (QGraphicsEllipseItem *handle, handles)
        {
            handle->setFlag(QGraphicsItem::ItemIsMovable);
            handle->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
            handle->setBrush(QBrush(Qt::red));
            if (scene())
            {
                scene()->addItem(handle);
            }
        }
    }
}

// Resize shape
void ResizableItem::resizeShape(const QPointF &newPos)
{
    if (shapeItem)
    {
        QRectF newRect(initialRect);
        qreal dx = newPos.x() - resizeStartPoint.x();
        qreal dy = newPos.y() - resizeStartPoint.y();

        if (auto rectItem = dynamic_cast<QGraphicsRectItem *>(shapeItem))
        {
            newRect.setWidth(initialRect.width() + dx);
            newRect.setHeight(initialRect.height() + dy);
            rectItem->setRect(newRect.normalized());
        }
        else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem *>(shapeItem))
        {
            newRect.setWidth(initialRect.width() + dx);
            newRect.setHeight(initialRect.height() + dy);
            ellipseItem->setRect(newRect.normalized());
        }

        updateHandles();
    }
}

// Check if position is on a handle
bool ResizableItem::isHandle(const QPointF &pos) const
{
    foreach (QGraphicsEllipseItem *handle, handles)
    {
        if (handle->contains(pos))
            return true;
    }
    return false;
}
