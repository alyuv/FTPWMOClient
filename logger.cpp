#include "stdafx.h"
#include "logger.h"

#include "logger.h"

#include <QDir>
#include <QDebug>
#include <QDateTime>

Logger * Logger::self_ = 0;
int Logger::refCount_ = 0;

Logger::Logger(QString path):path_(path)
{
                
}

Logger* Logger::instance(QString path)
{
        if (!self_)
                self_ = new Logger(path);

        ++refCount_;
        return self_;
}

void Logger::freeInstance()
{
        refCount_--;

        if (!refCount_)
        {
                filewriter_->device()->close();
                delete this;
                self_ = 0;
        }
}

void Logger::addEntry(QString textEntry, bool isError)
{
        
        QTextStream out(stdout);

        QDateTime datetime = QDateTime::currentDateTime();              
        QString sdatetime = datetime.toString("yyyy.MM.dd hh:mm:ss.zzz");
        QString sdate = datetime.toString("/Gyyyy") + "/M" + datetime.toString("MM") + "/D" + datetime.toString("dd") + "/";
        
        QDir dir(path_ + sdate);
        if (!dir.exists())
                dir.mkpath(".");
        logFile_ = new QFile(path_ + sdate + "FtpWmoClient.log");

        if (logFile_->open(QFile::Append))
                filewriter_ = new QTextStream(logFile_);
        if (isError)
                textEntry = sdatetime + " [ERROR] " + textEntry;
        else
                textEntry = sdatetime + " [INFO]  " + textEntry;

        out << textEntry << endl;
        *filewriter_ << textEntry << endl;
        out.flush();
        filewriter_->device()->close();
        
        
}
