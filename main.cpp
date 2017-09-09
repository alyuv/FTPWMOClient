#include "stdafx.h"
#include "ftpwmoclient.h"
#include <QtWidgets/QApplication>
#include <QFileSystemWatcher>


int main(int argc, char *argv[])
{	
	
	QApplication a(argc, argv);
	FTPWMOClient w;	
	w.show();
	return a.exec();
}
