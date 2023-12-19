#pragma once

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>

#include "mainwindow.h"
#include "../Interfaces.h"

class Scene : public QWidget
{
    Q_OBJECT

    MainWindow& m_mainWindow;
    
    IMouseEventHandler* m_mouseEventHandler = nullptr;

    QSize  m_windowSize;
    QSize  m_gameSize;
    QPoint m_ballPos;
    QPoint m_1playerPos;
    QPoint m_2playerPos;

    int m_radius = 15;
    int m_playerRadius = 50;

signals:
    void updateSignal();

public:
    Scene(MainWindow& mainWindow, QWidget* parent = nullptr) : QWidget(parent), m_mainWindow(mainWindow)
    {
        connect(this, &Scene::updateSignal, this, QOverload<>::of(&Scene::update));

        m_ballPos.setY(-100);
        m_1playerPos.setY(-100);
        m_2playerPos.setY(-100);
    }

    void setMouseEventHandler( IMouseEventHandler* mouseEventHandler ) { m_mouseEventHandler = mouseEventHandler; }

    void init()
    {
        m_mainWindow.resize(1200, 600);

        m_mainWindow.centralWidget()->setLayout(new QVBoxLayout);
        m_mainWindow.centralWidget()->layout()->addWidget(this);


        m_mainWindow.show();
    }

    void setSceneSize(double width, double height)
    {
        QMetaObject::invokeMethod(this, [=, this]
        {
            m_gameSize.setWidth(width);
            m_gameSize.setHeight(height * 0.8);
            m_windowSize.setWidth( width );
            m_windowSize.setHeight( height );
            m_mainWindow.resize( width + 50, height + 50 );
            //m_mainWindow.centralWidget()->setFixedSize(m_windowSize);
        }, Qt::QueuedConnection);
    }

    void setScene()
    {
        QMetaObject::invokeMethod(this, [=, this] {
            m_mainWindow.m_scoreLabel = new QLabel();
            QLabel* scoreLabel = m_mainWindow.m_scoreLabel;
            scoreLabel->setParent(&m_mainWindow);

            scoreLabel->setStyleSheet("font-size: 30pt; color: yellow; font-weight: bold;");
            scoreLabel->setFixedSize(200, 50);
            scoreLabel->setText("SCORE 0:0");
            scoreLabel->move(m_windowSize.width() / 2 - scoreLabel->width() / 2, m_windowSize.height() -  m_windowSize.height() * 0.2);
            scoreLabel->show();
        }, Qt::QueuedConnection);
    }

    void updateScore(int leftPlayerScore, int rightPlayerScore)
    {
        QLabel* scoreLabel = m_mainWindow.m_scoreLabel;
        scoreLabel->setText("SCORE " + QString::fromStdString(std::to_string(leftPlayerScore) + ':' + std::to_string(rightPlayerScore)));

    }

    void draw(double x, double y, double xPlayer1, double yPlayer1, double xPlayer2, double yPlayer2, double ballRadius, double playerRadius)
    {
        m_ballPos.setX(x);
        m_ballPos.setY(y);
        m_1playerPos.setX(xPlayer1);
        m_1playerPos.setY(yPlayer1);
        m_2playerPos.setX(xPlayer2);
        m_2playerPos.setY(yPlayer2);
        m_radius = ballRadius;
        m_playerRadius = playerRadius;

        emit updateSignal();
    }

protected:

    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::black);

        painter.setBrush(Qt::darkGreen);
        painter.drawRect(0, 0, m_gameSize.width(), m_gameSize.height());

        painter.setBrush(Qt::red);
        painter.drawEllipse(m_1playerPos.x(), m_1playerPos.y(), m_playerRadius * 2, m_playerRadius * 2);

        painter.setBrush(Qt::red);
        painter.drawEllipse(m_2playerPos.x(), m_2playerPos.y(), m_playerRadius * 2, m_playerRadius * 2);

        painter.setBrush(Qt::cyan);
        painter.drawEllipse(m_ballPos.x(), m_ballPos.y(), m_radius * 2, m_radius * 2);

        painter.drawLine(m_gameSize.width() / 2, 0, m_gameSize.width() / 2, m_gameSize.height());
    }

    void mousePressEvent(QMouseEvent* event) override {
//        if ( m_mouseEventHandler )
//        {
//            m_mouseEventHandler->mousePressEvent(event);
//        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if ( m_mouseEventHandler )
        {
            m_mouseEventHandler->mouseMoveEvent(event);
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
//        if ( m_mouseEventHandler )
//        {
//            m_mouseEventHandler->mouseReleaseEvent(event);
//        }
    }
};
