#include <iostream>
#include <map>
#include <list>
#include <optional>

#include "../Log.h"

//------------------------------------------------------------------
// Client->Server: StartGame, MatchId, width, height ( GameId random sequesnce)
// Server->Client: WaitingGame
//------------------------------------------------------------------
// Server->Client: GameStarded, direction [left|right]
// Server->Client: Ball, x,y,dx,dy
// Client->Server: ToolPosition, x, y
// Server->Client: Score, number1, number2
//------------------------------------------------------------------

class Match;

struct Player: public IClientSessionUserData
{
    Match&          m_match;
    IClientSession* m_session;
    bool            m_isLeft;
    
    // These variables used for saving scene size of 1-st player
    // They used only for calculation of min size of the scene
    int             m_tmpWidth;
    int             m_tmpHeight;

    Player( Match& match, IClientSession* session, int width, int height ) : m_match(match), m_session(session), m_tmpWidth(width), m_tmpHeight(height) {
    }

    Player( Match& match, IClientSession* session ) : m_match(match), m_session(session) {
    }
};

class Match
{
    io_context&             m_serverIoContext;
    
public:
    const std::string       m_matchId;

    std::shared_ptr<Player>   m_player1;
    std::shared_ptr<Player>   m_player2;

private:
    boost::asio::high_resolution_timer m_timer;
    
public:
    Match( io_context& serverIoContext, const std::string& matchId )
    : m_serverIoContext(serverIoContext),
      m_matchId(matchId),
      m_timer( m_serverIoContext )
    {}
    
public:
    double m_gameWindowWidth;
    double m_gameWindowHeight;
    double m_mainWindowWidth;
    double m_mainWindowHeight;
    
    double m_xBall;
    double m_yBall;
    double m_dx;
    double m_dy;
    
    double m_x1Player;
    double m_y1Player;
    int    scoreLeftPlayer = 0;
    
    double m_x2Player;
    double m_y2Player;
    int    scoreRightPlayer = 0;
    
    double m_ballRadius = 15;
    double m_playerRadius = 50;
    
    bool   m_isIntersected1 = false;
    bool   m_isIntersected2 = false;

    void init( int width, int height )
    {
        m_gameWindowWidth = width;
        m_gameWindowHeight = height * 0.8;

        m_mainWindowWidth = width;
        m_mainWindowHeight = height;
        
        m_ballRadius = (7.5 * 1800) / (m_gameWindowWidth + m_gameWindowHeight);
        m_playerRadius = (15 * 1800) / (m_gameWindowWidth + m_gameWindowHeight);
        
        m_xBall = m_gameWindowWidth / 2.0;
        m_yBall = m_gameWindowHeight / 2.0;
        m_dx = 6;
        m_dy = 2;

        m_x1Player = 2*m_playerRadius;
        m_y1Player = m_gameWindowHeight * 2;
        
        m_x2Player = m_gameWindowWidth - 2 * m_playerRadius;
        m_y2Player = m_gameWindowHeight * 2;
    }
    
