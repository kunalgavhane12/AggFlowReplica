#ifndef RESIZEHANDLE_H
#define RESIZEHANDLE_H

#include <QGraphicsEllipseItem>
#include <QBrush>

class ResizeHandle : public QGraphicsEllipseItem {
public:
    ResizeHandle(QGraphicsItem *parent = nullptr);

    void setResizeItem(QGraphicsItem *item);
    QGraphicsItem* resizeItem() const;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsItem *m_resizeItem = nullptr;
    bool m_isResizingStart;
    QPointF m_resizeCorner;
};

#endif // RESIZEHANDLE_H
