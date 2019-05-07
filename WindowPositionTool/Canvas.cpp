#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include <QFileInfoList>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;
	clickCount = 0;

	x1 = 200;
	y1 = 200;
	x2 = 600;
	y2 = 600;
}

Canvas::~Canvas() {
}

void Canvas::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.drawImage(0, 0, image);

	painter.setPen(QPen(QColor(255, 255, 0), 1));
	painter.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	if (clickCount >= 2) clickCount = 0;

	if (clickCount == 0) {
		x1 = e->x();
		y1 = e->y();
		clickCount++;
	}
	else if (clickCount == 1) {
		x2 = e->x();
		y2 = e->y();
		clickCount++;
	}

	params[filenames[curIndex]].resize(4);
	params[filenames[curIndex]][0] = (float)x1 / 800.0f;
	params[filenames[curIndex]][1] = (float)y1 / 800.0f;
	params[filenames[curIndex]][2] = (float)x2 / 800.0f;
	params[filenames[curIndex]][3] = (float)y2 / 800.0f;

	update();
}

void Canvas::load(const QString& filename) {
	image = QImage("../tiles/" + filename);

	int max_size = std::max(image.width(), image.height());
	float scale = 800.0 / max_size;
	image = image.scaled(image.width() * scale, image.height() * scale);

	// set the winow boundary
	if (params.contains(filename) && params[filename].size() == 4) {
		x1 = params[filename][0] * 800;
		y1 = params[filename][1] * 800;
		x2 = params[filename][2] * 800;
		y2 = params[filename][3] * 800;
	}
}

QString Canvas::start() {
	// read parameter file
	params.clear();
	QFile file("window_params.txt");
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		QTextStream in(&file);
		while (true) {
			QString line = in.readLine();
			QStringList list = line.split(",");
			if (list.size() != 5) break;
			if (list[0] == "") continue;

			params[list[0]].clear();
			for (int k = 1; k < list.size(); ++k) {
				params[list[0]].push_back(list[k].toFloat());
			}
		}
	}

	std::cout << params.size() << std::endl;

	// read file list
	filenames.clear();
	QDir dir("../tiles/");
	QFileInfoList list = dir.entryInfoList();
	foreach (QFileInfo finfo, list) {
		QString name = finfo.fileName();		if (name == "." || name == "..") continue;		filenames.push_back(name);
	}

	std::cout << filenames.size() << std::endl;

	curIndex = 0;
	load(filenames[curIndex]);

	return filenames[curIndex];
}

QString Canvas::next() {
	outputParams();

	curIndex++;
	if (curIndex >= filenames.size()) {
		curIndex = filenames.size() - 1;
		QMessageBox msgBox(this);
		msgBox.setText("This image is the last one!");
		msgBox.exec();
	}
	else {
		load(filenames[curIndex]);
	}

	return filenames[curIndex];
}

QString Canvas::prev() {
	outputParams();

	curIndex--;
	if (curIndex < 0) {
		curIndex = 0;
		QMessageBox msgBox(this);
		msgBox.setText("This image is the first one!");
		msgBox.exec();
	}
	else {
		load(filenames[curIndex]);
	}

	return filenames[curIndex];
}

void Canvas::keyPressEvent(QKeyEvent* e) {
	ctrlPressed = false;
	ctrlPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Space:
		outputParams();
		break;
	}

	update();
}

void Canvas::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	case Qt::Key_Right:
		curIndex += 99;
		next();
	default:
		break;
	}
}

void Canvas::outputParams() {
	QFile file("window_params.txt");
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);

	for (int i = 0; i < filenames.size(); ++i) {
		if (params.contains(filenames[i]) && params[filenames[i]].size() == 4) {
			float x1 = params[filenames[i]][0];
			float y1 = params[filenames[i]][1];
			float x2 = params[filenames[i]][2];
			float y2 = params[filenames[i]][3];

			if (x1 > x2) std::swap(x1, x2);
			if (y1 > y2) std::swap(y1, y2);

			out << filenames[i] << ", " << x1 << ", " << y1 << ", " << x2 << ", " << y2 << "\n";
		}
	}

	file.close();
}