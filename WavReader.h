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
#ifndef WAVREADER_H_
#define WAVREADER_H_

#include <QtWidgets>
#include <qglobal.h>

typedef struct
{
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
} SYS_TIME;

typedef struct
{
	char           riff[4];
	unsigned long  chunkSize;
	char           wave[4];
	char           fmt[4];
	unsigned long  subchunk1Size;
	unsigned short audioFormat;
	unsigned short channels;
	unsigned long  samplesPerSec;
	unsigned long  bytesPerSec;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char           subchunk2ID[4];
	unsigned long  subchunk2Size;
}WAVHEADER;

typedef	struct{
	SYS_TIME StartTime;
	SYS_TIME StopTime;
	quint32 CenterFreq;
	quint32 ADFrequency;
	quint32 IFFrequency;
	quint32 Bandwidth;
	quint32 IQOffset;
	quint32 DBOffset;
	quint32 Unused3;
	quint32 Unused4;
	quint32 Unused5;
	quint32 Unused6;
}AUXINFO;

typedef	struct{
	SYS_TIME StartTime;
	SYS_TIME StopTime;
	quint32 CenterFreq;
	quint32 ADFrequency;
	quint32 IFFrequency;
	quint32 Bandwidth;
	quint32 IQOffset;
	quint32 DBOffset;
	quint32 Unused3;
	quint32 Unused4;
	quint32 Unused5;
}AUX3INFO;

typedef	struct{
	SYS_TIME StartTime;
	SYS_TIME StopTime;
	quint32 CenterFreq;
	quint32 ADFrequency;
	quint32 Bandwidth;
}AUX2INFO;

typedef	struct{
	SYS_TIME StartTime;
	quint32 CenterFreq;
	quint32 ADFrequency;
	quint32 Bandwidth;
}AUX1INFO;

typedef	struct{
	SYS_TIME StartTime;
	SYS_TIME StopTime;
	quint32 CenterFreq;
	quint32 Spare[32];
}AUXWINRAD;

typedef struct
{
	long nCenterFrequencyHz;
	long samplingRateIdx;
	long timeStart;
	unsigned short wAttenId;
	char bAdcPresel;
	char bAdcPreamp;
	char bAdcDither;
	char bSpare;
	char rsrvd[16];
 }PERSEUSE;

class WavReader : public QObject
{
		Q_OBJECT

	private:
		typedef enum
		{
			NONE = 0,
			EXPERTSDR,
			AUXI0,
			AUXI1,
			AUXI2,
			AUXI3,
			WINRAD,
			PERCEUSE
		}WAV_FORMAT;

	public:
		WavReader(QObject *parent = 0);

		bool open(QIODevice *pDev);
		void close();
		bool isOpen();
		int sampleRate();
		quint32 centralFrequency();

		bool readBuf(void *pInL, void *pInR, quint32 FrameCount);
		quint64 pos();
		quint64 size();
		QTime time();
        QDateTime dateTime();
        quint32 tuningFreq();
		QTime currentTime();
		bool setPos(quint64 position);
		void setLoop(bool on);

	private:

		typedef union
		{
			struct{
				qint8 b0;
				qint8 b1;
				qint8 b2;
				qint8 b3;
			}bytes;
			qint32 all;
		}sample32;

		QIODevice *pDevice;
		qint64 startPos;
		AUXINFO auxInfo;
		WAVHEADER wavHeader;
		bool isOpened;
		bool loop;
		qint32 fileSize;
		WAV_FORMAT wavFormat;
	signals:
		void isStart(bool);
		void waveSampleRate(int);
};

class WavWriter
{
	public:
		WavWriter();
		virtual ~WavWriter();
		bool open(QIODevice *pDev);
		void close();
		bool isOpen();
		void setFormat();
		void setFileName(QString name);
		QString getFileName();
		bool writeBuf(void *pInL, void *pInR, quint32 FrameCount);
		quint64 pos();
		quint64 size();
		bool setPos(quint64 position);
		void setSampleRate(int rate);
		void setCentralFreq(int freq);
		int getCentralFreq();

	private:
		QString fileName;
		QByteArray data;
		QBuffer *pBuffer;
		QIODevice *pDevice;
		QDateTime startDT, stopDt;
		qint64 startPos;
		bool isOpened;
		qint32 fileSize;
		AUXWINRAD winradInfo;
		WAVHEADER wavHeader;
		int sampleRate;

		typedef union
		{
			struct{
				qint8 b0;
				qint8 b1;
				qint8 b2;
				qint8 b3;
			}bytes;
			qint32 all;
		}sample32;

		typedef union
		{
			struct{
				qint8 b0;
				qint8 b1;
			}bytes;
			short all;
		}sample16;
};

#endif /* WAVREADER_H_ */
