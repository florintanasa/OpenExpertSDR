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
#include "WavReader.h"
#include <cstring>

WavReader::WavReader(QObject *parent) : QObject(parent)
{
	loop = false;
	isOpened = false;
	wavFormat = NONE;
	wavHeader.samplesPerSec = 0;

    //qMemSet(&auxInfo, 0, sizeof(auxInfo));
    std::memset(&auxInfo, 0, sizeof(auxInfo));
}

bool WavReader::open(QIODevice *pDev)
{
	close();
	if(!pDev)
	{
        //qMemSet(&auxInfo, 0, sizeof(auxInfo));
        std::memset(&auxInfo, 0, sizeof(auxInfo));
		return false;
	}
	pDevice = pDev;
	pDevice->open(QIODevice::ReadOnly);
	if(!pDevice->isOpen())
	{
		 QMessageBox msgBox;
		 msgBox.setIcon(QMessageBox::Critical);
		 msgBox.setText("Can't open a file for reading!");
		 msgBox.exec();
		qDebug() << "WavReader object: Can't open file!";
        //qMemSet(&auxInfo, 0, sizeof(auxInfo));
        std::memset(&auxInfo, 0, sizeof(auxInfo));
		return false;
	}
	if(pDevice->read(reinterpret_cast<char *>(&wavHeader), sizeof(wavHeader)) < 0)
	{
		pDevice->close();
		isOpened = false;
		qDebug() << "WavReader object: read error!";
        //qMemSet(&auxInfo, 0, sizeof(auxInfo));
        std::memset(&auxInfo, 0, sizeof(auxInfo));
		return false;
	}
	if(qstrncmp(wavHeader.subchunk2ID, "auxi", 4) == 0)
	{
		AUX1INFO oldinfo1;
		AUX2INFO oldinfo2;
		AUX3INFO oldinfo3;
		AUXWINRAD WinradInfo;
		switch(wavHeader.subchunk2Size)
		{
			case (sizeof(AUXINFO)):
				if(pDevice->read(reinterpret_cast<char *>(&auxInfo), sizeof(auxInfo)) < 0)
				{
					qDebug() << "WavReader object: Can't read auxi header!";
					pDevice->close();
					isOpened = false;
                    //qMemSet(&auxInfo, 0, sizeof(auxInfo));
                    std::memset(&auxInfo, 0, sizeof(auxInfo));
					return false;
				}
				wavFormat = AUXI0;
			break;
			case (sizeof(AUX3INFO)):
				if(pDevice->read(reinterpret_cast<char *>(&oldinfo3), sizeof(oldinfo3)) < 0)
				{
					qDebug() << "WavReader object: Can't read auxi3 header!";
					pDevice->close();
					isOpened = false;
                    //qMemSet(&auxInfo, 0, sizeof(auxInfo));
                    std::memset(&auxInfo, 0, sizeof(auxInfo));
					return false;
				}
				wavFormat = AUXI3;
				auxInfo.ADFrequency = oldinfo3.ADFrequency;
				auxInfo.Bandwidth = oldinfo3.Bandwidth;
				auxInfo.CenterFreq = oldinfo3.CenterFreq;
				auxInfo.IFFrequency = oldinfo3.IFFrequency;
				auxInfo.IQOffset = oldinfo3.IQOffset;
				auxInfo.DBOffset = oldinfo3.DBOffset;
				auxInfo.Unused3 = oldinfo3.Unused3;
				auxInfo.Unused4 = oldinfo3.Unused4;
				auxInfo.Unused5 = oldinfo3.Unused5;
				auxInfo.Unused6 = 0;
				break;
			case (sizeof(AUX2INFO)):
				if(pDevice->read(reinterpret_cast<char *>(&oldinfo2), sizeof(oldinfo2)) < 0)
				{
					qDebug() << "WavReader object: Can't read auxi2 header!";
					pDevice->close();
					isOpened = false;
                    //qMemSet(&auxInfo, 0, sizeof(auxInfo));
                    std::memset(&auxInfo, 0, sizeof(auxInfo));
                    return false;
				}
				wavFormat = AUXI2;
				auxInfo.ADFrequency = oldinfo2.ADFrequency;
				auxInfo.Bandwidth = oldinfo2.Bandwidth;
				auxInfo.CenterFreq = oldinfo2.CenterFreq;
				auxInfo.StartTime = oldinfo2.StartTime;
				auxInfo.StopTime = oldinfo2.StopTime;
				auxInfo.IFFrequency = 0;
				auxInfo.IQOffset = 0;
				auxInfo.DBOffset = 0;
				auxInfo.Unused3 = 0;
				auxInfo.Unused4 = 0;
				auxInfo.Unused5 = 0;
			break;
			case (sizeof(AUX1INFO)):
				if(pDevice->read(reinterpret_cast<char *>(&oldinfo1), sizeof(oldinfo1)) < 0)
				{
					qDebug() << "WavReader object: Can't read auxi1 header!";
					pDevice->close();
					isOpened = false;
                    //qMemSet(&auxInfo, 0, sizeof(auxInfo));
                    std::memset(&auxInfo, 0, sizeof(auxInfo));
                    return false;
				}
				wavFormat = AUXI1;
				auxInfo.ADFrequency = oldinfo1.ADFrequency;
				auxInfo.Bandwidth = oldinfo1.Bandwidth;
				auxInfo.CenterFreq = oldinfo1.CenterFreq;
				auxInfo.StartTime = oldinfo1.StartTime;
				auxInfo.StopTime = oldinfo1.StartTime;
				auxInfo.IFFrequency = 0;
				auxInfo.IQOffset = 0;
				auxInfo.DBOffset = 0;
				auxInfo.Unused3 = 0;
				auxInfo.Unused4 = 0;
				auxInfo.Unused5 = 0;
			break;
			case (sizeof(AUXWINRAD)):
				if(pDevice->read(reinterpret_cast<char *>(&WinradInfo), sizeof(WinradInfo)) < 0)
				{
					qDebug() << "WavReader object: Can't read WinRad header!";
					pDevice->close();
					isOpened = false;
                    //qMemSet(&auxInfo, 0, sizeof(auxInfo));
                    std::memset(&auxInfo, 0, sizeof(auxInfo));
					return false;
				}
				wavFormat = WINRAD;
				auxInfo.CenterFreq = WinradInfo.CenterFreq;
				auxInfo.ADFrequency = 66666667;
				auxInfo.Bandwidth = wavHeader.samplesPerSec;
				auxInfo.StartTime = WinradInfo.StartTime;
				auxInfo.StopTime = WinradInfo.StopTime;
				auxInfo.IFFrequency = 0;
				auxInfo.IQOffset = 0;
				auxInfo.DBOffset = 0;
				auxInfo.Unused3 = 0;
				auxInfo.Unused4 = 0;
				auxInfo.Unused5 = 0;
			break;
			default:
				qDebug() << "WavReader object: Undefined auxi header!";
				pDevice->close();
				isOpened = false;
                //qMemSet(&auxInfo, 0, sizeof(auxInfo));
                std::memset(&auxInfo, 0, sizeof(auxInfo));
            return false;
			break;
		}
		if(pDevice->read(reinterpret_cast<char *>(&wavHeader.subchunk2ID), 8) < 0)
		{
			qDebug() << "WavReader object: Can't read data chank in auxi header!";
			pDevice->close();
			isOpened = false;
            //qMemSet(&auxInfo, 0, sizeof(auxInfo));
            std::memset(&auxInfo, 0, sizeof(auxInfo));
            return false;
		}
		if(qstrncmp(wavHeader.subchunk2ID, "data", 4) != 0)
		{
			pDevice->close();
			isOpened = false;
            //qMemSet(&auxInfo, 0, sizeof(auxInfo));
            std::memset(&auxInfo, 0, sizeof(auxInfo));
            return false;
		}
		startPos = pDevice->pos();
	}
	else if(qstrncmp(wavHeader.subchunk2ID, "rcvr", 4) == 0)
	{
		PERSEUSE perseuseHdr;
		if(pDevice->read(reinterpret_cast<char *>(&perseuseHdr), sizeof(perseuseHdr)) < 0)
		{
			qDebug() << "WavReader object: Can't read perseuse header!";
			pDevice->close();
			isOpened = false;
            //qMemSet(&auxInfo, 0, sizeof(auxInfo));
            std::memset(&auxInfo, 0, sizeof(auxInfo));
            return false;
		}
		wavFormat = PERCEUSE;
		auxInfo.ADFrequency = 80000000;
		switch(perseuseHdr.samplingRateIdx)
		{
			case 0:
				auxInfo.Bandwidth = 125000;
				break;
			case 1:
				auxInfo.Bandwidth = 250000;
				break;
			case 2:
				auxInfo.Bandwidth = 500000;
				break;
			case 3:
				auxInfo.Bandwidth = 1000000;
				break;
			case 4:
				auxInfo.Bandwidth = 2000000;
				break;
			default:
				auxInfo.Bandwidth = 125000;
				break;
		}
		auxInfo.CenterFreq = perseuseHdr.nCenterFrequencyHz;

		auxInfo.StopTime = auxInfo.StartTime;
		auxInfo.IFFrequency = 0;
		auxInfo.IQOffset = 0;
		auxInfo.DBOffset = 0;
		auxInfo.Unused3 = 0;
		auxInfo.Unused4 = 0;
		auxInfo.Unused5 = 0;

		if(pDevice->read(reinterpret_cast<char *>(&wavHeader.subchunk2ID), 8) < 0)
		{
			qDebug() << "WavReader object: Can't read data chank in perseuse data header!";
			pDevice->close();
            //qMemSet(&auxInfo, 0, sizeof(auxInfo));
            std::memset(&auxInfo, 0, sizeof(auxInfo));
            return false;
		}
		if(qstrncmp(wavHeader.subchunk2ID, "data", 4) != 0)
		{
			qDebug() << "WavReader object: Can't find data chank in perseuse data header!";
			pDevice->close();
			isOpened = false;
            //qMemSet(&auxInfo, 0, sizeof(auxInfo));
            std::memset(&auxInfo, 0, sizeof(auxInfo));
            return false;
		}
		startPos = pDevice->pos();
	}
	else if(qstrncmp(wavHeader.subchunk2ID, "data", 4) == 0)
		startPos = pDevice->pos();
	else
		return false;

	isOpened = true;
	return true;
}

