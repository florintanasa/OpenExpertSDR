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
 * Copyright (C) 2014 Tsukihime
 */

#include "PluginCtrl.h"
#include "extioplugin.h"
#include <QByteArray>

pluginCtrl::pluginCtrl(QString libpath)
{
    SampleRate = 48000;
    DdsFreq = 0;
    rxtx_mode = hmRX;
    memset(&routs, 0, sizeof(InternalPluginRouts));
    pluginLoaded = false;
    pPlugLib = new QLibrary(libpath);
    pPlugLib->load();
    if(!pPlugLib->isLoaded())
        return;

    routs.getInfo = (PluginFunc_getInfo)pPlugLib->resolve("getInfo");
    if(routs.getInfo == NULL)
    {
        if(ExtIOPlugin::IsExtIO(pPlugLib))
        {
            connect(this, SIGNAL(Ptt(bool)), this, SIGNAL(PttChanged(bool)));
            ExtIOPluginInit(pPlugLib);
        }
        else
        {
            pPlugLib->unload();
            return;
        }
    }

    if(!IsExtIOMode())
    {
        #  if defined (__GNUC__) && defined(__unix__)
         QByteArray bname(PATH_MAX, 0);
        #  elif defined WIN32
        QByteArray bname(MAX_PATH, 0);
        #  endif
        routs.getInfo(bname.data());
        InfoStr = QString::fromLatin1(bname.data());

        routs.init = (PluginFunc_init)pPlugLib->resolve("init");
        routs.deinit = (PluginFunc_deinit)pPlugLib->resolve("deinit");
        routs.open = (PluginFunc_open)pPlugLib->resolve("open");
        routs.close = (PluginFunc_close)pPlugLib->resolve("close");
        routs.isOpen = (PluginFunc_isOpen)pPlugLib->resolve("isOpen");
        routs.setPreamp = (PluginFunc_setPreamp)pPlugLib->resolve("setPreamp");
        routs.setExtCtrl = (PluginFunc_setExtCtrl)pPlugLib->resolve("setExtCtrl");
        routs.setDdsFreq = (PluginFunc_setDdsFreq)pPlugLib->resolve("setDdsFreq");
        routs.setTrxMode = (PluginFunc_setTrxMode)pPlugLib->resolve("setTrxMode");
        routs.setMute = (PluginFunc_setMute)pPlugLib->resolve("setMute");
        routs.setVhfOsc = (PluginFunc_setVhfOsc)pPlugLib->resolve("setVhfOsc");
        routs.setUhfOsc = (PluginFunc_setUhfOsc)pPlugLib->resolve("setUhfOsc");
        routs.setCalGen = (PluginFunc_setCalGen)pPlugLib->resolve("setCalGen");
        routs.setXvAnt = (PluginFunc_setXvAnt)pPlugLib->resolve("setXvAnt");
        routs.showPluginGui = (PluginFunc_showPluginGui)pPlugLib->resolve("showPluginGui");
    }

    pluginLoaded = true;
}

pluginCtrl::~pluginCtrl()
{
    if(IsExtIOMode())
        ExtIOPluginDeinit();

    if(!pPlugLib->isLoaded())
        pPlugLib->unload();

    pPlugLib->deleteLater();
}

void pluginCtrl::init()
{
    if(IsExtIOMode())
        OpenHW();
    else
        if(routs.init && pluginLoaded)
            routs.init(this, &SdrStateChanged);
}

void pluginCtrl::deinit()
{
    if(IsExtIOMode())
        CloseHW();
    else
        if(routs.deinit && pluginLoaded)
            routs.deinit();
}

void pluginCtrl::open(int SdrNum, int DdsMul)
{
    if(IsExtIOMode())
        StartHW(DdsFreq);
    else
        if(routs.open && pluginLoaded)
            routs.open(SdrNum,DdsMul);
}

void pluginCtrl::close()
{
    if(IsExtIOMode())
        StopHW();
    else
        if(routs.close && pluginLoaded)
            routs.close();
}

bool pluginCtrl::isOpen()
{
    if(IsExtIOMode())
        return IsExtIOOpen();
    else
        if(routs.isOpen && pluginLoaded)
            return routs.isOpen();

    return false;
}

void pluginCtrl::setPreamp(int Preamp)
{
    if(routs.setPreamp && pluginLoaded)
        routs.setPreamp(Preamp);
}

