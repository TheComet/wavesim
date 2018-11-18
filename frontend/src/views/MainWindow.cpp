#include "frontend/models/Scene.hpp"
#include "frontend/views/MainWindow.hpp"
#include "frontend/views/SceneView.hpp"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>

using namespace frontend;

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    scene_(new Scene),
    sceneView_(new SceneView)
{
    ui_->setupUi(this);

    ui_->layout_sceneView->addWidget(QWidget::createWindowContainer(sceneView_));

    connect(ui_->action_openScene, SIGNAL(triggered()), this, SLOT(onAction_openSceneTriggered()));
    connect(ui_->action_closeCcene, SIGNAL(triggered()), this, SLOT(onAction_closeSceneTriggered()));
    connect(ui_->action_importMesh, SIGNAL(triggered()), this, SLOT(onAction_importMeshTriggered()));
    connect(ui_->action_quit, SIGNAL(triggered()), this, SLOT(onAction_quitTriggered()));
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
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

    QStringList errors;
    for(QStringList::iterator it = fileNames.begin(); it != fileNames.end(); ++it)
    {
        QString errorMsg;
        Qt3DCore::QEntity* entity = scene_->loadFile(*it, &errorMsg);
        if (entity == nullptr)
        {
            errors.append(errorMsg);
            continue;
        }

        //sceneView_->addEntity()
    }

    if (errors.length() > 0)
    {
        QMessageBox errorMessageBox;
        errorMessageBox.setWindowTitle("Import error");
        errorMessageBox.setText(
            QString("One or more errors occurred during import:\n\n") +
            errors.join("\n\n")
        );
        errorMessageBox.exec();
    }
}

// ----------------------------------------------------------------------------
void MainWindow::onAction_quitTriggered()
{
}
