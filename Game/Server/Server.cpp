#include "Server.h"
#include "Game.h"


int main(int argc, char* argv[])
{
    io_context serverIoContext;
    Game game(serverIoContext);

    TcpServer server(serverIoContext, game, 1234);
    server.execute();
}

