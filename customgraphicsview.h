#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QPointF>
#include <QMap>
#include <QPolygonF>
#include <QPainterPath>
#include "CustomPixmapItem.h"
#include "arrowlineitem.h"
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QUndoStack>
#include <QMessageBox>
#include <QIcon>
#include <QInputDialog>
#include <addcommand.h>
#include <QDebug>
#include <QDomDocument>
#include <QBuffer>
#include "customshapeitem.h"
#include <QPen>
#include <QBrush>

using LineConnectionsMap = QMap<QGraphicsLineItem *, QPair<QGraphicsEllipseItem *, QGraphicsEllipseItem *>>;

class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    CustomGraphicsView(QWidget *parent = nullptr);
    void ClearScene();
    void setFixedSizeAndScene(const QSize& size);

    void setShapeType(CustomShapeItem::ShapeType shape);
    void handleProxyWidgetInteraction(const QPointF &scenePos, QGraphicsProxyWidget *proxyWidget);
    void handleEllipseInteraction(const QPointF &scenePos, QGraphicsEllipseItem *ellipseItem);
    void handleItemInteraction(const QPointF &scenePos, QGraphicsItem *item);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event)override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void UndoTriggered();
    void RedoTriggered();
    void PublishOldData(QString data);
    void PublishNewData(QString data);
    void PublishUndoData(QString data);
    void PublishRedoData(QString data);
    void resultUpdated(const QString &result);

private slots:
    void updateLinePosition();
    void onSetValue();

public slots:
    void onActionDelete();
    void saveToFile(const QString &fileName);
    void loadFromFile(const QString &fileName);
    void onResult();
    void saveToXml(const QString &fileName);
    void loadFromXml(const QString &fileName);
    void onActionTopTriggered();
    void onActionBottomTriggered();
    void onActionLeftTriggered();
    void onActionRightTriggered();

private:
    void RemoveLines();
    void RemoveAllLines();
    void reconnectLines(QList<ArrowLineItem*> lineItems, QMap<int, CustomPixmapItem*> customItems);
    void EmitDebugData(QPoint pos);
    void AddItemToAddStack(QGraphicsItem *item);
    void AddItemToMoveStack(QGraphicsItem *item);

    LineConnectionsMap lineConnections;
    QGraphicsScene *scene;
    QGraphicsItem *currentItem = nullptr;
    QGraphicsItem *selectedItem = nullptr;
    ArrowLineItem *currentLine = nullptr;
    QPointF startPoint;
    QPointF lineStartPoint;
    QPointF itemStartPosition;
    QMenu contextMenu;
    QAction *acnDel;
    QAction *acnSetVal;
    QAction *acnResult;
    QUndoStack* UndoStack;

    CustomShapeItem *currentItem1 = nullptr;
    bool drawing;
    bool selection;
    bool resizing;
    CustomShapeItem::ShapeType shapeType;
    QPointF origin;

    CustomShapeItem* resizableShapeItem = nullptr;
    QGraphicsEllipseItem* handle = nullptr;
    QList<QGraphicsEllipseItem*> handles;
    QList<QGraphicsEllipseItem*> endpoints;

    void addBlueHandles(CustomShapeItem* shapeItem)
    {

        QRectF rect = shapeItem->boundingRect();

        handles.append(addHandleAt(rect.topLeft(), Qt::blue, shapeItem));
        handles.append(addHandleAt(rect.topRight(), Qt::blue, shapeItem));
        handles.append(addHandleAt(rect.bottomLeft(), Qt::blue, shapeItem));
        handles.append(addHandleAt(rect.bottomRight(), Qt::blue, shapeItem));
    }

    QGraphicsEllipseItem* addHandleAt(const QPointF& position, const QColor& color, QGraphicsItem* parent)
    {
        QGraphicsEllipseItem* handle = new QGraphicsEllipseItem(-5, -5, 10, 10, parent);
        handle->setBrush(QBrush(color));
        handle->setPen(QPen(color));
        handle->setPos(position);
        scene->addItem(handle);
        return handle;
    }


    void updateHandlesPosition(CustomShapeItem* shapeItem)
    {
        if (!shapeItem || handles.isEmpty())
        {
            return;
        }
        QRectF rect = shapeItem->getShapeRect();
        handles[0]->setPos(rect.topLeft() - QPointF(3, 3));
        handles[1]->setPos(rect.topRight() - QPointF(3, 3));
        handles[2]->setPos(rect.bottomLeft() - QPointF(3, 3));
        handles[3]->setPos(rect.bottomRight() - QPointF(3, 3));
    }

    void updateBlueHandles(CustomShapeItem* shapeItem)
    {
        if (!shapeItem)
        {
            return;
        }

        QRectF rect = shapeItem->getShapeRect();
        if (handles.size() == 4)
        {
            handles[0]->setPos(rect.topLeft() - QPointF(3, 3));
            handles[1]->setPos(rect.topRight() - QPointF(3, 3));
            handles[2]->setPos(rect.bottomLeft() - QPointF(3, 3));
            handles[3]->setPos(rect.bottomRight() - QPointF(3, 3));
        }
    }

    void removeHandles()
    {
        for (QGraphicsEllipseItem* h : handles)
        {
            scene->removeItem(h);
            delete h;
        }
        handles.clear();
    }
};

#endif // CUSTOMGRAPHICSVIEW_H
