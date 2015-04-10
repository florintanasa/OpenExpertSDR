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

#include "Options.h"
#include "SdrPlugin/PluginCtrl.h"

Options::Options(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	SetupPluginList();

	ui.sbPaTxDelayValue->setVisible(false);
	ui.sbVoiceRepeatTime->setVisible(false);

	ui.tbSDR->setCurrentIndex(0);
	ui.SwMain->setCurrentIndex(0);

    //QString DocumentsLocation = QDesktopServices::storageLocation(QDesktopServices:: DocumentsLocation);
    QString DocumentsLocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	QDir dir;

	if(dir.exists(DocumentsLocation))
	{
		if(!dir.exists(DocumentsLocation + "/ExpertSDR"))
			dir.mkdir(DocumentsLocation + "/ExpertSDR");
		pathDefaultWaveIQ = DocumentsLocation + "/ExpertSDR/";
	}
	else
	{
		if(dir.mkdir(QDir::homePath() + "/ExpertSDR"))
			pathDefaultWaveIQ = QDir::homePath() + "/ExpertSDR/";
		else
		{
			QMessageBox msgBox;
			msgBox.setText("Choose a directory where wave files will be located.");
			msgBox.exec();
			//
			QString path = QDir::homePath();
			if(path.isEmpty())
			{
				msgBox.setText("Wave file location:\n" + QDir::homePath());
				msgBox.exec();
				pathDefaultWaveIQ = QDir::homePath() + "/";
			}
			else
				pathDefaultWaveIQ = path + "/";
		}
	}
	pathDefaultWaveIQDefault = pathDefaultWaveIQ;
    ui.lbWavePathIQ->setText(pathDefaultWaveIQ);
	pProg0 = new QProcess(this);
	pProg1 = new QProcess(this);
	pProg2 = new QProcess(this);
	pProg3 = new QProcess(this);
	pProg4 = new QProcess(this);

    PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};

   //prepare to pupulate list in combobox with serial ports
    ui.cbPttPortName->clear();
    ui.cbAddKeyPortName->clear();
    ui.cbKeyPortName->clear();

       QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
       if (ports.size()!=0)
       {
           for (int i = 0; i < ports.size(); i++)
           {
                  ui.cbPttPortName->addItem(ports.at(i).portName.toLocal8Bit().constData(),0);
                  ui.cbAddKeyPortName->addItem(ports.at(i).portName.toLocal8Bit().constData(),0);
                  ui.cbKeyPortName->addItem(ports.at(i).portName.toLocal8Bit().constData(),0);
              // ui.cbPttPortName->addItem(ports.at(i).physName.toLocal8Bit().constData(),0);
           }
       }


    #ifdef Q_OS_UNIX
        pPttPort = new QextSerialPort(QLatin1String("/dev/ttyS0"), QextSerialPort::Polling);
        pKeyPort = new QextSerialPort(QLatin1String("/dev/ttyS0"), QextSerialPort::Polling);
        pAddKeyPort = new QextSerialPort(QLatin1String("/dev/ttyS0"), QextSerialPort::Polling);
    #else
        pPttPort = new QextSerialPort(QLatin1String("COM1"), QextSerialPort::Polling);
        pKeyPort = new QextSerialPort(QLatin1String("COM1"), QextSerialPort::Polling);
        pAddKeyPort = new QextSerialPort(QLatin1String("COM1"), QextSerialPort::Polling);
    #endif /*Q_OS_UNIX*/

    connect(pPttPort, SIGNAL(dsrChanged(bool)), this, SLOT(OnPttDsr(bool)));
    connect(pPttPort, SIGNAL(ctsChanged(bool)), this, SLOT(OnPttCts(bool)));
    connect(pKeyPort, SIGNAL(dsrChanged(bool)), this, SLOT(OnKeyDsr(bool)));
    connect(pKeyPort, SIGNAL(ctsChanged(bool)), this, SLOT(OnKeyCts(bool)));
    connect(pAddKeyPort, SIGNAL(dsrChanged(bool)), this, SLOT(OnAddKeyDsr(bool)));
    connect(pAddKeyPort, SIGNAL(ctsChanged(bool)), this, SLOT(OnAddKeyCts(bool)));
	connect(ui.LwOptions, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(ChangePage(QListWidgetItem*, QListWidgetItem*)));
	connect(ui.pbOK, SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(ui.pbCancel, SIGNAL(clicked()), this, SLOT(OnCancel()));
	connect(ui.slTxImRejMag, SIGNAL(valueChanged(int)), this, SLOT(OnTxGainChange(int)));
	connect(ui.slTxImRejPhase, SIGNAL(valueChanged(int)), this, SLOT(OnTxPhaseChange(int)));
	connect(ui.slTxImRejMagCw, SIGNAL(valueChanged(int)), this, SLOT(OnTxGainChangeCw(int)));
	connect(ui.sbTxImRejMagCw, SIGNAL(valueChanged(int)), this, SLOT(OnTxGainChangeCw(int)));
	connect(ui.slTxImRejPhaseCw, SIGNAL(valueChanged(int)), this, SLOT(OnTxPhaseChangeCw(int)));
	connect(ui.sbTxImRejPhaseCw, SIGNAL(valueChanged(int)), this, SLOT(OnTxPhaseChangeCw(int)));
	connect(ui.slTxImRejMagAmFm, SIGNAL(valueChanged(int)), this, SLOT(OnTxGainChangeAmFm(int)));
	connect(ui.sbTxImRejMagAmFm, SIGNAL(valueChanged(int)), this, SLOT(OnTxGainChangeAmFm(int)));
	connect(ui.slTxImRejPhaseAmFm, SIGNAL(valueChanged(int)), this, SLOT(OnTxPhaseChangeAmFm(int)));
	connect(ui.sbTxImRejPhaseAmFm, SIGNAL(valueChanged(int)), this, SLOT(OnTxPhaseChangeAmFm(int)));
	connect(ui.pbProg0, SIGNAL(clicked()), this, SLOT(OnProg0()));
	connect(ui.pbProg1, SIGNAL(clicked()), this, SLOT(OnProg1()));
	connect(ui.pbProg2, SIGNAL(clicked()), this, SLOT(OnProg2()));
	connect(ui.pbProg3, SIGNAL(clicked()), this, SLOT(OnProg3()));
	connect(ui.pbProg4, SIGNAL(clicked()), this, SLOT(OnProg4()));
	connect(ui.pbApply, SIGNAL(clicked()), this, SLOT(LaunchProgs()));
	connect(ui.pbOK, SIGNAL(clicked()), this, SLOT(LaunchProgs()));
	if(ui.chbExtCtrl->isChecked())
		OnEnableExControl(1);
	else
		OnEnableExControl(0);
	connect(ui.chbExtCtrl, SIGNAL(stateChanged(int)), this, SLOT(OnEnableExControl(int)));
	connect(ui.spinBox_0, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect160(int)));
	connect(ui.spinBox_1, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect80(int)));
	connect(ui.spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect60(int)));
	connect(ui.spinBox_3, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect40(int)));
	connect(ui.spinBox_4, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect30(int)));
	connect(ui.spinBox_5, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect20(int)));
	connect(ui.spinBox_6, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect17(int)));
	connect(ui.spinBox_7, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect15(int)));
	connect(ui.spinBox_8, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect12(int)));
	connect(ui.spinBox_9, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect10(int)));
	connect(ui.spinBox_10, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect6(int)));
	connect(ui.spinBox_11, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect2(int)));
	connect(ui.spinBox_12, SIGNAL(valueChanged(int)), this, SLOT(PowerCorrect07(int)));
	connect(ui.chbPttEnable, SIGNAL(clicked(bool)), this, SLOT(pttOpen(bool)));
	ui.chbPttDtr->setVisible(false);
	ui.chbPttRts->setVisible(false);
	connect(ui.chbKeyEnable, SIGNAL(clicked(bool)), this, SLOT(keyOpen(bool)));
	connect(ui.chbAddKeyEnable, SIGNAL(clicked(bool)), this, SLOT(addKeyOpen(bool)));
	connect(ui.cbPaDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(soundDrvChanged(int)));
	connect(ui.cbPaIn, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbPaOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbPaBufferSize, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbPaSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbPaChannels, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.sbPaLattency, SIGNAL(valueChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbPaVacDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(soundVacDrvChanged(int)));
	connect(ui.cbPaVacIn, SIGNAL(currentIndexChanged(int)), this, SLOT(soundVacChanged(int)));
	connect(ui.cbPaVacOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundVacChanged(int)));
	connect(ui.cbPaVacBufferSize, SIGNAL(currentIndexChanged(int)), this, SLOT(soundVacChanged(int)));
	connect(ui.cbPaVacSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(soundVacChanged(int)));
	connect(ui.sbPaVacLattency, SIGNAL(valueChanged(int)), this, SLOT(soundVacChanged(int)));
	connect(ui.chbVacEnable, SIGNAL(stateChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbSdrType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSdrType(int)));
	connect(ui.cbSdrType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSdrTypeChanged(int)));
	onEnableXvtrx(false);
	connect(ui.chbXvtrxEnable, SIGNAL(clicked(bool)), this, SLOT(onEnableXvtrx(bool)));
	connect(ui.pbWavePath, SIGNAL(clicked()), this, SLOT(openWaveDir()));
	connect(ui.pbLog, SIGNAL(clicked()), this, SLOT(viewLocationLogFile()));
}

