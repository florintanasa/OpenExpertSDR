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

#include "Panel.h"

Panel::Panel(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	pPan = new Ea1dev;

	Func0 = 0;
	Func1 = 0;
	Func2 = 0;
	Func3 = 0;
	Func4 = 0;
	Func5 = 0;
	Func6 = 0;
	Func7 = 0;
	Func8 = 0;
	Func9 = 0;
	Func10 = 0;
	Func11 = 0;
	Func12 = 0;
	Func13 = 0;
	Func14 = 0;

	FuncDial0 = 0;
	FuncDial1 = 0;
	FuncDial2 = 0;
	FuncDial3 = 0;

	Encoder0Val = 0;
	Encoder1Val = 0;
	Encoder2Val = 0;
	Encoder3Val = 0;

	EncFuncVal0 = 0;
	EncFuncVal1 = 0;
	EncFuncVal2 = 0;
	EncFuncVal3 = 0;

	createMenu();
	createDialMenu();

	connect(pPan, SIGNAL(Key1(bool)), ui.pbFunc1_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key2(bool)), ui.pbFunc2_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key3(bool)), ui.pbFunc3_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key4(bool)), ui.pbFunc4_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key5(bool)), ui.pbFunc5_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key6(bool)), ui.pbFunc6_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key7(bool)), ui.pbFunc7_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key8(bool)), ui.pbFunc8_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key9(bool)), ui.pbFunc9_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key10(bool)), ui.pbFunc10_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key11(bool)), ui.pbFunc11_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key12(bool)), ui.pbFunc12_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key13(bool)), ui.pbFunc13_2, SIGNAL(clicked(bool)));
	connect(pPan, SIGNAL(Key14(bool)), ui.pbFunc14_2, SIGNAL(clicked(bool)));

    connect(pPan, SIGNAL(KeyEnc1(bool)), ui.pbDial1_PressF, SIGNAL(clicked(bool)));
    connect(pPan, SIGNAL(KeyEnc2(bool)), ui.pbDial2_PressF, SIGNAL(clicked(bool)));
    connect(pPan, SIGNAL(KeyEnc3(bool)), ui.pbDial3_PressF, SIGNAL(clicked(bool)));

    connect(pPan, SIGNAL(KeyDash(bool)), this, SIGNAL(KeyDash(bool)));
    connect(pPan, SIGNAL(KeyDot(bool)), this, SIGNAL(KeyDot(bool)));
    connect(pPan, SIGNAL(KeyPtt(bool)), this, SIGNAL(KeyPtt(bool)));

    connect(ui.checkBox, SIGNAL(clicked(bool)), this, SLOT(onOpen(bool)));

    connect(ui.chbEncoderLed, SIGNAL(clicked(bool)), this, SLOT(onEncLed(bool)));
    connect(ui.chbKeyLed, SIGNAL(clicked(bool)), this, SLOT(onKeyLed(bool)));
}
Panel::~Panel()
{
    if(pPan->isOpen())
    {
        onEncLed(false);
        onKeyLed(false);
        onMox(false);
        pPan->close();
    }
}
void Panel::writeSettings(QSettings *pSett)
{
	pSett->beginGroup("Panel");
		pSett->setValue("Enable", ui.checkBox->checkState());
		pSett->setValue("UseIF", ui.chbUseIf->checkState());
		pSett->setValue("EncoderLed", ui.chbEncoderLed->checkState());
		pSett->setValue("KeyLed", ui.chbKeyLed->checkState());
		pSett->setValue("VolumeStep", ui.sbVolumeStep->value());
		pSett->setValue("MicGainStep", ui.sbMicGainStep->value());
		pSett->setValue("SqlStep", ui.sbSqlStep->value());
		pSett->setValue("FilterHighStep", ui.sbFilterHighStep->value());
		pSett->setValue("FilterLowStep", ui.sbFilterLowStep->value());
		pSett->setValue("FilterWidthStep", ui.sbFilterWidthStep->value());
		pSett->setValue("FilterShiftStep", ui.sbFilterShiftStep->value());
		pSett->setValue("DriveStep", ui.sbDriveStep->value());
		pSett->setValue("IfStep", ui.sbIfStep->value());
		pSett->setValue("RfGainStep", ui.sbRfGainStep->value());
		pSett->setValue("PushButton0", Func0);
		pSett->setValue("PushButton1", Func1);
		pSett->setValue("PushButton2", Func2);
		pSett->setValue("PushButton3", Func3);
		pSett->setValue("PushButton4", Func4);
		pSett->setValue("PushButton5", Func5);
		pSett->setValue("PushButton6", Func6);
		pSett->setValue("PushButton7", Func7);
		pSett->setValue("PushButton8", Func8);
		pSett->setValue("PushButton9", Func9);
		pSett->setValue("PushButton10", Func10);
		pSett->setValue("PushButton11", Func11);
		pSett->setValue("PushButton12", Func12);
		pSett->setValue("PushButton13", Func13);
		pSett->setValue("PushButton14", Func14);
		pSett->setValue("DialButton0", EncFuncVal0);
		pSett->setValue("DialButton1", EncFuncVal1);
		pSett->setValue("DialButton2", EncFuncVal2);
		pSett->setValue("DialButton3", EncFuncVal3);
		pSett->setValue("DialFunc0", FuncDial0);
		pSett->setValue("DialFunc1", FuncDial1);
		pSett->setValue("DialFunc2", FuncDial2);
		pSett->setValue("DialFunc3", FuncDial3);
	pSett->endGroup();
}
void Panel::readSettings(QSettings *pSett)
{
	pSett->beginGroup("Panel");
		ui.checkBox->setChecked(pSett->value("Enable", false).toBool());
		ui.chbUseIf->setChecked(pSett->value("UseIF", false).toBool());
		ui.chbEncoderLed->setChecked(pSett->value("EncoderLed", false).toBool());
		ui.chbKeyLed->setChecked(pSett->value("KeyLed", false).toBool());
		ui.sbVolumeStep->setValue(pSett->value("VolumeStep", 1).toInt());
		ui.sbMicGainStep->setValue(pSett->value("MicGainStep", 1).toInt());
		ui.sbSqlStep->setValue(pSett->value("SqlStep", 1).toInt());
		ui.sbFilterHighStep->setValue(pSett->value("FilterHighStep", 50).toInt());
		ui.sbFilterLowStep->setValue(pSett->value("FilterLowStep", 50).toInt());
		ui.sbFilterWidthStep->setValue(pSett->value("FilterWidthStep", 50).toInt());
		ui.sbFilterShiftStep->setValue(pSett->value("FilterShiftStep", 50).toInt());
		ui.sbDriveStep->setValue(pSett->value("DriveStep", 1).toInt());
		ui.sbIfStep->setValue(pSett->value("IfStep", 1).toInt());
		ui.sbRfGainStep->setValue(pSett->value("RfGainStep", 1).toInt());
		Func0 = pSett->value("PushButton0", 0).toInt();
		Func1 = pSett->value("PushButton1", 1).toInt();
		Func2 = pSett->value("PushButton2", 2).toInt();
		Func3 = pSett->value("PushButton3", 3).toInt();
		Func4 = pSett->value("PushButton4", 4).toInt();
		Func5 = pSett->value("PushButton5", 5).toInt();
		Func6 = pSett->value("PushButton6", 6).toInt();
		Func7 = pSett->value("PushButton7", 7).toInt();
		Func8 = pSett->value("PushButton8", 8).toInt();
		Func9 = pSett->value("PushButton9", 9).toInt();
		Func10 = pSett->value("PushButton10", 10).toInt();
		Func11 = pSett->value("PushButton11", 11).toInt();
		Func12 = pSett->value("PushButton12", 12).toInt();
		Func13 = pSett->value("PushButton13", 13).toInt();
		Func14 = pSett->value("PushButton14", 14).toInt();
		EncFuncVal0 = pSett->value("DialButton0", 0).toInt();
		EncFuncVal1 = pSett->value("DialButton1", 0).toInt();
		EncFuncVal2 = pSett->value("DialButton2", 0).toInt();
		EncFuncVal3 = pSett->value("DialButton3", 0).toInt();
		FuncDial0 = pSett->value("DialFunc0", 0).toInt();
		FuncDial1 = pSett->value("DialFunc1", 0).toInt();
		FuncDial2 = pSett->value("DialFunc2", 0).toInt();
		FuncDial3 = pSett->value("DialFunc3", 0).toInt();

		pActionFunc[0]->actions().at(Func1)->setChecked(true);
		ChangeFunc1(pActionFunc[0]->actions().at(Func1));
		pActionFunc[1]->actions().at(Func2)->setChecked(true);
		ChangeFunc2(pActionFunc[1]->actions().at(Func2));
		pActionFunc[2]->actions().at(Func3)->setChecked(true);
		ChangeFunc3(pActionFunc[2]->actions().at(Func3));
		pActionFunc[3]->actions().at(Func4)->setChecked(true);
		ChangeFunc4(pActionFunc[3]->actions().at(Func4));
		pActionFunc[4]->actions().at(Func5)->setChecked(true);
		ChangeFunc5(pActionFunc[4]->actions().at(Func5));
		pActionFunc[5]->actions().at(Func6)->setChecked(true);
		ChangeFunc6(pActionFunc[5]->actions().at(Func6));
		pActionFunc[6]->actions().at(Func7)->setChecked(true);
		ChangeFunc7(pActionFunc[6]->actions().at(Func7));
		pActionFunc[7]->actions().at(Func8)->setChecked(true);
		ChangeFunc8(pActionFunc[7]->actions().at(Func8));
		pActionFunc[8]->actions().at(Func9)->setChecked(true);
		ChangeFunc9(pActionFunc[8]->actions().at(Func9));
		pActionFunc[9]->actions().at(Func10)->setChecked(true);
		ChangeFunc10(pActionFunc[9]->actions().at(Func10));
		pActionFunc[10]->actions().at(Func11)->setChecked(true);
		ChangeFunc11(pActionFunc[10]->actions().at(Func11));
		pActionFunc[11]->actions().at(Func12)->setChecked(true);
		ChangeFunc12(pActionFunc[11]->actions().at(Func12));
		pActionFunc[12]->actions().at(Func13)->setChecked(true);
		ChangeFunc13(pActionFunc[12]->actions().at(Func13));
		pActionFunc[13]->actions().at(Func14)->setChecked(true);
		ChangeFunc14(pActionFunc[13]->actions().at(Func14));

        pActionDialFunc[0]->actions().at(EncFuncVal0)->setChecked(true);
        ChangeDial0(pActionDialFunc[0]->actions().at(EncFuncVal0));
        pActionDialFunc[1]->actions().at(EncFuncVal1)->setChecked(true);
		ChangeDial1(pActionDialFunc[1]->actions().at(EncFuncVal1));
        pActionDialFunc[2]->actions().at(EncFuncVal2)->setChecked(true);
		ChangeDial2(pActionDialFunc[2]->actions().at(EncFuncVal2));
		pActionDialFunc[3]->actions().at(EncFuncVal3)->setChecked(true);
        ChangeDial3(pActionDialFunc[3]->actions().at(EncFuncVal3));

        pActionDial[1]->actions().at(FuncDial1)->setChecked(true);
        ChangeMoveDial1(pActionDial[1]->actions().at(FuncDial1));
        pActionDial[2]->actions().at(FuncDial2)->setChecked(true);
        ChangeMoveDial2(pActionDial[2]->actions().at(FuncDial2));
        pActionDial[3]->actions().at(FuncDial3)->setChecked(true);
        ChangeMoveDial3(pActionDial[3]->actions().at(FuncDial3));
	pSett->endGroup();
    onWinEvent();
}
void Panel::onWinEvent()
{
	QStringList lst = pPan->devList();
	if(lst.count() > 0)
	{
		if(pPan->isOpen())
		{
			pPan->close();
			if(ui.checkBox->isChecked())
			{
				pPan->open(0);
                onEncLed(ui.chbEncoderLed->isChecked());
                onKeyLed(ui.chbKeyLed->isChecked());
			}
		}
		else
		{
			if(ui.checkBox->isChecked())
			{
				pPan->open(0);
                onEncLed(ui.chbEncoderLed->isChecked());
                onKeyLed(ui.chbKeyLed->isChecked());
			}
		}
	}
	else
	{
		onEncLed(false);
		onKeyLed(false);
		pPan->close();
	}
}

