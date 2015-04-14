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

#include "WdgMem.h"

WdgMem::WdgMem(int *MainFreq, QButtonGroup *pMode, QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	pAddStation = new WdgAddStation;
	pIqReader = new WavReader;

	pMainFreq = MainFreq;
	pModeButtons = pMode;
	connect(pAddStation, SIGNAL(Station(QString, int, int)), this, SLOT(addNewStation(QString, int, int)));
	m_pEditorFocusAction = new QAction(tr("Set focus to editor"), this);
	m_pEditorFocusAction->setShortcut(Qt::Key_Return);
	m_pEditorFocusAction->setShortcutContext(Qt::WidgetShortcut);
	connect(m_pEditorFocusAction, SIGNAL(triggered()), this, SLOT(OnThisMemory()));
	ui.leComment->addAction(m_pEditorFocusAction);
	ui.twMem->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.twMem->horizontalHeader()->setStretchLastSection(true);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    ui.twMem->horizontalHeader()->setSectionsMovable(true);
    ui.twMem->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
    ui.twMem->horizontalHeader()->setMovable(true);
    ui.twMem->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif

    ui.twMem->horizontalHeader()->setMinimumHeight(20);
	connect(ui.pbMemory, SIGNAL(clicked()), this, SLOT(OnThisMemory()));
	connect(ui.twMem, SIGNAL(cellDoubleClicked (int, int)), this, SLOT(OnTwDoubleClick(int, int)));
	connect(ui.pbDel, SIGNAL(clicked()), this, SLOT(OnDelStation()));
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(OnAddMemory()));
	pIqList = new fileList(ui.tabWidget->widget(1));
	connect(pIqList, SIGNAL(isStart(bool)), this, SIGNAL(isStart(bool)));
	connect(pIqList, SIGNAL(isChangedPlayFile(bool)), this, SIGNAL(isChangedPlayFile(bool)));
	iqPlayed = false;
    pIqList->resize(ui.tabWidget->widget(0)->size());
}

WdgMem::~WdgMem()
{
	delete pIqList;
	delete pIqReader;
}

void WdgMem::resizeEvent(QResizeEvent *event)
{
	ui.tabWidget->resize(event->size());
	pIqList->resize(ui.tabWidget->widget(ui.tabWidget->currentIndex())->size());
}

void WdgMem::showEvent(QShowEvent *event)
{
    pIqList->resize(ui.tabWidget->widget(1)->size());
}

void WdgMem::writeSettings(QSettings *pS)
{
	pS->beginGroup("WdgMem");
        pS->setValue("IqRecorderLastPath", lastPath);
        pIqList->writeSettings(pS);
        pS->setValue("memCtationCount", memList.count());
        for(int i = 0; i < memList.count(); i++)
        {
        	pS->setValue("memStationName" + QString::number(i), memList.at(i).Comment);
        	pS->setValue("memStationMode" + QString::number(i), memList.at(i).Mode);
        	pS->setValue("memStationFreq" + QString::number(i), memList.at(i).Freq);
        }
	pS->endGroup();
}

void WdgMem::readSettings(QSettings *pS)
{
	Clear();
	pS->beginGroup("WdgMem");
		lastPath = pS->value("IqRecorderLastPath", "C:/").toString();
        pIqList->readSettings(pS);
        int tmpCnt = pS->value("memCtationCount", 0).toInt();
        if(tmpCnt > 0)
        {
        	MEM_ITEM tmpItem;
        	for(int i = 0; i < tmpCnt; i++)
        	{
        		tmpItem.Comment = pS->value("memStationName" + QString::number(i), "").toString();
        		tmpItem.Mode = pS->value("memStationMode" + QString::number(i), "").toInt();
        		tmpItem.Freq = pS->value("memStationFreq" + QString::number(i), "").toInt();
        		addNewStation(tmpItem.Comment, tmpItem.Freq, tmpItem.Mode);
        	}
        }

	pS->endGroup();
}

void WdgMem::OnTwDoubleClick(int Row, int Column)
{
	bool Ok;
	QString Str;
	int Mode;
	Str = ui.twMem->item(Row, 1)->text();
	Str.remove(QChar('.'), Qt::CaseInsensitive);

    int Freq = Str.toInt(&Ok);
	if(Ok)
	{
		if(ui.twMem->item(Row, 2)->text() == "LSB")
			Mode = LSB;
		else if(ui.twMem->item(Row, 2)->text() == "USB")
			Mode = USB;
		else if(ui.twMem->item(Row, 2)->text() == "DSB")
			Mode = DSB;
		else if(ui.twMem->item(Row, 2)->text() == "AM")
			Mode = AM;
		else if(ui.twMem->item(Row, 2)->text() == "SAM")
			Mode = SAM;
		else if(ui.twMem->item(Row, 2)->text() == "FMN")
			Mode = FMN;
		else if(ui.twMem->item(Row, 2)->text() == "CWL")
			Mode = CWL;
		else if(ui.twMem->item(Row, 2)->text() == "CWU")
			Mode = CWU;
		else if(ui.twMem->item(Row, 2)->text() == "SPEC")
			Mode = SPEC;
		else if(ui.twMem->item(Row, 2)->text() == "DIGL")
			Mode = DIGL;
		else if(ui.twMem->item(Row, 2)->text() == "DIGU")
			Mode = DIGU;
		else
			Mode = DRM;
		emit FreqMem(Freq, Mode);
	}
}