void WavReader::close()
{
	if(isOpened)
	{
		pDevice->close();
		isOpened = false;
	}
}

bool WavReader::isOpen()
{
	return isOpened;
}

int WavReader::sampleRate()
{
	return wavHeader.samplesPerSec;
}

quint32 WavReader::centralFrequency()
{
	return auxInfo.CenterFreq;
}

bool WavReader::readBuf(void *pInL, void *pInR, quint32 FrameCount)
{
	int i = 0;
	float *pL, *pR;
	QByteArray Buf;
	int step = wavHeader.channels*(wavHeader.bitsPerSample/8);
	int End;
	pL = (float*)pInL;
	pR = (float*)pInR;
	sample32 sampL, sampR;
	Buf = pDevice->read(FrameCount*wavHeader.channels*(wavHeader.bitsPerSample/8));
	if(Buf.size() == (int)(FrameCount * step))
	{
		for(i = 0; i < Buf.size(); i += step)
		{
			if(wavHeader.bitsPerSample == 16)
			{
				sampL.bytes.b0 = 0;
				sampL.bytes.b1 = 0;
				sampL.bytes.b2 = Buf.data()[i];
				sampL.bytes.b3 = Buf.data()[i+1];
				*pL++ = (float)(sampL.all/2147483647.0);

				sampR.bytes.b0 = 0;
				sampR.bytes.b1 = 0;
				sampR.bytes.b2 = Buf.data()[i+2];
				sampR.bytes.b3 = Buf.data()[i+3];
				*pR++ = (float)(sampR.all/2147483647.0);
			}
			else if(wavHeader.bitsPerSample == 24)
			{
				sampL.bytes.b0 = 0;
				sampL.bytes.b1 = Buf.data()[i];
				sampL.bytes.b2 = Buf.data()[i+1];
				sampL.bytes.b3 = Buf.data()[i+2];
				*pL++ = (float)(sampL.all/2147483647.0);

				sampR.bytes.b0 = 0;
				sampR.bytes.b1 = Buf.data()[i+3];
				sampR.bytes.b2 = Buf.data()[i+4];
				sampR.bytes.b3 = Buf.data()[i+5];
				*pR++ = (float)(sampR.all/2147483647.0);
			}
			else if(wavHeader.bitsPerSample == 32)
			{
				sampL.bytes.b0 = Buf.data()[i];
				sampL.bytes.b1 = Buf.data()[i+1];
				sampL.bytes.b2 = Buf.data()[i+2];
				sampL.bytes.b3 = Buf.data()[i+3];
				*pL++ = (float)(sampL.all/2147483647.0);

				sampR.bytes.b0 = Buf.data()[i+4];
				sampR.bytes.b1 = Buf.data()[i+5];
				sampR.bytes.b2 = Buf.data()[i+6];
				sampR.bytes.b3 = Buf.data()[i+7];
				*pR++ = (float)(sampR.all/2147483647.0);
			}
		}
	}
	else
	{
		for(i = 0; i < Buf.size(); i += step)
		{
			if(wavHeader.bitsPerSample == 16)
			{
				sampL.bytes.b0 = 0;
				sampL.bytes.b1 = 0;
				sampL.bytes.b2 = Buf.data()[i];
				sampL.bytes.b3 = Buf.data()[i+1];
				*pL++ = (float)(sampL.all/2147483647.0);

				sampR.bytes.b0 = 0;
				sampR.bytes.b1 = 0;
				sampR.bytes.b2 = Buf.data()[i+2];
				sampR.bytes.b3 = Buf.data()[i+3];
				*pR++ = (float)(sampR.all/2147483647.0);
			}
			else if(wavHeader.bitsPerSample == 24)
			{
				sampL.bytes.b0 = 0;
				sampL.bytes.b1 = Buf.data()[i];
				sampL.bytes.b2 = Buf.data()[i+1];
				sampL.bytes.b3 = Buf.data()[i+2];
				*pL++ = (float)(sampL.all/2147483647.0);

				sampR.bytes.b0 = 0;
				sampR.bytes.b1 = Buf.data()[i+3];
				sampR.bytes.b2 = Buf.data()[i+4];
				sampR.bytes.b3 = Buf.data()[i+5];
				*pR++ = (float)(sampR.all/2147483647.0);
			}
			else if(wavHeader.bitsPerSample == 32)
			{
				sampL.bytes.b0 = Buf.data()[i];
				sampL.bytes.b1 = Buf.data()[i+1];
				sampL.bytes.b2 = Buf.data()[i+2];
				sampL.bytes.b3 = Buf.data()[i+3];
				*pL++ = (float)(sampL.all/2147483647.0);

				sampR.bytes.b0 = Buf.data()[i+4];
				sampR.bytes.b1 = Buf.data()[i+5];
				sampR.bytes.b2 = Buf.data()[i+6];
				sampR.bytes.b3 = Buf.data()[i+7];
				*pR++ = (float)(sampR.all/2147483647.0);
			}
		}

		pDevice->seek(startPos);
		End = (FrameCount*wavHeader.channels*(wavHeader.bitsPerSample/8)) - Buf.size();
		Buf = pDevice->read(End);

		if(loop)
		{
			while(i < End)
			{
				if(wavHeader.bitsPerSample == 16)
				{
					sampL.bytes.b0 = 0;
					sampL.bytes.b1 = 0;
					sampL.bytes.b2 = Buf.data()[i];
					sampL.bytes.b3 = Buf.data()[i+1];
					*pL++ = (float)(sampL.all/2147483647.0);

					sampR.bytes.b0 = 0;
					sampR.bytes.b1 = 0;
					sampR.bytes.b2 = Buf.data()[i+2];
					sampR.bytes.b3 = Buf.data()[i+3];
					*pR++ = (float)(sampR.all/2147483647.0);
				}
				else if(wavHeader.bitsPerSample == 24)
				{
					sampL.bytes.b0 = 0;
					sampL.bytes.b1 = Buf.data()[i];
					sampL.bytes.b2 = Buf.data()[i+1];
					sampL.bytes.b3 = Buf.data()[i+2];
					*pL++ = (float)(sampL.all/2147483647.0);

					sampR.bytes.b0 = 0;
					sampR.bytes.b1 = Buf.data()[i+3];
					sampR.bytes.b2 = Buf.data()[i+4];
					sampR.bytes.b3 = Buf.data()[i+5];
					*pR++ = (float)(sampR.all/2147483647.0);
				}
				else if(wavHeader.bitsPerSample == 32)
				{
					sampL.bytes.b0 = Buf.data()[i];
					sampL.bytes.b1 = Buf.data()[i+1];
					sampL.bytes.b2 = Buf.data()[i+2];
					sampL.bytes.b3 = Buf.data()[i+3];
					*pL++ = (float)(sampL.all/2147483647.0);

					sampR.bytes.b0 = Buf.data()[i+4];
					sampR.bytes.b1 = Buf.data()[i+5];
					sampR.bytes.b2 = Buf.data()[i+6];
					sampR.bytes.b3 = Buf.data()[i+7];
					*pR++ = (float)(sampR.all/2147483647.0);
				}
				i += step;
			}
		}
		else
		{
			while(i < End)
			{
				*pL++ = 0.0;
				*pR++ = 0.0;
				i += step;
			}
			emit isStart(false);
		}
	}
	return true;
}

