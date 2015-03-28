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

#include "Calibrator.h"

Calibrator::Calibrator(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
    ui.progressBar->setVisible(false);

	pCalibrProc = new CalibrProc;
    connect(ui.pbBack,   SIGNAL(clicked()), this, SLOT(onBack()));
    connect(ui.pbSkip,   SIGNAL(clicked()), this, SLOT(onSkip()));
    connect(ui.pbNext,   SIGNAL(clicked()), this, SLOT(onNext()));
    connect(ui.sbFreqHf, SIGNAL(valueChanged(double)), this, SLOT(onCalFreq(double)));
    connect(ui.sbLevelHf, SIGNAL(valueChanged(double)), pCalibrProc, SLOT(setSmeterValue(double)));

    connect(pCalibrProc, SIGNAL(freq(int)), this, SIGNAL(freq(int)));
    connect(pCalibrProc, SIGNAL(push()), this, SIGNAL(push()));
    connect(pCalibrProc, SIGNAL(pop()), this, SIGNAL(pop()));
    connect(pCalibrProc, SIGNAL(mode(int)), this, SIGNAL(mode(int)));
    connect(pCalibrProc, SIGNAL(ifFreq(int)), this, SIGNAL(ifFreq(int)));
    connect(pCalibrProc, SIGNAL(preamp(int)), this, SIGNAL(preamp(int)));
    connect(pCalibrProc, SIGNAL(end()), this, SLOT(onEnd()));
    connect(pCalibrProc, SIGNAL(progress(int)), ui.progressBar, SLOT(setValue(int)));

    connect(this, SIGNAL(emitFreqDbm(int, float)), pCalibrProc, SLOT(setFreqDbm(int, float)));
    connect(pCalibrProc, SIGNAL(getFreqDbm()), this, SIGNAL(getFreqDbm()));
    connect(pCalibrProc, SIGNAL(calibrationFreqCoeff(float)), this, SIGNAL(calibrationFreqCoeff(float)));

    connect(pCalibrProc, SIGNAL(getDbm()), this, SIGNAL(getDbm()));
    connect(this, SIGNAL(setDbm(float)), pCalibrProc, SLOT(setDbm(float)));
    connect(pCalibrProc, SIGNAL(calibrationDbmCoeff(int, float)), this, SIGNAL(calibrationDbmCoeff(int, float)));

    connect(pCalibrProc, SIGNAL(setWinType(int)), this, SIGNAL(setWinType(int)));

    pCalibrProc->setFreqValue(ui.sbFreqHf->value()*1000000);
    pCalibrProc->setSmeterValue(ui.sbLevelHf->value());
    xvtrxEnable = false;
}

Calibrator::~Calibrator()
{
	delete pCalibrProc;
}

void Calibrator::showEvent(QShowEvent* e)
{
	ui.stWdg->setCurrentIndex(0);
    ui.pbBack->setVisible(false);
    ui.pbSkip->setVisible(false);
	ui.pbNext->setText("Next");
    ui.pbBack->setEnabled(true);
    ui.pbNext->setEnabled(true);
    ui.pbSkip->setEnabled(true);
    ui.progressBar->setVisible(false);
    emit showCalibration();
}

void Calibrator::closeEvent(QCloseEvent *event)
{
	if(pCalibrProc->isProcess())
		event->ignore();
}

void Calibrator::onBack()
{
    if(ui.stWdg->currentIndex() <= 0)
        return;

    ui.stWdg->setCurrentIndex(ui.stWdg->currentIndex() - 1);

    if(ui.stWdg->currentIndex() > 0)
    {
        ui.pbBack->setVisible(true);
        ui.pbSkip->setVisible(true);
    }
    else
    {
        ui.pbBack->setVisible(false);
        ui.pbSkip->setVisible(false);
    }
}

void Calibrator::onSkip()
{
    if(ui.stWdg->currentIndex() == (ui.stWdg->count() - 1))
		return;

    onEnd();
}