void Panel::onClose()
{
    if(pPan->isOpen())
    {
        onEncLed(false);
        onKeyLed(false);
        pPan->close();
    }
}

void Panel::createMenu()
{
	for(int i = 0; i < 14; i++)
	{
		pMenuFunc[i] = new QMenu;
		pActionFunc[i] = new QActionGroup(this);
		AddActionButton(pMenuFunc[i]);
	}
	for(int i = 0; i < 4; i++)
	{
	    pMenuDialPress[i] = new QMenu;
	    pActionDialFunc[i] = new QActionGroup(this);
	    AddActionButton(pMenuDialPress[i]);
	}

    for(int i = 0; i < 33; i++)
	{
		for(int j = 0; j < 14; j++)
		{
			pMenuFunc[j]->actions().at(i)->setCheckable(true);
			pActionFunc[j]->addAction(pMenuFunc[j]->actions().at(i));
		}
        pMenuDialPress[0]->actions().at(i)->setCheckable(true);
        pMenuDialPress[1]->actions().at(i)->setCheckable(true);
        pMenuDialPress[2]->actions().at(i)->setCheckable(true);
        pMenuDialPress[3]->actions().at(i)->setCheckable(true);
		pActionDialFunc[0]->addAction(pMenuDialPress[0]->actions().at(i));
		pActionDialFunc[1]->addAction(pMenuDialPress[1]->actions().at(i));
		pActionDialFunc[2]->addAction(pMenuDialPress[2]->actions().at(i));
		pActionDialFunc[3]->addAction(pMenuDialPress[3]->actions().at(i));
	}
	for(int j = 0; j < 14; j++) pMenuFunc[j]->actions().at(0)->setChecked(true);
	pMenuDialPress[0]->actions().at(0)->setChecked(true);
	pMenuDialPress[1]->actions().at(0)->setChecked(true);
	pMenuDialPress[2]->actions().at(0)->setChecked(true);
	pMenuDialPress[3]->actions().at(0)->setChecked(true);
    ui.pbFunc1_2->setMenu(pMenuFunc[0]);
    ui.pbFunc2_2->setMenu(pMenuFunc[1]);
    ui.pbFunc3_2->setMenu(pMenuFunc[2]);
    ui.pbFunc4_2->setMenu(pMenuFunc[3]);
    ui.pbFunc5_2->setMenu(pMenuFunc[4]);
    ui.pbFunc6_2->setMenu(pMenuFunc[5]);
    ui.pbFunc7_2->setMenu(pMenuFunc[6]);
    ui.pbFunc8_2->setMenu(pMenuFunc[7]);
    ui.pbFunc9_2->setMenu(pMenuFunc[8]);
    ui.pbFunc10_2->setMenu(pMenuFunc[9]);
    ui.pbFunc11_2->setMenu(pMenuFunc[10]);
    ui.pbFunc12_2->setMenu(pMenuFunc[11]);
    ui.pbFunc13_2->setMenu(pMenuFunc[12]);
    ui.pbFunc14_2->setMenu(pMenuFunc[13]);
    ui.pbDial0_PressF->setMenu(pMenuDialPress[0]);
    ui.pbDial1_PressF->setMenu(pMenuDialPress[1]);
    ui.pbDial2_PressF->setMenu(pMenuDialPress[2]);
    ui.pbDial3_PressF->setMenu(pMenuDialPress[3]);
    connect(pMenuFunc[0], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc1(QAction*)));
    connect(pMenuFunc[1], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc2(QAction*)));
    connect(pMenuFunc[2], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc3(QAction*)));
    connect(pMenuFunc[3], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc4(QAction*)));
    connect(pMenuFunc[4], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc5(QAction*)));
    connect(pMenuFunc[5], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc6(QAction*)));
    connect(pMenuFunc[6], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc7(QAction*)));
    connect(pMenuFunc[7], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc8(QAction*)));
    connect(pMenuFunc[8], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc9(QAction*)));
    connect(pMenuFunc[9], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc10(QAction*)));
    connect(pMenuFunc[10], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc11(QAction*)));
    connect(pMenuFunc[11], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc12(QAction*)));
    connect(pMenuFunc[12], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc13(QAction*)));
    connect(pMenuFunc[13], SIGNAL(triggered(QAction*)), this, SLOT(ChangeFunc14(QAction*)));
    connect(pMenuDialPress[0], SIGNAL(triggered(QAction*)), this, SLOT(ChangeDial0(QAction*)));
    connect(pMenuDialPress[1], SIGNAL(triggered(QAction*)), this, SLOT(ChangeDial1(QAction*)));
    connect(pMenuDialPress[2], SIGNAL(triggered(QAction*)), this, SLOT(ChangeDial2(QAction*)));
    connect(pMenuDialPress[3], SIGNAL(triggered(QAction*)), this, SLOT(ChangeDial3(QAction*)));
    connect(pPan, SIGNAL(Enc0(int)), this, SLOT(ChangeFreq(int)));
}
void Panel::AddActionButton(QMenu *p)
{
	p->addAction("Band Up");
	p->addAction("Band Down");
	p->addAction("Mode Up");
	p->addAction("Mode Down");
	p->addAction("Volume Up");
	p->addAction("Volume Down");
	p->addAction("Mute");
	p->addAction("BIN");
	p->addAction("NR");
	p->addAction("ANF");
	p->addAction("NB");
	p->addAction("Power On/Off");
	p->addAction("NB2");
	p->addAction("VFO Lock");
	p->addAction("Memory Up");
    p->addAction("Memory Down");
	p->addAction("Filter Up");
	p->addAction("Filter Down");
	p->addAction("SQL");
	p->addAction("Filter Shift Reset");
    p->addAction("None");
	p->addAction("PA");
	p->addAction("Agc Up");
	p->addAction("Agc Down");
	p->addAction("Preamp Up");
	p->addAction("Preamp Down");
	p->addAction("Next Station");
	p->addAction("Previous Station");
    p->addAction("Step Up");
    p->addAction("Step Down");
    p->addAction("Zoom Up");
    p->addAction("Zoom Down");
    p->addAction("D/I");
    p->actions().at(0)->setChecked(true);
}
void Panel::createDialMenu()
{
	for(int i = 0; i < 4; i++)
	{
		pMenuDial[i] = new QMenu;
        pActionDial[i] = new QActionGroup(this);
		AddActionDial(pMenuDial[i]);
	}
    ui.pbDial1->setMenu(pMenuDial[1]);
    ui.pbDial2->setMenu(pMenuDial[2]);
    ui.pbDial3->setMenu(pMenuDial[3]);
    for(int i = 0; i < 10; i++)
    {
    	pActionDial[0]->addAction(pMenuDial[0]->actions().at(i));
    	pActionDial[1]->addAction(pMenuDial[1]->actions().at(i));
    	pActionDial[2]->addAction(pMenuDial[2]->actions().at(i));
    	pActionDial[3]->addAction(pMenuDial[3]->actions().at(i));
    	pMenuDial[0]->actions().at(i)->setCheckable(true);
    	pMenuDial[1]->actions().at(i)->setCheckable(true);
    	pMenuDial[2]->actions().at(i)->setCheckable(true);
    	pMenuDial[3]->actions().at(i)->setCheckable(true);
    }
	pMenuDial[0]->actions().at(0)->setChecked(true);
	pMenuDial[1]->actions().at(0)->setChecked(true);
	pMenuDial[2]->actions().at(0)->setChecked(true);
	pMenuDial[3]->actions().at(0)->setChecked(true);
    connect(pMenuDial[1], SIGNAL(triggered(QAction*)), this, SLOT(ChangeMoveDial1(QAction*)));
    connect(pMenuDial[2], SIGNAL(triggered(QAction*)), this, SLOT(ChangeMoveDial2(QAction*)));
    connect(pMenuDial[3], SIGNAL(triggered(QAction*)), this, SLOT(ChangeMoveDial3(QAction*)));
}
void Panel::AddActionDial(QMenu *p)
{
    p->addAction("Volume");
    p->addAction("Mic Gain");
    p->addAction("SQL");
    p->addAction("Filter High");
    p->addAction("Filter Low");
    p->addAction("Filter Width");
    p->addAction("Filter Shift");
    p->addAction("Drive");
    p->addAction("IF");
    p->addAction("RF Gain");
}
void Panel::ChangeFunc1(QAction *action)
{
	Func1 = pMenuFunc[0]->actions().indexOf(action);
	SetIndexButton(Func1, ui.pbFunc1_2);
}
void Panel::ChangeFunc2(QAction *action)
{
	Func2 = pMenuFunc[1]->actions().indexOf(action);
	SetIndexButton(Func2, ui.pbFunc2_2);
}
void Panel::ChangeFunc3(QAction *action)
{
	Func3 = pMenuFunc[2]->actions().indexOf(action);
	SetIndexButton(Func3, ui.pbFunc3_2);
}
void Panel::ChangeFunc4(QAction *action)
{
	Func4 = pMenuFunc[3]->actions().indexOf(action);
	SetIndexButton(Func4, ui.pbFunc4_2);
}
void Panel::ChangeFunc5(QAction *action)
{
	Func5 = pMenuFunc[4]->actions().indexOf(action);
	SetIndexButton(Func5, ui.pbFunc5_2);
}
void Panel::ChangeFunc6(QAction *action)
{
	Func6 = pMenuFunc[5]->actions().indexOf(action);
	SetIndexButton(Func6, ui.pbFunc6_2);
}
void Panel::ChangeFunc7(QAction *action)
{
	Func7 = pMenuFunc[6]->actions().indexOf(action);
	SetIndexButton(Func7, ui.pbFunc7_2);
}
void Panel::ChangeFunc8(QAction *action)
{
	Func8 = pMenuFunc[7]->actions().indexOf(action);
	SetIndexButton(Func8, ui.pbFunc8_2);
}
void Panel::ChangeFunc9(QAction *action)
{
	Func9 = pMenuFunc[8]->actions().indexOf(action);
	SetIndexButton(Func9, ui.pbFunc9_2);
}
void Panel::ChangeFunc10(QAction *action)
{
	Func10 = pMenuFunc[9]->actions().indexOf(action);
	SetIndexButton(Func10, ui.pbFunc10_2);
}
void Panel::ChangeFunc11(QAction *action)
{
	Func11 = pMenuFunc[10]->actions().indexOf(action);
	SetIndexButton(Func11, ui.pbFunc11_2);
}
void Panel::ChangeFunc12(QAction *action)
{
	Func12 = pMenuFunc[11]->actions().indexOf(action);
	SetIndexButton(Func12, ui.pbFunc12_2);
}
void Panel::ChangeFunc13(QAction *action)
{
	Func13 = pMenuFunc[12]->actions().indexOf(action);
	SetIndexButton(Func13, ui.pbFunc13_2);
}
void Panel::ChangeFunc14(QAction *action)
{
	Func14 = pMenuFunc[13]->actions().indexOf(action);
	SetIndexButton(Func14, ui.pbFunc14_2);
}
void Panel::ChangeDial0(QAction *action)
{
	EncFuncVal0 = pMenuDialPress[0]->actions().indexOf(action);
	SetIndexButton(EncFuncVal0, ui.pbDial0_PressF);
}
void Panel::ChangeDial1(QAction *action)
{
	EncFuncVal1 = pMenuDialPress[1]->actions().indexOf(action);
	SetIndexButton(EncFuncVal1, ui.pbDial1_PressF);
}
void Panel::ChangeDial2(QAction *action)
{
	EncFuncVal2 = pMenuDialPress[2]->actions().indexOf(action);
	SetIndexButton(EncFuncVal2, ui.pbDial2_PressF);
}
void Panel::ChangeDial3(QAction *action)
{
	EncFuncVal3 = pMenuDialPress[3]->actions().indexOf(action);
	SetIndexButton(EncFuncVal3, ui.pbDial3_PressF);
}
void Panel::ChangeMoveDial1(QAction *action)
{
	FuncDial1 = pMenuDial[1]->actions().indexOf(action);
    SetIndexDial(1, FuncDial1, ui.pbDial1);
}
void Panel::ChangeMoveDial2(QAction *action)
{
	FuncDial2 = pMenuDial[2]->actions().indexOf(action);
    SetIndexDial(2, FuncDial2, ui.pbDial2);
}
void Panel::ChangeMoveDial3(QAction *action)
{
	FuncDial3 = pMenuDial[3]->actions().indexOf(action);
    SetIndexDial(3, FuncDial3, ui.pbDial3);
}
void Panel::onOpen(bool stat)
{
    if(stat)
    {
        if(pPan->devList().count() > 0)
        {
        	pPan->open(0);
			onEncLed(ui.chbEncoderLed->isChecked());
			onKeyLed(ui.chbKeyLed->isChecked());
        }
        else
            ui.checkBox->setChecked(false);
    }
    else
    {
		onEncLed(false);
		onKeyLed(false);
        pPan->close();
    }
}