Options::~Options()
{
	delete pProg0;
	delete pProg1;
	delete pProg2;
	delete pProg3;
	delete pProg4;
    delete pPttPort;
    delete pKeyPort;
    delete pAddKeyPort;
}

void Options::setWaveFilesDirLocation(QString path)
{
	if(path.isEmpty())
	{
		pathDefaultWaveIQ = getWaveFilesDirLocationDefault();
		ui.lbWavePathIQ->setText(pathDefaultWaveIQ);
		return;
	}
	QDir dir;
	if(!dir.exists(path))
	{
		QMessageBox msgBox;
		msgBox.setText("Directory does not exist!\n" + path);
		msgBox.setInformativeText("You want to create this directory?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();
		if(ret == QMessageBox::Yes)
		{
			if(!dir.mkpath(path))
			{
				QMessageBox msgBox1;
				msgBox1.setText("I can't create directory!");
				msgBox1.setInformativeText("Default wave file directory:\n" + pathDefaultWaveIQ);
				msgBox1.exec();
				return;
			}
		}
	}
	else
	{
		if(path.right(1) != "/")
			pathDefaultWaveIQ = path + "/";
		else
			pathDefaultWaveIQ = path;
	}
	ui.lbWavePathIQ->setText(pathDefaultWaveIQ);
	emit wavePathChanged(pathDefaultWaveIQ);
}

QString Options::getWaveFilesDirLocation()
{
	return (pathDefaultWaveIQ);
}

QString Options::getWaveFilesDirLocationDefault()
{
	return (pathDefaultWaveIQDefault);
}

void Options::SetupPluginList()
{
	QString plugin_dir = QDir::currentPath() + "/device";
    QDir plug_dir(plugin_dir, "*.so", QDir::Name, QDir::Files | QDir::Hidden | QDir::System);

	for(uint i = 0; i < plug_dir.count(); i++)
	{
		QString libpath = plugin_dir + "/" + plug_dir[i];
		QString InfoStr = "";

//		if(pluginCtrl::getInfo(libpath, InfoStr))
			ui.cbSdrType->addItem(InfoStr, libpath);
	}
}

void Options::StartProgs()
{
	if(PathProg0.size() > 0)
	{
		FileInfo.setFile(PathProg0);
		if(!FileInfo.isFile())
		{
			PathProg0.clear();
			ui.leProg0->clear();
		}
		else
			DirProg0 = FileInfo.absoluteDir().absolutePath();
	}
	if(PathProg1.size() > 0)
	{
		FileInfo.setFile(PathProg1);
		if(!FileInfo.isFile())
		{
			PathProg1.clear();
			ui.leProg1->clear();
		}
		else
			DirProg1 = FileInfo.absoluteDir().absolutePath();
	}
	if(PathProg2.size() > 0)
	{
		FileInfo.setFile(PathProg2);
		if(!FileInfo.isFile())
		{
			PathProg2.clear();
			ui.leProg2->clear();
		}
		else
			DirProg2 = FileInfo.absoluteDir().absolutePath();
	}
	if(PathProg3.size() > 0)
	{
		FileInfo.setFile(PathProg3);
		if(!FileInfo.isFile())
		{
			PathProg3.clear();
			ui.leProg3->clear();
		}
		else
			DirProg3 = FileInfo.absoluteDir().absolutePath();
	}
	if(PathProg4.size() > 0)
	{
		FileInfo.setFile(PathProg4);
		if(!FileInfo.isFile())
		{
			PathProg4.clear();
			ui.leProg4->clear();
		}
		else
			DirProg4 = FileInfo.absoluteDir().absolutePath();
	}

	if(ui.cbProg0->isChecked())
	{
		pProg0->setWorkingDirectory(DirProg0);
		qDebug() << DirProg0;
		pProg0->start(PathProg0);

	}
	if(ui.cbProg1->isChecked())
	{
		pProg1->start(PathProg1, arguments);
	}
	if(ui.cbProg2->isChecked())
	{
		pProg2->start(PathProg2, arguments);
	}
	if(ui.cbProg3->isChecked())
	{
		pProg3->start(PathProg3, arguments);
	}
	if(ui.cbProg4->isChecked())
	{
		pProg4->start(PathProg4, arguments);
	}
}

void Options::StopProgs()
{
	if(ui.cbProg0->isChecked())
	{
		pProg0->terminate();
		if(!pProg0->waitForFinished(1000))
			pProg0->close();
	}
	if(ui.cbProg1->isChecked())
	{
		pProg1->terminate();
		if(!pProg1->waitForFinished(1000))
			pProg1->close();
	}
	if(ui.cbProg2->isChecked())
	{
		pProg2->terminate();
		if(!pProg2->waitForFinished(1000))
			pProg2->close();
	}
	if(ui.cbProg3->isChecked())
	{
		pProg3->terminate();
		if(!pProg3->waitForFinished(1000))
			pProg3->close();
	}
	if(ui.cbProg4->isChecked())
	{
		pProg4->terminate();
		if(!pProg4->waitForFinished(1000))
			pProg4->close();
	}
}

void Options::ChangePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	QModelIndex p = ui.LwOptions->currentIndex();
	int i = p.row();
	ui.SwMain->setCurrentIndex(i);
}

