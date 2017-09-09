#include "stdafx.h"
#include "QFtpThread.h"


QFtpThread::QFtpThread()
{
}


QFtpThread::~QFtpThread()
{
}

void QFtpThread::run()
{
	ThreadInitialize();
	exec();
}

bool QFtpThread::ThreadInitialize()
{
	return false;
}

void QFtpThread::ThreadUninitialize()
{
}
