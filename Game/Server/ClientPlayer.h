#include <iostream>
#include <boost/asio.hpp>

#include "../Interfaces.h"
#include "../Client/TcpClient.h"

class TcpClient;

class VirtualScene
{
protected:
    bool m_isLeftPlayer = false;

    int m_width;
    int m_height;

    // Ball
    int m_x;
    int m_y;

    int m_dx;
    int m_dy;

    double m_playerX;
    double m_playerY;


public:
    VirtualScene() = default;
    ~VirtualScene() = default;
    
    void acceptBallPosition()
    {
        double dX = m_dx;
        double dY = m_dy;

        if ( (dX > 0) && !m_isLeftPlayer )
        {
            m_playerX = m_width-100;
            m_playerY = (double(dY)/double(dX)) * (m_playerX - m_x) + m_y;
            
//            LOG( "virtual: " << (double(dY)/double(dX)) << " dY:" << dY << " dX:" << dX );
//            LOG( "virtual: " << m_playerX << " " << m_playerY );
        }
        else if ( (dX < 0) && m_isLeftPlayer )
        {
            m_playerX = 100;
            m_playerY = (double(dY)/double(dX)) * m_playerX;
        }

        if ( m_playerX < 0 )
        {
            m_playerX = 0;
        }
        if ( m_playerX > m_width )
        {
            m_playerY = m_width;
        }

        if ( m_playerY < 0 )
        {
            m_playerY = 0;
        }
        if ( m_playerY > m_height )
        {
            m_playerY = m_height;
        }
//        m_playerX = m_gameWindowWidth/2;
//        m_playerY = m_gameWindowHeight/2;
    }
};

class ClientPlayer : public IClientPlayer, protected VirtualScene
{
    std::string m_playerName;
    
    TcpClient* m_tcpClient = nullptr;
    
public:
    ClientPlayer( std::string playerName ) : m_playerName(playerName) {}
    
    //const std::string& playerName() const override { return m_playerName; }

    void setTcpClient( TcpClient* tcpClient ) { m_tcpClient = tcpClient; }
    
    void sendBallMessage( double x, double y )
    {
        std::shared_ptr<boost::asio::streambuf> wrStreambuf = std::make_shared<boost::asio::streambuf>();
        std::ostream os(&(*wrStreambuf));
        os << "Ball;" << x << ";" << y << ";\n";

        m_tcpClient->sendMessageToServer(wrStreambuf);
    }
    
protected:
    
    int counter = 0;
    
    virtual void handleServerMessage( const std::string& command, boost::asio::streambuf& message ) override
    {
        //LOG("Client: Recieved from server: " << m_playerName.c_str() << ": " << command.c_str() << " " << std::string((const char*)message.data().data(), message.size()-1).c_str() );
        std::istringstream input;
        input.str(std::string((const char*)message.data().data(), message.size()));

        if (command == WAIT_2d_PLAYER_CMD)
        {
        }
        else if (command == GAME_STARTED_CMD)
        {
            std::string direction;
            std::getline(input, direction, ';');
            
            if ( direction == "left" )
            {
                m_isLeftPlayer = true;
            }
            else {
                m_isLeftPlayer = false;
            }
            
            std::string widthStr;
            std::getline(input, widthStr, ';');
            double width = std::stod(widthStr);

            std::string heightStr;
            std::getline(input, heightStr, ';');
            double height = std::stod(heightStr);

            m_width = width;
            m_height = height;

        }
        else if (command == UPDATE_SCENE_CMD)
        {
            std::string number;

            std::getline(input, number, ';');
            double x = std::stod(number);
            m_x = x;

            std::getline(input, number, ';');
            double y = std::stod(number);
            m_y = y;

            std::getline(input, number, ';');
            double dX = std::stod(number);
            m_dx = dX;

            std::getline(input, number, ';');
            double dY = std::stod(number);
            m_dy = dY;

            std::getline(input, number, ';');
            double x1Player = std::stod(number);

            std::getline(input, number, ';');
            double y1Player = std::stod(number);

            std::getline(input, number, ';');
            double x2Player = std::stod(number);

            std::getline(input, number, ';');
            double y2Player = std::stod(number);

            std::getline(input, number, ';');
            double ballRadius = std::stod(number);

            std::getline(input, number, ';');
            double playerRadius = std::stod(number);
            
            acceptBallPosition();

            {
                std::shared_ptr<boost::asio::streambuf> wrStreambuf = std::make_shared<boost::asio::streambuf>();
                std::ostream os(&(*wrStreambuf));
                int x = m_playerX;
                int y = m_playerY;
                os << CLIENT_POSITION_CMD ";" << x << ";" << y << "\n";
                
                m_tcpClient->sendMessageToServer( wrStreambuf );
            }
        }
        else if (command == "Score")
        {
            std::string score;
            std::getline(input, score, ';');
            int leftScore = std::stoi(score);

            std::getline(input, score, ';');
            int rightScore = std::stoi(score);

            // TODO : Qt -> set score
            // TODO : Qt -> set x and y to the beginning location
        }
    }
};

