#include "frontend/MainWindow.hpp"
#include "ui_MainWindow.h"

using namespace frontend;

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui_;
}
