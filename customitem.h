#ifndef customitem_H
#define customitem_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTextEdit>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPolygonF>
#include "customtextitem.h"

class CustomItem : public QGraphicsPolygonItem
{    

public:

    enum CustomType { None, Rectangle, Circle, Line, PolygonLine, ArrowLine};

    enum Direction {TopLeft = 0, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };

    CustomItem(QGraphicsItem *parent = nullptr);

    QList<QPointF> resizeHandlePoints();
    bool isCloseEnough(QPointF const& p1, QPointF const& p2);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPolygonF scaledPolygon(QPolygonF const& old, Direction direction, QPointF const& newPos);

    CustomType myCustomType;
    QPolygonF myPolygon;
    static constexpr qreal resizeHandlePointWidth = 5;
    static constexpr qreal closeEnoughDistance = 5;
    bool resizeMode = false;
    Direction scaleDirection;

    QPointF movingStartPosition;
    bool isMoved = false;
    QPolygonF previousPolygon;
    bool isResized = false;

};

#endif // customitem_H