void Calibrator::onNext()
{
    if(ui.stWdg->currentIndex() == (ui.stWdg->count() - 1))
    {
		close();
        return;
    }

    ui.progressBar->setVisible(true);
    ui.pbBack->setEnabled(false);
    ui.pbNext->setEnabled(false);
    ui.pbSkip->setEnabled(false);
    ui.sbFreqHf->setEnabled(false);
    ui.sbLevelHf->setEnabled(false);
    CalibrProc::CALIBRATION_TYPE indx = (CalibrProc::CALIBRATION_TYPE)ui.stWdg->currentIndex();
    switch(indx)
    {
     case  CalibrProc::FREQ_HF:
        pCalibrProc->begin(CalibrProc::FREQ_HF);
        break;
    case  CalibrProc::SMETER_HF:
        pCalibrProc->begin(CalibrProc::SMETER_HF);
       break;
    case  CalibrProc::POWER_HF:
        pCalibrProc->begin(CalibrProc::POWER_HF);
       break;
    case  CalibrProc::FREQ_VHF:
        pCalibrProc->begin(CalibrProc::FREQ_VHF);
       break;
    case  CalibrProc::SMETER_VHF:
        pCalibrProc->begin(CalibrProc::SMETER_VHF);
       break;
    case  CalibrProc::FREQ_UHF:
        pCalibrProc->begin(CalibrProc::FREQ_UHF);
       break;
    case  CalibrProc::SMETER_UHF:
        pCalibrProc->begin(CalibrProc::SMETER_UHF);
       break;
    default:
        ui.stWdg->setCurrentIndex(ui.stWdg->currentIndex() + 1);
        ui.progressBar->setVisible(false);
        ui.pbBack->setEnabled(true);
        ui.pbNext->setEnabled(true);
        ui.pbSkip->setEnabled(true);
        ui.pbBack->setVisible(true);
        ui.pbSkip->setVisible(true);
        ui.sbFreqHf->setEnabled(true);
        ui.sbLevelHf->setEnabled(true);
        break;
    }
    show();
}

void Calibrator::onCalFreq(double freq)
{
    pCalibrProc->setFreqValue(freq*1000000);
}

void Calibrator::onCalLevel(double freq)
{
	;
}

void Calibrator::onEnd()
{

    if(ui.stWdg->currentIndex() > 0)
    {
        ui.pbBack->setVisible(true);
        ui.pbSkip->setVisible(true);
    }
    else
    {
        ui.pbBack->setVisible(false);
        ui.pbSkip->setVisible(false);
    }
    ui.progressBar->setVisible(false);
    ui.pbBack->setEnabled(true);
    ui.pbNext->setEnabled(true);
    ui.pbSkip->setEnabled(true);
    ui.sbFreqHf->setEnabled(true);
    ui.sbLevelHf->setEnabled(true);

    if(!xvtrxEnable)
    {
        if(ui.stWdg->currentIndex() > 2)
            ui.stWdg->setCurrentIndex(ui.stWdg->count() - 1);
        else
        {
        	if(ui.stWdg->currentIndex() < 2)
        		ui.stWdg->setCurrentIndex(ui.stWdg->currentIndex() + 1);
        	else
        	{
        		ui.stWdg->setCurrentIndex(8);
        		ui.pbBack->setVisible(false);
        		ui.pbSkip->setVisible(false);
        		ui.pbNext->setText("OK");
        	}
        }
    }
    else
    {
    	if(ui.stWdg->currentIndex() < 2)
    		ui.stWdg->setCurrentIndex(ui.stWdg->currentIndex() + 1);
    	else
    	{
    		ui.stWdg->setCurrentIndex(8);
    		ui.pbBack->setVisible(false);
    		ui.pbSkip->setVisible(false);
    		ui.pbNext->setText("OK");
    	}
    }
}

void Calibrator::setParam(int freq, int fltFreq, int mode, int filtId, int preamp)
{
    freqVal = freq;
    fltFreqVal = fltFreq;
    modeVal = mode;
    filtIdVal = filtId;
    preampVal = preamp;
}

void Calibrator::setXvtrx(bool en)
{
    xvtrxEnable = en;
}
