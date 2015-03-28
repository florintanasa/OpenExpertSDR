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

#ifndef DTTSP_H_
#define DTTSP_H_

#include <QtGui>
#include <QFuture>
#ifdef _MSC_VER // Windows
#include <process.h>
#include "qt_windows.h"
#else // Linux
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif /*_MSC_VER*/
#include "Defines.h"
#include "ThreadFunc.h"
#include "dttsplibrary.h"

const int MAX_DSP_MEMORY_BUF = 1000;
const double DSP_PI = 3.1415926536;

typedef enum {
    RX_PANARAMA_CHANNEL = 0,
    RX1_CHANNEL = 1,
    RX2_CHANNEL = 2
} DspChannels;

typedef enum
{
    PIN_12	= 0x08,
    DASH	= 0x10,
    DOT		= 0x20,
    PA_DATA	= 0x40,
    PIN_11	= 0x80,
}STATUS_PIN;

class DttSP : public QThread
{
    Q_OBJECT

	private:
		bool isRx2On;
		int Rx1Bal, Rx2Bal;

		int RitValue;
		bool RitEnable;

		DttSPLibrary *pDsp;
	public:

		float DspCircleBuf[MAX_DSP_MEMORY_BUF][2][4096];
		int 	DspPos;
		int 	DspCurrentPos;
		SDRMODE SdrMode;

		int		GlitchBufTxIn;
		int		GlitchBufTxOut;
		int		GlitchBufRxIn;
		int		GlitchBufRxOut;

		double DspSamleRate;
		double SamleRate();

		int DurationMsec;
		bool IsRunningThreadCWTone;

		DttSP(void *UsrData = 0, QObject *parent = 0);

		void *UserData;
		bool isLoad;
		TRXMODE TrxMode;

		BYTE DashDot;

		void Open();
		void Close();
		void AudioReset();
		void SetMode(SDRMODE Mode);
		SDRMODE GetMode();
		void SetDcBlock(bool Status);
		void SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx);
		void SetRxOutputGain(double Gain);
		void SetRx1Osc(double Freq);
		void SetTxOsc(double Freq);
		void SetSampleRate(double Rate);
		void SetNr(bool Status);
		void SetNrVals(int taps, int delay, double gain, double leak);
		void SetTxCompandSt(int Param);
		void SetTxCompand(double Param);
		void SetTxSquelchSt(bool Status);
		void SetTxSquelchVal(float Val);
		void SetAnf(bool Status);
		void SetANFvals(int taps, int delay, double gain, double leak);
		void SetNB(bool Status);
		void SetNBvals(double Val);
		void SetSdrOm(bool Status);
		void SetSdrOmvals(double Val);
		void SetBin(bool Status);
		void SetRxAgc(AGCMODE Mode);
		void SetRxAgcAttack(int Val);
		void SetRxAgcDecay(int Val);
		void SetRxAgcHang(int Val);
		void SetRxAgcSlope(int Val);
		void SetRxAgcHangThreshold(int Val);
		void SetTxAlcHang(int Val);
		void SetTxLevelerTop(double Val);
		void SetFixedAgc(double Val);
		void SetRxAgcTop(double Val);
		void SetCorrectTXIQPhase(double Phase);
		void SetCorrectTXIQGain(double Gain);
		void SetPwsMode(int Val);
		void SetWindow(Windowtype Wtype);
		void SetSpectrumPolyphase(bool Status);
		void SetTxEQ(int *p);
		void SetGrphTXEQcmd(bool Status);
		void SetGrphRxEQ(int *p);
		void SetGrphRxEQcmd(bool Status);
		void SetTxCarrierLevel(float Val);
		void SetSquelchVal(double Val);
		void SetSquelchState(bool Status);
		void SetTrx(TRXMODE Mode);
		void SetDSPBuflen(int BufLen);
		void SetAudioSize(unsigned int Size);
		void ProcessSpectrum(float *pBuf);
		void ProcessPanadapter(float *pBuf);
		void ProcessPhase(float *pBuf, int Param);
		void ProcessScope(float *pBuf, int Param);
		float CalculateMeters(METERTYPE Meter);
		void AudioCallback(float* pInL, float* pInR, float* pOutL, float* pOutR, unsigned int FrameCount);
		void run();

		void SetPanaramaOn(bool on);
		void SetRx1On(bool on);

		double SineWave(float* Buf, int Samples, double Phase, double Freq);
		double CosineWave(float* Buf, int Samples, double Phase, double Freq);
		void ScaleBuffer(float *InBuf, float *OutBuf, int Samples, float Scale);

		void SineWave2Tone(float* buf, int samples, double phase1, double phase2, double freq1, double freq2, double *updated_phase1, double *updated_phase2);
		void CosineWave2Tone(float* buf, int samples, double phase1, double phase2, double freq1, double freq2, double *updated_phase1, double *updated_phase2);

		bool use_last;
		double Boxmuller(double m,double s);
		void Noise(float* buf, int samples);

		int tri_direction;
		double tri_val;
		void Triangle(float* buf, int samples, double freq);

		double saw_val;
		int saw_direction;
		void Sawtooth(float* buf, int samples, double freq);

		void ClearBuffer(float *buf, int samples);

public slots:
		void OnGlitchCompChanged(QByteArray p);

		void SetRx2On(bool on);
		void SetRx1Volume(int Val);
		void SetRx2Volume(int Val);
		void SetRx1Balance(int Val);
		void SetRx2Balance(int Val);
		void SetRx2Osc(double Freq);

		void SetRitEnable(bool state);
		void SetRitValue(int value);


signals:
		void TrxModeChanged(bool TxMode);
};

#endif /* DTTSP_H_ */
