#include "CustomGraphicsView.h"
#include <QGraphicsProxyWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDataStream>
#include "adjustfeedstream.h"
#include "ui_adjustfeedstream.h"
#include "adjustfeeder.h"
#include "conveyorcalculation.h"
#include "resizehandle.h"
#include "splitter.h"
#include "crushingequipment.h"
#include "adjustpassthroughsurgebin.h"
#include "screeningequipment.h"
#include "endproduct.h"
#include "cleanwaterequipment.h"
#include "measurementequipment.h"
#include "powersourcesandauxiliaryequipment.h"

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , currentLine(nullptr)
    , UndoStack(new QUndoStack(this))
    , drawing(false)
    , selection(false)
{
    setScene(scene);
    setAcceptDrops(true);
    setRenderHints(QPainter::HighQualityAntialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setFixedSizeAndScene(QSize(800, 600));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    acnDel = new QAction(tr("Delete line"), this);
    acnDel->setShortcuts(QKeySequence::Delete);
    acnSetVal = new QAction(tr("Set value"), this);

    connect(acnDel, &QAction::triggered, this, &CustomGraphicsView::onActionDelete);
    connect(acnSetVal, &QAction::triggered, this, &CustomGraphicsView::onSetValue);
    connect(this, &CustomGraphicsView::UndoTriggered, UndoStack, &QUndoStack::undo);
    connect(this, &CustomGraphicsView::RedoTriggered, UndoStack, &QUndoStack::redo);
}

void CustomGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
        event->acceptProposedAction();
    }
}

void CustomGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
        event->acceptProposedAction();
    }
}

void CustomGraphicsView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
        QByteArray encodedData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int row, col;
        QMap<int, QVariant> roleDataMap;
        stream >> row >> col >> roleDataMap;

        QIcon icon = qvariant_cast<QIcon>(roleDataMap[Qt::UserRole + 1]);
        QString itemName = qvariant_cast<QString>(roleDataMap[(Qt::ToolTipRole)]);
        QPixmap pixmap = icon.pixmap(64, 64);
        CustomPixmapItem* item = new CustomPixmapItem(pixmap, itemName);
        item->setPos(mapToScene(event->pos()));
        scene->addItem(item);
        connect(item, &CustomPixmapItem::positionChanged, this, &CustomGraphicsView::updateLinePosition);

        EmitDebugData(event->pos());
        AddItemToAddStack(item);

        event->acceptProposedAction();
    }
}


//void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {

//    startPoint = mapToScene(event->pos());

//    if(drawing){
//        switch (shapeType) {
//        case CustomShapeItem::Rectangle:
//            rectItem = new QGraphicsRectItem();
//            rectItem->setPen(QPen(Qt::black));
//            scene->addItem(rectItem);
//            break;
//        case CustomShapeItem::Ellipse:
//            ellipseItem = new QGraphicsEllipseItem();
//            ellipseItem->setPen(QPen(Qt::black));
//            scene->addItem(ellipseItem);
//            break;
//        case CustomShapeItem::Line:
//            lineItem = new QGraphicsLineItem();
//            lineItem->setPen(QPen(Qt::red, 2));
//            scene->addItem(lineItem);
//            break;

//        default:
//            break;
//        }
//    }
//}

//void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {

//    QPointF endPoint = mapToScene(event->pos());
//    if(drawing)
//    {
//        if (rectItem)
//        {
//            QRectF rect(startPoint, endPoint);
//            rectItem->setRect(rect.normalized());
//        }
//        else if (ellipseItem)
//        {
//            QRectF rect(startPoint, endPoint);
//            ellipseItem->setRect(rect.normalized());
//        }
//        else if (lineItem)
//        {
//            QLineF line(startPoint, endPoint);
//            lineItem->setLine(line);
//        }
//    }
//}

//void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event) {

//    if(drawing)
//    {
//        selection = true;
//    }
//    QPointF scenePos = mapToScene(event->pos());
//    QGraphicsItem *item = scene->itemAt(scenePos, QTransform());