void pluginCtrl::setExtCtrl(DWORD ExtData)
{
    if(routs.setExtCtrl && pluginLoaded)
        routs.setExtCtrl(ExtData);
}

void pluginCtrl::setDdsFreq(int Freq)
{
    bool dds_chg = DdsFreq != Freq;
    DdsFreq = Freq;

    if(IsExtIOMode()&&IsExtIOOpen())
    {
        if(dds_chg && (rxtx_mode == hmRX))
        {
            SetHWLO(Freq);
            UbdateIfLimits(DdsFreq, SampleRate);
        }
    }
    else
        if(routs.setDdsFreq && pluginLoaded)
            routs.setDdsFreq(Freq);
}

void pluginCtrl::setTrxMode(bool Mode)
{
    if(IsExtIOMode())
    {
        if(Mode)
            rxtx_mode = hmTX;
        else
            rxtx_mode = hmRX;

        StopHW();
        SetModeRxTx(rxtx_mode);
        StartHW(DdsFreq);
    }
    else
        if(routs.setTrxMode && pluginLoaded)
            routs.setTrxMode(Mode);
}

void pluginCtrl::setMute(bool Status)
{
    if(routs.setMute && pluginLoaded)
        routs.setMute(Status);
}

void pluginCtrl::setVhfOsc(quint32 freq)
{
    if(routs.setVhfOsc && pluginLoaded)
        routs.setVhfOsc(freq);
}

void pluginCtrl::setUhfOsc(quint32 freq)
{
    if(routs.setUhfOsc && pluginLoaded)
        routs.setUhfOsc(freq);
}

void pluginCtrl::setCalGen(bool Mode)
{
    if(routs.setCalGen && pluginLoaded)
        routs.setCalGen(Mode);
}

void pluginCtrl::setXvAnt(int Mode)
{
    if(routs.setXvAnt && pluginLoaded)
        routs.setXvAnt(Mode);
}

bool pluginCtrl::getInfo(QString libpath, QString &PlugName)
{
    QLibrary plugin(libpath);
    plugin.load();
    if(!plugin.isLoaded())
        return false;

    PluginFunc_getInfo getInfoProc = (PluginFunc_getInfo)plugin.resolve("getInfo");
    if(getInfoProc == NULL)
    {
        if(ExtIOPlugin::IsExtIO(&plugin))
        {
            QFileInfo fi(libpath);
            PlugName = fi.fileName();
            plugin.unload();
            return true;
        }
        else
        {
            plugin.unload();
            return false;
        }
    }
    #  if defined (__GNUC__) && defined(__unix__)
        QByteArray bname(PATH_MAX, 0);
    #  elif defined WIN32
        QByteArray bname(MAX_PATH, 0);
    #  endif
//    QByteArray bname(MAX_PATH, 0);
    getInfoProc(bname.data());
    plugin.unload();

    PlugName = QString::fromLatin1(bname.data());
    return true;
}

bool pluginCtrl::isLoaded()
{
    return pluginLoaded;
}

void pluginCtrl::showPluginGui()
{
    if(IsExtIOMode())
        ShowGUI();
    else
        if(routs.showPluginGui && pluginLoaded)
            routs.showPluginGui();
}

void pluginCtrl::SoundCardSampleRateChanged(int rate)
{
    SampleRate = rate;
    UbdateIfLimits(DdsFreq, SampleRate);
}

void pluginCtrl::SdrStateChanged(QObject *PlugCtrl, StateChgReason reason, bool arg1, int arg2, int arg3)
{
    if(PlugCtrl == NULL)
        return;

    switch (reason) {
    case crPtt:
        QMetaObject::invokeMethod(PlugCtrl, "PttChanged",  Qt::QueuedConnection,  Q_ARG(bool, arg1));
        break;
    case crDash:
        QMetaObject::invokeMethod(PlugCtrl, "DashChanged",  Qt::QueuedConnection,  Q_ARG(bool, arg1));
        break;
    case crDot:
        QMetaObject::invokeMethod(PlugCtrl, "DotChanged",  Qt::QueuedConnection,  Q_ARG(bool, arg1));
        break;
    case crAdc:
        QMetaObject::invokeMethod(PlugCtrl, "AdcChanged",  Qt::QueuedConnection,  Q_ARG(int, arg2), Q_ARG(int, arg3));
        break;
    default:
        break;
    }
}
