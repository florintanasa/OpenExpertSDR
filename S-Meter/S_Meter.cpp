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

#include "S_Meter.h"

S_Meter::S_Meter(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);

	pDraw = new Draw(ui.wdgDraw);

	pNum0 = new Number2(ui.wdg0);
	pNum1 = new Number2(ui.wdg1);
	pNum2 = new Number2(ui.wdg2);
	pNum3 = new Number(ui.wdg3);
	pNum4 = new Number(ui.wdg4);
	pNum5 = new Number(ui.wdg5);
	pNum6 = new Number(ui.wdg6);
	pNum7 = new Number(ui.wdg7);
	pNum8 = new Number(ui.wdg8);

    pMenuBand = new QMenu;
    pMenuMod = new QMenu;
    pMgrBand = new QActionGroup(this);
    pMgrMod = new QActionGroup(this);
    pMenuBand->addAction("160M");
    pMenuBand->addAction("80M");
    pMenuBand->addAction("60M");
    pMenuBand->addAction("40M");
    pMenuBand->addAction("30M");
    pMenuBand->addAction("20M");
    pMenuBand->addAction("17M");
    pMenuBand->addAction("15M");
    pMenuBand->addAction("12M");
    pMenuBand->addAction("10M");
    pMenuBand->addAction("6M");
    pMenuBand->addAction("2M");
    pMenuBand->addAction("0.7M");
    pMenuBand->addAction("GEN");

    pMenuMod->addAction("LSB");
    pMenuMod->addAction("USB");
    pMenuMod->addAction("DSB");
    pMenuMod->addAction("CWL");
    pMenuMod->addAction("CWU");
    pMenuMod->addAction("FMN");
    pMenuMod->addAction("AM");
    pMenuMod->addAction("DIGU");
    pMenuMod->addAction("SPEC");
    pMenuMod->addAction("DIGL");
    pMenuMod->addAction("SAM");
    pMenuMod->addAction("DRM");

    ui.pbBand->setMenu(pMenuBand);
    ui.pbMode->setMenu(pMenuMod);

    for(int i = 0; i < 14; i++)
	{
    	pMgrBand->addAction(pMenuBand->actions().at(i));
    	pMenuBand->actions().at(i)->setCheckable(true);
	}
    for(int i = 0; i < 12; i++)
	{
    	pMgrMod->addAction(pMenuMod->actions().at(i));
    	pMenuMod->actions().at(i)->setCheckable(true);
	}

    pMenuBand->actions().at(0)->setChecked(true);
    pMenuMod->actions().at(0)->setChecked(true);
	ui.pbBand->setText("160M");
	ui.pbMode->setText("LSB");

    connect(pMenuBand, SIGNAL(triggered(QAction*)), this, SLOT(ChangeBand(QAction*)));
    connect(pMenuMod, SIGNAL(triggered(QAction*)), this, SLOT(ChangeMod(QAction*)));
    connect(ui.pbMute, SIGNAL(toggled(bool)), this, SLOT(SetMute(bool)));

    connect(pNum0, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq0(int)));
    connect(pNum1, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq1(int)));
    connect(pNum2, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq2(int)));
    connect(pNum3, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq3(int)));
    connect(pNum4, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq4(int)));
    connect(pNum5, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq5(int)));
    connect(pNum6, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq6(int)));
    connect(pNum7, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq7(int)));
    connect(pNum8, SIGNAL(ChangedNum(int)), this, SLOT(ChangetFreq8(int)));

    connect(ui.pbTranceparent, SIGNAL(clicked()), this, SLOT(ChangedTranceparent()));

    Frequency = 7000000;
    MinFreq = 500000;
    MaxFreq = 65000000;
}

void S_Meter::setLockBand(bool state)
{
	ui.pbBand->setDisabled(state);
}

void S_Meter::setLockMode(bool state)
{
	ui.pbMode->setDisabled(state);
}

void S_Meter::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void S_Meter::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
    	dragPosition = event->globalPos() - frameGeometry().topLeft();
    	oldPos = dragPosition;
        event->accept();
    }
}

void S_Meter::closeEvent(QCloseEvent *event)
{
	emit Close();
}

void S_Meter::ChangedVolume(int val)
{
	VolumeVal = val;
	emit VolumeChange(VolumeVal);
}

void S_Meter::SetDbm(float dB)
{
        pDraw->setDB(dB);
}