//    if(selection){
//    // Clear previous handles
//    for (auto handle : handles) {
//        scene->removeItem(handle);
//        //            delete handle;
//    }
//    handles.clear();

//    if (item) {
//        // Get the bounding rect in the item's coordinate system
//        QRectF boundingRect = item->boundingRect();
//        // Map the bounding rect corners to scene coordinates
//        QPointF topLeft = item->mapToScene(boundingRect.topLeft());
//        QPointF bottomRight = item->mapToScene(boundingRect.bottomRight());

//        // Add resize handles at each corner
//        for (const auto &corner : {topLeft, QPointF(topLeft.x(), bottomRight.y()), QPointF(bottomRight.x(), topLeft.y()), bottomRight}) {
//            ResizeHandle *handle = new ResizeHandle();
//            handle->setResizeItem(item);
//            handle->setPos(item->mapFromScene(corner));  // Set the handle position relative to the item's coordinate system
//            scene->addItem(handle);
//            handles.append(handle);
//        }
//    }

//    // Handle other modes
//    if (rectItem) {
//        rectItem->setFlag(QGraphicsItem::ItemIsMovable);
//        rectItem = nullptr;
//    }  else if (ellipseItem) {
//        ellipseItem->setFlag(QGraphicsItem::ItemIsMovable);
//        ellipseItem = nullptr;
//    }  else if (lineItem) {
//        lineItem->setFlag(QGraphicsItem::ItemIsMovable);
//        lineItem = nullptr;
//    }

//    QGraphicsView::mouseReleaseEvent(event);
//    return;
//    }