void Options::OnOK()
{
	close();
}

void Options::OnCancel()
{
	close();
}

void Options::OnTxGainChange(int Val)
{
	emit TxMagRej(Val);
}

void Options::OnTxPhaseChange(int Val)
{
	emit TxPhaseRej(Val);
}

void Options::OnTxGainChangeCw(int Val)
{
	emit TxMagRejCw(Val);
}

void Options::OnTxPhaseChangeCw(int Val)
{
	emit TxPhaseRejCw(Val);
}

void Options::OnTxGainChangeAmFm(int Val)
{
	emit TxMagRejAmFm(Val);
}

void Options::OnTxPhaseChangeAmFm(int Val)
{
	emit TxPhaseRejAmFm(Val);
}

void Options::OnProg0()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Programs (*.exe)"));
	if(fileName.isEmpty())
		return;
	PathProg0 = fileName;
	ui.leProg0->setText(PathProg0);
}

void Options::OnProg1()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Programs (*.exe)"));
	if(fileName.isEmpty())
		return;
	PathProg1 = fileName;
	ui.leProg1->setText(PathProg1);
}

void Options::OnProg2()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Programs (*.exe)"));
	if(fileName.isEmpty())
		return;
	PathProg2 = fileName;
	ui.leProg2->setText(PathProg2);
}

