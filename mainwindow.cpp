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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(new QWidget(this))
    , tabPlant(new QTabWidget(this))
    , tabPage(new QTabWidget(this))
    , listView(new QListView(this))
    , delegate(new CustomDelegate(64, this))
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

    connect(delegate, &CustomDelegate::sizeHintChanged, listView, &QListView::doItemsLayout);
    connect(graphicsView, &CustomGraphicsView::PublishOldData, this, &MainWindow::onOldPos);
    connect(graphicsView, &CustomGraphicsView::PublishNewData, this, &MainWindow::onNewPos);
    connect(graphicsView, &CustomGraphicsView::PublishUndoData, this, &MainWindow::onUndoPos);
    connect(graphicsView, &CustomGraphicsView::PublishRedoData, this, &MainWindow::onRedoPos);
    connect(graphicsView, &CustomGraphicsView::resultUpdated, this, &MainWindow::updateResult);

    runAction = new QAction("Run", this);
    menuBar()->addAction(runAction);
    connect(runAction, &QAction::triggered, graphicsView, &CustomGraphicsView::onResult);

    exportAction = new QAction("Export", this);
    menuBar()->addAction(exportAction);
    connect(exportAction, &QAction::triggered, this, &MainWindow::onSave);

    status->setText("Result : 0");
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
    QPainterPath path;
    path.moveTo(200, 50);
    path.arcTo(150, 0, 50, 50, 0, 90);
    path.arcTo(50, 0, 50, 50, 90, 90);
    path.arcTo(50, 50, 50, 50, 180, 90);
    path.arcTo(150, 50, 50, 50, 270, 90);
    path.lineTo(200, 50);
    QPolygonF myPolygon;
    myPolygon = path.toFillPolygon().translated(-125, -50);

    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    QStringList labels = {"Item 0", "Item 1", "Item 2", "Item 3", "Item 4", "Item 5", "Item 6", "Item 7",
                          "Item 8", "Item 9", "Item 10", "Item 11", "Item 12", "Item 13", "Item 14"};
    QList<QIcon> icons;
    icons << QIcon(":/icons/images/start_point.png")
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
          << QIcon(":/icons/images/notes_Coloring_and_Drawing.png")
          << QIcon(pixmap);

    IconListModel *model = new IconListModel(this);
    model->setData(labels, icons);

    listView->setModel(model);
    listView->setIconSize(QSize(64, 64));
    listView->setItemDelegate(delegate);
    listView->setDragEnabled(true);
    listView->setFixedWidth(150);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *vlayout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout(centralWidget);
    tabPlant->addTab(tabPage, tr("Plant Stage"));
//    tabPlant->insertTab(0,tabPage,tr("Plant Stage"));
//    tabPlant->insertTab(1,tabPage,tr("+ Plant Stage"));
    tabPage->addTab(graphicsView, tr("Page"));
    tabPlant->setTabsClosable(true);
    tabPage->setTabsClosable(true);

    vlayout->addWidget(listView);
    hlayout->addLayout(vlayout);
    hlayout->addWidget(tabPlant);
    centralWidget->setLayout(hlayout);
    setCentralWidget(centralWidget);

    //    vlayout->addWidget(oldData);
    //    vlayout->addWidget(newData);
    //    vlayout->addWidget(new QLabel("After Undo/Redo"));
    //    vlayout->addWidget(UndoData);
    //    vlayout->addWidget(RedoData);

    setMinimumSize(800, 600);
    statusBar();
    connect(tabPlant, &QTabWidget::tabBarClicked, this, &MainWindow::addNewPlantTab);
    connect(tabPlant, &QTabWidget::tabCloseRequested, this, &MainWindow::closePlantTab);
    connect(tabPage, &QTabWidget::tabBarClicked, this, &MainWindow::addNewPageTab);
    connect(tabPage, &QTabWidget::tabCloseRequested, this, &MainWindow::closePageTab);
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
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Ctrl+O"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Ctrl+S"));
    saveAction->setIcon(QIcon(":/icons/images/save.png"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);

    saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAction->setIcon(QIcon(":/icons/images/save_as.png"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAs);

    loadAction = new QAction(tr("&Load"), this);
    loadAction->setShortcuts(QKeySequence::Open);
    loadAction->setStatusTip(tr("Ctrl+O"));
    loadAction->setIcon(QIcon(":/icons/images/loading.png"));
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoad);

    clearAction = new QAction(tr("&Clear"), this);
    clearAction->setStatusTip(tr("Clear"));
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClear);

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setStatusTip(tr("Undo last operation"));
    undoAction->setIcon(QIcon(":/icons/images/undo.png"));
    connect(undoAction, &QAction::triggered, graphicsView, &CustomGraphicsView::UndoTriggered);

    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setShortcut(tr("Ctrl+Shift+Z"));
    redoAction->setStatusTip(tr("Redo last operation"));
    redoAction->setIcon(QIcon(":/icons/images/redo.png"));
    connect(redoAction, &QAction::triggered, graphicsView, &CustomGraphicsView::RedoTriggered);

    zoomInAction = new QAction(tr("&Zoom In"), this);
    zoomInAction->setStatusTip(tr("Zoom In"));
    zoomInAction->setIcon(QIcon(":/icons/images/zoom_in.png"));
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    zoomOutAction = new QAction(tr("&Zoom Out"), this);
    zoomOutAction->setStatusTip(tr("Zoom Out"));
    zoomOutAction->setIcon(QIcon(":/icons/images/zoom_out.png"));
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    zoomToFitAction = new QAction(tr("&Zoom to Fit"), this);
    zoomToFitAction->setStatusTip(tr("Zoom to Fit"));
    zoomToFitAction->setIcon(QIcon(":/icons/images/zoom_to_fit.PNG"));
    connect(zoomToFitAction, &QAction::triggered, this, &MainWindow::zoomToFit);
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
    status->setText("Result : " + result);
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
