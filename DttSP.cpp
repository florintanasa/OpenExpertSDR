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
#include "DttSP.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <QDebug>

static double y2=0.0;

DttSP::DttSP(void *UsrData, QObject *parent): QThread(parent)
{
	UserData = UsrData;

	DspPos = 0;
	DspCurrentPos = 0;

	use_last = false;
	tri_direction = 1;
	tri_val = 0.0;
	saw_val = 0.0;
	saw_direction = 1;

	DspSamleRate = 48000;

    pDsp = new DttSPLibrary();
    isLoad = pDsp->load();

	if(!isLoad)
		return;

	TrxMode = RX;
	DspSamleRate = 48000.0;
	GlitchBufTxIn = 1;
	GlitchBufTxOut = 1;
	GlitchBufRxIn = 1;
	GlitchBufRxOut = 1;
	RitEnable = false;
	RitValue = 0;

	isRx2On = false;
	Rx1Bal = Rx2Bal = 50;
}

void DttSP::Open()
{
    pDsp->Setup_SDR();
    pDsp->Release_Update();
	start(QThread::HighestPriority);
}

void DttSP::Close()
{
    pDsp->Destroy_SDR();
}

void DttSP::AudioReset()
{
    pDsp->AudioReset();
}

void DttSP::SetMode(SDRMODE Mode)
{
    SdrMode = Mode;
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetMode(Mode);
    qDebug()<<"RX1_CHANNEL is set:"<<Mode;
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetMode(Mode);
    qDebug()<<"RX2_CHANNEL is set:"<<Mode;
}

SDRMODE DttSP::GetMode()
{
    return SdrMode;
}

void DttSP::SetDcBlock(bool Status)
{
    pDsp->SetDCBlock(Status);
}

void DttSP::SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetFilter(low_frequency,high_frequency,taps,trx);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetFilter(low_frequency,high_frequency,taps,trx);
}

void DttSP::SetRxOutputGain(double Gain)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXOutputGain(Gain);
}

void DttSP::SetRx1Osc(double Freq)
{
	if(TrxMode == RX)
	{
		if(RitEnable)
			Freq += RitValue;
	}

    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetOsc(Freq);
   qDebug()<<"SetRx1Osc is set:"<<Freq;
}

void DttSP::SetTxOsc(double Freq)
{
    pDsp->SetTXOsc(Freq);
}

void DttSP::SetSampleRate(double Rate)
{
	DspSamleRate = Rate;
    pDsp->SetSampleRate(Rate);
}

void DttSP::SetNr(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetNR(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetNR(Status);
}

void DttSP::SetNrVals(int taps, int delay, double gain, double leak)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetNRvals(taps, delay, gain, leak);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetNRvals(taps, delay, gain, leak);
}

void DttSP::SetTxCompandSt(int Param)
{
    pDsp->SetTXCompandSt(Param);
}

void DttSP::SetTxCompand(double Param)
{
    pDsp->SetTXCompand(Param);
}

void DttSP::SetTxSquelchSt(bool Status)
{
    pDsp->SetTXSquelchSt(Status);
}

void DttSP::SetTxSquelchVal(float Val)
{
    pDsp->SetTXSquelchVal(Val);
}

void DttSP::SetAnf(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetANF(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetANF(Status);
}

void DttSP::SetANFvals(int taps, int delay, double gain, double leak)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetANFvals(taps,delay,gain,leak);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetANFvals(taps,delay,gain,leak);
}