quint64 WavReader::pos()
{
	return (pDevice->pos()/wavHeader.channels/(wavHeader.bitsPerSample/8));
}

quint64 WavReader::size()
{
	return (wavHeader.subchunk2Size/wavHeader.channels/(wavHeader.bitsPerSample/8));
}

QTime WavReader::time()
{
	QTime time;
	int secs = size() / (qreal)wavHeader.samplesPerSec;
	int minute;
	int hour;

	hour = secs / 3600;
	secs = secs % 3600;
	minute = secs / 60;
	secs = secs % 60;

	time.setHMS(hour, minute, secs);

	return time;
}

QDateTime WavReader::dateTime()
{
	QDate date(auxInfo.StartTime.wYear, auxInfo.StartTime.wMonth, auxInfo.StartTime.wDay);
	QTime time(auxInfo.StartTime.wHour, auxInfo.StartTime.wMinute, auxInfo.StartTime.wSecond);
	QDateTime dTime(date, time);
	return(dTime);
}

quint32 WavReader::tuningFreq()
{
	return auxInfo.CenterFreq;
}

QTime WavReader::currentTime()
{
	QTime time;
	return time.addSecs(pos() / (qreal)wavHeader.samplesPerSec);
}

bool WavReader::setPos(quint64 position)
{
	quint64 p = position * wavHeader.channels * (wavHeader.bitsPerSample/8);
	return (pDevice->seek(p));
}

