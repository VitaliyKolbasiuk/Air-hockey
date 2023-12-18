#include <QApplication>

#include "mainwindow.h"
#include "Scene.h"
#include "TcpClient.h"
#include "QtClientPlayer.h"

#include "../Interfaces.h"

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
        client1.execute("127.0.0.1", 1234, START_GAME_CMD ";001;1000;800;");
//        client1.execute("?.?.?.?", 1234, START_GAME_CMD ";001;1000;800;");

        ioContext1.run();
    }).detach();

    return a.exec();
}

