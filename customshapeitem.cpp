#include "customshapeitem.h"
#include "resizehandle.h"  // Include the header for ResizeHandle
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <cmath>

CustomShapeItem::CustomShapeItem(ShapeType shapeType, QGraphicsItem *parent)
    : QGraphicsItem(parent), shapeType(shapeType)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
}

QRectF CustomShapeItem::boundingRect() const
{
    if (shapeType == Line || shapeType == Arrow)
    {
        return QRectF(shapeLine.p1(), shapeLine.p2());
    }
    else
    {
        return shapeRect;
    }
}

QPainterPath CustomShapeItem::shape() const
{
    QPainterPath path;
    switch (shapeType)
    {
    case ConvLine:
    case ConvReverseLine:
    case Line:
        path.moveTo(shapeLine.p1());
        path.lineTo(shapeLine.p2());
        break;
    case Rectangle:
        path.addRect(shapeRect);
        break;
    case Ellipse:
        path.addEllipse(shapeRect);
        break;
    case Arrow:
        {
            QLineF line(shapeLine);
            path.moveTo(line.p1());
            path.lineTo(line.p2());

            double angle = std::atan2(-line.dy(), line.dx());
            QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * 10, cos(angle + M_PI / 3) * 10);
            QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * 10, cos(angle + M_PI - M_PI / 3) * 10);

            QPolygonF arrowHead;
            arrowHead << line.p2() << arrowP1 << arrowP2;

            path.addPolygon(arrowHead);
        }
        break;
    case PolygonLine:
        path.moveTo(shapeLine.p1());
        path.lineTo(shapeLine.p2());
        break;
    }
    return path;
}

void CustomShapeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    switch (shapeType)
    {
    case ConvLine:
        painter->setBrush(Qt::white);
        painter->drawEllipse(shapeLine.p1(), 5, 5);
        painter->drawLine(shapeLine.p1(), shapeLine.p2());
        painter->setBrush(Qt::green);
        painter->drawEllipse(shapeLine.p2(), 5, 5);
        break;
    case ConvReverseLine:
        painter->setBrush(Qt::yellow);
        painter->drawEllipse(shapeLine.p1(), 5, 5);
        painter->drawLine(shapeLine.p1(), shapeLine.p2());
        painter->setBrush(Qt::green);
        painter->drawEllipse(shapeLine.p2(), 5, 5);
        break;
    case Line:
        painter->drawLine(shapeLine.p1(), shapeLine.p2());
        break;
    case Rectangle:
        painter->drawRect(shapeRect);
        break;
    case Ellipse:
        painter->drawEllipse(shapeRect);
        break;
    case Arrow:
        {
            QLineF line(shapeLine);
            painter->drawLine(line);

            double angle = std::atan2(-line.dy(), line.dx());
            QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * 10, cos(angle + M_PI / 3) * 10);
            QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * 10, cos(angle + M_PI - M_PI / 3) * 10);

            QPolygonF arrowHead;
            arrowHead << line.p2() << arrowP1 << arrowP2;

            painter->setBrush(Qt::black);
            painter->drawPolygon(arrowHead);
        }
        break;
    case PolygonLine:
        painter->drawLine(shapeLine.p1(), shapeLine.p2());
        break;
    }

    if (option->state & QStyle::State_Selected)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->drawRect(boundingRect());
    }
}

void CustomShapeItem::setShapeRect(const QRectF &rect)
{
    qDebug() << "InSetShapeRect";
    prepareGeometryChange();
    shapeRect = rect;
    update();
}

QRectF CustomShapeItem::getShapeRect() const
{
    return shapeRect;
}

void CustomShapeItem::setShapeLine(const QLineF &line)
{
    prepareGeometryChange();
    shapeLine = line;
    update();
}

QLineF CustomShapeItem::getShapeLine() const
{
    return shapeLine;
}

void CustomShapeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        qDebug()<<"SHAPECLICK";
        QGraphicsItem::mousePressEvent(event);
    }
}

void CustomShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void CustomShapeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}