void DttSP::SetNB(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetNB(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetNB(Status);
	pDsp->SetRXListen(RX_PANARAMA_CHANNEL);
	pDsp->SetNB(Status);
}

void DttSP::SetNBvals(double Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetNBvals(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetNBvals(Val);
	pDsp->SetRXListen(RX_PANARAMA_CHANNEL);
	pDsp->SetNBvals(Val);
}

void DttSP::SetSdrOm(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetSDROM(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetSDROM(Status);
	pDsp->SetRXListen(RX_PANARAMA_CHANNEL);
	pDsp->SetSDROM(Status);
}

void DttSP::SetSdrOmvals(double Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetSDROMvals(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetSDROMvals(Val);
	pDsp->SetRXListen(RX_PANARAMA_CHANNEL);
	pDsp->SetSDROMvals(Val);
}

void DttSP::SetBin(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetBIN(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetBIN(Status);
}

void DttSP::SetRxAgc(AGCMODE Mode)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGC(Mode);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGC(Mode);
}

void DttSP::SetRxAgcAttack(int Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGCAttack(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGCAttack(Val);
}

void DttSP::SetRxAgcDecay(int Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGCDecay(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGCDecay(Val);
}

void DttSP::SetRxAgcHang(int Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGCHang(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGCHang(Val);
}

void DttSP::SetRxAgcSlope(int Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGCSlope(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGCSlope(Val);
}

void DttSP::SetRxAgcHangThreshold(int Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGCHangThreshold(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGCHangThreshold(Val);
}

void DttSP::SetTxLevelerTop(double Val)
{
    pDsp->SetTXLevelerTop(Val);
}

void DttSP::SetFixedAgc(double Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetFixedAGC(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetFixedAGC(Val);
}

void DttSP::SetRxAgcTop(double Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXAGCTop(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXAGCTop(Val);
}

void DttSP::SetCorrectTXIQPhase(double Phase)
{
    pDsp->SetCorrectTXIQPhase(Phase/10.0);
}

void DttSP::SetCorrectTXIQGain(double Gain)
{
    pDsp->SetCorrectTXIQGain(Gain/10.0);
}

void DttSP::SetPwsMode(int Val)
{
    pDsp->SetPWSmode(Val);
}

void DttSP::SetWindow(Windowtype Wtype)
{
    pDsp->SetWindow(Wtype);
}

void DttSP::SetSpectrumPolyphase(bool Status)
{
    pDsp->SetSpectrumPolyphase(Status);
}

void DttSP::SetTxEQ(int *p)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetGrphTXEQ(p);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetGrphTXEQ(p);
}

void DttSP::SetGrphTXEQcmd(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetGrphTXEQcmd(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetGrphTXEQcmd(Status);
}

void DttSP::SetGrphRxEQ(int *p)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetGrphRXEQ(p);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetGrphRXEQ(p);
}

void DttSP::SetGrphRxEQcmd(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetGrphRXEQcmd(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetGrphRXEQcmd(Status);
}

void DttSP::SetTxCarrierLevel(float Val)
{
    pDsp->SetTXCarrierLevel(Val);
}

void DttSP::SetSquelchVal(double Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetSquelchVal(Val);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetSquelchVal(Val);
}

void DttSP::SetSquelchState(bool Status)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetSquelchState(Status);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetSquelchState(Status);
}

void DttSP::SetTrx(TRXMODE Mode)
{
    pDsp->SetTRX(Mode);
    TrxMode = Mode;
}

void DttSP::SetDSPBuflen(int BufLen)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetDSPBuflen(BufLen);
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetDSPBuflen(BufLen);
}

void DttSP::SetAudioSize(unsigned int Size)
{
    pDsp->SetAudioSize(Size);
}

void DttSP::ProcessSpectrum(float *pBuf)
{
    pDsp->Process_Spectrum(pBuf);
}

void DttSP::ProcessPanadapter(float *pBuf)
{
    pDsp->Process_Panadapter(pBuf);
}

void DttSP::ProcessPhase(float *pBuf, int Param)
{
    pDsp->Process_Phase(pBuf, Param);
}

void DttSP::ProcessScope(float *pBuf, int Param)
{
    pDsp->Process_Scope(pBuf, Param);
}

float DttSP::CalculateMeters(METERTYPE Meter)
{
	pDsp->SetRXListen(RX1_CHANNEL);
    return(pDsp->Calculate_Meters(Meter));
}

void DttSP::AudioCallback(float* pInL, float* pInR, float* pOutL, float* pOutR, unsigned int FrameCount)
{
    pDsp->AudioCallback(pInL, pInR, pOutL, pOutR, FrameCount);
}

void DttSP::run()
{
    pDsp->ProcessSamplesThread();
}

void DttSP::SetPanaramaOn(bool on)
{
    pDsp->SetRXListen(RX_PANARAMA_CHANNEL);
    if(on)
    {
        pDsp->SetRXOn(RX_PANARAMA_CHANNEL);
        pDsp->SetOsc(0.0);
        pDsp->SetRXOutputGain(0.0);
        pDsp->SetMode(SPEC);
        pDsp->SetRXAGC(agcOFF);
    }
    else
    {
        pDsp->SetRXOff(RX_PANARAMA_CHANNEL);
    }
}

double DttSP::SineWave(float* Buf, int Samples, double Phase, double Freq)
{
	double phase_step = Freq/DspSamleRate*2*DSP_PI;
	double cosval = cos(Phase);
	double sinval = sin(Phase);
	double cosdelta = cos(phase_step);
	double sindelta = sin(phase_step);
	double tmpval;

	for(int i = 0; i < Samples; i++ )
	{
		tmpval = cosval*cosdelta - sinval*sindelta;
		sinval = cosval*sindelta + sinval*cosdelta;
		cosval = tmpval;

		Buf[i] = (float)(sinval);
		Phase += phase_step;
	}
	return Phase;
}

double DttSP::CosineWave(float* Buf, int Samples, double Phase, double Freq)
{
	double phase_step = Freq/DspSamleRate*2*DSP_PI;
	double cosval = cos(Phase);
	double sinval = sin(Phase);
	double cosdelta = cos(phase_step);
	double sindelta = sin(phase_step);
	double tmpval;

	for(int i = 0; i < Samples; i++ )
	{
		tmpval = cosval*cosdelta - sinval*sindelta;
		sinval = cosval*sindelta + sinval*cosdelta;
		cosval = tmpval;

		Buf[i] = (float)(cosval);

		Phase += phase_step;
	}

	return Phase;
}

void DttSP::ScaleBuffer(float *InBuf, float *OutBuf, int Samples, float Scale)
{
	for(int i = 0; i < Samples; i++)
		OutBuf[i] = InBuf[i] * Scale;
}

void DttSP::SineWave2Tone(float* buf, int samples, double phase1, double phase2, double freq1, double freq2, double *updated_phase1, double *updated_phase2)
{
	double phase_step1 = freq1/DspSamleRate*2*DSP_PI;
	double cosval1 = cos(phase1);
	double sinval1 = sin(phase1);
	double cosdelta1 = cos(phase_step1);
	double sindelta1 = sin(phase_step1);

	double phase_step2 = freq2/DspSamleRate*2*DSP_PI;
	double cosval2 = cos(phase2);
	double sinval2 = sin(phase2);
	double cosdelta2 = cos(phase_step2);
	double sindelta2 = sin(phase_step2);
	double tmpval;

	for(int i = 0; i < samples; i++ )
	{
		tmpval = cosval1*cosdelta1 - sinval1*sindelta1;
		sinval1 = cosval1*sindelta1 + sinval1*cosdelta1;
		cosval1 = tmpval;

		tmpval = cosval2*cosdelta2 - sinval2*sindelta2;
		sinval2 = cosval2*sindelta2 + sinval2*cosdelta2;
		cosval2 = tmpval;

		buf[i] = (float)(sinval1*0.5 + sinval2*0.5);

		phase1 += phase_step1;
		phase2 += phase_step2;
	}

	(*updated_phase1) = phase1;
	(*updated_phase2) = phase2;
}

void DttSP::CosineWave2Tone(float* buf, int samples, double phase1, double phase2, double freq1, double freq2, double *updated_phase1, double *updated_phase2)
{
	double phase_step1 = freq1/DspSamleRate*2*DSP_PI;
	double cosval1 = cos(phase1);
	double sinval1 = sin(phase1);
	double cosdelta1 = cos(phase_step1);
	double sindelta1 = sin(phase_step1);

	double phase_step2 = freq2/DspSamleRate*2*DSP_PI;
	double cosval2 = cos(phase2);
	double sinval2 = sin(phase2);
	double cosdelta2 = cos(phase_step2);
	double sindelta2 = sin(phase_step2);
	double tmpval;

	for(int i=0; i<samples; i++ )
	{
		tmpval = cosval1*cosdelta1 - sinval1*sindelta1;
		sinval1 = cosval1*sindelta1 + sinval1*cosdelta1;
		cosval1 = tmpval;

		tmpval = cosval2*cosdelta2 - sinval2*sindelta2;
		sinval2 = cosval2*sindelta2 + sinval2*cosdelta2;
		cosval2 = tmpval;

		buf[i] = (float)(cosval1*0.5 + cosval2*0.5);

		phase1 += phase_step1;
		phase2 += phase_step2;
	}

	(*updated_phase1) = phase1;
	(*updated_phase2) = phase2;
}

double DttSP::Boxmuller(double m,double s)
{
	double x1,x2,w,y1;
	if (use_last)
	{
		y1 = y2;
		use_last = false;
	}
	else
	{
		do
		{
			x1 = (2.0 * ((rand()%1000)/1000.0) - 1.0);
			x2 = (2.0 * ((rand()%1000)/1000.0) - 1.0);
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0);

		w = qSqrt((-2.0 * qLn(w))/w);
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = true;
	}

	return( m + y1 * s );
}

void DttSP::Noise(float* buf, int samples)
{
	for(int i=0; i<samples; i++)
	{
		buf[i] = (float)Boxmuller(0.0,0.2);
	}
}

void DttSP::Triangle(float* buf, int samples, double freq)
{
	double step = freq/DspSamleRate*2*tri_direction;
	for(int i=0; i<samples; i++)
	{
		buf[i] = (float)tri_val;
		tri_val += step;
		if(tri_val >= 1.0 || tri_val <= -1.0)
		{
			step = -step;
			tri_val += 2*step;
			if(step < 0)
				tri_direction = -1;
			else
				tri_direction = 1;
		}
	}
}

void DttSP::Sawtooth(float* buf, int samples, double freq)
{
	double step = freq/DspSamleRate*saw_direction;
	for(int i=0; i<samples; i++)
	{
		buf[i] = (float)saw_val;
		saw_val += step;

		if(saw_val >= 1.0)
			saw_val -= 2.0;
		if(saw_val <= -1.0)
			saw_val += 2.0;
	}
}

void DttSP::ClearBuffer(float *buf, int samples)
{
	memset(buf, 0, samples*sizeof(float));
}

double DttSP::SamleRate()
{
	return DspSamleRate;
}

void DttSP::OnGlitchCompChanged(QByteArray p)
{
	GlitchBufTxIn = p.data()[0];
	GlitchBufTxOut =p.data()[1];
	GlitchBufRxIn = p.data()[2];
	GlitchBufRxOut = p.data()[3];
}

void DttSP::SetRx1On(bool on)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    if(on)
        pDsp->SetRXOn(RX1_CHANNEL);
    else
        pDsp->SetRXOff(RX2_CHANNEL);
}

void DttSP::SetRx2On(bool on)
{
	isRx2On = on;
    pDsp->SetRXListen(RX2_CHANNEL);
	if(on)
	{
        pDsp->SetRXOn(RX2_CHANNEL);
        pDsp->SetRXListen(RX1_CHANNEL);
        pDsp->SetRXPan(Rx1Bal/100.0);
	}
	else
	{
        pDsp->SetRXOff(RX2_CHANNEL);
        pDsp->SetRXListen(RX1_CHANNEL);
        pDsp->SetRXPan(0.5);
	}
}

void DttSP::SetRx1Volume(int Val)
{
    pDsp->SetRXListen(RX1_CHANNEL);
    pDsp->SetRXOutputGain(Val/100.0);
}

void DttSP::SetRx2Volume(int Val)
{
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXOutputGain(Val/100.0);
}

void DttSP::SetRx1Balance(int Val)
{
	if(isRx2On)
	{
        pDsp->SetRXListen(RX1_CHANNEL);
        pDsp->SetRXPan(Val/100.0);
	}
	Rx1Bal = Val;
}

void DttSP::SetRx2Balance(int Val)
{
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetRXPan(Val/100.0);
}

void DttSP::SetRx2Osc(double Freq)
{
    pDsp->SetRXListen(RX2_CHANNEL);
    pDsp->SetOsc(Freq);
}

void DttSP::SetRitEnable(bool state)
{
	RitEnable = state;
}

void DttSP::SetRitValue(int value)
{
	RitValue = -value;
}