//}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem *item = scene->itemAt(scenePos, QTransform());

    if (item && dynamic_cast<QGraphicsEllipseItem *>(item))
    {
        handleEllipseInteraction(scenePos, dynamic_cast<QGraphicsEllipseItem *>(item));
    }

    if (event->button() == Qt::LeftButton)
    {

        if (drawing)
        {
            origin = scenePos;
            drawItem = new CustomShapeItem(shapeType);
            drawItem->setFlag(QGraphicsItem::ItemIsMovable, false);
            switch (shapeType)
            {
            case CustomShapeItem::ConvLine:
            case CustomShapeItem::ConvReverseLine:
            case CustomShapeItem::Line:
            case CustomShapeItem::Arrow:
            case CustomShapeItem::PolygonLine:
                drawItem->setShapeLine(QLineF(origin, origin));
                break;
            case CustomShapeItem::Rectangle:
            case CustomShapeItem::Ellipse:
                drawItem->setShapeRect(QRectF(origin, QSizeF(50, 50)));
                break;
            }
            scene->addItem(drawItem);
        }
        else if (selection && item)
        {
            qDebug() << "In selection";
            resizing = true;

            drawItem = dynamic_cast<CustomShapeItem*>(item);
            if (drawItem)
            {
                drawItem->setFlag(QGraphicsItem::ItemIsMovable, true);
                resizableShapeItem = drawItem;
                QRectF originalRect = resizableShapeItem->boundingRect();
                QRectF newRect = originalRect;

                QGraphicsEllipseItem* activeHandle = nullptr;
                for (auto handleItem : handles)
                {
                    if (handleItem->contains(mapFromScene(scenePos)))
                    {
                        activeHandle = handleItem;
                        break;
                    }
                }

                if (activeHandle)
                {
                    if (activeHandle == handles[0]) // Top-left
                    {
                        newRect.setTopLeft(scenePos);
                    }
                    else if (activeHandle == handles[1]) // Top-right
                    {
                        newRect.setTopRight(scenePos);
                    }
                    else if (activeHandle == handles[2]) // Bottom-left
                    {
                        newRect.setBottomLeft(scenePos);
                    }
                    else if (activeHandle == handles[3]) // Bottom-right
                    {
                        newRect.setBottomRight(scenePos);
                    }

                    resizableShapeItem->setShapeRect(newRect);
                    scene->update();
                }
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF endPoint = mapToScene(event->pos());

    if (currentLine)
    {
        QLineF newLine(lineStartPoint, endPoint);
        currentLine->setLine(newLine);
    }

    if (drawing && drawItem)
    {
        QPointF currentPos = mapToScene(event->pos());
        switch (shapeType)
        {
        case CustomShapeItem::ConvLine:
        case CustomShapeItem::ConvReverseLine:
        case CustomShapeItem::Line:
        case CustomShapeItem::Arrow:
        case CustomShapeItem::PolygonLine:
            drawItem->setShapeLine(QLineF(origin, currentPos));
            break;
        case CustomShapeItem::Rectangle:
        case CustomShapeItem::Ellipse:
            drawItem->setShapeRect(QRectF(origin, currentPos));
            break;
        }
        scene->update();
    }
    else if (selection && resizing && !handles.isEmpty())
    {
        drawing = true;
        drawItem->setFlag(QGraphicsItem::ItemIsMovable, false);
        QGraphicsEllipseItem* activeHandle = nullptr;
        for (auto handleItem : handles)
        {
            if (handleItem->contains(mapFromScene(endPoint)))
            {
                activeHandle = handleItem;
                break;
            }
        }

        if (activeHandle)
        {
            QRectF originalRect = resizableShapeItem->boundingRect();
            QRectF newRect = originalRect;

            if (activeHandle == handles[0]) // Top-left
            {
                newRect.setTopLeft(endPoint);
            }
            else if (activeHandle == handles[1]) // Top-right
            {
                newRect.setTopRight(endPoint);
            }
            else if (activeHandle == handles[2]) // Bottom-left
            {
                newRect.setBottomLeft(endPoint);
            }
            else if (activeHandle == handles[3]) // Bottom-right
            {
                newRect.setBottomRight(endPoint);
            }

            resizableShapeItem->setShapeRect(newRect);
            scene->update();
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (currentLine)
    {
        lineConnections[currentLine].first->parentItem()->setFlag(QGraphicsItem::ItemIsMovable, true);
        QPointF scenePos = mapToScene(event->pos());
        QList<QGraphicsItem *> items = scene->items(scenePos);

        bool lineDrawn = false;
        for (auto item : items)
        {
            if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem *>(item))
            {
                QLineF newLine(lineStartPoint, scenePos);
                currentLine->setLine(newLine);
                lineConnections[currentLine].second = ellipseItem;
                currentLine->SetEndCircle(ellipseItem);
                currentLine->SetEndCircleAttributes();
                lineDrawn = true;
                break;
            }
        }
        if (!lineDrawn || (lineConnections[currentLine].first->parentItem() == lineConnections[currentLine].second->parentItem()))
        {
            scene->removeItem(currentLine);
            lineConnections.remove(currentLine);
            delete currentLine;
        }
        else
        {
            AddItemToAddStack(currentLine);
        }
        currentLine = nullptr;
    }

    if (drawing && event->button() == Qt::LeftButton)
    {
        drawing = false;
        selection = true;
        resizing = false;
        if (drawItem)
        {
            drawItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            drawItem = nullptr;
        }
    }
    else if (resizing && event->button() == Qt::LeftButton)
    {
        resizing = false;
        resizableShapeItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        drawItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        resizableShapeItem = nullptr;
        drawItem = nullptr;
    }
    else
    {
        QPointF scenePos = mapToScene(event->pos());
        QList<QGraphicsItem *> items = scene->items(scenePos);
        for (QGraphicsItem *itm : items)
        {
            if (auto cpItm = dynamic_cast<CustomPixmapItem *>(itm))
            {
                emit PublishNewData(QString("(%1, %2)").arg(cpItm->pos().x()).arg(cpItm->pos().y()));
                AddItemToMoveStack(cpItm);
                break;
            }
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void CustomGraphicsView::setShapeType(CustomShapeItem::ShapeType shape)
{
    shapeType = shape;
    drawing = true;
}

void CustomGraphicsView::handleEllipseInteraction(const QPointF &scenePos, QGraphicsEllipseItem *ellipseItem)
{
    QColor ellipseColor = ellipseItem->brush().color();
    if (ellipseColor == Qt::blue || ellipseColor == Qt::red)
    {
        lineStartPoint = scenePos;
        currentLine = new ArrowLineItem(QLineF(lineStartPoint, lineStartPoint));
        scene->addItem(currentLine);
        lineConnections[currentLine].first = ellipseItem;
        currentLine->SetStartCircle(ellipseItem);
        ellipseItem->parentItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
        currentLine->SetStartCircleAttributes();
    }
}

void CustomGraphicsView::onActionTopTriggered()
{
    qDebug()<<"top";
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : qAsConst(allItems))
    {
        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->isSelected())
        {
            qDebug() << "Found a QGraphicsPixmapItem at position:" << pixmapItem->pos();
            pixmapItem->setPos(pixmapItem->x(), 0);
        }
    }
}

void CustomGraphicsView::onActionBottomTriggered()
{
    qDebug()<<"top";
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : allItems)
    {
        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->isSelected())
        {
            qDebug() << "Found a QGraphicsPixmapItem at position:" << pixmapItem->pos();
            pixmapItem->setPos(pixmapItem->x(), scene->height() - pixmapItem->boundingRect().height());
        }
    }
}

void CustomGraphicsView::onActionLeftTriggered()
{
    qDebug()<<"top";
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : qAsConst(allItems))
    {
        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->isSelected())
        {
            qDebug() << "Found a QGraphicsPixmapItem at position:" << pixmapItem->pos();
            pixmapItem->setPos(0, pixmapItem->y());
        }
    }
}