void S_Meter::ChangeMod(QAction* action)
{
    IndexMod = pMenuMod->actions().indexOf(action);
    emit ModChange(IndexMod);
switch(IndexMod)
{
case 0:
	ui.pbMode->setText("LSB");
	break;
case 1:
	ui.pbMode->setText("USB");
	break;
case 2:
	ui.pbMode->setText("DSB");
	break;
case 3:
	ui.pbMode->setText("CWL");
	break;
case 4:
	ui.pbMode->setText("CWU");
	break;
case 5:
	ui.pbMode->setText("FMN");
	break;
case 6:
	ui.pbMode->setText("AM");
	break;
case 7:
	ui.pbMode->setText("DIGU");
	break;
case 8:
	ui.pbMode->setText("SPEC");
	break;
case 9:
	ui.pbMode->setText("DIGL");
	break;
case 10:
	ui.pbMode->setText("SAM");
	break;
case 11:
	ui.pbMode->setText("DRM");
	break;
default:
	ui.pbMode->setText("LSB");
	break;
}
}
void S_Meter::ChangeBand(QAction* action)
{
	IndexBand = pMenuBand->actions().indexOf(action);
	emit BandChange(IndexBand);
	switch(IndexBand)
	{
	case 0:
		ui.pbBand->setText("160M");
		break;
	case 1:
		ui.pbBand->setText("80M");
		break;
	case 2:
		ui.pbBand->setText("60M");
		break;
	case 3:
		ui.pbBand->setText("40M");
		break;
	case 4:
		ui.pbBand->setText("30M");
		break;
	case 5:
		ui.pbBand->setText("20M");
		break;
	case 6:
		ui.pbBand->setText("17M");
		break;
	case 7:
		ui.pbBand->setText("15M");
		break;
	case 8:
		ui.pbBand->setText("12M");
		break;
	case 9:
		ui.pbBand->setText("10M");
		break;
	case 10:
		ui.pbBand->setText("6M");
		break;
	case 11:
		ui.pbBand->setText("2M");
		break;
	case 12:
		ui.pbBand->setText("0.7M");
		break;
	case 13:
		ui.pbBand->setText("GEN");
		break;
	default:
		ui.pbBand->setText("160M");
		break;
	}
}
void S_Meter::SetVolume(int val)
{
	VolumeVal = val;
	ui.slVolume->setValue(val);
}
void S_Meter::SetMute(bool val)
{
	EMute = val;
	ui.pbMute->setChecked(val);
	ui.slVolume->setEnabled(!val);
}
void S_Meter::SetSQL(bool val)
{
	ESql = val;
	ui.pbSql->setChecked(val);
}
void S_Meter::SetBand(int val)
{
	IndexBand = val;
	pMenuBand->actions().at(IndexBand)->setChecked(true);
	switch(IndexBand)
	{
	case 0:
		ui.pbBand->setText("160M");
		break;
	case 1:
		ui.pbBand->setText("80M");
		break;
	case 2:
		ui.pbBand->setText("60M");
		break;
	case 3:
		ui.pbBand->setText("40M");
		break;
	case 4:
		ui.pbBand->setText("30M");
		break;
	case 5:
		ui.pbBand->setText("20M");
		break;
	case 6:
		ui.pbBand->setText("17M");
		break;
	case 7:
		ui.pbBand->setText("15M");
		break;
	case 8:
		ui.pbBand->setText("12M");
		break;
	case 9:
		ui.pbBand->setText("10M");
		break;
	case 10:
		ui.pbBand->setText("6M");
		break;
	case 11:
		ui.pbBand->setText("2M");
		break;
	case 12:
		ui.pbBand->setText("0.7M");
		break;
	case 13:
		ui.pbBand->setText("GEN");
		break;
	default:
		ui.pbBand->setText("160M");
		break;
	}
}
void S_Meter::SetMod(int val)
{
	SdrMod = val;
	pMenuMod->actions().at(SdrMod)->setChecked(true);
	switch(SdrMod)
	{
	case 0:
		ui.pbMode->setText("LSB");
		break;
	case 1:
		ui.pbMode->setText("USB");
		break;
	case 2:
		ui.pbMode->setText("DSB");
		break;
	case 3:
		ui.pbMode->setText("CWL");
		break;
	case 4:
		ui.pbMode->setText("CWU");
		break;
	case 5:
		ui.pbMode->setText("FMN");
		break;
	case 6:
		ui.pbMode->setText("AM");
		break;
	case 7:
		ui.pbMode->setText("DiGU");
		break;
	case 8:
		ui.pbMode->setText("SPEC");
		break;
	case 9:
		ui.pbMode->setText("DIGL");
		break;
	case 10:
		ui.pbMode->setText("SAM");
		break;
	case 11:
		ui.pbMode->setText("DRM");
		break;
	default:
		ui.pbMode->setText("LSB");
		break;
	}
}

void S_Meter::SetPitch(int val)
{
	Pitch = val;
}

