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

#include "SdrPlugin.h"

SdrPlugin::SdrPlugin(Options *pOpt, StreamCallback *pCallBack, StreamCallback *pCallBack4, void *UsrData, QWidget *parent) : QWidget(parent)
{
    CalibrateFreq = 1.0;
    FreqDDS = 1800000;
    isStarted = false;
    pUi = &pOpt->ui;
    AudioCallBack = pCallBack;
    AudioCallBack4 = pCallBack4;
    SdrType = SUNSDR;
    pAudio = new pa19(UsrData);
    pSDRhw = NULL;
    pAudio->open();
    pUi->cbPaDriver->addItems(pAudio->driverName());
    pUi->cbPaIn->addItems(pAudio->inDevName(0));
    pUi->cbPaOut->addItems(pAudio->outDevName(0));
    connect(pOpt, SIGNAL(driverChanged(int)), this, SLOT(onPaDriverChanged(int)));
    connect(pUi->tbSDR, SIGNAL(currentChanged(int)), this, SLOT(onSdrTypeChanged(int)));
    connect(pOpt, SIGNAL(SdrPluginChanged(QString)), this, SLOT(onSdrPluginChanged(QString)));
    connect(pUi->pbPluginConfig, SIGNAL(clicked()), this, SLOT(onSdrShowConfig()));

    Sun2CalibrateFreq = 1.0;
}

SdrPlugin::~SdrPlugin()
{
    if(pSDRhw != NULL)
    {
        pSDRhw->deinit();
        delete pSDRhw;
    }
    delete pAudio;
}

void SdrPlugin::onPaDriverChanged(int Index)
{
	pUi->cbPaIn->clear();
	pUi->cbPaIn->addItems(pAudio->inDevName(Index));
	pUi->cbPaOut->clear();
	pUi->cbPaOut->addItems(pAudio->outDevName(Index));
}

void SdrPlugin::onSdrTypeChanged(int Index)
{
	SdrType = (SDR_DEVICE)Index;
}

void SdrPlugin::onSdrPluginChanged(QString path)
{
    if(pSDRhw != NULL)
    {
        pSDRhw->close();
        pSDRhw->deinit();
        delete pSDRhw;
    }

    pSDRhw = new pluginCtrl(path);
    pSDRhw->init();

    connect(pSDRhw, SIGNAL(PttChanged(bool)), this, SIGNAL(PttChanged(bool)));
    connect(pSDRhw, SIGNAL(DashChanged(bool)), this, SIGNAL(DashChanged(bool)));
    connect(pSDRhw, SIGNAL(DotChanged(bool)), this, SIGNAL(DotChanged(bool)));
    connect(pSDRhw, SIGNAL(AdcChanged(int, int)), this, SIGNAL(AdcChanged(int, int)));
    connect(pSDRhw, SIGNAL(Start(bool)), this, SIGNAL(Start(bool)));
    connect(pSDRhw, SIGNAL(ChangeMode(int)), this, SIGNAL(ChangeMode(int)));
    connect(pSDRhw, SIGNAL(DDSChanged(long)), this, SIGNAL(DDSChanged(long)));
    connect(pSDRhw, SIGNAL(TuneChanged(long)), this, SIGNAL(TuneChanged(long)));
}

void SdrPlugin::onSdrShowConfig()
{
    if(pSDRhw)
        pSDRhw->showPluginGui();
}

void SdrPlugin::OnModeChanged(int mode)
{
    if(pSDRhw)
        pSDRhw->OnModeChanged(mode);
}

void SdrPlugin::OnTuneChanged(int Freq)
{
    if(pSDRhw)
        pSDRhw->OnTuneChanged(Freq);
}

void SdrPlugin::SoundCardSampleRateChanged(int rate)
{
    pSDRhw->SoundCardSampleRateChanged(rate);
}

void SdrPlugin::onS2ChangeBuffers(int NumBuffers)
{
	QString s;
	s.setNum(NumBuffers);
}

