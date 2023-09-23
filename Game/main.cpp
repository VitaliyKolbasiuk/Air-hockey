#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>

#include <iostream>

#include "BoostClientServer/Server.h"
#include "BoostClientServer/TcpClient.h"
#include "BoostClientServer/Game.h"
#include "BoostClientServer/ClientPlayer.h"

#include "Scene.h"
#include "QtClientPlayer.h"
#include "CircleWidget.h"


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Scene scene(w);
    scene.init();

    // Real player
    std::thread([&scene]
                {
                    QtClientPlayer player1{ scene };

                    io_context  ioContext1;
                    TcpClient client1(ioContext1, player1);
                    player1.setTcpClient(&client1);
                    client1.execute("25.60.188.173", 1234, START_GAME_CMD ";001;1000;800;");
                    //        client1.execute("?.?.?.?", 1234, START_GAME_CMD ";001;1000;800;");

                    ioContext1.run();
                }).detach();

    return a.exec();
}