void S_Meter::SetLockFreq(bool state)
{
    pNum0->setEnabled(!state);
    pNum1->setEnabled(!state);
    pNum2->setEnabled(!state);
    pNum3->setEnabled(!state);
    pNum4->setEnabled(!state);
    pNum5->setEnabled(!state);
    pNum6->setEnabled(!state);
    pNum7->setEnabled(!state);
    pNum8->setEnabled(!state);
}

void S_Meter::ChangetFreq0(int Num)
{
	if((Frequency + Num < MaxFreq) && (Frequency + Num > MinFreq))
	{
		Frequency += Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();

}
void S_Meter::ChangetFreq1(int Num)
{
	if((Frequency + 10*Num < MaxFreq) && (Frequency + 10*Num > MinFreq))
	{
		Frequency += 10*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq2(int Num)
{
	if((Frequency + 100*Num < MaxFreq) && (Frequency + 100*Num > MinFreq))
	{
		Frequency += 100*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq3(int Num)
{
	if((Frequency + 1000*Num < MaxFreq) && (Frequency + 1000*Num > MinFreq))
	{
		Frequency += 1000*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq4(int Num)
{
	if((Frequency + 10000*Num < MaxFreq) && (Frequency + 10000*Num > MinFreq))
	{
		Frequency += 10000*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq5(int Num)
{
	if((Frequency + 100000*Num < MaxFreq) && (Frequency + 100000*Num > MinFreq))
	{
		Frequency += 100000*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq6(int Num)
{
	if((Frequency + 1000000*Num < MaxFreq) && (Frequency + 1000000*Num > MinFreq))
	{
		Frequency += 1000000*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq7(int Num)
{
	if((Frequency + 10000000*Num < MaxFreq) && (Frequency + 10000000*Num > MinFreq))
	{
		Frequency += 10000000*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}
void S_Meter::ChangetFreq8(int Num)
{
	if((Frequency + 100000000*Num < MaxFreq) && (Frequency + 100000000*Num > MinFreq))
	{
		Frequency += 100000000*Num;
		emit FreqChange(Frequency - Pitch);
	}
	InitNum();
}

void S_Meter::SetFreq(int Freq)
{
	Frequency = Freq;
	InitNum();
}

void S_Meter::InitNum()
{
	//
	int Digits[10];
	int Temp = Frequency;

	Digits[9] = Frequency/1000000000;
	Temp -= Digits[9]*1000000000;

	Digits[8] = Temp/100000000;
	Temp -= Digits[8]*100000000;

	Digits[7] = Temp/10000000;
	Temp -= Digits[7]*10000000;

	Digits[6] = Temp/1000000;
	Temp -= Digits[6]*1000000;

	Digits[5] = Temp/100000;
	Temp -= Digits[5]*100000;

	Digits[4] = Temp/10000;
	Temp -= Digits[4]*10000;

	Digits[3] = Temp/1000;
	Temp -= Digits[3]*1000;

	Digits[2] = Temp/100;
	Temp -= Digits[2]*100;

	Digits[1] = Temp/10;
	Temp -= Digits[1]*10;

	Digits[0] = Temp;

	pNum0->SetNumber(Digits[0]);
	pNum1->SetNumber(Digits[1]);
	pNum2->SetNumber(Digits[2]);
	pNum3->SetNumber(Digits[3]);
	pNum4->SetNumber(Digits[4]);
	pNum5->SetNumber(Digits[5]);
	pNum6->SetNumber(Digits[6]);
	pNum7->SetNumber(Digits[7]);
	pNum8->SetNumber(Digits[8]);
}

void S_Meter::ChangedTranceparent()
{
	if(isVisible())
	{
		if(ui.pbTranceparent->isChecked())
		{
				setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
				setAttribute(Qt::WA_TranslucentBackground);
				ui.label->setPixmap(QPixmap(":images/S-Meter.png"));
				show();
		}
		else
		{
				setAttribute(Qt::WA_TranslucentBackground, false);
				setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
				ui.label->setPixmap(QPixmap(":images/S-MeterXP.png"));
				show();
		}
	}
	else
	{
		if(ui.pbTranceparent->isChecked())
		{
			setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
			setAttribute(Qt::WA_TranslucentBackground);
			ui.label->setPixmap(QPixmap(":images/S-Meter.png"));
		}
		else
		{
			setAttribute(Qt::WA_TranslucentBackground, false);
			setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
			ui.label->setPixmap(QPixmap(":images/S-MeterXP.png"));
		}
	}
}

void S_Meter::setSettings(QVariant settings)
{
	ui.pbTranceparent->setChecked(settings.toBool());
	ChangedTranceparent();
}

QVariant S_Meter::getSettings()
{
	QVariant val(ui.pbTranceparent->isChecked());
	return val;
}




