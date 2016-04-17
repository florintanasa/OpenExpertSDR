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

#include "pa19.h"
#include "portaudio.h"

int Pa19CallBack(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, unsigned long statusFlags, void *userData)
{
	pa19 *pPa19 = (pa19*)userData;
	pPa19->pCallbackFunc(input, output, frameCount, pPa19->pUserData);
	return 0;
};

pa19::pa19(void *UserData)
{
	IsOpened = false;
	isStarted = false;
	outOffsetIndex = inOffsetIndex = 0;
	pCallbackFunc = 0;
	pUserData = UserData;
    pStream1 = 0;
}
pa19::~pa19()
{

}
void pa19::open()
{
	if(!IsOpened)
	{
		IsOpened = true;
		Pa_Initialize();
	}
}
void pa19::close()
{
	if(IsOpened)
	{
		IsOpened = false;
		Pa_Terminate();
	}
}
int pa19::start(StreamCallback *Func)
{
	if(Func == 0)
    {
		qDebug()<< "pa19: start( " <<  Func << " )";
		return -1;
    }

	QMessageBox msgBox;
	if(!isStarted)
	{
		int Err = 0;

        if(inParam.device < 0)
			return -1;
        if(outParam.device < 0)
			return -1;

		int latency = Pa_GetDeviceInfo(outParam.device)->defaultLowOutputLatency;

		pCallbackFunc = Func;
        if(pStream1 != 0)
        {
            qDebug()<< "pa19: start(StreamCallback): Pa_IsStreamStopped: " << Pa_GetErrorText(Err);
            Pa_AbortStream(pStream1);
            Pa_CloseStream(pStream1);
            pStream1 = 0;
            return -1;
        }

		Err = Pa_OpenStream(&pStream1, &inParam, &outParam, sampleRate, bufferSize, 0, Pa19CallBack, this);
		if(Err < 0)
		{
			qWarning() << "pa19::start(): " << Pa_GetErrorText(Err);
			msgBox.setText("PortAudio Error! Can't open stream.");
			msgBox.exec();
			return -1;
		}
		Err = Pa_StartStream(pStream1);
		if(Err < 0)
		{
			qWarning() << "pa19::start(): " << Pa_GetErrorText(Err);
			msgBox.setText("PortAudio Error! Can't start stream.");
			msgBox.exec();
			return -1;
		}
		isStarted = true;
		return 0;
	}
	return -2;
}
void pa19::stop()
{
	if(isStarted)
	{
		isStarted = false;
		Pa_AbortStream(pStream1);
		Pa_CloseStream(pStream1);

        pStream1 = 0;
	}
}
bool pa19::isStart()
{
	return isStarted;
}
void pa19::setParam(PLUGIN_OPTIONS *Param)
{
    bool set_in = false;
    bool set_out = false;
    bool set_drv = false;

    for(int j = 0; j < Pa_GetHostApiCount(); j++)
    {
        const PaHostApiInfo *pInfo = Pa_GetHostApiInfo(j);
        if(!set_drv && pInfo->name == Param->cbPaDriverItem)
        {
            set_drv = true;
            DrvIndex = j;
        }
    }

    const PaHostApiInfo *pInfo = Pa_GetHostApiInfo(DrvIndex);

    for(int i = 0; i < pInfo->deviceCount; i++)
    {
        int DeviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(DrvIndex, i);
        const PaDeviceInfo *pDevInfo = Pa_GetDeviceInfo(DeviceIndex);
        if(pDevInfo->maxInputChannels > 0 || pDevInfo->maxOutputChannels > 0)
        {
            if(!set_in && pDevInfo->name == Param->cbPaInItem)
            {
                set_in = true;
                inOffsetIndex = i;
            }

            if(!set_out && pDevInfo->name == Param->cbPaOutItem)
            {
                set_out = true;
                outOffsetIndex = i;
            }
        }
    }

//    outParam.device = Pa_HostApiDeviceIndexToDeviceIndex(Param->cbPaDriverIndex, Param->cbPaOutIndex + outOffsetIndex);
//    outParam.device = Pa_HostApiDeviceIndexToDeviceIndex(Param->cbPaDriverIndex, outOffsetIndex);
    outParam.device = Pa_HostApiDeviceIndexToDeviceIndex(DrvIndex, outOffsetIndex);
    outParam.channelCount = (Param->cbPaChannelsIndex + 1)*2;
	outParam.sampleFormat = paFloat32 | paNonInterleaved;
	outParam.suggestedLatency = Param->sbPaLattency/1000.0;
	outParam.hostApiSpecificStreamInfo = 0;

//    inParam.device = Pa_HostApiDeviceIndexToDeviceIndex(Param->cbPaDriverIndex, Param->cbPaInIndex + inOffsetIndex);
//    inParam.device = Pa_HostApiDeviceIndexToDeviceIndex(Param->cbPaDriverIndex, inOffsetIndex);
    inParam.device = Pa_HostApiDeviceIndexToDeviceIndex(DrvIndex, inOffsetIndex);
	inParam.channelCount = (Param->cbPaChannelsIndex + 1)*2;
	inParam.sampleFormat = paFloat32 | paNonInterleaved;
	inParam.suggestedLatency = Param->sbPaLattency/1000.0;
	inParam.hostApiSpecificStreamInfo = 0;

	sampleRate = (double)(Param->cbPaSampleRate);
	bufferSize = 0x100 << Param->cbPaBufferSizeIndex;
}
QString pa19::getVersion()
{
	return QString(Pa_GetVersionText());
}
QStringList pa19::driverName()
{
	QStringList lst;
	for(int i = 0; i < Pa_GetHostApiCount(); i++)
	{
		const PaHostApiInfo *pInfo = Pa_GetHostApiInfo(i);
		lst << QString::fromLocal8Bit(pInfo->name);
	}
	return lst;
}
QStringList pa19::outDevName(int Host)
{
	bool set = false;
	QStringList lst;

    if(Host < 0) Host = 0;

	const PaHostApiInfo *pInfo = Pa_GetHostApiInfo(Host);

	for(int i = 0; i < pInfo->deviceCount; i++)
	{
		int DeviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(Host, i);
		const PaDeviceInfo *pDevInfo = Pa_GetDeviceInfo(DeviceIndex);
		if(pDevInfo->maxOutputChannels > 0)
		{
			lst << QString::fromLocal8Bit(pDevInfo->name);
            if(!set)
			{
				set = true;
                outOffsetIndex = i;
			}
		}
	}
	return lst;
}
QStringList pa19::inDevName(int Host)
{
    bool set = false;
	QStringList lst;

    if(Host < 0) Host = 0;

    const PaHostApiInfo *pInfo = Pa_GetHostApiInfo(Host);

	for(int i = 0; i < pInfo->deviceCount; i++)
	{
		int DeviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(Host, i);
		const PaDeviceInfo *pDevInfo = Pa_GetDeviceInfo(DeviceIndex);
		if(pDevInfo->maxInputChannels > 0)
		{
            lst << QString::fromLocal8Bit(pDevInfo->name);
            if(!set)
			{
				set = true;
                inOffsetIndex = i;
            }
		}
    }
	return lst;
}
