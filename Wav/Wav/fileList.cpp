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

#include "fileList.h"

fileList::fileList(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	pReader = new WavReader;
	pIqReader = new WavReader;
	pIqWriter = new WavWriter;
	connect(pIqReader, SIGNAL(isStart(bool)), this, SLOT(onStart(bool)));
	ui.twList->horizontalHeader()->setStretchLastSection(true);
	ui.twList->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(ui.twList, SIGNAL(cellDoubleClicked (int, int)), this, SLOT(onTwDoubleClick(int, int)));
	lastRow = 1;
	loopMode = 0;
	isPlayed = false;
	isRecorded = false;
	isPause = false;

	connect(ui.pbAddFile, SIGNAL(clicked()), this, SLOT(onAddFile()));
	connect(ui.pbDelFile, SIGNAL(clicked()), this, SLOT(onDelFile()));
	connect(ui.pbRec, SIGNAL(clicked()), this, SLOT(onRec()));
	connect(ui.pbPlay, SIGNAL(clicked()), this, SLOT(onPlay()));
	connect(ui.pbPause, SIGNAL(clicked()), this, SLOT(onPause()));
	connect(ui.pbStop, SIGNAL(clicked()), this, SLOT(onStop()));
	connect(ui.pbBack, SIGNAL(clicked()), this, SLOT(onBack()));
	connect(ui.pbLoop, SIGNAL(clicked(bool)), this, SLOT(onLoop(bool)));

	ui.pbPause->setEnabled(false);
}

fileList::~fileList()
{
	delete pReader;
	delete pIqReader;
	delete pIqWriter;
}

void fileList::setSampleRate(int rate)
{
	pIqWriter->setSampleRate(rate);
}


void fileList::writeSettings(QSettings *pSet)
{
    QStringList List;
    for(int i = 0; i < ui.twList->rowCount(); i++)
        List << reinterpret_cast<wavSample*>(ui.twList->cellWidget(i ,0))->fileName();
    pSet->beginGroup("IqWavList");
         pSet->setValue("LastIqFilePath", lastPath);
         pSet->setValue("IqFiles", List);
    pSet->endGroup();
}

void fileList::readSettings(QSettings *pSet)
{
    QStringList files;
    QFileInfo info;

    pSet->beginGroup("IqWavList");
        lastPath = pSet->value("LastIqFilePath", QDir::homePath()).toString(); // место открытия файлов
        files = pSet->value("IqFiles", 0).toStringList();
    pSet->endGroup();

    for(int i = 0; i < files.count(); i++)
    {
        info.setFile(files.at(i));
        if(info.isFile())
            addFile(info);
    }
}

void fileList::timerEvent(QTimerEvent *event)
{
	ui.teIqTime->setTime(pIqReader->currentTime());
	ui.slIqTrack->setValue(pIqReader->currentTime().second());
}

void fileList::addFile(QFileInfo &fileName)
{
	QFile file;
	file.setFileName(fileName.absoluteFilePath());
	if(!pReader->open(&file))
	{
		QMessageBox msgBox;
		msgBox.setText("Can't open Wav file. It have incorrect format.");
		msgBox.exec();
		return;
	}
	int index = ui.twList->rowCount();
	wavSample *pSmpl = new wavSample;
	pSmpl->setFileName(file.fileName());
	pSmpl->setNumber(index+1);
    pSmpl->setPlayTime(pReader->time());
	pSmpl->setDateTime(pReader->dateTime());
    pSmpl->setFrequency(pReader->tuningFreq());
	ui.twList->insertRow(index);
	if(ui.twList->columnCount() < 1) ui.twList->insertColumn(0);
	ui.twList->setCellWidget(index, 0, pSmpl);
	pReader->close();
}

void fileList::Clear()
{
	for(int i = ui.twList->rowCount()-1; i >= 0; i--)
		ui.twList->removeRow(i);
}

void fileList::onTwDoubleClick(int row, int colum)
{
    if(lastRow < ui.twList->rowCount())
    {
        reinterpret_cast<wavSample*>(ui.twList->cellWidget(lastRow ,0))->setActive(false);
        reinterpret_cast<wavSample*>(ui.twList->cellWidget(lastRow ,0))->setStyleSheet("QLabel{background-color: transparent; color: rgb(220, 220, 220);}");
    }
	reinterpret_cast<wavSample*>(ui.twList->cellWidget(row ,0))->setActive(true);
	reinterpret_cast<wavSample*>(ui.twList->cellWidget(row ,0))->setStyleSheet("QLabel{background-color: transparent; color: rgb(255, 106, 0);}");
	lastRow = row;
	onPlay();
}

void fileList::onAddFile()
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Open IQ Wav file"), lastPath, tr("IQ Wav files (*.wav *.wave )"));
	QFileInfo info;

	for(int i = 0; i < files.count(); i++)
	{
		info.setFile(files.at(i));
		if(info.isFile())
		{
			lastPath = info.filePath();
			addFile(info);
		}
	}
}

void fileList::onDelFile()
{
	ui.twList->removeRow(ui.twList->currentRow());
	for(int i = 0; i < ui.twList->rowCount(); i++)
		reinterpret_cast<wavSample*>(ui.twList->cellWidget(i ,0))->setNumber(i+1);
}

void fileList::stop()
{
	onStop();
}

bool fileList::isPlay()
{
	return isPlayed;
}

int fileList::sampleRate()
{
	return pIqReader->sampleRate();
}

quint32 fileList::centralFrequency()
{
	return pIqReader->centralFrequency();
}

