#include "stdafx.h"
#include "ftpwmoclient.h"
#include "WatchDirectory.h"
#include "settings.h"
//#include "General.h"

FTPWMOClient::FTPWMOClient(QWidget *parent)
        : QMainWindow(parent), ftpwmoclient_(0), connected_(false)
{
        QSettings settings_(QDir::currentPath() + "/FTPWMOClient.ini", QSettings::IniFormat);

        ftpwmoserver_= settings_.value("FTP/Host", "192.168.98.110").toString();
        user_ = settings_.value("FTP/User", "UAMC").toString();
        password_ = settings_.value("FTP/Password", "SadisU16").toString();
        loggingpath_ = settings_.value("LoggingPath", "D:/IRAM/ARX_PROT").toString();
        opmetdir_ = settings_.value("OpmetDir", "D:/IRAM/LINK/PRD_SADIS_FTP").toString();
        opmetftp_ = settings_.value("OpmetFtp", "OPMET").toString();
        marsdir_ = settings_.value("MarsDir", "D:/IRAM/LINK/PRD_MJMJ2").toString();
        marsftp_ = settings_.value("MarsFtp", "MARS").toString();
        cleardirs_ = settings_.value("ClearDirs", "true").toBool();
        firstlogging_ = true;

        ftpdirs_[opmetdir_] = opmetftp_;
        ftpdirs_[marsdir_] = marsftp_;

        logger_ = Logger::instance(loggingpath_);
        ui.setupUi(this);                               
        connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(actionAbout()));
        QString filename;       
        connectToFtpWmo();      

        if (cleardirs_)
        {               
                deleteFilesInDir(opmetdir_);
                deleteFilesInDir(marsdir_);
        }

        addWatchPath(opmetdir_);
        addWatchPath(marsdir_);
        connect(&fswatcher_, SIGNAL(directoryChanged(QString)), this, SLOT(changedDirectory(QString))); 
}

FTPWMOClient::~FTPWMOClient()
{
        logger_->freeInstance();        
}

void FTPWMOClient::doneCommand(bool error)
{
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif  
        if (error)
        {
                QString error;
                bool isknownerror = true;
                switch (ftpwmoclient_->error())
                {                       
                        case QFtp::HostNotFound:
                        {
                                error = "Cann't find FTP server - " + ftpwmoserver_;                            
                                break;
                        }               
                        case QFtp::NotConnected:
                        {
                                error = "Cann't connected to FTP server - " + ftpwmoserver_;                            
                                break;
                        }
                        case QFtp::ConnectionRefused:
                        {
                                error = "Connection refused by FTP server - " + ftpwmoserver_;                          
                                break;
                        }                       

                        default:
                        {
                                error = ftpwmoclient_->error() + " " + ftpwmoclient_->errorString().replace("\n", " "); // errorString() часто содержит \n                              
                                break;
                        }
                }               
                writeToLog("" + error, true);                                           
        }       
}

void FTPWMOClient::finishedCommand(int id, bool error)
{
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif
        switch (ftpwmoclient_->currentCommand())
        {
                case QFtp::Login:
                {
                        ui.statusBar->showMessage("Connected to FTP server: " + ftpwmoserver_);
                        return;
                }
                case QFtp::Put:
                {
                        if (currentfiles_->contains(id)) 
                        {
                                QFile *file = currentfiles_->take(id);

                                if (file->isOpen()) 
                                {               
                                        file->close();                                                                  
                                }                                                                       
                                writeToLog("File: " + file->fileName() + " is uploaded", false, bufr_);                         
                                file->remove();                         
                                file = NULL;                            
                        }               
                }               
                case QFtp::Rename:
                {               
                        if (error)
                        {
                                writeToLog("Unable to rename file on FTP server", error);
                        }
                }
                case QFtp::List:
                {
                        if (error)
                        {                               
                                writeToLog("Unable execute command list on FTP server", error);
                        }
                        else
                        {
                                bool exist1 = false;                            
                                bool exist2 = false;                            
                                for (int i = 0; i < dirlist_.size(); i++)
                                {
                                        if (dirlist_[i] == opmetftp_)                                           
                                                exist1 = true;                                  
                                        if (dirlist_[i] == marsftp_)
                                                exist2 = true;
                                }
                                if (!exist1)
                                        ftpwmoclient_->mkdir(opmetftp_);                                                                                
                                if (!exist2)
                                        ftpwmoclient_->mkdir(marsftp_);
                                
                        }                       
                }
                case QFtp::Mkdir:
                {
                        if (error)
                        {
                                //writeToLog("Unable execute command mkdir on FTP server", error);
                        }
                }
        }
}