void Panel::onMox(bool tx)
{
    if(pPan->isOpen())
    {
        pPan->setLedPwr(tx);
        pPan->setLedRit(tx);
        pPan->setLedXit(tx);
    }
}

void Panel::SetIndexButton(int funcNum, QPushButton *pPb)
{
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc0(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc1(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc2(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc3(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc4(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc5(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc6(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc7(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc8(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc9(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc10(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc11(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc12(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc13(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc14(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc15(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc16(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc17(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc18(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc19(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc20(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc21(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc22(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc23(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc24(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc25(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc26(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc27(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc28(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc29(bool)));
	disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc30(bool)));
    disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc31(bool)));
    disconnect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc32(bool)));

	switch(funcNum)
	{
		case 0:
			pPb->setText("BU");
			pPb->setToolTip("Band Up");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc0(bool)));
		break;

		case 1:
			pPb->setText("BD");
			pPb->setToolTip("Band Down");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc1(bool)));
		break;

		case 2:
			pPb->setText("MU");
			pPb->setToolTip("Mode Up");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc2(bool)));
		break;

		case 3:
			pPb->setText("MD");
			pPb->setToolTip("Mode Down");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc3(bool)));
		break;

		case 4:
			pPb->setText("V+");
			pPb->setToolTip("Volume increment");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc4(bool)));
		break;

		case 5:
			pPb->setText("V-");
			pPb->setToolTip("Volume decrement");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc5(bool)));
		break;

		case 6:
			pPb->setText("MUT");
			pPb->setToolTip("MUTE");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc6(bool)));
		break;

		case 7:
			pPb->setText("BIN");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc7(bool)));
		break;

		case 8:
			pPb->setText("NR");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc8(bool)));
		break;

		case 9:
			pPb->setText("ANF");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc9(bool)));
		break;

		case 10:
			pPb->setText("NB");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc10(bool)));
		break;

		case 11:
			pPb->setText("PWR");
			pPb->setToolTip("Power On/Off");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc11(bool)));
		break;

		case 12:
			pPb->setText("NB2");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc12(bool)));
		break;

		case 13:
			pPb->setText("LOCK");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc13(bool)));
		break;

		case 14:
			pPb->setText("M+");
			pPb->setToolTip("Memory Up");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc14(bool)));
		break;

		case 15:
			pPb->setText("M-");
			pPb->setToolTip("Memory Down");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc15(bool)));
		break;

		case 16:
			pPb->setText("Filter Up");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc16(bool)));
		break;

		case 17:
			pPb->setText("Filter Down");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc17(bool)));
		break;

		case 18:
			pPb->setText("SQL");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc18(bool)));
		break;

		case 19:
			pPb->setText("FSR");
			pPb->setToolTip("-");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc19(bool)));
		break;

		case 20:
			pPb->setText("None");
			pPb->setToolTip("No Effect");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc20(bool)));
		break;

		case 21:
			pPb->setText("PA");
			pPb->setToolTip("Enable/disable PreAmplifier");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc21(bool)));
		break;

		case 22:
			pPb->setText("AgcU");
			pPb->setToolTip("Agc Up");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc22(bool)));
		break;

		case 23:
			pPb->setText("AgcD");
			pPb->setToolTip("Agc Down");
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc23(bool)));
		break;

		case 24:
			pPb->setText(tr("PrU"));
			pPb->setToolTip(tr("Preamp Up"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc24(bool)));
		break;

		case 25:
			pPb->setText(tr("PrD"));
			pPb->setToolTip(tr("Preamp down"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc25(bool)));
		break;

		case 26:
			pPb->setText(tr(">"));
			pPb->setToolTip(tr("Next Station Button"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc26(bool)));
		break;

		case 27:
			pPb->setText(tr("<"));
			pPb->setToolTip(tr("Previous Station Button"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc27(bool)));
		break;

		case 28:
			pPb->setText(tr("St+"));
			pPb->setToolTip(tr("Step Up"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc28(bool)));
		break;

		case 29:
			pPb->setText(tr("St-"));
			pPb->setToolTip(tr("Step Down"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc29(bool)));
		break;

		case 30:
			pPb->setText(tr("Z+"));
			pPb->setToolTip(tr("Zoom Up"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc30(bool)));
		break;

		case 31:
			pPb->setText(tr("Z-"));
			pPb->setToolTip(tr("Zoom Down"));
			connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc31(bool)));
		break;

        case 32:
            pPb->setText(tr("D/I"));
            pPb->setToolTip(tr("Switch control between DDS and IF"));
            connect(pPb, SIGNAL(clicked(bool)), this, SLOT(SendFunc32(bool)));
        break;
		default:
		break;
	}
}
void Panel::SetIndexDial(int Enc, int funcNum, QPushButton *pPb)
{
    switch(Enc)
    {
        case 1:
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial0(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial1(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial2(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial3(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial4(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial5(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial6(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial7(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial8(int)));
            disconnect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial9(int)));
        break;
        case 2:
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial0(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial1(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial2(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial3(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial4(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial5(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial6(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial7(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial8(int)));
            disconnect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial9(int)));
        break;
        case 3:
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial0(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial1(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial2(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial3(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial4(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial5(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial6(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial7(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial8(int)));
            disconnect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial9(int)));
        break;
        default:
        break;
    }

	switch(funcNum)
	{
		case 0:
			pPb->setText(tr("Vol"));
			pPb->setToolTip(tr("Volume"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial0(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial0(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial0(int)));
                break;

                default:
                break;
            }
		break;

		case 1:
			pPb->setText(tr("Mic"));
			pPb->setToolTip(tr("Mic Gain"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial1(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial1(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial1(int)));
                break;

                default:
                break;
            }
		break;

		case 2:
			pPb->setText(tr("SQL"));
			pPb->setToolTip(tr("SQL Value"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial2(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial2(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial2(int)));
                break;

                default:
                break;
            }
		break;

		case 3:
			pPb->setText(tr("FH"));
			pPb->setToolTip(tr("Filter High"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial3(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial3(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial3(int)));
                break;

                default:
                break;
            }
		break;

		case 4:
			pPb->setText(tr("FL"));
			pPb->setToolTip(tr("Filter Low"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial4(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial4(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial4(int)));
                break;

                default:
                break;
            }
		break;

		case 5:
			pPb->setText(tr("FW"));
			pPb->setToolTip(tr("Filter Width"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial5(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial5(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial5(int)));
                break;

                default:
                break;
            }
		break;

		case 6:
			pPb->setText(tr("FS"));
			pPb->setToolTip(tr("Filter Shift"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial6(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial6(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial6(int)));
                break;

                default:
                break;
            }
		break;

		case 7:
			pPb->setText(tr("DRV"));
			pPb->setToolTip(tr("Drive Value"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial7(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial7(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial7(int)));
                break;

                default:
                break;
            }
		break;

		case 8:
			pPb->setText(tr("IF"));
			pPb->setToolTip(tr("Intermediate Frequency"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial8(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial8(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial8(int)));
                break;

                default:
                break;
            }
		break;

		case 9:
            pPb->setText(tr("RG"));
			pPb->setToolTip(tr("RF Gain"));
            switch(Enc)
            {
                case 1:
                    connect(pPan, SIGNAL(Enc1(int)), this, SLOT(SendFuncDial9(int)));
                break;

                case 2:
                    connect(pPan, SIGNAL(Enc2(int)), this, SLOT(SendFuncDial9(int)));
                break;

                case 3:
                    connect(pPan, SIGNAL(Enc3(int)), this, SLOT(SendFuncDial9(int)));
                break;

                default:
                break;
            }
		break;

		default:
		break;
	}
}
void Panel::SendFunc0(bool stat)
{
	if(stat)
		emit band(1);
}
void Panel::SendFunc1(bool stat)
{
	if(stat)
		emit band(-1);
}
void Panel::SendFunc2(bool stat)
{
	if(stat)
		emit mode(1);
}
void Panel::SendFunc3(bool stat)
{
	if(stat)
		emit mode(-1);
}
void Panel::SendFunc4(bool stat)
{
	if(stat)
		emit volume(5);
}
void Panel::SendFunc5(bool stat)
{
	if(stat)
		emit volume(-5);
}
void Panel::SendFunc6(bool stat)
{
	if(stat)
		emit mute();
}
void Panel::SendFunc7(bool stat)
{
	if(stat)
		emit bin();
}
void Panel::SendFunc8(bool stat)
{
	if(stat)
		emit nr();
}
void Panel::SendFunc9(bool stat)
{
	if(stat)
		emit anf();
}
void Panel::SendFunc10(bool stat)
{
	if(stat)
		emit nb();
}
void Panel::SendFunc11(bool stat)
{
	if(stat)
		emit power();
}
void Panel::SendFunc12(bool stat)
{
	if(stat)
		emit nb2();
}
void Panel::SendFunc13(bool stat)
{
	if(stat)
		emit lock();
}
void Panel::SendFunc14(bool stat)
{
	if(stat)
		emit memory(-1);
}
void Panel::SendFunc15(bool stat)
{
	if(stat)
		emit memory(1);
}
void Panel::SendFunc16(bool stat)
{
	if(stat)

		emit filter(1);
}
void Panel::SendFunc17(bool stat)
{
	if(stat)
		emit filter(-1);
}
void Panel::SendFunc18(bool stat)
{
	if(stat)
		emit sqlOnOff();
}
void Panel::SendFunc19(bool stat)
{

}
void Panel::SendFunc20(bool stat)
{

}
void Panel::SendFunc21(bool stat)
{
	if(stat)
		emit pa();
}
void Panel::SendFunc22(bool stat)
{
	if(stat)
		emit agc(1);
}
void Panel::SendFunc23(bool stat)
{
	if(stat)
		emit agc(-1);
}
void Panel::SendFunc24(bool stat)
{
	if(stat)
		emit preamp(1);
}
void Panel::SendFunc25(bool stat)
{
	if(stat)
		emit preamp(-1);
}
void Panel::SendFunc26(bool stat)
{
    if(stat)
    	emit station(1);
}
void Panel::SendFunc27(bool stat)
{
    if(stat)
    	emit station(-1);
}
void Panel::SendFunc28(bool stat)
{
	if(stat)
		emit step(1);
}
void Panel::SendFunc29(bool stat)
{
	if(stat)
		emit step(-1);
}
void Panel::SendFunc30(bool stat)
{
	if(stat)
		emit zoom(1);
}
void Panel::SendFunc31(bool stat)
{
	if(stat)
		emit zoom(-1);
}
void Panel::SendFunc32(bool stat)
{
    if(stat)
        ui.chbUseIf->setChecked(!ui.chbUseIf->isChecked());
}
void Panel::SendFuncDial0(int val)
{
	emit volume(val*ui.sbVolumeStep->value());
}
void Panel::SendFuncDial1(int val)
{
	emit micGain(val*ui.sbMicGainStep->value());
}
void Panel::SendFuncDial2(int val)
{
	emit squelch(val*ui.sbSqlStep->value());
}
void Panel::SendFuncDial3(int val)
{
	emit filterHigh(val*ui.sbFilterHighStep->value());
}
void Panel::SendFuncDial4(int val)
{
	emit filterLow(val*ui.sbFilterLowStep->value());
}
void Panel::SendFuncDial5(int val)
{
	emit filterWidth(val*ui.sbFilterWidthStep->value());
}
void Panel::SendFuncDial6(int val)
{
	emit filterShift(val*ui.sbFilterShiftStep->value());
}
void Panel::SendFuncDial7(int val)
{
	emit drive(val*ui.sbDriveStep->value());
}
void Panel::SendFuncDial8(int val)
{
	if(ui.chbUseIf->isChecked())
		emit ifVal(val*ui.sbIfStep->value());
}

void Panel::SendFuncDial9(int val)
{
	emit rfGain(val*ui.sbRfGainStep->value());
}
void Panel::ChangeFreq(int val)
{
	if(ui.chbUseIf->isChecked())
		emit ifVal(val*ui.sbIfStep->value());
	else
		emit mainFreq(val);
}

void Panel::onEncLed(bool on)
{
	if(pPan->isOpen())
		pPan->setLedEnc(on);
}

void Panel::onKeyLed(bool on)
{
	if(pPan->isOpen())
		pPan->setLedKey(on);
}