void fileList::onRec()
{
	if(ui.pbPlay->isChecked())
	{
		ui.pbRec->setChecked(false);
	}
	else
	{
		ui.pbPlay->setEnabled(false);
		ui.pbLoop->setChecked(false);
		ui.pbLoop->setEnabled(false);
		ui.pbBack->setEnabled(false);
		ui.pbRec->setChecked(true);

		QDate date = QDate::currentDate();
		QTime time = QTime::currentTime();

		QString fileName;
		QDir dir;
		QTextCodec *pTxtCodec = QTextCodec::codecForName("CP1251");

		switch(QSysInfo::WindowsVersion)
		{
		case QSysInfo::WV_2000:
		case QSysInfo::WV_XP:
		case QSysInfo::WV_2003:
				if(!dir.exists(QDir::homePath() + "/" + pTxtCodec->toUnicode("Мои документы") + "/ExpertSDR"))
					dir.mkdir(QDir::homePath() + "/" + pTxtCodec->toUnicode("Мои документы") + "/ExpertSDR");
				fileName = QDir::homePath() + "/" + pTxtCodec->toUnicode("Мои документы") + "/ExpertSDR/" + "ExpertSDR_" + date.toString("dd_MM_yyyy") +"__"
											+ time.toString("hh_mm_ss") + "__F"+ QString::number(pIqWriter->getCentralFreq()) + "Hz.wav";
			break;
		case QSysInfo::WV_VISTA:
		case QSysInfo::WV_WINDOWS7:
				if(!dir.exists(QDir::homePath() + "/Documents/ExpertSDR"))
					dir.mkdir(QDir::homePath() + "/Documents/ExpertSDR");
				fileName = QDir::homePath() + "/Documents/ExpertSDR/" + "ExpertSDR_" + date.toString("dd_MM_yyyy") +"__"
											+ time.toString("hh_mm_ss") + "__F"+ QString::number(pIqWriter->getCentralFreq()) + "Hz.wav";
			break;
		}

		iqFileWr.setFileName(fileName);
		if(!pIqWriter->open(&iqFileWr))
			ui.pbRec->setChecked(false);
		else
		{
			isRecorded = true;
			pIqWriter->setFileName(fileName);
			ui.pbPause->setEnabled(true);
			emit isRec(true);
		}
	}
}

void fileList::onPlay()
{
    int currentTrack = 0;
	if(ui.pbRec->isChecked())
	{
		ui.pbPlay->setChecked(false);
		isPlayed = false;
	}
	else
	{
		if(ui.twList->rowCount() < 1)
		{
			ui.pbPlay->setChecked(false);
			isPlayed = false;
			QMessageBox::warning(this, tr("ExpertSDR message"),
			                           tr("Can't play IQ files.\n"
			                              "Playlist is empty. Please add IQ wav files to the list and try again."),
			                              QMessageBox::Ok);
			onAddFile();
			return;
		}
		ui.pbRec->setEnabled(false);
		ui.pbPlay->setChecked(true);
		if(pIqReader->isOpen()) pIqReader->close();
        currentTrack = ui.twList->currentRow();
        if(currentTrack < 0)
        {
            currentTrack = 0;
            ui.twList->setCurrentCell(0, 0);
            reinterpret_cast<wavSample*>(ui.twList->cellWidget(0 ,0))->setStyleSheet("QLabel{background-color: transparent; color: rgb(255, 106, 0);}");
            reinterpret_cast<wavSample*>(ui.twList->cellWidget(0 ,0))->setActive(true);
            lastRow = 0;
        }
        iqFile.setFileName(reinterpret_cast<wavSample*>(ui.twList->cellWidget(currentTrack ,0))->fileName());
        pIqReader->open(&iqFile);
        if(!pIqReader->isOpen())
        {
            ui.pbPlay->setChecked(false);
            isPlayed = false;
            return;
        }
        ui.slIqTrack->setMaximum(pIqReader->time().second() + pIqReader->time().minute()*60 + pIqReader->time().hour()*3600);
        timerId = startTimer(100);
        if(!isPlayed)
        	emit isPlay(true);
        else
        	emit isChangedPlayFile(true);
		isPlayed = true;
		ui.pbPause->setEnabled(true);

	}
}

void fileList::onPause()
{
	isPause = ui.pbPause->isChecked();
	emit isPaused(isPause);
}

void fileList::onStop()
{
	if(isRecorded)
	{
		pIqWriter->close();
		ui.pbPlay->setEnabled(true);
		ui.pbLoop->setEnabled(true);
		ui.pbBack->setEnabled(true);
		ui.pbRec->setChecked(false);
		ui.slIqTrack->setMaximum(0);
		isRecorded = false;
		emit isRec(false);
		QFileInfo info;
		info.setFile(pIqWriter->getFileName());
		if(info.isFile())
			addFile(info);
	}
	else if(isPlayed)
	{
		ui.pbRec->setEnabled(true);
		killTimer(timerId);
		ui.pbPlay->setChecked(false);
		pIqReader->close();
		ui.slIqTrack->setMaximum(0);
		isPlayed = false;
		emit isPlay(false);
	}
	ui.pbPause->setEnabled(false);
}

void fileList::onLoop(bool status)
{
	pIqReader->setLoop(status);
}

void fileList::onStart(bool status)
{
	if(!status)
		onStop();
}

void fileList::readBuf(void *pInL, void *pInR, quint32 FrameCount)
{
	if(isPlayed)
	{
		if(!isPause)
			pIqReader->readBuf(pInL, pInR, FrameCount);
		else
		{
			qMemSet(pInL, 0.0f, FrameCount*sizeof(float));
			qMemSet(pInR, 0.0f, FrameCount*sizeof(float));
		}
	}
	else if(isRecorded)
	{
		if(!isPause)
			pIqWriter->writeBuf(pInL, pInR, FrameCount);
	}
}

void fileList::setCentralFreq(int freq)
{
	pIqWriter->setCentralFreq(freq);
}


