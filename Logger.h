#pragma once
class Logger
{
public:
	static Logger* instance(QString path);

	void freeInstance();

	void addEntry(QString textEntry, bool isError = false);

protected:
	//Logger();

private:
	//TODO: use QScopedPointer
	Logger(QString path);
	static Logger * self_;
	static int refCount_;

	QFile * logFile_;
	QString path_;
	QTextStream * filewriter_;
};

