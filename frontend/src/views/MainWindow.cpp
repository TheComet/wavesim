#include "frontend/views/MainWindow.hpp"
#include "frontend/models/Scene.hpp"
#include "ui_MainWindow.h"

#include <QFileDialog>

using namespace frontend;

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    scene_(new Scene)
{
    ui_->setupUi(this);

    connect(ui_->action_openScene, SIGNAL(triggered()), this, SLOT(onAction_openSceneTriggered()));
    connect(ui_->action_closeCcene, SIGNAL(triggered()), this, SLOT(onAction_closeSceneTriggered()));
    connect(ui_->action_importMesh, SIGNAL(triggered()), this, SLOT(onAction_importMeshTriggered()));
    connect(ui_->action_quit, SIGNAL(triggered()), this, SLOT(onAction_quitTriggered()));
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete scene_;
    delete ui_;
}

// ----------------------------------------------------------------------------
void MainWindow::onAction_openSceneTriggered()
{
}

// ----------------------------------------------------------------------------
void MainWindow::onAction_closeSceneTriggered()
{
}

// ----------------------------------------------------------------------------
void MainWindow::onAction_importMeshTriggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        "Select one or more files to open",
        "",
        scene_->getSupportedFormatsFilter()
    );

    for(QStringList::iterator it = fileNames.begin(); it != fileNames.end(); ++it)
    {
        scene_->loadFile(*it);
    }
}

// ----------------------------------------------------------------------------
void MainWindow::onAction_quitTriggered()
{
}
