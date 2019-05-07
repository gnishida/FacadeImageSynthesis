#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionStart, SIGNAL(triggered()), this, SLOT(onStart()));
	connect(ui.actionNext, SIGNAL(triggered()), this, SLOT(onNext()));
	connect(ui.actionPrevious, SIGNAL(triggered()), this, SLOT(onPrev()));

	setCentralWidget(&canvas);
}

MainWindow::~MainWindow() {
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
	canvas.keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	canvas.keyReleaseEvent(e);
}

void MainWindow::onStart() {
	setWindowTitle(canvas.start());
	update();
}

void MainWindow::onNext() {
	setWindowTitle(canvas.next());
	update();
}

void MainWindow::onPrev() {
	setWindowTitle(canvas.prev());
	update();
}