void FTPWMOClient::addDirToList(const QUrlInfo & urlInfo)
{       
        ftpwmoclient_->cd("/");
        if (urlInfo.isDir())
        {
                isdir_[urlInfo.name()] = urlInfo.isDir();
                dirlist_.append(urlInfo.name());
        }
}

void FTPWMOClient::changedState()
{       
        switch (ftpwmoclient_->state())
        {
                case QFtp::LoggedIn:
                {
                        if ((ftpwmoclient_->error() == QFtp::NoError)&&(firstlogging_))
                        {
                                uploadFilesInDir(QDir(opmetdir_));
                                uploadFilesInDir(QDir(marsdir_));
                                firstlogging_ = false;                          
                        }
                }
                case QFtp::Unconnected:         
                {
                        //ftpwmoclient_->connectToHost(ftpwmoserver_, 21);
                        //ftpwmoclient_->login(username_, password_);
                }
                case QFtp::HostLookup:
                {
                        //writeToLog("A host name " +ftpwmoserver_+" lookup is in progress...");
                }
                case QFtp::Connecting:
                {
                        //writeToLog("An attempt to connect to the host " + ftpwmoserver_ + " is in progress...");
                }
                case QFtp::Connected:
                {
                        //writeToLog("Connection to the host " + ftpwmoserver_ + " has been achieved");
                }
                case QFtp::Closing:
                {
                        //connected_ = false;
                        //writeToLog("The connection is closing down, but it is not yet closed");
                }
        }               
}

void FTPWMOClient::actionAbout()
{
        QMessageBox::information(this, tr("About FTPWMOClient"), tr("FTP client for upload data to host via FTP WMO protocol"
                                                      "\n\nVersion 1.0.0.1\n@ 2016 Iuvchenko Oleksandr"
                                                                "\nAll rights reserved"), QMessageBox::Ok);     
}

void FTPWMOClient::processCommandReply(int replyCode, const QString & detail)
{
        //writeToLog(replyCode + " " + detail);
        //writeToLog(detail);
}

void FTPWMOClient::connectToFtpWmo()
{
        if (!ftpwmoclient_)
        {
#ifndef QT_NO_CURSOR
                setCursor(Qt::WaitCursor);
#endif
                ftpwmoclient_ = new QFtp(this);
                connect(ftpwmoclient_, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addDirToList(QUrlInfo)));
                connect(ftpwmoclient_, SIGNAL(done(bool)), this, SLOT(doneCommand(bool)));              
                connect(ftpwmoclient_, SIGNAL(commandStarted(int)), this, SLOT(startedCommand(int)));
                connect(ftpwmoclient_, SIGNAL(commandFinished(int, bool)), this, SLOT(finishedCommand(int, bool)));
                connect(ftpwmoclient_, SIGNAL(stateChanged(int)), this, SLOT(changedState()));          
                currentfiles_ = new QHash<int, QFile *>();

                isdir_.clear();
                dirlist_.clear();
                                
                ftpwmoclient_->connectToHost(ftpwmoserver_, 21);                
                ftpwmoclient_->login(user_, password_);
                ftpwmoclient_->list();  
        }       
}

void FTPWMOClient::uploadFile(QString filepath)
{       
        QFile *file = new QFile(filepath);              
        QFileInfo fileinfo(file->fileName());
        QString tmpfilename(fileinfo.fileName());
        tmpfilename = tmpfilename + ".tmp";
        
        if (!file->open(QIODevice::ReadWrite))
        {
                uploadFile(filepath);
                Sleep(10);
                return;
        }

        QByteArray filedata = file->readAll();
        QString bufr(filedata); 
        
        bufr.replace(QString("ZCZC 025"), QString("\x01"));
        bufr.replace(QString("NNNN"), QString("\x03")); 
        bufr_ = bufr;   
        filedata.clear();
        filedata.append(bufr);
        int commandId = ftpwmoclient_->put(filedata, tmpfilename);
        currentfiles_->insert(commandId, file); 
        ftpwmoclient_->rename(tmpfilename, tmpfilename.left(tmpfilename.length() - 4));
}

