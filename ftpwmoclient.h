#ifndef FTPWMOCLIENT_H
#define FTPWMOCLIENT_H


#include <QtWidgets/QMainWindow>
#include "ui_ftpwmoclient.h"

#include <QFileSystemWatcher>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QSettings>
#include <QtFtp/qftp.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QDateTime>
#include <QTimer>
#include <QThread>

#include <QtCore/QMutex>

#include "logger.h"


class FTPWMOClient : public QMainWindow
{
        Q_OBJECT

public:
        FTPWMOClient(QWidget *parent = 0);
        ~FTPWMOClient();        
        
public slots:
        void doneCommand(bool);
        void finishedCommand(int, bool);
        void addDirToList(const QUrlInfo &urlInfo);
        void changedState();    
        void actionAbout();
        void processCommandReply(int replyCode, const QString & detail);


public slots:
        void changedDirectory(QString path);
        
public:
        void connectToFtpWmo();
        QString getCurrentDateTime();   
        void uploadFile(QString file);
        void uploadFiles(QString path, QStringList filelist);
        void uploadFilesInDir(QDir dir);
        void deleteFilesInDir(QDir dir);
        void writeToLog(QString logstring, bool error = false, QString subitemstr = "");
        void addWatchPath(QString path);                

private:
        
        Ui::FTPWMOClientClass ui;
        QFtp *ftpwmoclient_;            
        bool connected_;        
        
        QHash<int, QFile*> *currentfiles_;      
        QMap<QString, QString> ftpdirs_;
        QFileSystemWatcher fswatcher_;          
        QMap<QString, QStringList> currContents_;  
        QTimer *timer;                  
        Logger *logger_;        

        QSettings settings_;
        QString user_;
        QString password_;
        QString ftpwmoserver_;
        QHash<QString, bool> isdir_;
        QStringList dirlist_;
        QString opmetdir_;
        QString opmetftp_;
        QString marsdir_;
        QString marsftp_;
        QString loggingpath_;
        bool cleardirs_;
        bool firstlogging_;
        QString bufr_;
                
        int commandId_; 
};




#endif // FTPWMOCLIENT_H