void WavReader::setLoop(bool on)
{
	loop = on;
}

WavWriter::WavWriter()
{

}

WavWriter::~WavWriter()
{
	isOpened = false;
}

bool WavWriter::open(QIODevice *pDev)
{
	qDebug() << "WavWriter: open( " << pDev << " )";
	if(pDev == 0)
	{
		qWarning() << "Device for write is not specified!";
		return false;
	}
	pDevice = pDev;
	pDevice->open(QIODevice::WriteOnly);
	if(!pDevice->isOpen())
	{
		 QMessageBox msgBox;
		 msgBox.setIcon(QMessageBox::Critical);
		 msgBox.setText("Can't open a file for writing!");
		 msgBox.exec();
		qWarning() << "WavWriter object: Can't open file!";
		return false;
	}
	pDevice->write(reinterpret_cast<char *>(&wavHeader), sizeof(wavHeader));
	pDevice->write(reinterpret_cast<char *>(&winradInfo), sizeof(winradInfo));
	pDevice->write(reinterpret_cast<char *>(&wavHeader.subchunk2ID), 8);
	data.clear();
	isOpened = true;
	startDT = QDateTime::currentDateTime();
	return true;
}

void WavWriter::close()
{
	if(!isOpened)
		return;

	stopDt = QDateTime::currentDateTime();
	int tmpSize = pDevice->pos();
	QString str;
    str = "RIFF";
    //qMemCopy(wavHeader.riff, str.toAscii().data(), 4);
    //std::memset(wavHeader.riff, str.toLatin1().data(), 4);
    std::memset(wavHeader.riff, str.toLatin1().toInt(), 4);
    wavHeader.chunkSize = 8;
	str = "WAVE";
    //qMemCopy(wavHeader.wave, str.toAscii().data(), 4);
    //std::memset(wavHeader.wave, str.toLatin1().data(), 4);
    std::memset(wavHeader.wave, str.toLatin1().toInt(), 4);
    str = "fmt ";
    //qMemCopy(wavHeader.fmt, str.toAscii().data(), 4);
    //std::memset(wavHeader.fmt, str.toLatin1().data(), 4);
    std::memset(wavHeader.fmt, str.toLatin1().toInt(), 4);
    wavHeader.subchunk1Size = 16;
	wavHeader.audioFormat = 1;
	wavHeader.channels = 2;
	wavHeader.samplesPerSec = sampleRate;
	wavHeader.bitsPerSample = 24;
	wavHeader.bytesPerSec = sampleRate*wavHeader.channels*wavHeader.bitsPerSample/8;
	wavHeader.blockAlign = 6;
	str = "auxi";
    //qMemCopy(wavHeader.subchunk2ID, str.toAscii().data(), 4);
    //std::memset(wavHeader.subchunk2ID, str.toLatin1().data(), 4);
    std::memset(wavHeader.subchunk2ID, str.toLatin1().toInt(), 4);
    wavHeader.subchunk2Size = sizeof(winradInfo);
	pDevice->seek(0);
	pDevice->write(reinterpret_cast<char *>(&wavHeader), sizeof(wavHeader));
	SYS_TIME sysTime;
	QDate date;
	QTime time;
	date = startDT.date();
	time = startDT.time();
	sysTime.wYear = date.year();
	sysTime.wMonth = date.month();
	sysTime.wDay = date.day();
	sysTime.wDayOfWeek = date.dayOfWeek();
	sysTime.wHour = time.hour();
	sysTime.wMinute = time.minute();
	sysTime.wSecond = time.second();
	sysTime.wMilliseconds = time.msec();
	winradInfo.StartTime = sysTime;
	date = stopDt.date();
	time = stopDt.time();
	sysTime.wYear = date.year();
	sysTime.wMonth = date.month();
	sysTime.wDay = date.day();
	sysTime.wDayOfWeek = date.dayOfWeek();
	sysTime.wHour = time.hour();
	sysTime.wMinute = time.minute();
	sysTime.wSecond = time.second();
	sysTime.wMilliseconds = time.msec();
	winradInfo.StopTime = sysTime;
	pDevice->write(reinterpret_cast<char *>(&winradInfo), sizeof(winradInfo));
	str = "data";
    //qMemCopy(wavHeader.subchunk2ID, str.toAscii().data(), 4);
    //std::memset(wavHeader.subchunk2ID, str.toLatin1().data(), 4);
    std::memset(wavHeader.subchunk2ID, str.toLatin1().toInt(), 4);
    wavHeader.subchunk2Size = tmpSize;
	pDevice->write(reinterpret_cast<char *>(&wavHeader.subchunk2ID), 8);
	pDevice->close();
	isOpened = false;
}

