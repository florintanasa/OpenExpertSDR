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

#ifndef VOICERECORDER_H
#define VOICERECORDER_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QtCore>
#include <QtGlobal>

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
}WAVHEADER_V;

const unsigned short IEEE_FLOAT = 0x0003;

class VoiceRecorder : public QObject
{
	Q_OBJECT

	bool isOpened;
	bool isPlaying;
	bool isRecording;
	uint SampleRate;
	qint64  playStartPos;
	uint samplesPerSec;

	QString fileName;
	QFile waveFile;
	QByteArray voiceArray;
	WAVHEADER_V wavHeader;

	typedef union
	{
		struct{
			qint8 b0;
			qint8 b1;
		}bytes;
		short all;
	}sample16;

	int scaleDb;
	bool isTx;

public:
	VoiceRecorder();
	bool open(QString file);
	bool isOpen();
	void close();
	void Stop();
	bool onRec(bool state);
	bool isRec();
	bool onPlay(bool state);
	bool isPlay();
	void setSampleRate(uint value);
	QString getFileName();
	void BufferIO(void *pInL, void *pInR, quint32 FrameCount);

public slots:
	void setScaleDb(int value);
	void setTrxMode(bool state);

signals:
	void statePlay(bool);
	void stateRec(bool);
};

#endif // VOICERECORDER_H