    void onClientPositionChanged( Player* player, int x, int y )
    {

        int diameter = m_playerRadius * 2;
        if (y + diameter < 0)
        {
            y = 0;
        }
        else if (y + diameter > m_gameWindowHeight)
        {
            y = m_gameWindowHeight - diameter;
        }

        if ( player->m_isLeft )
        {
            if (x + diameter > m_gameWindowWidth / 2)
            {
                x = m_gameWindowWidth / 2 - diameter;
            }
            else if (x + diameter < 0)
            {
                x = 0;
            }
            m_x1Player = x;
            m_y1Player = y;
        }
        else
        {
            if (x < m_gameWindowWidth / 2)
            {
                x = m_gameWindowWidth / 2;
            }
            else if (x + diameter > m_gameWindowWidth)
            {
                x = m_gameWindowWidth - diameter;
            }
            m_x2Player = x;
            m_y2Player = y;
        }
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTimestamp;
    
    void start()
    {
        m_lastTimestamp = std::chrono::high_resolution_clock::now();

        // delay on start
        m_timer.expires_after( std::chrono::milliseconds( 3000 ));
        m_timer.async_wait([this](const boost::system::error_code& ec )
        {
            if ( ec )
            {
                LOG( "Timer error:" << ec.message().c_str() );
                exit(1);
            }
            tick();
        });
    }
    
    int startCounter = 0;
    const int skipNumber = 1;

    void tick()
    {
        m_timer.expires_after( std::chrono::milliseconds( 30 ));
        m_timer.async_wait([this](const boost::system::error_code& ec )
        {
            if ( ++startCounter < skipNumber )
            {
                tick();
                return;
            }
            else if ( startCounter == skipNumber )
            {
                m_lastTimestamp = std::chrono::high_resolution_clock::now();
                tick();
                return;
            }
//            else if ( startCounter > skipNumber+1000 )
//            {
//                exit(0);
//            }
            
            if ( ec )
            {
                LOG( "Timer error:" << ec.message().c_str() );
                exit(1);
            }
            
            auto durationMs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_lastTimestamp);
            m_lastTimestamp = std::chrono::high_resolution_clock::now();

            calculateScene( durationMs.count()/30000.0 );
            tick();
        });
    }

    void calcBump( double& dx, double& dy, double ballX, double ballY, double playerX, double playerY )
    {
        // rotate 180
        double rDx = -dx;
        double rDy = -dy;

        // get 2-d axis
        double x2 = ballX - playerX;
        double y2 = ballY - playerY;

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
    }

