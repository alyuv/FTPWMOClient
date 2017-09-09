#pragma once
#include <QThread>

class QFtpThread: public QThread
{
	Q_OBJECT
public:
	static QFtpThread* CreateInstance();

	QFtpThread();
	~QFtpThread();
protected:
	void run();

	bool ThreadInitialize();
	void ThreadUninitialize();

};

