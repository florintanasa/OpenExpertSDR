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

#include "CwMacro.h"

CwMacro::CwMacro(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	pCwCore = new CwCore;
	connect(ui.pbM1, SIGNAL(clicked()), this, SLOT(onM1()));
	connect(ui.pbM2, SIGNAL(clicked()), this, SLOT(onM2()));
	connect(ui.pbM3, SIGNAL(clicked()), this, SLOT(onM3()));
	connect(ui.pbM4, SIGNAL(clicked()), this, SLOT(onM4()));
	connect(ui.pbM5, SIGNAL(clicked()), this, SLOT(onM5()));
	connect(ui.pbM6, SIGNAL(clicked()), this, SLOT(onM6()));
	connect(ui.pbM7, SIGNAL(clicked()), this, SLOT(onM7()));
	connect(ui.pbM8, SIGNAL(clicked()), this, SLOT(onM8()));
	connect(ui.pbM9, SIGNAL(clicked()), this, SLOT(onM9()));
	connect(pCwCore, SIGNAL(trxChanged(bool)), ui.pbTx, SLOT(setChecked(bool)));
	connect(pCwCore, SIGNAL(trxChanged(bool)), this, SLOT(onTx(bool)), Qt::QueuedConnection);
	connect(pCwCore, SIGNAL(cwSignal(bool)), ui.pbSig, SLOT(setChecked(bool)));
	connect(ui.sbDropDel, SIGNAL(valueChanged(int)), this, SLOT(onDropDel(int)));
	connect(ui.sbPttDel, SIGNAL(valueChanged(int)), this, SLOT(onPttDel(int)));
	sdrMode = LSB;
	isStart = false;
}
CwMacro::~CwMacro()
{
	delete pCwCore;
}
void CwMacro::writeSetting(QSettings *pSett)
{
	qDebug() << "CwMacro: writeSettings()";
	pSett->beginGroup("CW_Macro");
		pSett->setValue("Position", pos());
		pSett->setValue("Speed", ui.sbSpeed->value());
		pSett->setValue("RepDelay", ui.sbRepDel->value());
		pSett->setValue("PttDelay", ui.sbPttDel->value());
		pSett->setValue("DropDelay", ui.sbDropDel->value());
		pSett->setValue("M1", ui.leM1->text());
		pSett->setValue("M2", ui.leM2->text());
		pSett->setValue("M3", ui.leM3->text());
		pSett->setValue("M4", ui.leM4->text());
		pSett->setValue("M5", ui.leM5->text());
		pSett->setValue("M6", ui.leM6->text());
		pSett->setValue("M7", ui.leM7->text());
		pSett->setValue("M8", ui.leM8->text());
		pSett->setValue("M9", ui.leM9->text());
	pSett->endGroup();
}
void CwMacro::readSettings(QSettings *pSett)
{
	pSett->beginGroup("CW_Macro");
		move(pSett->value("Position", QPoint(300, 300)).toPoint());
		ui.sbSpeed->setValue(pSett->value("Speed", 10).toInt());
		ui.sbRepDel->setValue(pSett->value("RepDelay", 10).toInt());
		ui.sbPttDel->setValue(pSett->value("PttDelay", 10).toInt());
		ui.sbDropDel->setValue(pSett->value("DropDelay", 10).toInt());
		pCwCore->setTxToRxDelay(ui.sbDropDel->value());
		pCwCore->setPttDelay(ui.sbPttDel->value());
		ui.leM1->setText(pSett->value("M1", "").toString());
		ui.leM2->setText(pSett->value("M2", "").toString());
		ui.leM3->setText(pSett->value("M3", "").toString());
		ui.leM4->setText(pSett->value("M4", "").toString());
		ui.leM5->setText(pSett->value("M5", "").toString());
		ui.leM6->setText(pSett->value("M6", "").toString());
		ui.leM7->setText(pSett->value("M7", "").toString());
		ui.leM8->setText(pSett->value("M8", "").toString());
		ui.leM9->setText(pSett->value("M9", "").toString());
	pSett->endGroup();
}
void CwMacro::start()
{
	isStart = true;
	if((sdrMode == CWU) || (sdrMode == CWL) || (sdrMode == LSB) || (sdrMode == USB))
		setMode(sdrMode);
}
void CwMacro::stop()
{
	isStart = false;
	pCwCore->stop();
	ui.pbSig->setChecked(false);
	ui.pbTx->setChecked(false);
}
void CwMacro::setMode(SDRMODE mode)
{
	sdrMode = mode;
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		if((sdrMode!= USB) && (sdrMode!= LSB))
		{
			if(pCwCore->isStart())
			{
				pCwCore->stop();
				ui.pbSig->setChecked(false);
				ui.pbTx->setChecked(false);
			}
		}
		else
		{
			if(!pCwCore->isStart())
			{
				pCwCore->start();
				ui.pbSig->setChecked(false);
				ui.pbTx->setChecked(false);
			}
		}
	}
	else
	{
		if(!pCwCore->isStart())
		{
			pCwCore->start();
			ui.pbSig->setChecked(false);
			ui.pbTx->setChecked(false);
		}
	}
}
void CwMacro::onM1()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	ui.teInput->insertPlainText(ui.leM1->text()+" ");
	pCwCore->transferString(ui.leM1->text()+" ");
}
void CwMacro::onM2()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM2->text()+" ");
}
void CwMacro::onM3()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM3->text()+" ");
}
void CwMacro::onM4()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM4->text()+" ");
}
void CwMacro::onM5()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM5->text()+" ");
}
void CwMacro::onM6()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM6->text()+" ");
}
void CwMacro::onM7()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM7->text()+" ");
}
void CwMacro::onM8()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM8->text()+" ");
}
void CwMacro::onM9()
{
	if(!isStart)
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Programm is not started.\nPlease push start button and try again!"));
		return;
	}
	if((sdrMode!= CWU) && (sdrMode!= CWL))
	{
		QMessageBox::information(this, tr("Can't transfer data"), tr("Current mode is not CWU or CWL.\nPlease switch to CW mode and try again!"));
		return;
	}
	pCwCore->transferString(ui.leM9->text()+" ");
}
void CwMacro::onDropDel(int val)
{
	pCwCore->setTxToRxDelay(val);
}
void CwMacro::onPttDel(int val)
{
	pCwCore->setPttDelay(val);
}
void CwMacro::onTx(bool val)
{
	if(val == true)
	{
		if((sdrMode == LSB) || (sdrMode == USB))
		{
			rxMode = sdrMode;
			if(sdrMode == USB)
				emit changeMode(CWU);
			else
				emit changeMode(CWL);
			emit trxChanged(val);
		}
		else if((sdrMode == CWL) || (sdrMode == CWU))
		{
			rxMode = sdrMode;
			emit trxChanged(val);
		}
	}
	else
	{
		if((rxMode == LSB) || (rxMode == USB))
		{
			emit trxChanged(val);
			emit changeMode(rxMode);
		}
		else if((rxMode == CWL) || (rxMode == CWU))
		{
			emit trxChanged(val);
		}
	}

}