    bool ballsIntersecting(double x1, double y1, double radius1, double x2, double y2, double radius2)
    {
        // Calculate the distance between the centers of the two balls
        double distance = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));

        // Check if the distance is less than or equal to the sum of the radii
        if (distance <= radius1 + radius2)
        {
            return true; // The balls are intersecting
        }
        return false; // The balls are not intersecting
    }


    void calculateScene( double deltaTime )
    {
        auto x1PlayerCenter = m_x1Player + m_playerRadius;
        auto y1PlayerCenter = m_y1Player + m_playerRadius;

        auto x2PlayerCenter = m_x2Player + m_playerRadius;
        auto y2PlayerCenter = m_y2Player + m_playerRadius;

        auto xBallCenter = m_xBall + m_ballRadius;
        auto yBallCenter = m_yBall + m_ballRadius;

        auto ballX = xBallCenter;
        auto ballY = yBallCenter;

        auto playerX1 = x1PlayerCenter;
        auto playerY1 = y1PlayerCenter;

        auto playerX2 = x2PlayerCenter;
        auto playerY2 = y2PlayerCenter;

        auto ellipseRadius = m_playerRadius;
        auto radius = m_ballRadius;

        auto& dx = m_dx;
        auto& dy = m_dy;

        auto& x = m_xBall;
        auto& y = m_yBall;

        // intersected with player1
        if ( ballsIntersecting(ballX, ballY, radius, playerX1, playerY1, ellipseRadius) )
        {
            if ( !m_isIntersected1 )
            {
                calcBump( dx, dy, ballX, ballY, playerX1, playerY1 );
                m_isIntersected1 = true;
                static int counter = 0;
                qDebug() << "--" << ++counter << " bump1\n";
            }
        }
        // intersected with player2
        else if ( ballsIntersecting(ballX, ballY, radius, playerX2, playerY2, ellipseRadius)  )
        {
            if ( !m_isIntersected2 )
            {
                calcBump( dx, dy, ballX, ballY, playerX2, playerY2 );
                m_isIntersected2 = true;
                //std::cout << "--bump2\n";
            }
        }
        // not intersected
        else
        {
            if (x + dx > m_gameWindowWidth - radius) {
                scoreLeftPlayer++;

                std::shared_ptr<boost::asio::streambuf> wrStreambuf1 = std::make_shared<boost::asio::streambuf>();
                std::ostream os1(&(*wrStreambuf1));
                os1 << UPDATE_SCORE_CMD ";" << std::to_string(scoreLeftPlayer) + ';' << std::to_string(scoreRightPlayer) << ";\n";
                m_player1->m_session->sendMessage( wrStreambuf1 );

                std::shared_ptr<boost::asio::streambuf> wrStreambuf2 = std::make_shared<boost::asio::streambuf>();
                std::ostream os2(&(*wrStreambuf2));
                os2 << UPDATE_SCORE_CMD ";" << std::to_string(scoreLeftPlayer) + ';' << std::to_string(scoreRightPlayer) << ";\n";
                m_player1->m_session->sendMessage( wrStreambuf2 );
            }
            else if (x + dx < 0)
            {
                scoreRightPlayer++;

                std::shared_ptr<boost::asio::streambuf> wrStreambuf1 = std::make_shared<boost::asio::streambuf>();
                std::ostream os1(&(*wrStreambuf1));
                os1 << UPDATE_SCORE_CMD ";" << std::to_string(scoreLeftPlayer) + ';' << std::to_string(scoreRightPlayer) << ";\n";
                m_player1->m_session->sendMessage( wrStreambuf1 );

                std::shared_ptr<boost::asio::streambuf> wrStreambuf2 = std::make_shared<boost::asio::streambuf>();
                std::ostream os2(&(*wrStreambuf2));
                os2 << UPDATE_SCORE_CMD ";" << std::to_string(scoreLeftPlayer) + ';' << std::to_string(scoreRightPlayer) << ";\n";
                m_player1->m_session->sendMessage( wrStreambuf2 );
            }

            if (y + dy > m_gameWindowHeight - radius || y + dy < 0) {
                dy = -dy;
            }

            if ( m_isIntersected1 )
            {
                m_isIntersected1 = false;
            }
            if ( m_isIntersected2 )
            {
                m_isIntersected2 = false;
            }
        }

        m_xBall = m_xBall + m_dx*deltaTime;
        m_yBall = m_yBall + m_dy*deltaTime;


        sendUpdateScene();
    }
    
    void sendUpdateScene()
    {
        {
            std::shared_ptr<boost::asio::streambuf> wrStreambuf1 = std::make_shared<boost::asio::streambuf>();
            std::ostream os1(&(*wrStreambuf1));
            os1 << UPDATE_SCENE_CMD ";"
                << int(m_xBall) << ";" << int(m_yBall) << ";"
                << double(m_dx) << ";" << double(m_dy) << ";"
                << int(m_x1Player) << ";" << int(m_y1Player) << ";"
                << int(m_x2Player) << ";" << int(m_y2Player) << ";"
                << int(m_ballRadius) << ";" << int(m_playerRadius) << ";\n";

            m_player1->m_session->sendMessage( wrStreambuf1 );
        }

        {
            std::shared_ptr<boost::asio::streambuf> wrStreambuf2 = std::make_shared<boost::asio::streambuf>();
            std::ostream os2(&(*wrStreambuf2));
            os2 << UPDATE_SCENE_CMD ";"
                << int(m_xBall) << ";" << int(m_yBall) << ";"
                << double(m_dx) << ";" << double(m_dy) << ";"
                << int(m_x1Player) << ";" << int(m_y1Player) << ";"
                << int(m_x2Player) << ";" << int(m_y2Player) << ";"
                << int(m_ballRadius) << ";" << int(m_playerRadius) << ";\n";

            m_player2->m_session->sendMessage( wrStreambuf2 );
        }
    }
};

class Game: public IGame
{
    io_context&             m_serverIoContext;

    std::list<Match> m_matchList;
    
public:
    
    Game( io_context& serverIoContext ) : m_serverIoContext( serverIoContext ) {}
    
