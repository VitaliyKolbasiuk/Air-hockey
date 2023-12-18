#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include "../Interfaces.h"

using namespace boost::asio;
using ip::tcp;

class ClientSession : public std::enable_shared_from_this<ClientSession>, public IClientSession
{
    io_context&             m_ioContext;
    IGame&                  m_game;
    tcp::socket             m_socket;
    boost::asio::streambuf  m_streambuf;
    
    std::weak_ptr<IClientSessionUserData> m_userInfoPtr;

public:
    ClientSession( io_context& ioContext, IGame& game, tcp::socket&& socket)
      : m_ioContext(ioContext),
        m_game(game),
        m_socket(std::move(socket))
    {
    }

    ~ClientSession() { std::cout << "!!!! ~ClientSession()" << std::endl; }
    
    virtual void  setUserInfoPtr( std::weak_ptr<IClientSessionUserData> userInfoPtr ) override { m_userInfoPtr = userInfoPtr; }
    virtual std::weak_ptr<IClientSessionUserData> getUserInfoPtr() override { return m_userInfoPtr; }

    virtual void sendMessage( std::string command ) override
    {
        std::shared_ptr<boost::asio::streambuf> wrStreambuf = std::make_shared<boost::asio::streambuf>();
        std::ostream os(&(*wrStreambuf));
        os << command;

        sendMessage( wrStreambuf );
    }

    virtual void sendMessage( std::shared_ptr<boost::asio::streambuf> wrStreambuf ) override
    {
        auto self(shared_from_this());

        async_write( m_socket, *wrStreambuf,
            [this,self,wrStreambuf] ( const boost::system::error_code& ec, std::size_t bytes_transferred  )
            {
                if ( ec )
                {
                    std::cout << "!!!! ClientSession::sendMessage error: " << ec.message() << std::endl;
                    exit(-1);
                }
            });
    }

    void readMessage()
    {
        auto self(shared_from_this());

        async_read_until( m_socket, m_streambuf, '\n',
            [this,self] ( const boost::system::error_code& ec, std::size_t bytes_transferred )
            {
                if ( ec )
                {
                    std::cout << "!!!! ClientSession::readMessage error: " << ec.message() << std::endl;
                    exit(-1);
                }
                else
                {
                    // handle message
                    m_game.handlePlayerMessage( *this, m_streambuf );
                    m_streambuf.consume(bytes_transferred);

                    // read next message
                    readMessage();
                }
        });
    }
};


class TcpServer
{
    IGame&          m_game;
    
    io_context&     m_ioContext;
    tcp::acceptor   m_acceptor;
    tcp::acceptor   m_acceptor2;
    tcp::socket     m_socket;
    tcp::socket     m_socket2;

    std::vector<std::shared_ptr<ClientSession>> m_sessions;

public:
    TcpServer( io_context& ioContext, IGame& game, int port ) :
        m_game(game),
        m_ioContext(ioContext),
        m_acceptor( m_ioContext, tcp::endpoint(tcp::v4(), port) ),
        m_acceptor2( m_ioContext, tcp::endpoint(tcp::v4(), port+1) ),
        m_socket(m_ioContext),
        m_socket2(m_ioContext)
    {
    }

    void execute()
    {
        post( m_ioContext, [this] { accept(); } );
        post( m_ioContext, [this] { accept2(); } );
        m_ioContext.run();
    }
    
    void accept()
    {
//        m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec) {
//            if (!ec)
//            {
//                std::make_shared<ClientSession>( m_ioContext, m_game, std::move(m_socket) )->readMessage();
//            }
//
//            accept();
//        });
        m_acceptor.async_accept( [this] (boost::system::error_code ec, ip::tcp::socket socket ) {
            if (!ec)
            {
                std::make_shared<ClientSession>( m_ioContext, m_game, std::move(socket) )->readMessage();
            }

            accept();
        });
    }
    void accept2()
    {
//        m_acceptor2.async_accept(m_socket2, [this](boost::system::error_code ec) {
//            if (!ec)
//            {
//                std::make_shared<ClientSession>( m_ioContext, m_game, std::move(m_socket2) )->readMessage();
//            }
//
//            accept2();
//        });
        m_acceptor2.async_accept( [this] (boost::system::error_code ec, ip::tcp::socket socket ) {
            if (!ec)
            {
                std::make_shared<ClientSession>( m_ioContext, m_game, std::move(socket) )->readMessage();
            }

            accept2();
        });
    }
};