void SdrPlugin::SetSdrType(SDR_DEVICE Type)
{
	SdrType = Type;
}

void SdrPlugin::SetPreamp(int Preamp)
{
    qDebug() << __FUNCTION__ << ":" << __LINE__ << " Preamp:" << Preamp;
    if(pSDRhw)
        pSDRhw->setPreamp(Preamp);
}

void SdrPlugin::SetWpm(int Wpm)
{
    qDebug() << __FUNCTION__ << ":" << __LINE__ << " Wpm:" << Wpm;
    if(pSDRhw)
        pSDRhw->setWpm(Wpm);
}

void SdrPlugin::SetExtCtrl(DWORD ExtData)
{
    if(pSDRhw)
        pSDRhw->setExtCtrl((BYTE)ExtData);
}

void SdrPlugin::SetDdsFreq(float Freq)
{
	FreqDDS = Freq*CalibrateFreq;

    if(pSDRhw)
        pSDRhw->setDdsFreq(FreqDDS);
}

float SdrPlugin::GetDdsFreq()
{
	return FreqDDS/CalibrateFreq;
}

void SdrPlugin::SetTrxMode(bool Mode)
{
    if(pSDRhw)
        pSDRhw->setTrxMode(Mode);
}

void SdrPlugin::Close()
{
	pAudio->close();
}

int SdrPlugin::Start()
{
	if(isStarted)
		return -4;

    if(pSDRhw == NULL)
        return -1;

	OptPlug.cbPaDriverIndex = pUi->cbPaDriver->currentIndex();
	OptPlug.cbPaOutIndex = pUi->cbPaOut->currentIndex();
	OptPlug.cbPaInIndex = pUi->cbPaIn->currentIndex();
	OptPlug.cbPaChannelsIndex = pUi->cbPaChannels->currentIndex();
	OptPlug.cbPaSampleRate = pUi->cbPaSampleRate->currentText().toInt();
	OptPlug.cbPaBufferSizeIndex = pUi->cbPaBufferSize->currentIndex();
	OptPlug.sbPaLattency = pUi->sbPaLattency->value();
	OptPlug.sbDdsMul = pUi->cbDdsOsc->currentIndex()+1;
	pAudio->setParam(&OptPlug);
	if(OptPlug.cbPaChannelsIndex == 0)
	{
		if(pAudio->start(AudioCallBack)!=0)
			return -1;
	}
	else
	{
		if(pAudio->start(AudioCallBack4)!=0)
			return -1;
	}
    pSDRhw->open(0, OptPlug.sbDdsMul);
    if(pSDRhw->isOpen()==0)
		emit sunsdrConnectInfo("SDR device is not connected.");
	else
		emit sunsdrConnectInfo("SDR device is connected.");
	return 0;

	pUi->tbSDR->setEnabled(false);
	isStarted = true;
	return 0;
}

void SdrPlugin::Stop()
{
	pAudio->stop();
    if(pSDRhw)
        pSDRhw->close();
	pUi->tbSDR->setEnabled(true);
	isStarted = false;
}

bool SdrPlugin::IsStart()
{
	return isStarted;
}

void SdrPlugin::SetDdsCalibrate(double val)
{
	CalibrateFreq = val;
}

void SdrPlugin::setCalGen(bool Mode)
{
    if(pSDRhw)
        pSDRhw->setCalGen(Mode);
}

void SdrPlugin::setVhfOsc(quint32 freq)
{
    if(pSDRhw)
        pSDRhw->setVhfOsc(freq);
}

void SdrPlugin::setUhfOsc(quint32 freq)
{
    if(pSDRhw)
        pSDRhw->setUhfOsc(freq);
}

void SdrPlugin::setMute(bool status)
{
    if(pSDRhw)
        pSDRhw->setMute(status);
}

void SdrPlugin::setXvAnt(int Mode)
{
    if(pSDRhw)
        pSDRhw->setXvAnt(Mode);
}