void CustomGraphicsView::onActionRightTriggered()
{
    qDebug()<<"top";
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : qAsConst(allItems))
    {
        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->isSelected())
        {
            qDebug() << "Found a QGraphicsPixmapItem at position:" << pixmapItem->pos();
            pixmapItem->setPos(scene->width() - pixmapItem->boundingRect().width(), pixmapItem->y());
        }
    }
}

void CustomGraphicsView::handleProxyWidgetInteraction(const QPointF &scenePos, QGraphicsProxyWidget *proxyWidget)
{
    auto selectedItem = scene->selectedItems();
    for (auto item : selectedItem)
    {
        item->setSelected(false);
    }
    proxyWidget->setSelected(true);
    itemStartPosition = proxyWidget->scenePos();
    emit PublishNewData(QString("(%1, %2)").arg(scenePos.x()).arg(scenePos.y()));
}

void CustomGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    contextMenu.clear();
    QList<QGraphicsItem *> lst = items(event->pos());
    for(QGraphicsItem* item:lst)
    {
        CustomPixmapItem *widget = dynamic_cast<CustomPixmapItem *>(item);
        if(widget)
        {
            contextMenu.addAction(acnSetVal);
            selectedItem = widget;
        }
    }
    contextMenu.exec(event->globalPos());
}

void CustomGraphicsView::updateLinePosition()
{
    for (auto it = lineConnections.begin(); it != lineConnections.end(); ++it)
    {
        QGraphicsLineItem *line = it.key();
        QGraphicsEllipseItem *StartCircle = it.value().first;
        QGraphicsEllipseItem *EndCircle = it.value().second;

        if (StartCircle && EndCircle)
        {
            line->setLine(QLineF(StartCircle->scenePos(), EndCircle->scenePos()));
        }
    }
}

void CustomGraphicsView::ClearScene()
{
    RemoveAllLines();
    scene->clear();
    UndoStack->clear();
    emit PublishUndoData(QString());
    emit PublishRedoData(QString());
    emit PublishNewData(QString());
    emit PublishOldData(QString());
}

void CustomGraphicsView::setFixedSizeAndScene(const QSize &size)
{
    setFixedSize(size);
    scene->setSceneRect(0, 0, size.width(), size.height());
}

void CustomGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu.clear();
    ArrowLineItem *line = dynamic_cast<ArrowLineItem *>(itemAt(event->pos()));
    if (line)
    {
        // Add actions to the context menu
        contextMenu.addAction(acnDel);
        selectedItem = line;
        // Show the context menu at the cursor position
    }
    contextMenu.exec(event->globalPos());
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    setTransformationAnchor(AnchorUnderMouse);
    double scalefactor = 1.5;

    if(event->modifiers() & Qt::ControlModifier)
    {
        if(event->delta() > 0)
        {
            scale(scalefactor,scalefactor);
        }
        else
        {
            scale(1/scalefactor,1/scalefactor);
        }
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void CustomGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    scene->setSceneRect(0, 0, event->size().width(), event->size().height());
}

//remove lines and break connections . Remember to delete pointers
void CustomGraphicsView::RemoveLines()
{
    ArrowLineItem * arrowLine = dynamic_cast<ArrowLineItem *>(selectedItem);
    lineConnections.remove(arrowLine);
}

//remove lines and break connections . Remember to delete pointers
void CustomGraphicsView::RemoveAllLines()
{
    for (auto it = lineConnections.begin(); it != lineConnections.end(); ++it)
    {
        delete it.key();
    }

    lineConnections.clear();
}

void CustomGraphicsView::onActionDelete()
{
    if (selectedItem)
    {
        scene->removeItem(selectedItem);
        RemoveLines();
        delete selectedItem;
        selectedItem = nullptr;
    }
}

