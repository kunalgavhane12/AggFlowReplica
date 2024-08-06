#include "MainWindow.h"
#include <QStringListModel>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMimeData>
#include <QDrag>
#include <QMenuBar>
#include <QLabel>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QGroupBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(new QWidget(this))
    , tabPlant(new QTabWidget(this))
    , tabPage(new QTabWidget(this))
    , listView(new QListView(this))
    , menuListView(new QListView(this))
    , delegate(new CustomDelegate(40, this))
    , graphicsView(new CustomGraphicsView(this))
    , oldData(new QLabel)
    , newData(new QLabel)
    , UndoData(new QLabel)
    , RedoData(new QLabel)
    , status(new QLabel(this))
    , currentFile("saveTest.scene")
    , zoomFactor(1.5)
{
    SetupUI();
    createActions();
    createMenus();
    createToolbar();

    runAction = new QAction("Run", this);
    menuBar()->addAction(runAction);
    connect(runAction, &QAction::triggered, graphicsView, &CustomGraphicsView::onResult);

    exportAction = new QAction("Export", this);
    menuBar()->addAction(exportAction);
    connect(exportAction, &QAction::triggered, this, &MainWindow::onSave);

    status->setText("<span style='font-size: 16px; font-weight: bold;'>Result : 0" "</span>");
    statusBar()->addPermanentWidget(status);
}

void MainWindow::onClear()
{
    graphicsView->ClearScene();
    CustomPixmapItem::GlobalItemId = 0;
    status->setText("Result : 0");
    statusBar()->showMessage(tr("Scene cleared"), 2000);
}

