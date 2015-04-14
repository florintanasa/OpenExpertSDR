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
#include <QtGlobal>

#include "VoiceRecorderWave.h"
#include <cstring>

float voiceFromDb(float Db)
{
	return (qPow(10.0,0.05*(Db)));
}

VoiceRecorder::VoiceRecorder()
	: QObject()
{
	isOpened = false;
	isPlaying = false;
	isRecording = false;
	playStartPos = 0;
	samplesPerSec = 0;

	scaleDb = 0;
	isTx = false;
}

bool VoiceRecorder::open(QString file)
{
	if(waveFile.isOpen())
		waveFile.close();

	waveFile.setFileName(file);
	if(!waveFile.open(QIODevice::ReadWrite))
	{
		 QMessageBox msgBox;
		 msgBox.setText("Can't open voice wave file.");
		 msgBox.setIcon(QMessageBox::Critical);
		 msgBox.exec();
		 qWarning() << "VoiceRecorder : Can't open voice wave file!";
		 isOpened = false;
		 return (isOpened);
	}
	waveFile.close();
	isOpened = false;
	return true;
}

bool VoiceRecorder::isOpen()
{
	return (isOpened);
}

void VoiceRecorder::Stop()
{
	if(isPlaying)
		onPlay(false);
	else if(isRecording)
		onRec(false);
}

void VoiceRecorder::close()
{
	isOpened = false;
	isPlaying = false;
	isRecording = false;
	waveFile.close();
}

bool VoiceRecorder::onRec(bool state)
{
	if(isOpened && isRecording && state)
		return (true);

	if(isPlaying)
		return (false);

	if(!state && !isRecording)
		return (true);
	else if(state && isRecording)
		return (true);

	if(!state)
	{
		unsigned long file_sz = waveFile.size();
		QString str;
		str = "RIFF";
        str = "WAVE";
        str = "fmt ";
        str = "data";

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        std::memcpy(wavHeader.riff, str.toLatin1().data(), 4);
        std::memcpy(wavHeader.wave, str.toLatin1().data(), 4);
        std::memcpy(wavHeader.fmt, str.toLatin1().data(), 4);
        std::memcpy(wavHeader.subchunk2ID, str.toLatin1().data(), 4);
#else
        qMemCopy(wavHeader.riff, str.toAscii().data(), 4);
        qMemCopy(wavHeader.wave, str.toAscii().data(), 4);
        qMemCopy(wavHeader.fmt, str.toAscii().data(), 4);
        qMemCopy(wavHeader.subchunk2ID, str.toAscii().data(), 4);
#endif

        wavHeader.chunkSize = file_sz - 8;
        wavHeader.subchunk1Size = 16;
		wavHeader.audioFormat = IEEE_FLOAT;
		wavHeader.channels = 2;
		wavHeader.samplesPerSec = samplesPerSec;
		wavHeader.bitsPerSample = 32;
		wavHeader.bytesPerSec = samplesPerSec*wavHeader.channels*wavHeader.bitsPerSample/8;
		wavHeader.blockAlign = wavHeader.channels * ((wavHeader.bitsPerSample + 7) / 8);

        wavHeader.subchunk2Size = file_sz - sizeof(WAVHEADER_V);
		waveFile.seek(0);
		waveFile.write(reinterpret_cast<char *>(&wavHeader), sizeof(wavHeader));
		waveFile.close();
		isRecording = false;
		isOpened = false;
	}
	else
	{
		if(!waveFile.open(QIODevice::WriteOnly))
		{
			 QMessageBox msgBox;
			 msgBox.setIcon(QMessageBox::Critical);
			 msgBox.setText("Can't open a file for writing!");
			 msgBox.exec();
			 isOpened = false;
			 qWarning() << "VoiceRecorder : can't open a file for writing!";
			 return (false);
		}
		waveFile.write(reinterpret_cast<char *>(&wavHeader), sizeof(wavHeader));
		voiceArray.clear();
		isOpened = true;
		isRecording = true;
	}
	return (true);
}

