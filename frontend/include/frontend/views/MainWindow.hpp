#pragma once

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace frontend {
class SceneLoader;
class SceneView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow();

private slots:
    void onAction_openSceneTriggered(void);
    void onAction_closeSceneTriggered(void);
    void onAction_importMeshTriggered(void);
    void onAction_quitTriggered(void);

private:
    Ui::MainWindow* ui_;
    SceneLoader* sceneLoader_;
    SceneView* sceneView_;
};

}