void MainWindow::newFile()
{
    graphicsView->ClearScene();
    setCurrentFile(QString());
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("XML Files (*.xml);;Scene Files(*.scene)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Open File"), tr("Cannot open file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }
    setCurrentFile(fileName);
    onLoad();
}

void MainWindow::SetupUI()
{
    QStringList menulabels = {"Start point", "In line Equipment", "Transport Equipment", "Spiltters and flop_gates",
                              "Crushing Equipment", "Screening Equipment", "Mobile Equipment", "Wash Equipment",
                              "Inventory (temporary storage)", "End Products", "Clean Water Equipment", "Measurement Equipment", "Power Sources and Auxiliary Equipment",
                              "Notes Coloring and Drawing"};
    QList<QIcon> menuIcons;
    menuIcons << QIcon(":/icons/images/start_point.png")
              << QIcon(":/icons/images/in_line_Equipment.png")
              << QIcon(":/icons/images/transport_Equipment.png")
              << QIcon(":/icons/images/spiltters_and_flop_gates.png")
              << QIcon(":/icons/images/crushing_Equipment.png")
              << QIcon(":/icons/images/screening_Equipment.png")
              << QIcon(":/icons/images/mobile_Equipment.png")
              << QIcon(":/icons/images/wash_Equipment.png")
              << QIcon(":/icons/images/inventory_(temporary storage).png")
              << QIcon(":/icons/images/end_Products.png")
              << QIcon(":/icons/images/clean_Water_Equipment.png")
              << QIcon(":/icons/images/measurement_Equipment.png")
              << QIcon(":/icons/images/power_Sources_and_Auxiliary_Equipment.png")
              << QIcon(":/icons/images/notes_Coloring_and_Drawing.png");

    IconListModel *menuModel = new IconListModel(this);
    menuModel->setData(menulabels, menuIcons);
    menuListView->setModel(menuModel);
    menuListView->setFixedWidth(70);
    menuListView->setIconSize(QSize(40, 40));

    IconListModel *model = new IconListModel(this);
    QStringList labels = {"Item 0", "Item 1", "Item 2", "Item 3", "Item 4", "Item 5", "Item 6"};
    QList<QIcon> icons;
    icons = {QIcon(":/icons/dragIcon/start_points_loader.png"),
             QIcon(":/icons/dragIcon/start_points_dump_truck.png"),
             QIcon(":/icons/dragIcon/start_points_excavator.png"),
             QIcon(":/icons/dragIcon/start_points_bull_dozer.png"),
             QIcon(":/icons/dragIcon/start_points_dredge.png"),
             QIcon(":/icons/dragIcon/start_points_generic_material_source.png"),
             QIcon(":/icons/dragIcon/start_points_start_suger_pile.png")};

    model->setData(labels, icons);
    listView->setModel(model);
    listView->setFixedWidth(70);
    listView->setIconSize(QSize(40, 40));
    listView->setItemDelegate(delegate);
    listView->setDragEnabled(true);

    tabPlant->addTab(tabPage, tr("Plant Stage"));
    tabPage->addTab(graphicsView, tr("Page"));
    tabPlant->setTabsClosable(true);
    tabPage->setTabsClosable(true);

    setCentralWidget(centralWidget);
    QVBoxLayout *vlayout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout(centralWidget);
    QGroupBox *groupBox = new QGroupBox();

    QHBoxLayout *groupBoxLayout = new QHBoxLayout(groupBox);
    groupBoxLayout->addWidget(menuListView);
    groupBoxLayout->addWidget(listView);
    groupBox->setLayout(groupBoxLayout);
    hlayout->setMargin(5);
    hlayout->addLayout(vlayout);
    hlayout->addWidget(groupBox);
    hlayout->addWidget(tabPlant);
    setMinimumSize(800, 600);
    statusBar();

    //connect which index selected according to list view display
    connect(menuListView, &QListView::clicked, this, &MainWindow::onItemClicked);
}


void MainWindow::onItemClicked(const QModelIndex &index)
{
    QStringList labels;
    IconListModel *menuModel = new IconListModel(this);
    QList<QIcon> menuIcons;

    switch (index.row()) {
    case 0:
        menuIcons = {QIcon(":/icons/dragIcon/start_points_loader.png"),
                     QIcon(":/icons/dragIcon/start_points_dump_truck.png"),
                     QIcon(":/icons/dragIcon/start_points_excavator.png"),
                     QIcon(":/icons/dragIcon/start_points_bull_dozer.png"),
                     QIcon(":/icons/dragIcon/start_points_dredge.png"),
                     QIcon(":/icons/dragIcon/start_points_generic_material_source.png"),
                     QIcon(":/icons/dragIcon/start_points_start_suger_pile.png")};
        break;
    case 1:
        menuIcons = {QIcon(":/icons/dragIcon/apron_feeder.png"),
                     QIcon(":/icons/dragIcon/feeder.png"),
                     QIcon(":/icons/dragIcon/belt_feeder.png"),
                     QIcon(":/icons/dragIcon/suger_bin.png"),
                     QIcon(":/icons/dragIcon/pan_feeder.png"),
                     QIcon(":/icons/dragIcon/suger_bin_with_feeder.png"),
                     QIcon(":/icons/dragIcon/dust_collector.png")};
        break;
    case 2:
        menuIcons = {QIcon(":/icons/dragIcon/place_a_conveyor_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_reversible_conveyor_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_haul_truck_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_front_end_loader_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_stock_pile_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_surge_bin_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/bucket_elevator.png"),
                     QIcon(":/icons/dragIcon/screw_conveyor.png")};
        break;
    case 3:
        menuIcons = {QIcon(":/icons/dragIcon/place_a_splitter_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_three_way_splitter_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_flop_gate_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_an_overflow_box_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_finger_gate_in_the_flow.png")};
        break;
    case 4:
        menuIcons = {QIcon(":/icons/dragIcon/place_a_jaw_crusher_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_cone_crusher_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_an_hsi_crusher_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_vsi_crusher_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_roll_crusher_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/place_a_mill_crusher_in_the_flow.png")};
        break;
    case 5:
        menuIcons = {QIcon(":/icons/dragIcon/grizzly_feeder_vibrating_scalper.png"),
                     QIcon(":/icons/dragIcon/1deck.png"),
                     QIcon(":/icons/dragIcon/2deck.png"),
                     QIcon(":/icons/dragIcon/3deck.png"),
                     QIcon(":/icons/dragIcon/4deck.png"),
                     QIcon(":/icons/dragIcon/5deck.png"),
                     QIcon(":/icons/dragIcon/place_a_custom_screen_in_the_flow_trommel_or_banana.png"),
                     QIcon(":/icons/dragIcon/place_a_custom_multi_screen_or_split_deck_screen_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/air_seperator.png")};
        break;
    case 6:
        menuIcons = {QIcon(":/icons/dragIcon/place_a_mobile_jaw_on_the_worksheet.png"),
                     QIcon(":/icons/dragIcon/place_a_mobile_cone_on_the_worksheet.png"),
                     QIcon(":/icons/dragIcon/place_a_mobile_hsi_on_the_worksheet.png"),
                     QIcon(":/icons/dragIcon/place_a_mobile_vsi_on_the_worksheet.png"),
                     QIcon(":/icons/dragIcon/place_a_mobile_screen_on_the_worksheet.png"),
                     QIcon(":/icons/dragIcon/place_a_mobile_wash_unit_on_the_worksheet.png"),
                     QIcon(":/icons/dragIcon/place_a_mobile_conveyor_on_the_worksheet.png")};
        break;
    case 7:
        menuIcons = {QIcon(":/icons/dragIcon/scrubbing_and_attrition_equipment.png"),
                     QIcon(":/icons/dragIcon/classification_equipment.png"),
                     QIcon(":/icons/dragIcon/sand_washing_dewatering.png"),
                     QIcon(":/icons/dragIcon/place_an_overflow_box_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/mixing_box.png"),
                     QIcon(":/icons/dragIcon/place_a_slurry_box_in_the_flow.png"),
                     QIcon(":/icons/dragIcon/slurry_pump.png"),
                     QIcon(":/icons/dragIcon/slurry_valve.png"),
                     QIcon(":/icons/dragIcon/water_treatment_or_recycling_recover_water.png")};
        break;
    case 8:
        menuIcons = {QIcon(":/icons/dragIcon/inventory_suger_pile_with_feeders.png"),
                     QIcon(":/icons/dragIcon/inventory_suger_bin_with_feeders.png")};
        break;
    case 9:
        menuIcons = {QIcon(":/icons/dragIcon/end_point_product_pile.png"),
                     QIcon(":/icons/dragIcon/end_point_haul_truck.png"),
                     QIcon(":/icons/dragIcon/end_point_haul_railway_transport.png"),
                     QIcon(":/icons/dragIcon/end_point_haul_water_transport.png"),
                     QIcon(":/icons/dragIcon/end_point_product_bin.png"),
                     QIcon(":/icons/dragIcon/end_point_blending_station.png")};
        break;
    case 10:
        menuIcons = {QIcon(":/icons/dragIcon/clean_water_source.png"),
                     QIcon(":/icons/dragIcon/clean_Watersource.png"),
                     QIcon(":/icons/dragIcon/water_pump.png"),
                     QIcon(":/icons/dragIcon/water_splitter.png"),
                     QIcon(":/icons/dragIcon/waterSplitter.png"),
                     QIcon(":/icons/dragIcon/water_tank.png"),
                     QIcon(":/icons/dragIcon/water_valve.png"),
                     QIcon(":/icons/dragIcon/water_spray_nozzles.png")};
        break;
    case 11:
        menuIcons = {QIcon(":/icons/dragIcon/sample_bucket.png")};
        break;
    case 12:
        menuIcons = {QIcon(":/icons/dragIcon/external_power_source.png"),
                     QIcon(":/icons/dragIcon/internal_power_source.png"),
                     QIcon(":/icons/dragIcon/fuel_tank.png"),
                     QIcon(":/icons/dragIcon/personnel.png")};
        break;
    case 13:
        menuIcons = {QIcon(":/icons/dragIcon/reset_to_pointer_default_mouse_function.png"),
                     QIcon(":/icons/dragIcon/paint_brush.png"),
                     QIcon(":/icons/dragIcon/select_color_for_paint_brush.png"),
                     QIcon(":/icons/dragIcon/erase_color.png"),
                     QIcon(":/icons/dragIcon/multi_line_notes.png"),
                     QIcon(":/icons/dragIcon/adjustable_text.png"),
                     QIcon(":/icons/dragIcon/arrow.png"),
                     QIcon(":/icons/dragIcon/line.png"),
                     QIcon(":/icons/dragIcon/polyline.png"),
                     QIcon(":/icons/dragIcon/ellipse.png"),
                     QIcon(":/icons/dragIcon/rectangle.png"),
                     QIcon(":/icons/dragIcon/symbols_open_to_select_model.png")};
        break;
    default:
        menuIcons = {QIcon()};
        break;
    }

    for (int i = 0 ; i<menuIcons.size(); i++ )
    {
        labels.append("Item " + QString::number(i));
    }

    menuModel->setData(labels, menuIcons);
    listView->setModel(menuModel);
    listView->setIconSize(QSize(40, 40));
    listView->setItemDelegate(delegate);
    listView->setDragEnabled(true);
    listView->setFixedWidth(70);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(loadAction);
    fileMenu->addAction(clearAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addSeparator();
}

void MainWindow::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Ctrl+N"));
    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Ctrl+O"));
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Ctrl+S"));
    saveAction->setIcon(QIcon(":/icons/images/save.png"));
    saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAction->setIcon(QIcon(":/icons/images/save_as.png"));
    loadAction = new QAction(tr("&Load"), this);
    loadAction->setShortcuts(QKeySequence::Open);
    loadAction->setStatusTip(tr("Ctrl+O"));
    loadAction->setIcon(QIcon(":/icons/images/loading.png"));
    clearAction = new QAction(tr("&Clear"), this);
    clearAction->setStatusTip(tr("Clear"));
    clearAction->setShortcut(tr("Ctrl+L"));
    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit"));
    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setStatusTip(tr("Undo last operation"));
    undoAction->setIcon(QIcon(":/icons/images/undo.png"));
    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setShortcut(tr("Ctrl+Shift+Z"));
    redoAction->setStatusTip(tr("Redo last operation"));
    redoAction->setIcon(QIcon(":/icons/images/redo.png"));
    zoomInAction = new QAction(tr("&Zoom In"), this);
    zoomInAction->setStatusTip(tr("Zoom In"));
    zoomInAction->setIcon(QIcon(":/icons/images/zoom_in.png"));
    zoomOutAction = new QAction(tr("&Zoom Out"), this);
    zoomOutAction->setStatusTip(tr("Zoom Out"));
    zoomOutAction->setIcon(QIcon(":/icons/images/zoom_out.png"));
    zoomToFitAction = new QAction(tr("&Zoom to Fit"), this);
    zoomToFitAction->setStatusTip(tr("Zoom to Fit"));
    zoomToFitAction->setIcon(QIcon(":/icons/images/zoom_to_fit.PNG"));

    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAs);
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoad);
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClear);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(undoAction, &QAction::triggered, graphicsView, &CustomGraphicsView::UndoTriggered);
    connect(redoAction, &QAction::triggered, graphicsView, &CustomGraphicsView::RedoTriggered);
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
    connect(zoomToFitAction, &QAction::triggered, this, &MainWindow::zoomToFit);
    connect(delegate, &CustomDelegate::sizeHintChanged, listView, &QListView::doItemsLayout);
    connect(graphicsView, &CustomGraphicsView::PublishOldData, this, &MainWindow::onOldPos);
    connect(graphicsView, &CustomGraphicsView::PublishNewData, this, &MainWindow::onNewPos);
    connect(graphicsView, &CustomGraphicsView::PublishUndoData, this, &MainWindow::onUndoPos);
    connect(graphicsView, &CustomGraphicsView::PublishRedoData, this, &MainWindow::onRedoPos);
    connect(graphicsView, &CustomGraphicsView::resultUpdated, this, &MainWindow::updateResult);
    connect(tabPlant, &QTabWidget::tabBarClicked, this, &MainWindow::addNewPlantTab);
    connect(tabPlant, &QTabWidget::tabCloseRequested, this, &MainWindow::closePlantTab);
    connect(tabPage, &QTabWidget::tabBarClicked, this, &MainWindow::addNewPageTab);
    connect(tabPage, &QTabWidget::tabCloseRequested, this, &MainWindow::closePageTab);
}

