#include "frontend/models/SceneLoader.hpp"
#include "frontend/views/MainWindow.hpp"
#include "frontend/views/SceneView.hpp"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>

namespace frontend {

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    sceneLoader_(new SceneLoader),
    sceneView_(nullptr)
{
    ui_->setupUi(this);

    sceneView_ = new SceneView;
    ui_->layout_sceneView->addWidget(QWidget::createWindowContainer(sceneView_));

    connect(ui_->action_openScene, SIGNAL(triggered()), this, SLOT(onAction_openSceneTriggered()));
    connect(ui_->action_closeCcene, SIGNAL(triggered()), this, SLOT(onAction_closeSceneTriggered()));
    connect(ui_->action_importMesh, SIGNAL(triggered()), this, SLOT(onAction_importMeshTriggered()));
    connect(ui_->action_quit, SIGNAL(triggered()), this, SLOT(onAction_quitTriggered()));
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete sceneLoader_;
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
        sceneLoader_->getSupportedFormatsFilter()
    );

    QStringList errors;
    for(QStringList::iterator it = fileNames.begin(); it != fileNames.end(); ++it)
    {
        QString errorMsg;
        QVector<Qt3DCore::QEntity*> entities;
        if (sceneLoader_->loadFile(*it, &entities, &errorMsg) == false)
        {
            errors.append(errorMsg);
            continue;
        }

        for (const auto& entity : entities)
            sceneView_->addEntity(entity);
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

}