void Options::OnProg3()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Programs (*.exe)"));
	if(fileName.isEmpty())
		return;
	PathProg3 = fileName;
	ui.leProg3->setText(PathProg3);
}

void Options::OnProg4()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Programs (*.exe)"));
	if(fileName.isEmpty())
		return;
	PathProg4 = fileName;
	ui.leProg4->setText(PathProg4);
}

void Options::SetProg0(bool IsStarted, QString Path)
{
	ui.cbProg0->setChecked(IsStarted);

	if(Path.size() > 0)
	{
		PathProg0 = Path;
		ui.leProg0->setText(PathProg0);
		FileInfo.setFile(PathProg0);
		if(FileInfo.isFile())
		{
			DirProg0 = FileInfo.absoluteDir().absolutePath();
			if(ui.cbProg0->isChecked())
			{
				qDebug()<< "Working directory 0: " << DirProg0;
				pProg0->setWorkingDirectory(DirProg0);
				pProg0->start(PathProg0, arguments);
				pProg0->waitForStarted(1000);
			}
		}
		else
		{
			PathProg0.clear();
			ui.leProg0->clear();
			ui.cbProg0->setChecked(false);
		}
	}
}

void Options::SetProg1(bool IsStarted, QString Path)
{
	ui.cbProg1->setChecked(IsStarted);

	if(Path.size() > 0)
	{
		PathProg1 = Path;
		ui.leProg1->setText(PathProg1);
		FileInfo.setFile(PathProg1);
		if(FileInfo.isFile())
		{
			DirProg1 = FileInfo.absoluteDir().absolutePath();
			if(ui.cbProg1->isChecked())
			{
				qDebug()<< "Working directory 1: " << DirProg1;
				pProg1->setWorkingDirectory(DirProg1);
				pProg1->start(PathProg1, arguments);
				pProg1->waitForStarted(1000);
			}
		}
		else
		{
			PathProg1.clear();
			ui.leProg1->clear();
			ui.cbProg1->setChecked(false);
		}
	}
}