void WdgMem::addNewStation(QString Comment, int Freq, int Mode)
{
	QTableWidgetItem *pTwiComment = new QTableWidgetItem;
	QTableWidgetItem *pTwiFreq = new QTableWidgetItem;
	QTableWidgetItem *pTwiMode = new QTableWidgetItem;
	int i = ui.twMem->rowCount();
	QString Str;
	QString Herz;
	MEM_ITEM item;
	item.Comment = Comment;
	item.Freq = Freq;
	item.Mode = Mode;
	memList.append(item);
	Str.setNum((double)Freq/1000000.0f, 'f', 6);
	Herz = Str.right(3);
	Str.resize(Str.size()-3);
	Str += "." + Herz;
    if(Comment != "")
	{
		ui.twMem->insertRow(i);
        pTwiComment->setText(Comment);
		ui.twMem->setItem(i, 0, pTwiComment);
		pTwiFreq->setText(Str);
		ui.twMem->setItem(i, 1, pTwiFreq);

		switch(Mode)
		{
			case LSB:
				pTwiMode->setText("LSB");
			break;
			case USB:
				pTwiMode->setText("USB");
			break;
			case DSB:
				pTwiMode->setText("DSB");
			break;
			case AM:
				pTwiMode->setText("AM");
			break;
			case SAM:
				pTwiMode->setText("SAM");
			break;
			case FMN:
				pTwiMode->setText("FMN");
			break;
			case CWL:
				pTwiMode->setText("CWL");
			break;
			case CWU:
				pTwiMode->setText("CWU");
			break;
			case SPEC:
				pTwiMode->setText("SPEC");
			break;
			case DIGL:
				pTwiMode->setText("DIGL");
			break;
			case DIGU:
				pTwiMode->setText("DIGU");
			break;
			default:
				pTwiMode->setText("DRM");
			break;
		}
		ui.twMem->setItem(i, 2, pTwiMode);
        }
}

void WdgMem::addNewStation(QString Comment, QString Freq, QString Mode)
{
	QTableWidgetItem *pTwiComment = new QTableWidgetItem;
	QTableWidgetItem *pTwiFreq = new QTableWidgetItem;
	QTableWidgetItem *pTwiMode = new QTableWidgetItem;
	int i = ui.twMem->rowCount();
	if(Comment != "")
	{
		ui.twMem->insertRow(i);
		pTwiComment->setText(Comment);
		pTwiFreq->setText(Freq);
		pTwiMode->setText(Mode);
		ui.twMem->setItem(i, 0, pTwiComment);
		ui.twMem->setItem(i, 1, pTwiFreq);
		ui.twMem->setItem(i, 2, pTwiMode);
	}
}

void WdgMem::OnAddMemory()
{
	pAddStation->show();
	pAddStation->activateWindow();
}

void WdgMem::OnDelStation()
{
	int i = ui.twMem->currentRow();
	if(ui.twMem->currentRow() < 0)
			return;
	ui.twMem->removeRow(i);
	ui.twMem->setCurrentCell(i, 1);
	memList.removeAt(i);
}

void WdgMem::OnThisMemory()
{
	QString Str = ui.leComment->text();
	addNewStation(Str , *pMainFreq, pModeButtons->checkedId());
}

void WdgMem::SetAllocateRow(int index)
{
	int size = ui.twMem->rowCount();

	if(ui.twMem->rowCount() == 0)
		return;
	if(index >= ui.twMem->rowCount())
		index = ui.twMem->rowCount()-1;
	else if(index < 0)
		index = 0;

	ui.twMem->selectRow(index);
	OnTwDoubleClick(index, 0);
}

int WdgMem::SelectedRowIndex()
{
	return ui.twMem->currentRow();
}

void WdgMem::Clear()
{
	pIqList->Clear();
}

void WdgMem::OnAddIqFile()
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Open IQ Wav file"), lastPath, tr("IQ Wav files (*.wav *.wave )"));
	QFileInfo info;
	QTableWidgetItem *pTwiEna;
	QTableWidgetItem *pTwiName;
	QTableWidgetItem *pTwiTime;
	for(quint32 i = 0; i < static_cast<quint32>(files.count()); i++)
	{
		info.setFile(files.at(i));
		if(info.isFile())
		{
			lastPath = info.filePath();
			FileInfo fInfo;
			fInfo.path = info.absoluteFilePath();
			fInfo.name = info.fileName();
			fInfo.time = QTime(0, 0, 10, 0);
			filesList << fInfo;
            pTwiEna  = new QTableWidgetItem;
			pTwiName = new QTableWidgetItem;
			pTwiTime = new QTableWidgetItem;
			pTwiName->setText(info.baseName());
		}
	}
}

void WdgMem::readIqBuf(void *pInL, void *pInR, quint32 FrameCount)
{
	pIqList->readBuf(pInL, pInR, FrameCount);
}

bool WdgMem::isWavPlay()
{
	return pIqList->isPlay();
}

int WdgMem::sampleRateIndex()
{
	if((pIqList->sampleRate() == 48000) || (pIqList->sampleRate() == 96000) || (pIqList->sampleRate() == 192000))
        return (pIqList->sampleRate()/48000-1);
	return -1;
}

int WdgMem::sampleRate()
{
	return (pIqList->sampleRate());
}

quint32 WdgMem::centralFreq()
{
	return pIqList->centralFrequency();
}

void WdgMem::setCentralFreq(int freq)
{
	pIqList->setCentralFreq(freq);
}

int WdgMem::getCentralFreq()
{
	return pIqList->centralFrequency();
}

bool WdgMem::isPlay()
{
	return pIqList->isPlay();
}

void WdgMem::Stop()
{
	pIqList->stop();
}
