void FTPWMOClient::uploadFiles(QString path, QStringList filelist)
{
        if (ftpwmoclient_->state() == QFtp::Unconnected)
        {
                ftpwmoclient_->connectToHost(ftpwmoserver_, 21);
                ftpwmoclient_->login(user_, password_);
        }
        ftpwmoclient_->cd("/"); 
        ftpwmoclient_->cd(ftpdirs_[path]);      
        
        foreach(QString filename, filelist)
        {               
                uploadFile(path + "/" + filename);
        }                       
}

void FTPWMOClient::uploadFilesInDir(QDir dir)
{       
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        QStringList fileList = dir.entryList();
        uploadFiles(dir.absolutePath(), fileList);
}

void FTPWMOClient::deleteFilesInDir(QDir dir)
{
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        QStringList fileList = dir.entryList(); 
        QString filename;
        for (int i = 0; i < fileList.count(); i++)
        {
                filename = dir.path() + "/" + fileList[i];
                QFile *file = new QFile(filename);
                if (file->isOpen())
                {
                        file->close();
                }
                file->remove();         
        }
        if (fileList.count() > 0)
                writeToLog("Clear directory - " + dir.absolutePath() + " is done");
}

void FTPWMOClient::writeToLog(QString logstring, bool error, QString subitemstr)
{
        QTreeWidgetItem * item;
        item = new QTreeWidgetItem();   
        QPixmap pixmap(error ? ":/FTPWMOClient/Resources/error.png" : ":/FTPWMOClient/Resources/info.png");
        item->setIcon(0, pixmap);
        item->setText(0, getCurrentDateTime() + " "+ logstring );

        if (subitemstr != "")
        {       
                QTreeWidgetItem * subitem;
                subitem = new QTreeWidgetItem();
                subitem->setText(0, subitemstr);
                item->addChild(subitem);
                logger_->addEntry(logstring + "\n" + subitemstr + "\n", error);
        }
        else 
                logger_->addEntry(logstring, error);
        ui.loggingList->addTopLevelItem(item);

        if (ui.loggingList->topLevelItemCount() > 300)
        {               
                ui.loggingList->takeTopLevelItem(0);
        }
        
        ui.loggingList->topLevelItem(ui.loggingList->topLevelItemCount() - 1)->setSelected(true);
        if (ui.loggingList->topLevelItemCount() > 1)
                ui.loggingList->topLevelItem(ui.loggingList->topLevelItemCount() - 2)->setSelected(false);
        ui.loggingList->scrollToItem(item);
}

QString FTPWMOClient::getCurrentDateTime()
{
        QDateTime datetime = QDateTime::currentDateTime();      
        QString sdatetime = datetime.toString("yyyy.MM.dd hh:mm:ss");
        return sdatetime;
}

void FTPWMOClient::changedDirectory(QString path)
{
        QStringList currEntryList = currContents_[path];

        const QDir dir(path);
        QStringList newEntryList = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
        QSet<QString> newDirSet = QSet<QString>::fromList(newEntryList);
        QSet<QString> currentDirSet = QSet<QString>::fromList(currEntryList);
        QSet<QString> newFilesSet = newDirSet - currentDirSet;
        QStringList newFilesList = newFilesSet.toList();
        currContents_[path] = newEntryList;     
        if (!newFilesList.isEmpty())
        {
                uploadFiles(path, newFilesList);
        }
}

void FTPWMOClient::addWatchPath(QString path)
{               
        fswatcher_.addPath(path); 
        QFileInfo f(path);
        if (f.isDir())
        {
                const QDir dirw(path);          
                currContents_[path] = dirw.entryList(QDir::NoDotAndDotDot | QDir::Files);
        }
}

