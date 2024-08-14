#include "customitem.h"

CustomItem::CustomItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent),
      myCustomType(None),
      resizeMode(false),
      scaleDirection(TopLeft),
      isMoved(false),
      isResized(false)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
}

QList<QPointF> CustomItem::resizeHandlePoints()
{
    QList<QPointF> points;
    QRectF rect = boundingRect();
    points << rect.topLeft()
           << QPointF(rect.center().x(), rect.top())  // top center
           << rect.topRight()
           << QPointF(rect.left(), rect.center().y()) // left center
           << QPointF(rect.right(), rect.center().y()) // right center
           << rect.bottomLeft()
           << QPointF(rect.center().x(), rect.bottom()) // bottom center
           << rect.bottomRight();
    return points;
}


bool CustomItem::isCloseEnough(const QPointF &p1, const QPointF &p2)
{
    return QLineF(p1, p2).length() < closeEnoughDistance;
}

void CustomItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    movingStartPosition = event->pos();
    isMoved = false;
    isResized = false;

    QList<QPointF> points = resizeHandlePoints();
    for (int i = 0; i < points.size(); ++i) {
        if (isCloseEnough(event->pos(), points.at(i))) {
            resizeMode = true;
            scaleDirection = static_cast<Direction>(i);
            break;
        }
    }

    if (!resizeMode) {
        QGraphicsPolygonItem::mousePressEvent(event);
    }
}

void CustomItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (resizeMode) {
        QPolygonF newPolygon = scaledPolygon(myPolygon, scaleDirection, event->pos());
        setPolygon(newPolygon);
        isResized = true;
    } else {
        isMoved = true;
        QGraphicsPolygonItem::mouseMoveEvent(event);
    }
}

void CustomItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!resizeMode && isMoved) {
        // Finalize movement
    } else if (resizeMode && isResized) {
        // Finalize resizing
    }

    resizeMode = false;
    isMoved = false;
    isResized = false;
    QGraphicsPolygonItem::mouseReleaseEvent(event);
}

void CustomItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QList<QPointF> points = resizeHandlePoints();
    bool overHandle = false;

    for (const QPointF &point : points) {
        if (isCloseEnough(event->pos(), point)) {
            overHandle = true;
            break;
        }
    }

    if (overHandle) {
        setCursor(Qt::SizeFDiagCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsPolygonItem::hoverMoveEvent(event);
}

void CustomItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    switch (myCustomType) {
        case Rectangle:
            painter->drawRect(myPolygon.boundingRect());
            break;
        case Circle:
            painter->drawEllipse(myPolygon.boundingRect());
            break;
        case Line:
            if (myPolygon.size() > 1) {
                painter->drawLine(myPolygon.at(0), myPolygon.at(1));
            }
            break;
        case PolygonLine:
            painter->drawPolygon(myPolygon);
            break;
        case ArrowLine:
            if (myPolygon.size() > 1) {
                painter->drawLine(myPolygon.at(0), myPolygon.at(1));
                // Drawing arrowhead can be added here if needed
            }
            break;
        case None:
        default:
            // No specific shape to draw
            break;
    }

    // Call the base class implementation to draw selection/hover indicators
    QGraphicsPolygonItem::paint(painter, option, widget);

    // Draw resize handles if selected
    if (isSelected()) {
        painter->setPen(QPen(Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());

        QList<QPointF> points = resizeHandlePoints();
        painter->setPen(QPen(Qt::SolidLine));
        painter->setBrush(Qt::black);

        for (const QPointF &point : points) {
            QRectF handleRect(point.x() - resizeHandlePointWidth / 2, point.y() - resizeHandlePointWidth / 2, resizeHandlePointWidth, resizeHandlePointWidth);
            painter->drawRect(handleRect);
        }
    }
}


QPolygonF CustomItem::scaledPolygon(const QPolygonF &old, Direction direction, const QPointF &newPos)
{
    QPolygonF newPolygon = old;

    QRectF rect = old.boundingRect();
    switch (direction) {
        case TopLeft:
            rect.setTopLeft(newPos);
            break;
        case Top:
            rect.setTop(newPos.y());
            break;
        case TopRight:
            rect.setTopRight(newPos);
            break;
        case Left:
            rect.setLeft(newPos.x());
            break;
        case Right:
            rect.setRight(newPos.x());
            break;
        case BottomLeft:
            rect.setBottomLeft(newPos);
            break;
        case Bottom:
            rect.setBottom(newPos.y());
            break;
        case BottomRight:
            rect.setBottomRight(newPos);
            break;
    }

    QPolygonF scaledPolygon;
    for (const QPointF &point : old) {
        qreal xRatio = (point.x() - rect.left()) / rect.width();
        qreal yRatio = (point.y() - rect.top()) / rect.height();
        scaledPolygon << QPointF(rect.left() + xRatio * rect.width(), rect.top() + yRatio * rect.height());
    }

    return scaledPolygon;
}