void MainWindow::createToolbar()
{
    QToolBar *editToolBar;
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(saveAction);
    editToolBar->addSeparator();
    editToolBar->addAction(undoAction);
    editToolBar->addAction(redoAction);
    editToolBar->addSeparator();
    editToolBar->addAction(zoomInAction);
    editToolBar->addAction(zoomOutAction);
    editToolBar->addAction(zoomToFitAction);
    editToolBar->addSeparator();
    removeToolBar(editToolBar);
    addToolBar(editToolBar);
    editToolBar->show();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    setWindowModified(false);
    setWindowFilePath(currentFile.isEmpty() ? tr("untitled.xml") : currentFile);
}

void MainWindow::onSave()
{
    //    QString fileName = "saveTest.scene";//QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Scene Files (*.scene)"));
    //    if (!fileName.isEmpty()) {
    //        graphicsView->saveToFile(fileName);
    //    }

    QString file = "saveTest.xml";
    if (!file.isEmpty()) {
        graphicsView->saveToXml(file);
    }
}

void MainWindow::onSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As File"), "", tr("XML Files (*.xml);;Scene Files(*.scene)"));
    if (fileName.isEmpty())
        return;
    setCurrentFile(fileName);
    onSave();
}

void MainWindow::onLoad()
{
    //    graphicsView->ClearScene();
    //    QString fileName = "saveTest.scene";//QFileDialog::getOpenFileName(this, tr("Load File"), "", tr("Scene Files (*.scene)"));
    //    if (!fileName.isEmpty()) {
    //        graphicsView->loadFromFile(fileName);
    //    }
    QString file = "saveTest.xml";
    if (!file.isEmpty()) {
        graphicsView->loadFromXml(file);
    }
}

