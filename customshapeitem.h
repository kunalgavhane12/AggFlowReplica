#ifndef CUSTOMSHAPEITEM_H
#define CUSTOMSHAPEITEM_H

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QLineF>
#include <QRectF>
#include <QList>
#include <QPen>

class CustomShapeItem : public QGraphicsItem
{
public:
    enum ShapeType {
        ConvLine,
        ConvReverseLine,
        Line,
        Rectangle,
        Ellipse,
        Arrow,
        PolygonLine
    };

    enum HandleType {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    explicit CustomShapeItem(ShapeType shapeType, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void setShapeRect(const QRectF &rect);
    QRectF getShapeRect() const;

    void setShapeLine(const QLineF &line);
    QLineF getShapeLine() const;

    void setPen(const QPen &pen) { m_pen = pen; update(); }
    QPen pen() const { return m_pen; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void addHandles();
    void removeHandles();
    void updateHandles();
    HandleType handleAt(const QPointF &point) const;
    void resizeShape(HandleType handleType, const QPointF &newPos);

    ShapeType shapeType;
    QRectF shapeRect;
    QLineF shapeLine;
    HandleType m_currentHandle;
    QPen m_pen;
};

#endif // CUSTOMSHAPEITEM_H
