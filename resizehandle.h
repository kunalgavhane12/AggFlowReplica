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
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    QGraphicsItem *m_resizeItem = nullptr;
    bool isResizingStart;
};

#endif // RESIZEHANDLE_H
