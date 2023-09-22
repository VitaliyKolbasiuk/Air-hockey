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

    // server
    std::thread([]
        {
            io_context serverIoContext;
            Game game(serverIoContext);

            TcpServer server(serverIoContext, game, 1234);
            server.execute();
        }).detach();

        // Real player
        std::thread([&scene]
        {
            QtClientPlayer player1{ scene };

            io_context  ioContext1;
            TcpClient client1(ioContext1, player1);
            player1.setTcpClient(&client1);
            client1.execute("127.0.0.1", 1234, START_GAME_CMD ";001;1000;800;");

            ioContext1.run();
        }).detach();

        // Virtual Test Player (for testing)
        std::thread([]
        {
            io_context  ioContext;

            ClientPlayer player2{ "player2" };

            TcpClient client2(ioContext, player2);
            player2.setTcpClient(&client2);
            client2.execute("127.0.0.1", 1235, START_GAME_CMD ";001;800;600;");
            
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

            ioContext.run();
        }).detach();


                //    w.resize(1200, 600);
                //    CircleWidget circleWidget;
                //    w.centralWidget()->setLayout(new QVBoxLayout);
                //    w.centralWidget()->layout()->addWidget(&circleWidget);
                //
                //    w.show();

                return a.exec();
}