void Options::SetProg2(bool IsStarted, QString Path)
{
	ui.cbProg2->setChecked(IsStarted);

	if(Path.size() > 0)
	{
		PathProg2 = Path;
		ui.leProg2->setText(PathProg2);
		FileInfo.setFile(PathProg2);
		if(FileInfo.isFile())
		{
			DirProg2 = FileInfo.absoluteDir().absolutePath();
			if(ui.cbProg2->isChecked())
			{
				qDebug()<< "Working directory 2: " << DirProg2;
				pProg2->setWorkingDirectory(DirProg2);
				pProg2->start(PathProg2, arguments);
				pProg2->waitForStarted(1000);
			}
		}
		else
		{
			PathProg2.clear();
			ui.leProg2->clear();
			ui.cbProg2->setChecked(false);
		}
	}
}

void Options::SetProg3(bool IsStarted, QString Path)
{
	ui.cbProg3->setChecked(IsStarted);

	if(Path.size() > 0)
	{
		PathProg3 = Path;
		ui.leProg3->setText(PathProg3);
		FileInfo.setFile(PathProg3);
		if(FileInfo.isFile())
		{
			DirProg3 = FileInfo.absoluteDir().absolutePath();
			if(ui.cbProg3->isChecked())
			{
				qDebug()<< "Working directory 3: " << DirProg3;
				pProg3->setWorkingDirectory(DirProg3);
				pProg3->start(PathProg3, arguments);
				pProg3->waitForStarted(1000);
			}
		}
		else
		{
			PathProg3.clear();
			ui.leProg3->clear();
			ui.cbProg3->setChecked(false);
		}
	}
}

void Options::SetProg4(bool IsStarted, QString Path)
{
	ui.cbProg4->setChecked(IsStarted);

	if(Path.size() > 0)
	{
		PathProg4 = Path;
		ui.leProg4->setText(PathProg4);
		FileInfo.setFile(PathProg4);
		if(FileInfo.isFile())
		{
			DirProg4 = FileInfo.absoluteDir().absolutePath();
			if(ui.cbProg4->isChecked())
			{
				qDebug()<< "Working directory 4: " << DirProg4;
				pProg4->setWorkingDirectory(DirProg4);
				pProg4->start(PathProg4, arguments);
				pProg4->waitForStarted(1000);
			}
		}
		else
		{
			PathProg4.clear();
			ui.leProg4->clear();
			ui.cbProg4->setChecked(false);
		}
	}
}

void Options::LaunchProgs()
{
    if(ui.cbProg0->isChecked())
	{
		if(PathProg0.size() > 0)
		{
			FileInfo.setFile(PathProg0);
			DirProg0 = FileInfo.absoluteDir().absolutePath();
			pProg0->setWorkingDirectory(DirProg0);
			qDebug()<< "Working directory 0: " << DirProg0;
			pProg0->start(PathProg0, arguments);
			pProg0->waitForStarted(1000);
		}
	}
	if(ui.cbProg1->isChecked())
	{
		if(PathProg1.size() > 0)
		{
			FileInfo.setFile(PathProg1);
			DirProg1 = FileInfo.absoluteDir().absolutePath();
			pProg1->setWorkingDirectory(DirProg1);
			qDebug()<< "Working directory 1: " << DirProg1;
			pProg1->start(PathProg1, arguments);
			pProg1->waitForStarted(1000);
		}
	}
	if(ui.cbProg2->isChecked())
	{
		if(PathProg2.size() > 0)
		{
			FileInfo.setFile(PathProg2);
			DirProg2 = FileInfo.absoluteDir().absolutePath();
			pProg2->setWorkingDirectory(DirProg2);
			qDebug()<< "Working directory 2: " << DirProg2;
			pProg2->start(PathProg2, arguments);
			pProg2->waitForStarted(1000);
		}
	}
	if(ui.cbProg3->isChecked())
	{
		if(PathProg3.size() > 0)
		{
			FileInfo.setFile(PathProg3);
			DirProg3 = FileInfo.absoluteDir().absolutePath();
			pProg3->setWorkingDirectory(DirProg3);
			qDebug()<< "Working directory 3: " << DirProg3;
			pProg3->start(PathProg3, arguments);
			pProg3->waitForStarted(1000);
		}
	}
	if(ui.cbProg4->isChecked())
	{
		if(PathProg4.size() > 0)
		{
			FileInfo.setFile(PathProg4);
			DirProg4 = FileInfo.absoluteDir().absolutePath();
			pProg4->setWorkingDirectory(DirProg4);
			qDebug()<< "Working directory 4: " << DirProg4;
			pProg4->start(PathProg4, arguments);
			pProg4->waitForStarted(1000);
		}
	}
}

void Options::OnEnableExControl(int State)
{
	ui.groupBox_2->setEnabled((bool)State);
	ui.groupBox_3->setEnabled((bool)State);
}

