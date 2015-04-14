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

#ifndef CWKEY_H_
#define CWKEY_H_

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QVector>
#include "CwTimer.h"
#include "qmath.h"
#include "../ringBuf.h"
#include "correctIQ.h"

#define	CW_RING_BUFFER_SIZE		4096*2

#define CW_CHAR_DOT			0
#define CW_CHAR_DASH		1
#define CW_CHAR_PAUSE		2
#define CW_CHAR_LONG_PAUSE	3

typedef enum
{
	CW_READ_KEY,
	CW_PLAY_DOT_WAIT_DASH,
	CW_PLAY_DASH_WAIT_DOT,
	CW_PLAY_DOT_NEXT_PLAY_DASH,
	CW_PLAY_DASH_NEXT_PLAY_DOT,
	CW_WAIT_BREAK_IN,
    CW_PLAY_TONE_START,
	CW_PLAY_TONE,
	CW_PLAY_TONE_WAIT_END,
    CW_PLAY_TONE_END,
	CW_PLAY_STRING_PTT_DELAY,
	CW_PLAY_STRING,
	CW_PLAY_DOT,
	CW_PLAY_DASH,
	CW_PLAY_PAUSE,
	CW_PLAY_LONG_PAUSE,
	CW_PLAY_STRING_DELAY_TX2RX,
}CW_STATE;

class CwCore : public QThread
{
    Q_OBJECT

	public:
		CwCore(QThread *parent = 0);
		~CwCore();

		void setSampleRate(quint32 val);
		void setBufferSize(quint32 val);
		void setSpeed(int Wpm);
		void setFreq(qreal freq);
		void swapKeys(bool swap);
		void start();
		void stop();
		bool isStart();
		void correctGain(qreal gain);
		void correctPhase(qreal phase);
		void transferString(QString str);
		void setPttDelay(quint32 ms);
		void setTxToRxDelay(quint32 ms);
        void reset();
        bool isTxMode();
		void procSamples(float *pL, float *pR, quint32 FrameCount);

	public slots:
		void onDash(bool stat);
		void onDot(bool stat);
		void onKey(bool stat);

		void setRamp(int ms);
		void setWeight(int w);
		void setBreakIn(bool ena);
		void setBreakInDelay(int ms);
		void setAutoMode(bool mode);

	signals:
		void trxChanged(bool);
		void cwSignal(bool);

	private:
		bool isStarted;

		bool autoMode;

		qreal sampleRate;
		qreal weight;
		qreal cwFreq;

		qreal tau;

		bool txMode;
		bool breakInEna;
		quint32 breakInDelayMs;
		quint32 delayCount;

		bool dashStat;
		bool dotStat;
		bool keyStatus;
		bool transferStat;

		QQueue<quint8> queue;
		QQueue<bool> dashQueue;
		QQueue<bool> dotQueue;
		QQueue<bool> keyQueue;

		QVector<float> NewDotI;
		QVector<float> NewDotQ;
		QVector<float> NewDashI;
		QVector<float> NewDashQ;
        QVector<float> NewKeyI;
        QVector<float> NewKeyQ;

        QVector<float> Ex;
		QMutex mut;
		CW_STATE state;
		CwTimer	*pTimer;

		correctIQ *pCorrIQ;
		qreal gainCoef;
		qreal phaseCoef;

		float *pDotI;
		float *pDotQ;
		float *pSilence;
		qint32 dotLen;

		float *pDashI;
		float *pDashQ;
		qint32 dashLen;

        quint32 keyCnt;

		ringBuf	*pBufI;
		ringBuf	*pBufQ;

		quint32 sampleRateIndex;
		quint32 ramp;
		bool swapKey;

		quint32 pttDelay;
		quint32 txToRxDelay;

		void computeCwBuffer();

        double genSin(float *pBuf, quint32 len, double phase, double freq);
        double genCos(float *pBuf, quint32 len, double phase, double freq);
        void keyBuffer(float *pBufferI, float *pBufferQ, quint32 size);
		float cosBuf[96*4];
		float sinBuf[96*4];
		quint32 charToCw(QChar chr, quint8 *pCwBuf);

	private slots:
		void onTimer();
};

#endif /* CWKEY_H_ */
