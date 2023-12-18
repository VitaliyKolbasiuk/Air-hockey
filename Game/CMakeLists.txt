cmake_minimum_required(VERSION 3.5)

project(GameProj VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/vitaliykolbasiuk/Qt/6.6.0/gcc_64/lib/cmake")

if (APPLE)
    add_definitions(-DBOOST_ASIO_HAS_STD_INVOKE_RESULT)
    include_directories(SYSTEM /usr/local/include)
    set(CMAKE_PREFIX_PATH "~/Qt/6.4.0/macos")
    set(CMAKE_PREFIX_PATH "~/Qt/6.4.0/macos")
    set(BOOST_INCLUDE_DIR "/usr/local/include")
    set(BOOST_LIB_DIR "/usr/local/lib")

    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    set(OPENSSL_LIBRARIES "/usr/local/opt/openssl/lib")
    
    # for -lcrypto (I do not know who uses it!)
    link_directories(/usr/local/opt/openssl/lib)
endif()

if (WIN32)
    #set(BOOST_ROOT "C:/Boost/boost_1_82_0")
    #set(BOOST_INCLUDEDIR "C:/Boost/boost_1_82_0/")
    #set(BOOST_LIBRARYDIR "C:/Boost/boost_1_82_0/libs")
    #set(BOOST_COMPONENTS system)
    #find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

    #include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
    include_directories(SYSTEM ../../../Boost/boost_1_82_0/)
    #set(BOOST_INCLUDE_DIR "C:/Boost/boost_1_82_0/boost")
    #set(BOOST_LIB_DIR "C:/Boost/boost_1_82_0/libs")
    #set(CMAKE_PREFIX_PATH "X:/Qt/6.5.1/mingw_64")
    #set(CMAKE_PREFIX_PATH "X:/Qt/6.5.1/mingw_64")
    #set(BOOST_INCLUDE_DIR "/usr/local/include")
    #set(BOOST_LIB_DIR "/usr/local/lib")
    #set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    #set(OPENSSL_LIBRARIES "/usr/local/opt/openssl/lib")

# for -lcrypto (I do not know who uses it!)
    #link_directories(/usr/local/opt/openssl/lib)
endif()



find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(Server
        Server/Server.cpp
        Server/Server.h
        Server/ClientPlayer.h
        Server/Game.h
        Log.h
)

add_executable(Client
        Client/Client.cpp
        Client/TcpClient.h
        Client/QtClientPlayer.h
        Client/Scene.h
        Client/mainwindow.h
        Client/mainwindow.cpp
        Client/mainwindow.ui
        Log.h
)


add_executable(Client2
        Client/Client.cpp
        Client/TcpClient.h
        Client/QtClientPlayer.h
        Client/Scene.h
        Client/mainwindow.h
        Client/mainwindow.cpp
        Client/mainwindow.ui
        Log.h
)


target_link_libraries(Server PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Client2 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

