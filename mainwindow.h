#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include "CustomGraphicsView.h"
#include <customdelegate.h>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVector>
#include <QIcon>
#include <QLabel>
#include <QStringList>
#include <QList>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onClear();
    void onSave();
    void onSaveAs();
    void onLoad();
    void onOldPos(QString data);
    void onNewPos(QString data);
    void onUndoPos(QString data);
    void onRedoPos(QString data);
    void onItemClicked(const QModelIndex &index);
    void updateResult(const QString &result);
    void zoomIn();
    void zoomOut();
    void zoomToFit();

private:
    void SetupUI();
    void createMenus();
    void createActions();
    void createToolbar();
    void setCurrentFile(const QString &fileName);

    QWidget *centralWidget;
    QListView *listView;
    QListView *menuListView;
    CustomDelegate *delegate;
    CustomGraphicsView *graphicsView;
    QLabel* oldData;
    QLabel* newData;
    QLabel* UndoData;
    QLabel* RedoData;

    QLabel* status;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *resultMenu;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *loadAction;
    QAction *clearAction;
    QAction *exitAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *zoomToFitAction;
    QAction *runAction;
    QString currentFile;
    qreal zoomFactor;
};

#endif // MAINWINDOW_H