void Options::GetPowerCorrection(BAND_MODE Band, float *pValDb)
{
	switch (Band) {
	case BAND160M:
        (*pValDb) = ui.spinBox_0->value();
		break;
	case BAND80M:
		(*pValDb) = ui.spinBox_1->value();
		break;
	case BAND60M:
		(*pValDb) = ui.spinBox_2->value();
		break;
	case BAND40M:
		(*pValDb) = ui.spinBox_3->value();
		break;
	case BAND30M:
		(*pValDb) = ui.spinBox_4->value();
		break;
	case BAND20M:
		(*pValDb) = ui.spinBox_5->value();
		break;
	case BAND17M:
		(*pValDb) = ui.spinBox_6->value();
		break;
	case BAND15M:
		(*pValDb) = ui.spinBox_7->value();
		break;
	case BAND12M:
		(*pValDb) = ui.spinBox_8->value();
		break;
	case BAND10M:
		(*pValDb) = ui.spinBox_9->value();
		break;
	case BAND6M:
		(*pValDb) = ui.spinBox_10->value();
		break;
	case BAND2M:
		(*pValDb) = ui.spinBox_11->value();
		break;
	case BAND07M:
		(*pValDb) = ui.spinBox_12->value();
		break;
	default:
		break;
	}
}
void Options::SetPowerCorrection(BAND_MODE Band, float ValDb)
{
	switch (Band) {
	case BAND160M:
		ui.spinBox_0->setValue(ValDb);
		break;
	case BAND80M:
		ui.spinBox_1->setValue(ValDb);
		break;
	case BAND60M:
		ui.spinBox_2->setValue(ValDb);
		break;
	case BAND40M:
		ui.spinBox_3->setValue(ValDb);
		break;
	case BAND30M:
		ui.spinBox_4->setValue(ValDb);
		break;
	case BAND20M:
		ui.spinBox_5->setValue(ValDb);
		break;
	case BAND17M:
		ui.spinBox_6->setValue(ValDb);
		break;
	case BAND15M:
		ui.spinBox_7->setValue(ValDb);
		break;
	case BAND12M:
		ui.spinBox_8->setValue(ValDb);
		break;
	case BAND10M:
		ui.spinBox_9->setValue(ValDb);
		break;
	case BAND6M:
		ui.spinBox_10->setValue(ValDb);
		break;
	case BAND2M:
		ui.spinBox_11->setValue(ValDb);
		break;
	case BAND07M:
		ui.spinBox_12->setValue(ValDb);
		break;
	default:
		break;
	}
}

void Options::SetWindowType(int type)
{
	if(type < 0)
		ui.cbWinType->setCurrentIndex(0);
	else if(type > 11)
		ui.cbWinType->setCurrentIndex(11);
	else
		ui.cbWinType->setCurrentIndex(type);
}

int Options::GetWindowType()
{
	return ui.cbWinType->currentIndex();
}

void Options::setSampleRate(uint value)
{
	if(value == 48000)
		ui.cbPaSampleRate->setCurrentIndex(0);
	else if(value == 96000)
		ui.cbPaSampleRate->setCurrentIndex(1);
	else if(value == 192000)
		ui.cbPaSampleRate->setCurrentIndex(2);
}

float Options::getSampleRate()
{
	return (float)(48000<<ui.cbPaSampleRate->currentIndex());
}

void Options::OnGlitchCompChanged(int Val)
{
	emit GlitchCompChanged(GlitchBuf);
}
void Options::PowerCorrect160(int Val)
{
	emit PowerCorrectChanged160(Val);
}
void Options::PowerCorrect80(int Val)
{
	emit PowerCorrectChanged80(Val);
}
void Options::PowerCorrect60(int Val)
{
	emit PowerCorrectChanged60(Val);
}
void Options::PowerCorrect40(int Val)
{
	emit PowerCorrectChanged40(Val);
}
void Options::PowerCorrect30(int Val)
{
	emit PowerCorrectChanged30(Val);
}
void Options::PowerCorrect20(int Val)
{
	emit PowerCorrectChanged20(Val);
}
void Options::PowerCorrect17(int Val)
{
	emit PowerCorrectChanged17(Val);
}
void Options::PowerCorrect15(int Val)
{
	emit PowerCorrectChanged15(Val);
}
void Options::PowerCorrect12(int Val)
{
	emit PowerCorrectChanged12(Val);
}
void Options::PowerCorrect10(int Val)
{
	emit PowerCorrectChanged10(Val);
}
void Options::PowerCorrect6(int Val)
{
	emit PowerCorrectChanged6(Val);
}
void Options::PowerCorrect2(int Val)
{
	emit PowerCorrectChanged2(Val);
}
void Options::PowerCorrect07(int Val)
{
	emit PowerCorrectChanged07(Val);
}
void Options::OnPttDsr(bool stat)
{
	if(ui.chbPttDtr->isChecked())
		emit PttChanged(stat);
}

