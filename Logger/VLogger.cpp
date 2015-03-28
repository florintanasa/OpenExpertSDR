/*
 * This file is part of ExpertSDR
 *
 * ExpertSDR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * ExpertSDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Copyright (C) 2012 Valery Mikhaylovsky
 * The authors can be reached by email at maksimus1210@gmail.com
 */

#include "VLogger.h"


void logMassage(QtMsgType type, const char *msg)
{
	// TODO сейчас отключена запись лог файла
	//static VLogger log;
	//log.setMassage(type, msg);
}


VLogger::VLogger()
{
	fileName = QDir::homePath() + "/Application Data/Expert Electronics/ExpertSDR/ExpertSDR_Log.log";
	logFile.setFileName(fileName);
	messageOut.setDevice(&logFile);
}

void VLogger::setMassage(QtMsgType type, const char *msg)
{
	QString msClear("Log-clear");
	QString msgData("///");
	QString message = QString(msg);
	if(QString(msg) == msClear)
	{
		logFile.open(QIODevice::WriteOnly);
		logFile.close();
		return;
	}
	else if(message.left(3) == msgData)
	{
		addTextInLog(message);
		return;
	}
	switch (type)
	{
	case QtDebugMsg:
		addTextInLog("Date: " + QDate::currentDate().toString("dd MM yyyy  ") + "Time: " + QTime::currentTime().toString("hh:mm:ss  ") + "Debug: "    + message + "\n");
		break;
	case QtWarningMsg:
		addTextInLog("Date: " + QDate::currentDate().toString("dd MM yyyy  ") + "Time: " + QTime::currentTime().toString("hh:mm:ss  ") + "Warning: "  + message + "\n");
		break;
	case QtCriticalMsg:
		addTextInLog("Date: " + QDate::currentDate().toString("dd MM yyyy  ") + "Time: " + QTime::currentTime().toString("hh:mm:ss  ") + "Critical: " + message + "\n");
		break;
	case QtFatalMsg:
		addTextInLog("Date: " + QDate::currentDate().toString("dd MM yyyy  ") + "Time: " + QTime::currentTime().toString("hh:mm:ss  ") + "Fatal: "    + message + "\n");
		qApp->quit();
		break;
	}
}

void VLogger::addTextInLog(QString massage)
{
	logFile.open(QFile::WriteOnly | QFile::Append);
	messageOut << massage;
	logFile.close();
}
