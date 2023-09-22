cmake_minimum_required(VERSION 3.5)

project(GameProj VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        QtClientPlayer.h
        CircleWidget.h
        Scene.h
        BoostClientServer/Server.h
        BoostClientServer/TcpClient.h
        BoostClientServer/Game.h
        BoostClientServer/ClientPlayer.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GameProj
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GameProj APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GameProj SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GameProj
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GameProj PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(GameProj PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS GameProj
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GameProj)
endif()
