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

#ifndef FILELIST_H
#define FILELIST_H

#include <QtWidgets>
#include <QSysInfo>
#include "ui_fileList.h"
#include "wavSample.h"
#include "../WavReader.h"

class fileList : public QWidget
{
    Q_OBJECT

	QString wavePath;

	public:
		fileList(QWidget *parent = 0);
		~fileList();

		void addFile(QFileInfo &fileName);
		void Clear();

		void stop();
		bool isPlay();

		int sampleRate();
		void setSampleRate(int rate);
		quint32 centralFrequency();

		void readBuf(void *pInL, void *pInR, quint32 FrameCount);
        void writeSettings(QSettings *pSet);
        void readSettings(QSettings *pSet);
        void setCentralFreq(int freq);

public slots:
		void setWaveDir(QString path);

	private:
		Ui::fileListClass ui;
		wavSample *pSample;
		int lastRow;
		QString lastPath;
		WavReader *pReader;
		WavReader *pIqReader;
		WavWriter *pIqWriter;
		int loopMode;
		bool isPlayed;
		bool isRecorded;
		bool isPause;
        QFile iqFile;
        QFile iqFileWr;

        void timerEvent(QTimerEvent *event);
        int timerId;

	private slots:
		void onTwDoubleClick(int row, int colum);
		void onAddFile();
		void onDelFile();
		void onRec();
		void onPlay();
		void onPause();
		void onStop();
		void onLoop(bool status);
		void onStart(bool status);

	signals:
		void isStart(bool);
		void isRec(bool);
		void isPlay(bool);
		void isPaused(bool);
		void isChangedPlayFile(bool);
};

#endif // FILELIST_H
