#include "customshapeitem.h"
#include "resizehandle.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <cmath>

CustomShapeItem::CustomShapeItem(ShapeType shapeType, QGraphicsItem *parent)
    : QGraphicsItem(parent), shapeType(shapeType)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
    setZValue(1);
}

QRectF CustomShapeItem::boundingRect() const
{
    QRectF rect;
    switch (shapeType)
    {
    case ConvLine:
    case ConvReverseLine:
    case Line:
    case Arrow:
    case PolygonLine:
        rect = QRectF(shapeLine.p1(), shapeLine.p2());
        break;
    case Rectangle:
    case Ellipse:
        rect = shapeRect;
        break;
    }
    return rect.adjusted(-5, -5, 10, 10);
}

void CustomShapeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    switch (shapeType)
    {
    case ConvLine:
        painter->setBrush(Qt::white);
        painter->drawEllipse(shapeLine.p1() + QPointF(2, 5), 5, 5);
        painter->drawLine(shapeLine.p1(), shapeLine.p2());
        painter->setBrush(Qt::green);
        painter->drawEllipse(shapeLine.p2() + QPointF(-2, 5), 5, 5);
        break;
        break;
    case ConvReverseLine:
        painter->setBrush(Qt::yellow);
        painter->drawEllipse(shapeLine.p1() + QPointF(2, 5), 5, 5);
        painter->drawLine(shapeLine.p1(), shapeLine.p2());
        painter->setBrush(Qt::green);
        painter->drawEllipse(shapeLine.p2() + QPointF(-2, 5), 5, 5);
        break;
    case Line:
        painter->drawLine(shapeLine);
        break;
    case Rectangle:
        painter->drawRect(shapeRect);
        break;
    case Ellipse:
        painter->drawEllipse(shapeRect);
        break;
    case Arrow:
        painter->drawLine(shapeLine);
    {
        QLineF line(shapeLine);
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
        painter->drawLine(shapeLine);
        break;
    }

//    if (option->state & QStyle::State_Selected)
//    {
//        painter->setBrush(Qt::NoBrush);
//        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
//        painter->drawRect(boundingRect());
//    }
}


void CustomShapeItem::setShapeRect(const QRectF &rect)
{
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