void CustomGraphicsView::onSetValue()
{
    QStringList startPointsList {"start_points_loader", "start_points_dump_truck", "start_points_excavator",
                                 "start_points_bull_dozer", "start_points_dredge", "start_points_generic_material_source"
                                 /*,"start_points_start_sugar_pile"*/};

    QStringList inLineEquipmentList {"apron_feeder", "feeder", "belt_feeder", "suger_bin", "pan_feeder",
                                     "suger_bin_with_feeder", "dust_collector"};
    QStringList transportList {"place_a_conveyor_in_the_flow", "place_a_reversible_conveyor_in_the_flow",
                               "place_a_haul_truck_in_the_flow", "place_a_front_end_loader_in_the_flow",
                               "place_a_front_end_loader_in_the_flow","place_a_surge_bin_in_the_flow",
                               "bucket_elevator", "screw_conveyor" };

    QStringList splitterList {"place_a_splitter_in_the_flow", "place_a_three_way_splitter_in_the_flow",
                              "place_a_flop_gate_in_the_flow","place_an_overflow_box_in_the_flow",
                              "place_a_finger_gate_in_the_flow"};
    QStringList crushingList {"place_a_jaw_crusher_in_the_flow", "place_a_cone_crusher_in_the_flow",
                              "place_an_hsi_crusher_in_the_flow","place_a_vsi_crusher_in_the_flow",
                              "place_a_roll_crusher_in_the_flow", "place_a_mill_crusher_in_the_flow" };

    QStringList screeningList {"grizzly_feeder_vibrating_scalper", "1deck", "2deck", "3deck","4deck","5deck",
                               "place_a_custom_screen_in_the_flow_trommel_or_banana",
                               "place_a_custom_multi_screen_or_split_deck_screen_in_the_flow","air_seperator" };

    QStringList mobileList {"place_a_mobile_jaw_on_the_worksheet", "place_a_mobile_cone_on_the_worksheet",
                            "place_a_mobile_hsi_on_the_worksheet","place_a_mobile_vsi_on_the_worksheet",
                            "place_a_mobile_screen_on_the_worksheet", "place_a_mobile_wash_unit_on_the_worksheet",
                            "place_a_mobile_conveyor_on_the_worksheet"};

    QStringList washList {"scrubbing_and_attrition_equipment", "classification_equipment",
                          "sand_washing_dewatering","place_an_overflow_box_in_the_flow","mixing_box",
                          "place_a_slurry_box_in_the_flow", "slurry_pump", "slurry_valve",
                          "water_treatment_or_recycling_recover_water" };

    QStringList inventoryList {"inventory_suger_pile_with_feeders", "inventory_suger_bin_with_feeders"};

    QStringList endProductList {"end_point_product_pile", "end_point_haul_truck",
                                "end_point_haul_railway_transport","end_point_haul_water_transport"};

    QStringList cleanWaterList {"clean_water_source", "clean_Watersource", "water_pump","water_splitter",
                                "waterSplitter", "water_tank", "water_valve", "water_spray_nozzles" };

    QStringList measurementList {"sample_bucket" };

    QStringList powerSourceList {"external_power_source", "internal_power_source", "fuel_tank","personnel"};

    CustomPixmapItem* item = dynamic_cast<CustomPixmapItem *>(selectedItem);

    if(item)
    {
        if (startPointsList.contains(item->GetItemName()))
        {
            AdjustFeedStream *feedStream = new AdjustFeedStream();
            feedStream->show();
        }
        else if ("start_points_start_sugar_pile" == item->GetItemName())
        {
            AdjustFeedStream *feedStream = new AdjustFeedStream();
            feedStream->setWindowTitle("Adjust Feed Stream For Multiple Outputs");
            feedStream->on_clear();
            feedStream->show();
        }
        else if (inLineEquipmentList.contains(item->GetItemName()))
        {
            AdjustFeeder *feeder = new AdjustFeeder();
            feeder->setWindowTitle(item->GetItemName());
            feeder->show();
        }
        else if(transportList.contains(item->GetItemName()))
        {
            ConveyorCalculation *conveyor = new ConveyorCalculation();
            conveyor->show();
        }
        else if(splitterList.contains(item->GetItemName()))
        {
            Splitter *splitter = new Splitter();
            //           splitter->setWindowTitle(item->GetItemName());
            splitter->show();
        }
        else if(crushingList.contains(item->GetItemName()))
        {
            CrushingEquipment *crushing = new CrushingEquipment();
            crushing->show();
        }
        else if(screeningList.contains(item->GetItemName()))
        {
            ScreeningEquipment *screen = new ScreeningEquipment();
            screen->show();
        }
        else if(mobileList.contains(item->GetItemName()))
        {
            CrushingEquipment *crushing = new CrushingEquipment();
            //           crushing->setWindowTitle(item->GetItemName());
            crushing->show();
        }
        else if(washList.contains(item->GetItemName()))
        {
            CrushingEquipment *crushing = new CrushingEquipment();
            crushing->setWindowTitle(item->GetItemName());
            crushing->show();
        }
        else if(inventoryList.contains(item->GetItemName()))
        {
            AdjustPassThroughSurgeBin *surgeBin = new AdjustPassThroughSurgeBin();
            surgeBin->show();
        }
        else if(endProductList.contains(item->GetItemName()))
        {
            EndProduct *endProduct = new EndProduct();
            endProduct->show();
        }
        else if(cleanWaterList.contains(item->GetItemName()))
        {
            CleanWaterEquipment *cleanWter = new CleanWaterEquipment();
            cleanWter->show();
        }
        else if(measurementList.contains(item->GetItemName()))
        {
            MeasurementEquipment *measurement = new MeasurementEquipment();
            measurement->show();
        }
        else if(powerSourceList.contains(item->GetItemName()))
        {
            PowerSourcesandAuxiliaryEquipment *powerSource = new PowerSourcesandAuxiliaryEquipment();
            powerSource->setWindowTitle(item->GetItemName());
            powerSource->show();
        }
        //        double value = QInputDialog::getDouble(this, "Enter Value:", "Operation:", 0, 0, 1000, 2, nullptr);
        //        item->SetText(QString::number(value));
    }
}

