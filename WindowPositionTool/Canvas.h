#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QKeyEvent>

class Canvas : public QWidget {
private:
	std::vector<QString> filenames;
	QMap<QString, std::vector<float>> params;
	int curIndex;
	QImage image;
	float x1;
	float y1;
	float x2;
	float y2;

	int clickCount;

	bool ctrlPressed;
	bool shiftPressed;
	
public:
	Canvas(QWidget *parent = NULL);
    ~Canvas();
	
protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent* e);
    
public:
	void load(const QString& filename);
	QString start();
	QString next();
	QString prev();
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void outputParams();
};

#endif // CANVAS_H
