#include "resizehandle.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

ResizeHandle::ResizeHandle(QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), m_resizeItem(nullptr), isResizingStart(true)
{
    setRect(-5, -5, 10, 10);
    setBrush(QBrush(Qt::red));
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
}

void ResizeHandle::setResizeItem(QGraphicsItem *item)
{
    m_resizeItem = item;
}

QGraphicsItem* ResizeHandle::resizeItem() const
{
    return m_resizeItem;
}

void ResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
     QGraphicsEllipseItem::mousePressEvent(event);
}

void ResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resizeItem)
    {
        QPointF newPos = mapToScene(event->pos());

        if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(m_resizeItem))
        {
            QRectF newRect(rectItem->rect().topLeft(), newPos);
            rectItem->setRect(newRect);
        }
        else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(m_resizeItem))
        {
            QRectF newRect(ellipseItem->rect().topLeft(), newPos);
            ellipseItem->setRect(newRect);
        }
        else if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(m_resizeItem))
        {
            QPointF handlePos = mapToScene(event->pos());
            if (QLineF(handlePos, lineItem->line().p1()).length() < QLineF(handlePos, lineItem->line().p2()).length())
            {
                isResizingStart = true;
            }
            else
            {
                isResizingStart = false;
            }
            QLineF line = lineItem->line();
            if (isResizingStart)
            {
                line.setP1(handlePos);
            }
            else
            {
                line.setP2(handlePos);
            }
            lineItem->setLine(line);
        }
        m_resizeItem->update();
    }

    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void ResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}