void CustomGraphicsView::onResult()
{
    double result = 0.0;
    QSet<CustomPixmapItem*> visitItems;

    for (auto it = lineConnections.begin(); it != lineConnections.end(); ++it)
    {
        QGraphicsEllipseItem *startEllipse = it.value().first;
        QGraphicsEllipseItem *endEllipse = it.value().second;

        if (startEllipse && endEllipse)
        {
            CustomPixmapItem *startItem = dynamic_cast<CustomPixmapItem *>(startEllipse->parentItem());
            CustomPixmapItem *endItem = dynamic_cast<CustomPixmapItem *>(endEllipse->parentItem());

            int endId = endItem->GetItemId();
            int n;

            if(endId > 4){
                n = endId % 4;
            }else{
                n = endId;
            }

            switch(n){
            case 1 :
                result += startItem->GetText().toDouble() +  endItem->GetText().toDouble();
                visitItems.insert(startItem);
                qDebug()<<"case 1 :"<<result;
                break;
            case 2 :
                result += startItem->GetText().toDouble() * endItem->GetText().toDouble();
                visitItems.insert(endItem);
                qDebug()<<"case 1 :"<<result;
                break;
            case 3 :
                result += startItem->GetText().toDouble() / endItem->GetText().toDouble();
                visitItems.insert(endItem);
                qDebug()<<"case 1 :"<<result;
                break;
            default:
                result += startItem->GetText().toDouble() - endItem->GetText().toDouble();
                qDebug()<<"case 1 :"<<result;
                break;
            }
        }
    }

    emit resultUpdated(QString::number(result));
}

void CustomGraphicsView::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Could not open file for writing");
        return;
    }

    QDataStream out(&file);
    // Save all CustomPixmapItems
    QList<QGraphicsItem *> items = scene->items();
    for (QGraphicsItem *item : items) {
        if (CustomPixmapItem *pixmapItem = dynamic_cast<CustomPixmapItem *>(item)) {
            out << QString("CustomPixmapItem");
            pixmapItem->write(out);
        } else if (ArrowLineItem *lineItem = dynamic_cast<ArrowLineItem *>(item)) {
            out << QString("ArrowLineItem");
            lineItem->write(out);
        }
    }

    QMessageBox msgBox;
    msgBox.setText("Data Saved Succesfully!!!");
    msgBox.exec();
}

void CustomGraphicsView::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open file for reading");
        return;
    }

    QDataStream in(&file);
    scene->clear();
    lineConnections.clear();

    QList<ArrowLineItem*> lineItems;
    QMap<int, CustomPixmapItem*> customItems;
    while (!in.atEnd()) {
        QString itemType;
        in >> itemType;

        if (itemType == "CustomPixmapItem") {
            CustomPixmapItem *pixmapItem = new CustomPixmapItem(QPixmap(),"");
            pixmapItem->read(in);
            pixmapItem->HideLabelIfNeeded();
            scene->addItem(pixmapItem);
            customItems.insert(pixmapItem->GetItemId(), pixmapItem);
            connect(pixmapItem, &CustomPixmapItem::positionChanged, this, &CustomGraphicsView::updateLinePosition);
        } else if (itemType == "ArrowLineItem") {
            ArrowLineItem *lineItem = new ArrowLineItem(QLineF());
            lineItem->read(in);
            scene->addItem(lineItem);
            lineItems.append(lineItem);
        }
    }

    reconnectLines(lineItems, customItems);
}

void CustomGraphicsView::saveToXml(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Scene");

    // Serialize CustomPixmapItems
    for (QGraphicsItem* item : scene->items())
    {
        if (CustomPixmapItem* pixmapItem = dynamic_cast<CustomPixmapItem*>(item))
        {
            pixmapItem->saveToXml(xmlWriter);
        }
    }

    // Serialize ArrowLineItems
    for (QGraphicsItem* item : scene->items())
    {
        if (ArrowLineItem* arrowItem = dynamic_cast<ArrowLineItem*>(item))
        {
            arrowItem->saveToXml(xmlWriter);
        }
    }

    xmlWriter.writeEndElement(); // Scene
    xmlWriter.writeEndDocument();

    QMessageBox msgBox;
    msgBox.setText("Data in Xml Saved Successfully!");
    msgBox.exec();
}