bool VoiceRecorder::isRec()
{
	return (isRecording);
}
bool VoiceRecorder::onPlay(bool state)
{
	if(isOpened && isPlaying && state)
		return (false);
	if(isRecording)
		return (false);
	if(!state && !isPlaying)
		return (true);
	else if(state && isPlaying)
		return (true);

	if(!state)
	{
		isPlaying = false;
		waveFile.seek(0);
		waveFile.close();

	}
	else
	{
		if(!waveFile.open(QIODevice::ReadOnly))
		{
			isOpened = false;
			isPlaying = false;
			emit statePlay(false);
			qWarning() << "VoiceRecorder : Can't open file for read!";
			return (false);
		}
		waveFile.seek(0);
		if(waveFile.read(reinterpret_cast<char *>(&wavHeader), sizeof(wavHeader)) < 0)
		{
			waveFile.close();
			isOpened = false;
			isPlaying = false;
			emit statePlay(false);
			qWarning() << "VoiceRecorder : file for read is empty!";
			return (false);
		}
		if((qstrncmp(wavHeader.subchunk2ID, "data", 4) != 0) || (qstrncmp(wavHeader.riff, "RIFF", 4) != 0) || (wavHeader.samplesPerSec != samplesPerSec) ||
				  (qstrncmp(wavHeader.wave, "WAVE", 4) != 0) || (qstrncmp(wavHeader.fmt , "fmt ", 4) != 0) || (wavHeader.audioFormat != IEEE_FLOAT))
		{
			waveFile.close();
			isOpened = false;
			isPlaying = false;
			emit statePlay(false);
			qWarning() << "VoiceRecorder : file for read -> bad wave header!";
			return (false);
		}
		playStartPos = waveFile.pos();
		isOpened = true;
		isPlaying = true;
	}
	return (true);
}

bool VoiceRecorder::isPlay()
{
	return (isPlaying);
}

void VoiceRecorder::BufferIO(void *pInL, void *pInR, quint32 FrameCount)
{
	if(!isOpened)
		return;

	float scale = voiceFromDb(scaleDb);

	if(isPlaying)
	{
		QByteArray dataBuf;
		float *pL, *pR;
		pL = (float*)pInL;
		pR = (float*)pInR;
		uint step = wavHeader.channels*(wavHeader.bitsPerSample/8);
		dataBuf = waveFile.read(FrameCount*wavHeader.channels*(wavHeader.bitsPerSample/8));
		float *pData = new float[FrameCount*2];
        //qMemSet(pData, 0, sizeof(float)*FrameCount);
        std::memset(pData, 0, sizeof(float)*FrameCount);
        if(dataBuf.size() == (int)(FrameCount * step))
		{
			memcpy(reinterpret_cast<char *>(pData), dataBuf.data(), dataBuf.size());
			for(uint i = 0, j = 0; i < FrameCount; i++, j += 2)
			{
				if(isTx)
				{
					pL[i] = pData[j]*scale;
					pR[i] = pData[j+1]*scale;
				}
				else
				{
					pL[i] = pData[j];
					pR[i] = pData[j+1];
				}
			}
			delete pData;
		}
		else
		{
			memcpy(reinterpret_cast<char *>(pData), dataBuf.data(), dataBuf.size());
			for(uint i = 0, j = 0; i < FrameCount; i++, j += 2)
			{
				if(isTx)
				{
					pL[i] = pData[j]*scale;
					pR[i] = pData[j+1]*scale;
				}
				else
				{
					pL[i] = pData[j];
					pR[i] = pData[j+1];
				}
			}
			delete pData;
			emit statePlay(false);
			isPlaying = false;
			isOpened = false;
			waveFile.close();
		}
	}
	else if(isRecording)
	{
		float *pL, *pR, *pData = new float[FrameCount*2];
		pL = (float*)pInL;
		pR = (float*)pInR;
		for(uint i = 0, j = 0; i < FrameCount; i++, j += 2)
		{
			pData[j] = pL[i];
			pData[j+1] = pR[i];
		}
		voiceArray = QByteArray(reinterpret_cast<char *>(pData), (FrameCount*2*sizeof(float)));
		waveFile.write(voiceArray);
		delete pData;
	}
}

void VoiceRecorder::setSampleRate(uint value)
{
	samplesPerSec = value;
}

QString VoiceRecorder::getFileName()
{
	return (fileName);
}

void VoiceRecorder::setScaleDb(int value)
{
	if(value < -100)
		scaleDb = -100;
	else if(value > 10)
		scaleDb = 10;
	else
		scaleDb = value;
}

void VoiceRecorder::setTrxMode(bool state)
{
	isTx = state;
}

