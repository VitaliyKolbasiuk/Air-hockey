#pragma once

#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include <QMouseEvent>

// messages from server to client
#define WAIT_2d_PLAYER_CMD  "WaitingSecondPlayer"
#define GAME_STARTED_CMD    "GameStarted"
#define UPDATE_SCENE_CMD    "UpdateScene"

// messages from client to server
#define START_GAME_CMD      "StartGame"
#define CLIENT_POSITION_CMD "ClientPosition"

using namespace boost::asio;
using ip::tcp;

class IClientSessionUserData
{
protected:
    virtual ~IClientSessionUserData() = default;
};

class IClientSession
{
public:
    virtual ~IClientSession() = default;

    virtual void sendMessage( std::string message ) = 0;
    virtual void sendMessage( std::shared_ptr<boost::asio::streambuf> wrStreambuf ) = 0;

    virtual void  setUserInfoPtr( std::weak_ptr<IClientSessionUserData> userInfoPtr ) = 0;
    virtual std::weak_ptr<IClientSessionUserData> getUserInfoPtr() = 0;
};

class IGame
{
protected:
    virtual ~IGame() = default;

public:
    virtual void handlePlayerMessage( IClientSession&, boost::asio::streambuf& message ) = 0;
};

class IClientPlayer
{
protected:
    virtual ~IClientPlayer() = default;

public:
    virtual void handleServerMessage( const std::string& command, boost::asio::streambuf& message ) = 0;
    //virtual const std::string& playerName() const = 0;
};

class IMouseEventHandler
{
protected:
    virtual ~IMouseEventHandler() = default;

public:
    virtual void mousePressEvent(QMouseEvent* event)   = 0;
    virtual void mouseMoveEvent(QMouseEvent* event)    = 0;
    virtual void mouseReleaseEvent(QMouseEvent* event) = 0;
};
