#pragma once

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>
#include <iostream>

class CircleWidget : public QWidget {
public:
    CircleWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&CircleWidget::update));
        timer->start(1);
    }

private:
    double x = 0, y = 0;
    double dx = 1;
    double dy = 1;
    QTimer* timer;
    bool isMousePressed;
    QPoint ellipsePos;

    int radius = 15;
    int ellipseRadius = 50;

    bool isIntersected = false;

protected:

    void calculateScene()
    {
        auto realX = x + radius;
        auto realY = y + radius;
        //        std::cout << "x:" << realX << " y:" << realY << "\n";

        auto realX2 = ellipsePos.x() + ellipseRadius;
        auto realY2 = ellipsePos.y() + ellipseRadius;
        //        std::cout << "x:" << realX << " y:" << realY << "\n";

        if ((realX - realX2) * (realX - realX2) + (realY - realY2) * (realY - realY2) > (radius + ellipseRadius) * (radius + ellipseRadius))
        {
            if (x + dx > width() - radius || x + dx < 0) {
                dx = -dx;
            }
            if (y + dy > height() - radius || y + dy < 0) {
                dy = -dy;
            }

            if (isIntersected)
            {
                static int counter = 0;
                std::cout << "--" << counter++ << "\n";

                isIntersected = false;
            }
        }
        else if (!isIntersected)
        {
            // intersected

            // rotate 180
            double rDx = -dx;
            double rDy = -dy;

            // get 2-d axis
            double x2 = realX - realX2;
            double y2 = realY - realY2;

            // calculate cos(fi) and sin(Fi)
            double cosFi = (rDx * x2 + rDy * y2) / sqrt((rDx * rDx + rDy * rDy) * (x2 * x2 + y2 * y2));
            double sinFi = sqrt(1 - cosFi * cosFi);

            // do rotation
            double dxRotated = cosFi * rDx - sinFi * rDy;
            double dyRotated = sinFi * rDx + cosFi * rDy;

            // test direction
            double testCosFi = (dxRotated * dyRotated + x2 * y2) / sqrt((dxRotated * dxRotated + dyRotated * dyRotated) * (x2 * x2 + y2 * y2));

            if (testCosFi < cosFi)
            {
                dx = cosFi * dxRotated - sinFi * dyRotated;
                dy = sinFi * dxRotated + cosFi * dyRotated;
            }
            else
            {
                dxRotated = cosFi * rDx + sinFi * rDy;
                dyRotated = -sinFi * rDx + cosFi * rDy;
                dx = cosFi * dxRotated + sinFi * dyRotated;
                dy = -sinFi * dxRotated + cosFi * dyRotated;
            }

            std::cout << "isIntersected" << "\n";
            isIntersected = true;
        }
        //        else
        //        {
        //            isIntersected = true;
        //        }

        x += dx;
        if (x < 0) x = 0;
        y += dy;
        if (y < 0) y = 0;
    }

    void paintEvent(QPaintEvent* event) override
    {
        calculateScene();

        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.setBrush(Qt::lightGray);
        painter.drawRect(0, 0, width(), height());

        painter.setBrush(Qt::red);
        painter.drawEllipse(ellipsePos.x(), ellipsePos.y(), ellipseRadius * 2, ellipseRadius * 2);

        painter.setBrush(Qt::blue);
        painter.drawEllipse(x, y, radius * 2, radius * 2);
    }

    void mousePressEvent(QMouseEvent* event) override {
        calculateScene();
        if (event->button() == Qt::LeftButton) {
            isMousePressed = true;
            //            ellipsePos.setX( event->pos().x() - ellipseRadius );
            //            ellipsePos.setY( event->pos().y() - ellipseRadius );
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        calculateScene();
        if (isMousePressed) {
            ellipsePos.setX(event->pos().x() - ellipseRadius);
            ellipsePos.setY(event->pos().y() - ellipseRadius);
            calculateScene();
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        calculateScene();
        if (event->button() == Qt::LeftButton) {
            isMousePressed = false;
        }
    }
};