    virtual void handlePlayerMessage( IClientSession& client, boost::asio::streambuf& message ) override
    {
        //LOG( "SERVER: Recieved from client: " << std::string( (const char*)message.data().data(), message.size() ).c_str() <<"\n");

        std::istringstream input;
        input.str( std::string( (const char*)message.data().data(), message.size() ) );

        std::string command;
        std::getline( input, command, ';');
        
        if ( command == START_GAME_CMD )
        {
            // Get 'MatchId'
            std::string matchId;
            std::getline( input, matchId, ';');

            std::string widthStr;
            std::getline( input, widthStr, ';');
            int width = std::stoi(widthStr);

            std::string heightStr;
            std::getline( input, heightStr, ';');
            int height = std::stoi(heightStr);

            LOG( "m_matchList.size(): " << m_matchList.size() );
            {
                auto matchIt = std::find_if( m_matchList.begin(), m_matchList.end(), [&matchId] ( const auto& match ) {
                    return match.m_matchId == matchId;
                });
                
                // Match is created (we have received 'StartGame' message from 2-d player)
                if ( matchIt != m_matchList.end() )
                {
                    if ( matchIt->m_player1 && matchIt->m_player2 )
                    {
                        LOG_ERR( "MatchIdTaken" );
                        client.sendMessage( "MatchIdTaken;\n" );
                        return;
                    }
                    if ( ! matchIt->m_player1 || matchIt->m_player2 )
                    {
                        LOG_ERR( "MatchIdInternalError" );
                        client.sendMessage( "MatchIdInternalError;\n" );
                        return;
                    }
                    
                    LOG( "message from 2-d player" );
                    // calculate scene size
                    int minWidth = std::min( width, matchIt->m_player1->m_tmpWidth );
                    int minHeight = std::min( height, matchIt->m_player1->m_tmpHeight );

                    // initialize match scene size
                    matchIt->m_player1->m_match.init( minWidth, minHeight );
                    
                    // init 2-d player
                    matchIt->m_player2 = std::make_shared<Player>( *matchIt, &client );
                    
                    //
                    // Send GAME_STARTED_CMD message to 1-st player
                    //
                    std::shared_ptr<boost::asio::streambuf> wrStreambuf = std::make_shared<boost::asio::streambuf>();
                    std::ostream os(&(*wrStreambuf));
                    os << GAME_STARTED_CMD ";left;" << minWidth << ";" << minHeight << ";\n";

                    matchIt->m_player1->m_isLeft = true;
                    matchIt->m_player1->m_session->sendMessage( wrStreambuf );
                    
                    //
                    // Send GAME_STARTED_CMD message to 2-d player
                    //
                    std::shared_ptr<boost::asio::streambuf> wrStreambuf2 = std::make_shared<boost::asio::streambuf>();
                    std::ostream os2(&(*wrStreambuf2));
                    os2 << GAME_STARTED_CMD ";right;" << minWidth << ";" << minHeight << ";\n";

                    matchIt->m_player2->m_isLeft = false;
                    matchIt->m_player2->m_session->sendMessage( wrStreambuf2 );

                    // Set userPtr to 2-d player
                    auto base = std::dynamic_pointer_cast<IClientSessionUserData>( matchIt->m_player2 );
                    client.setUserInfoPtr( std::weak_ptr<IClientSessionUserData>( base ) );


                    // Start game
                    matchIt->m_player1->m_match.start();
                    return;
                }
            }
            
            // we have received 'StartGame' message from 1-st player
            LOG( "message from 1-d player" );
            
            // Add new match
            m_matchList.emplace_front( m_serverIoContext, matchId );
            
            //
            // Send WAIT_2d_PLAYER_CMD command to 1-st player
            //
            auto& front = m_matchList.front();
            front.m_player1 = std::make_shared<Player>( front, &client, width, height );
            client.sendMessage( WAIT_2d_PLAYER_CMD ";\n" );
            
            // set userPtr
            auto base = std::dynamic_pointer_cast<IClientSessionUserData>( front.m_player1 );
            client.setUserInfoPtr( std::weak_ptr<IClientSessionUserData>( base ) );
        }
        else if ( command == CLIENT_POSITION_CMD )
        {
            // Get mouse 'x'
            std::string xStr;
            std::getline( input, xStr, ';');
            int mouseX = std::stoi(xStr);

            // Get mouse 'y'
            std::string yStr;
            std::getline( input, yStr, ';');
            int mouseY = std::stoi(yStr);
            
            if ( auto ptr = client.getUserInfoPtr().lock(); ptr )
            {
                auto player = std::dynamic_pointer_cast<Player>( ptr );
                player->m_match.onClientPositionChanged( player.get(), mouseX, mouseY );
            }
        }
    }
};