void Options::OnPttCts(bool stat)
{
	if(ui.chbPttRts->isChecked())
		emit PttChanged(stat);
}

void Options::soundChanged(int val)
{
	emit SoundCardOptChanged();
}

void Options::soundVacChanged(int val)
{
	if(ui.chbVacEnable->isChecked())
		emit SoundCardOptChanged();
}

void Options::soundDrvChanged(int val)
{
	disconnect(ui.cbPaIn,  SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	disconnect(ui.cbPaOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	emit driverChanged(val);
	emit SoundCardOptChanged();
	connect(ui.cbPaIn,  SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	connect(ui.cbPaOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
}

void Options::soundVacDrvChanged(int val)
{
	if(ui.chbVacEnable->isChecked())
	{
		disconnect(ui.cbPaVacIn,  SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
		disconnect(ui.cbPaVacOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
		emit driverVacChanged(val);
		emit SoundCardOptChanged();
		connect(ui.cbPaVacIn,  SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
		connect(ui.cbPaVacOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	}
	else
	{
		disconnect(ui.cbPaVacIn,  SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
		disconnect(ui.cbPaVacOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
		emit driverVacChanged(val);
		connect(ui.cbPaVacIn,  SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
		connect(ui.cbPaVacOut, SIGNAL(currentIndexChanged(int)), this, SLOT(soundChanged(int)));
	}
}

void Options::onEnableXvtrx(bool stat)
{
	ui.chbXvtrxEnable->setChecked(stat);
	ui.gbXvtrxOpt->setVisible(stat);
	ui.gbXvtrxSmCorr->setVisible(stat);
	emit XvtrxEnable(stat);
}

void Options::pttOpen(bool stat)
{
	ui.chbPttEnable->setChecked(stat);
	if(stat)
	{
        pPttPort->setBaudRate(BAUD9600);
        pPttPort->setDataBits(DATA_8);
        pPttPort->setStopBits(STOP_1);
        pPttPort->setParity(PAR_NONE);
        pPttPort->setFlowControl(FLOW_OFF);
        pPttPort->setTimeout(500);

		if(ui.cbPttPortName->currentIndex() < 8)
		{
            pPttPort->setPortName(ui.cbPttPortName->currentText());
            pPttPort->open(QIODevice::ReadWrite);
		}
		else
		{
            pPttPort->setPortName("\\\\.\\" + ui.cbPttPortName->currentText());
            pPttPort->open(QIODevice::ReadWrite);
		}
		if(!pPttPort->isOpen())
		{
			ui.chbPttEnable->setChecked(false);
			QMessageBox msgBox;
			msgBox.setText(tr("Couldn't open PTT Com port!"));
			msgBox.exec();
			ui.cbPttPortName->setEnabled(true);
			ui.chbPttDtr->setVisible(false);
			ui.chbPttRts->setVisible(false);
			return;
		}
		ui.cbPttPortName->setEnabled(false);
		ui.chbPttDtr->setVisible(true);
		ui.chbPttRts->setVisible(true);
	}
	else
	{
        pPttPort->close();
		ui.chbPttEnable->setChecked(false);
		ui.cbPttPortName->setEnabled(true);
		ui.chbPttDtr->setVisible(false);
		ui.chbPttRts->setVisible(false);
	}
}

void Options::keyOpen(bool stat)
{
	ui.chbKeyEnable->setChecked(stat);
	if(stat)
	{
        pKeyPort->setBaudRate(BAUD9600);
        pKeyPort->setDataBits(DATA_8);
        pKeyPort->setStopBits(STOP_1);
        pKeyPort->setParity(PAR_NONE);
        pKeyPort->setFlowControl(FLOW_OFF);
        pKeyPort->setTimeout(1);

		if(ui.cbKeyPortName->currentIndex() < 8)
		{
			pKeyPort->setPortName(ui.cbKeyPortName->currentText());
			pKeyPort->open(QIODevice::ReadWrite);
		}
		else
		{
			pKeyPort->setPortName("\\\\.\\" + ui.cbKeyPortName->currentText());
			pKeyPort->open(QIODevice::ReadWrite);
		}
		if(!pKeyPort->isOpen())
		{
			ui.chbKeyEnable->setChecked(false);
			QMessageBox msgBox;
			msgBox.setText(tr("Couldn't open Alternate Key Com port!"));
			msgBox.exec();
			ui.cbKeyPortName->setEnabled(true);
			ui.cbKeyPttLine->setEnabled(true);
			ui.cbKeyKeyLine->setEnabled(true);
			return;
		}
		ui.cbKeyPortName->setEnabled(false);
		ui.cbKeyPttLine->setEnabled(false);
		ui.cbKeyKeyLine->setEnabled(false);
	}
	else
	{
        pKeyPort->close();
		ui.chbKeyEnable->setChecked(false);
		ui.cbKeyPortName->setEnabled(true);
		ui.cbKeyPttLine->setEnabled(true);
		ui.cbKeyKeyLine->setEnabled(true);
	}
}

void Options::addKeyOpen(bool stat)
{
	ui.chbAddKeyEnable->setChecked(stat);
	if(stat)
	{
        pAddKeyPort->setBaudRate(BAUD9600);
        pAddKeyPort->setDataBits(DATA_8);
        pAddKeyPort->setStopBits(STOP_1);
        pAddKeyPort->setParity(PAR_NONE);
        pAddKeyPort->setFlowControl(FLOW_OFF);
        pAddKeyPort->setTimeout(1);

		if(ui.cbAddKeyPortName->currentIndex() < 8)
		{
			pAddKeyPort->setPortName(ui.cbAddKeyPortName->currentText());
			pAddKeyPort->open(QIODevice::ReadWrite);
		}
		else
		{
			pAddKeyPort->setPortName("\\\\.\\" + ui.cbAddKeyPortName->currentText());
			pAddKeyPort->open(QIODevice::ReadWrite);
		}
		if(!pAddKeyPort->isOpen())
		{
			qDebug() << "Com port not opened!";
			ui.chbAddKeyEnable->setChecked(false);
			QMessageBox msgBox;
			msgBox.setText(tr("Couldn't open Additional Key Com port!"));
			msgBox.exec();
			ui.cbAddKeyPortName->setEnabled(true);
			ui.cbAddKeys->setEnabled(true);
			return;
		}
		ui.cbAddKeyPortName->setEnabled(false);
		ui.cbAddKeys->setEnabled(false);
//		pAddKeyPort->setDtr(true);
	}
	else
	{
        pAddKeyPort->close();
		ui.chbAddKeyEnable->setChecked(false);
		ui.cbAddKeyPortName->setEnabled(true);
		ui.cbAddKeys->setEnabled(true);
	}
}

void Options::OnKeyDsr(bool stat)
{
	if(ui.chbKeyEnable->isChecked())
	{
		if(ui.cbKeyPttLine->currentIndex() == 1)
			emit PttChanged(stat);
		if(ui.cbKeyKeyLine->currentIndex() == 1)
			emit KeyChanged(stat);
	}
}

void Options::OnKeyCts(bool stat)
{
	if(ui.chbKeyEnable->isChecked())
	{
		if(ui.cbKeyPttLine->currentIndex() == 2)
			emit PttChanged(stat);
		if(ui.cbKeyKeyLine->currentIndex() == 2)
			emit KeyChanged(stat);
	}
}

void Options::OnAddKeyDsr(bool stat)
{
	if(ui.chbAddKeyEnable->isChecked())
	{
		if(ui.cbAddKeys->currentIndex() == 0)
			emit DashChanged(stat);
		else
			emit DotChanged(stat);
	}
}

void Options::OnAddKeyCts(bool stat)
{
	if(ui.chbAddKeyEnable->isChecked())
	{
		if(ui.cbAddKeys->currentIndex() == 0)
			emit DotChanged(stat);
		else
			emit DashChanged(stat);
	}
}

void Options::OnSdrType(int Type)
{
	ui.cbSdrType->setCurrentIndex(Type);
	//ui.swSdrType->setCurrentIndex(Type);
}

void Options::openWaveDir()
{
	QString path = QFileDialog::getExistingDirectory(0, tr("Save Directory"), pathDefaultWaveIQ, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(path.isEmpty())
	{
		QDir dir;
		if(dir.exists(pathDefaultWaveIQ))
			return;
		QMessageBox msgBox;
		msgBox.setText("Wave file location:\n" + QDir::homePath());
		msgBox.exec();
		pathDefaultWaveIQ = QDir::homePath() + "/";
	}
	else
		pathDefaultWaveIQ = path + "/";
	ui.lbWavePathIQ->setText(pathDefaultWaveIQ);
	emit wavePathChanged(pathDefaultWaveIQ);
}

void Options::viewLocationLogFile()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::homePath() + "/ExpertSDR"));
}

void Options::onSdrTypeChanged(int index)
{
	QString path = ui.cbSdrType->itemData(index).toString();
	emit SdrPluginChanged(path);
}
