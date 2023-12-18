#pragma once


    #include <QDebug>
    #define LOG(expr)       qDebug() << "#" << expr;
    #define LOG_VAR(var)    qDebug() << "#" << #var << ": " << var;
    #define LOG_ERR(text)   qDebug() << "🟥 " << text;
//#else
//    #define LOG(expr) std::cerr << "#" << expr << std::endl;
//    #define LOG_VAR(var) std::cerr << "#" << #var << ": " << var << std::endl;
//    #define LOG_ERR(text) std::cerr << "🟥 " << text << std::endl;
//#endif