bool WavWriter::isOpen()
{
	return isOpened;
}

bool WavWriter::writeBuf(void *pInL, void *pInR, quint32 FrameCount)
{
	quint32 i = 0;
	float *pL, *pR;
	pL = (float*)pInL;
	pR = (float*)pInR;
	sample32 dTmp;
	data.clear();
	for(i = 0; i < FrameCount; i++)
	{
		dTmp.all = (qint32)(pL[i]*2147483647.0f);
		data.append(dTmp.bytes.b1);
		data.append(dTmp.bytes.b2);
		data.append(dTmp.bytes.b3);
		dTmp.all = (qint32)(pR[i]*2147483647.0f);
		data.append(dTmp.bytes.b1);
		data.append(dTmp.bytes.b2);
		data.append(dTmp.bytes.b3);
	}
	pDevice->write(data);
	return false;
}

quint64 WavWriter::pos()
{
	return (data.size()/wavHeader.channels/(wavHeader.bitsPerSample/8));
}

quint64 WavWriter::size()
{
	return (data.size()/wavHeader.channels/(wavHeader.bitsPerSample/8));
}

bool WavWriter::setPos(quint64 position)
{
	quint64 p = position * wavHeader.channels * (wavHeader.bitsPerSample/8);
	return (pDevice->seek(p));
}

void WavWriter::setSampleRate(int rate)
{
	sampleRate = rate;
}

void WavWriter::setCentralFreq(int freq)
{
	winradInfo.CenterFreq = (uint)freq;
}

int WavWriter::getCentralFreq()
{
	return winradInfo.CenterFreq;
}

void WavWriter::setFileName(QString name)
{
	fileName = name;
}

QString WavWriter::getFileName()
{
	return fileName;
}

