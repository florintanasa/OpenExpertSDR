
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
#ifndef WDGMEM_H
#define WDGMEM_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QSpinBox>
#include "ui_WdgMem.h"
#include "WdgAddStation.h"
#include "Defines.h"
#include "WavReader.h"
#include "Wav/fileList.h"

typedef struct
{
	QString path;
	QString name;
	QTime	time;
}FileInfo;

typedef struct
{
	QString Comment;
	int Freq;
	int Mode;
}MEM_ITEM;

class WdgMem : public QWidget
{
    Q_OBJECT

    friend class ExpertSDR_vA2_1;

	public:
		WdgMem(int *MainFreq, QButtonGroup *pMode, QWidget *parent = 0);
		~WdgMem();

		void readIqBuf(void *pInL, void *pInR, quint32 FrameCount);

	private:
		int *pMainFreq;
		QButtonGroup *pModeButtons;

		QAction *m_pEditorFocusAction;
        fileList *pIqList;

		WavReader *pIqReader;

		QString lastPath;
		QList<FileInfo> filesList;
		QFile currentFile;
		bool iqPlayed;

		int timerId;

		QList<MEM_ITEM> memList;

		void resizeEvent(QResizeEvent *event);
        void showEvent(QShowEvent *event);

		Ui::WdgMemClass ui;
		WdgAddStation *pAddStation;

		void addNewStation(QString Comment, QString Freq, QString Mode);
		bool isWavPlay();
		int  sampleRateIndex();
		int  sampleRate();
		quint32 centralFreq();
		void setCentralFreq(int freq);
		int getCentralFreq();

		void writeSettings(QSettings *pS);
		void readSettings(QSettings *pS);

		bool isPlay();
		void Stop();

	public slots:
		void SetAllocateRow(int index);
		int SelectedRowIndex();
		void Clear();


    private slots:
		void addNewStation(QString Comment, int Freq, int Mode);
		void OnTwDoubleClick(int Row, int Column);
		void OnAddMemory();
		void OnDelStation();
		void OnThisMemory();

		void OnAddIqFile();

	signals:
		void FreqMem(int Frequency, int Mode);
		void isStart(bool);
		void isChangedPlayFile(bool);

};

#endif // WDGMEM_H