void MainWindow::onOldPos(QString data)
{
    oldData->setText(data);
}

void MainWindow::onNewPos(QString data)
{
    newData->setText(data);
}

void MainWindow::onUndoPos(QString data)
{
    UndoData->setText(data);
}

void MainWindow::onRedoPos(QString data)
{
    RedoData->setText(data);
}

void MainWindow::updateResult(const QString &result)
{
    status->setText("<span style='font-size: 16px; font-weight: bold;'>Result : " + result + "</span>");
}

void MainWindow::zoomIn()
{
    zoomFactor *= 1.5;
    graphicsView->scale(1.5, 1.5);
}

void MainWindow::zoomOut()
{
    zoomFactor /= 1.5;
    graphicsView->scale(1.0 / 1.5, 1.0 / 1.5);
}

void MainWindow::zoomToFit()
{
    graphicsView->fitInView(graphicsView->sceneRect(), Qt::KeepAspectRatio);
    zoomFactor = 1.5;
}

void MainWindow::addNewPlantTab(int index)
{
    if (index == tabPlant->count() - 1)
    {
        QWidget *newTab = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(newTab);
        layout->addWidget(newTab);
        tabPlant->addTab(newTab, tr("+ Page plant"));
    }
}
void MainWindow::addNewPageTab(int index)
{
    if (index == tabPage->count() - 1)
    {
        QWidget *newTab = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(newTab);
        layout->addWidget(new QLabel("New Tab Content", newTab));
        tabPage->addTab(newTab, tr("+ Page"));
    }
}

void MainWindow::closePlantTab(const int &index)
{
    if (index == 0) {
        return;
    }

    QWidget* tabItem = tabPlant->widget(index);
    tabPlant->removeTab(index);
    delete(tabItem);
    tabItem = nullptr;
}

void MainWindow::closePageTab(const int &index)
{
    if (index == 0) {
        return;
    }

    QWidget* tabItem = tabPage->widget(index);
    tabPage->removeTab(index);
    delete(tabItem);
    tabItem = nullptr;
}