void CustomGraphicsView::loadFromXml(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open file for reading");
        return;
    }

    QXmlStreamReader xmlReader(&file);
    scene->clear();
    lineConnections.clear();

    QList<ArrowLineItem*> lineItems;
    QMap<int, CustomPixmapItem*> customItems;

    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "CustomPixmapItem") {
                CustomPixmapItem *pixmapItem = new CustomPixmapItem(QPixmap(), "");
                pixmapItem->loadFromXml(xmlReader);
                pixmapItem->HideLabelIfNeeded();
                scene->addItem(pixmapItem);
                customItems.insert(pixmapItem->GetItemId(), pixmapItem);
                connect(pixmapItem, &CustomPixmapItem::positionChanged, this, &CustomGraphicsView::updateLinePosition);
            } else if (xmlReader.name() == "ArrowLineItem") {
                ArrowLineItem *lineItem = new ArrowLineItem(QLineF());
                lineItem->loadFromXml(xmlReader);
                scene->addItem(lineItem);
                lineItems.append(lineItem);
            }
        }
    }

    if (xmlReader.hasError()) {
        qWarning("XML error: %s", xmlReader.errorString().toStdString().c_str());
    }

    xmlReader.clear();
    reconnectLines(lineItems, customItems);
}

void CustomGraphicsView::reconnectLines(QList<ArrowLineItem*> lineItems, QMap<int, CustomPixmapItem*> customItems)
{
    for (ArrowLineItem* line : lineItems) {
        CustomPixmapItem* startItem = customItems.value(line->GetStartCircleItemId(), nullptr);
        CustomPixmapItem* endItem = customItems.value(line->GetEndCircleItemId(), nullptr);

        if (startItem && endItem)
        {
            if(line->GetIsStartCircleStartConnected())
            {
                line->SetStartCircle(customItems[line->GetStartCircleItemId()]->GetStartCircle());
            }
            if(line->GetIsStartCircleEndConnected())
            {
                line->SetStartCircle(customItems[line->GetStartCircleItemId()]->GetEndCircle());
            }
            if(line->GetIsEndCircleStartConnected())
            {
                line->SetEndCircle(customItems[line->GetEndCircleItemId()]->GetStartCircle());
            }
            else if(line->GetIsEndCircleEndConnected())
            {
                line->SetEndCircle(customItems[line->GetEndCircleItemId()]->GetEndCircle());
            }

            if (line->GetStartCircle() && line->GetEndCircle()) {
                lineConnections[line].first = line->GetStartCircle();
                lineConnections[line].second = line->GetEndCircle();
            }
        }
    }

    updateLinePosition();
}

void CustomGraphicsView::EmitDebugData(QPoint pos)
{
    emit PublishUndoData(QString());
    emit PublishRedoData(QString());
    emit PublishNewData(QString());
    emit PublishOldData(QString());
    emit PublishOldData(QString("(%1, %2)").arg(mapToScene(pos).x()).arg(mapToScene(pos).y()));
}

void CustomGraphicsView::AddItemToAddStack(QGraphicsItem* item)
{
    AddCommand* command = new AddCommand(scene, item);
    connect(command, &AddCommand::PublishUndoData, this, &CustomGraphicsView::PublishUndoData);
    connect(command, &AddCommand::PublishRedoData, this, &CustomGraphicsView::PublishRedoData);
    connect(command, &AddCommand::NotifyUndoCompleted, this, &CustomGraphicsView::updateLinePosition);
    connect(command, &AddCommand::NotifyRedoCompleted, this, &CustomGraphicsView::updateLinePosition);
    UndoStack->push(command);
}

void CustomGraphicsView::AddItemToMoveStack(QGraphicsItem* item)
{
    MoveCommand* command = new MoveCommand(item, itemStartPosition, item->scenePos());
    connect(command, &MoveCommand::PublishUndoData, this, &CustomGraphicsView::PublishUndoData);
    connect(command, &MoveCommand::PublishRedoData, this, &CustomGraphicsView::PublishRedoData);
    connect(command, &MoveCommand::NotifyUndoCompleted, this, &CustomGraphicsView::updateLinePosition);
    connect(command, &MoveCommand::NotifyRedoCompleted, this, &CustomGraphicsView::updateLinePosition);
    UndoStack->push(command);
}
