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
#include <QApplication>
#include <QDomDocument>
#include <QBuffer>

using LineConnectionsMap = QMap<QGraphicsLineItem *, QPair<QGraphicsEllipseItem *, QGraphicsEllipseItem *>>;

class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    CustomGraphicsView(QWidget *parent = nullptr);
    void ClearScene();
    void setFixedSizeAndScene(const QSize& size);

    enum DrawingMode {
        None,
        ArrowMode,
        LineMode,
        PolylineMode,
        EllipseMode,
        RectangleMode
    };

    enum Direction {TopLeft = 0, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };
    QList<QPointF> resizeHandlePoints();
    bool isCloseEnough(QPointF const& p1, QPointF const& p2);

    void setDrawingMode(DrawingMode mode) {
        currentMode = mode;
        if (currentItem) {
            scene->removeItem(currentItem);
            delete currentItem;
            currentItem = nullptr;
        }
    }

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

private:
    void RemoveLines();
    void RemoveAllLines();
    void reconnectLines(QList<ArrowLineItem*> lineItems, QMap<int, CustomPixmapItem*> customItems);
    void EmitDebugData(QPoint pos);
    void AddItemToAddStack(QGraphicsItem *item);
    void AddItemToMoveStack(QGraphicsItem *item);

    QPointF startPoint;
    QGraphicsScene *scene;
    QGraphicsItem *currentItem;
    ArrowLineItem *currentLine;
    QPointF lineStartPoint;
    LineConnectionsMap lineConnections;
    QMenu contextMenu;
    QAction *acnDel;
    QAction *acnSetVal;
    QAction *acnResult;
    QGraphicsItem *selectedItem = nullptr;
    QPointF itemStartPosition;
    QUndoStack* UndoStack;

    QPolygonF scaledPolygon(QPolygonF const& old, Direction direction, QPointF const& newPos);
    DrawingMode currentMode;
    QPolygonF myPolygon;
    QPainterPath path;
    static constexpr qreal resizeHandlePointWidth = 5;
    static constexpr qreal closeEnoughDistance = 5;
    bool resizeMode = false;
    Direction scaleDirection;

    QPointF movingStartPosition;
    bool isMoved = false;
    QPolygonF previousPolygon;
    bool isResized = false;

};

#endif // CUSTOMGRAPHICSVIEW_H
