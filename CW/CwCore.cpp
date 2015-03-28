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

#include "CwCore.h"

#define DSP_PI M_PI

#define KEY_DIRECT_COMPUTE  0

static quint32 count = 0;
float Buffer[10000];

CwCore::CwCore(QThread *parent) : QThread(parent)
{
	pBufI = new ringBuf(CW_RING_BUFFER_SIZE);
	pBufQ = new ringBuf(CW_RING_BUFFER_SIZE);

	pTimer = new CwTimer;
	pCorrIQ = new correctIQ;
	sampleRate = 48000.0;
	weight = 0.5;
	cwFreq = 700.0;
	tau = 60;

	pDotI = new float[4200000];
	pDotQ = new float[4200000];
	pDashI = new float[4200000];
	pDashQ = new float[4200000];
	pSilence = new float[4200000];

	sampleRateIndex = 1;
	ramp = 5;
	swapKey = false;
	dotLen = 25;
	connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimer()), Qt::DirectConnection);

	dashStat = false;
	dotStat = false;
	keyStatus = false;
	transferStat = false;
	state = CW_READ_KEY;
	breakInDelayMs = 1000;

	breakInEna = false;
	txMode = false;
	delayCount = 0;

	pttDelay = 1000;
	txToRxDelay = 2000;

	gainCoef = 1.0;
	phaseCoef = 0.0;

	isStarted = false;
	transferStat = false;
	autoMode = false;

	keyCnt = 0;

	computeCwBuffer();
}

CwCore::~CwCore()
{
	if(pTimer->isStart())
	{
		pTimer->stopTimer();
	}
	if(isRunning())
	{
		terminate();
		wait();
	}
	delete pTimer;
	delete pCorrIQ;
	delete pDotI;
	delete pDotQ;
	delete pDashI;
	delete pDashQ;
	delete pBufI;
	delete pBufQ;
}

void CwCore::setBreakIn(bool ena)
{
	QMutexLocker locker(&mut);
	breakInEna = ena;
}

void CwCore::setBreakInDelay(int ms)
{
	QMutexLocker locker(&mut);
	if((ms < 10) || (ms > 5000)) ms = 10;
	breakInDelayMs = ms;
}

void CwCore::setAutoMode(bool mode)
{
	autoMode = mode;
}

void CwCore::setSampleRate(quint32 val)
{
	mut.lock();
	if((val < 48000) || (val > 192000))
	{
		val = 48000;
	}
	sampleRate = val;
	sampleRateIndex = (val / 48000);
	mut.unlock();
	computeCwBuffer();
}

void CwCore::setBufferSize(quint32 val)
{
	QMutexLocker locker(&mut);
	val /= 256;
	val *= 256;
	delete pBufI;
	delete pBufQ;
	pBufI = new ringBuf(val*2);
	pBufQ = new ringBuf(val*2);
	val *= 2;
}

void CwCore::setSpeed(int Wpm)
{
	if((Wpm < 1) || (Wpm > 100))
		Wpm = 20;
	tau = 1000.0/(27.5*Wpm/60.0);
	computeCwBuffer();
}

void CwCore::setFreq(qreal freq)
{
	mut.lock();
	if(freq < 50.0) freq = 50.0;
	if(freq > 2500.0) freq = 2500.0;
	cwFreq = freq;
	mut.unlock();
	computeCwBuffer();
}

void CwCore::setRamp(int ms)
{
	if((ms < 0) || (ms > 50)) ms = 5;
	ramp = ms;
	computeCwBuffer();
}

void CwCore::setWeight(int wgh)
{
	qreal w = wgh/100.0;
	if((w > 1.0) && (w < 0))
		weight = 0.5;
	else
		weight = w;
	computeCwBuffer();
}

void CwCore::swapKeys(bool swap)
{
	QMutexLocker locker(&mut);
	bool t;

	if(swapKey != swap)
	{
		t = dashStat;
		dashStat = dotStat;
		dotStat = t;
	}
	swapKey = swap;
}

void CwCore::start()
{
	if(!isStarted)
	{
		QMutexLocker locker(&mut);
		queue.clear();
		count = 0;
		pBufI->clear();
		pBufQ->clear();
		pTimer->startTimer(2);
		isStarted = true;
		dashStat = false;
		dotStat = false;
		dashQueue.clear();
		dotQueue.clear();
		keyQueue.clear();
	}
}

void CwCore::stop()
{
	if(isStarted)
	{
		QMutexLocker locker(&mut);
		pTimer->stopTimer();
		if(breakInEna)
		{
			if(txMode)
			{
				txMode = false;
				emit trxChanged(txMode);
			}
		}
		isStarted = false;
	}
}

bool CwCore::isStart()
{
	return isStarted;
}

void CwCore::correctGain(qreal gain)
{
	gainCoef = (gain*0.001) + 1.0;
	pCorrIQ->setCorrect(phaseCoef, gainCoef);
	computeCwBuffer();
}

void CwCore::correctPhase(qreal phase)
{
	phaseCoef = phase*0.001;
	pCorrIQ->setCorrect(phaseCoef, gainCoef);
	computeCwBuffer();
}

void CwCore::transferString(QString str)
{
	QMutexLocker locker(&mut);
	quint32 len;
	quint8 buf[10];

	if((state == CW_READ_KEY)||(state == CW_PLAY_STRING_PTT_DELAY)||(state == CW_PLAY_STRING)||
	   (state == CW_PLAY_DOT)||(state == CW_PLAY_DASH)||(state == CW_PLAY_PAUSE)||(state == CW_PLAY_LONG_PAUSE))
	{
		for(int i = 0; i < str.size(); i++)
		{
			len = charToCw(str[i], buf);
			for(quint32 j = 0; j < len; j++)
				queue.enqueue(buf[j]);
		}
		transferStat = true;
		if(isStarted)
		if(!txMode)
		{
			txMode = true;
			emit trxChanged(txMode);
		}
	}
}

void CwCore::setPttDelay(quint32 ms)
{
	QMutexLocker locker(&mut);
	pttDelay = ms;
}

void CwCore::setTxToRxDelay(quint32 ms)
{
	QMutexLocker locker(&mut);
	txToRxDelay = ms;
}

void CwCore::reset()
{
	QMutexLocker locker(&mut);
	pBufI->clear();
	pBufQ->clear();
	state = CW_READ_KEY;

	if(dashQueue.size() > 0) dashStat  = dashQueue.last();
	if(dotQueue.size() > 0)   dotStat  = dotQueue.last();
	if(keyQueue.size() > 0) keyStatus = keyQueue.last();

	dashQueue.clear();
	dotQueue.clear();
	keyQueue.clear();
}

bool CwCore::isTxMode()
{
	QMutexLocker locker(&mut);
	return txMode;
}

void CwCore::procSamples(float *pL, float *pR, quint32 FrameCount)
{
	QMutexLocker locker(&mut);
	quint32 size;
	if(pBufI->readCount() == 0)
	{
		memset(pL, 0, FrameCount * sizeof(float));
		memset(pR, 0, FrameCount * sizeof(float));
	}
	else
	{
		size = pBufI->readCount();
		if(size >= FrameCount)
		{
			pBufI->read(pL, FrameCount);
			pBufQ->read(pR, FrameCount);
		}
		else
		{
			memset(pL, 0, FrameCount * sizeof(float));
			memset(pR, 0, FrameCount * sizeof(float));
		}
	}
}

void CwCore::onTimer()
{
	QMutexLocker locker(&mut);
	quint32 dotBlocks = dotLen / 96 / sampleRateIndex;
	quint32 dashBlocks = dashLen / 96 / sampleRateIndex;
	quint32 pause = (int)(dotBlocks*3*weight);
	quint32 longPause = (int)(dotBlocks*4*weight);
	static quint32 expCount = 0;
	static quint32 expSize = 0;
	quint8 code;

	if(pBufI->writeCount() < ((int)(96*sampleRateIndex)))
		return;

	if(dashStat == false)
	{
		if(dashQueue.size() > 0)
		{
			while(dashQueue.size() > 0) dashStat  = dashQueue.dequeue();
			if(dashStat == false)
			{
				dashStat = true;
				dashQueue.enqueue(false);
			}
		}
	}
	else
	{
		while(dashQueue.size() > 0) dashStat  = dashQueue.dequeue();
	}
	if(dotStat == false)
	{
		if(dotQueue.size() > 0)
		{
			while(dotQueue.size() > 0) dotStat  = dotQueue.dequeue();
			if(dotStat == false)
			{
				dotStat = true;
				dotQueue.enqueue(false);
			}
		}
	}
	else
	{
		while(dotQueue.size() > 0) dotStat  = dotQueue.dequeue();
	}
	if(keyStatus == false)
	{
		if(keyQueue.size() > 0)
		{
			while(keyQueue.size() > 0) keyStatus  = keyQueue.dequeue();
			if(keyStatus == false)
			{
				keyStatus = true;
				keyQueue.enqueue(false);
			}
		}
	}
	else
	{
		while(keyQueue.size() > 0) keyStatus  = keyQueue.dequeue();
	}
	switch(state)
	{
		case CW_READ_KEY:
			if(dashStat)
			{
				count = 0;
				state = CW_PLAY_DASH_WAIT_DOT;
				goto dash;
			}
			if(dotStat)
			{
				count = 0;
				state  = CW_PLAY_DOT_WAIT_DASH;
				goto dot;
			}
			if(keyStatus)
			{
				count = 0;
				state  = CW_PLAY_TONE_START;
				goto key;
			}
			if(transferStat)
			{
				count = 0;
				state  = CW_PLAY_STRING_PTT_DELAY;
				goto trf;
			}
			if(breakInEna)
			{
				if(txMode)
				if(++count > breakInDelayMs)
				{
					txMode = false;
					emit trxChanged(txMode);
					count = 0;
				}
			}
			pBufI->write(pSilence, 96*sampleRateIndex);
			pBufQ->write(pSilence, 96*sampleRateIndex);
		break;
		case CW_PLAY_DOT_WAIT_DASH:
dot:		if(dashStat)
			{
				state = CW_PLAY_DOT_NEXT_PLAY_DASH;
			}
			pBufI->write(pDotI+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			pBufQ->write(pDotQ+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			if(++count >= dotBlocks)
			{
				count = 0;
				if(state != CW_PLAY_DOT_NEXT_PLAY_DASH)
				{
					state = CW_READ_KEY;
				}
			}
			delayCount = 0;
		break;
		case CW_PLAY_DASH_WAIT_DOT:
dash:		if(dotStat)
			{
				state = CW_PLAY_DASH_NEXT_PLAY_DOT; //
			}
			pBufI->write(pDashI+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			pBufQ->write(pDashQ+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			if(++count >= dashBlocks)
			{
				count = 0;
				if(state != CW_PLAY_DASH_NEXT_PLAY_DOT)
				{
					state = CW_READ_KEY;
				}
			}
			delayCount = 0;
		break;
		case CW_PLAY_DOT_NEXT_PLAY_DASH:
			pBufI->write(pDotI+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			pBufQ->write(pDotQ+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			if(++count >= dotBlocks)
			{
				count = 0;
				state = CW_PLAY_DASH_WAIT_DOT;
			}
			delayCount = 0;
		break;
		case CW_PLAY_DASH_NEXT_PLAY_DOT:
			pBufI->write(pDashI+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			pBufQ->write(pDashQ+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			if(++count >= dashBlocks)
			{
				count = 0;
				state = CW_PLAY_DOT_WAIT_DASH;
			}
			delayCount = 0;
		break;
		case CW_PLAY_TONE_START:
key:        expCount = 0;
			expSize = Ex.size();
			state = CW_PLAY_TONE;
			delayCount = 0;
		case CW_PLAY_TONE:
			keyBuffer(cosBuf, sinBuf, 96*sampleRateIndex);
			if(expCount < expSize)
			{
				for(quint32 i = 0; i < 96*sampleRateIndex; i++)
				{
					cosBuf[i] *= Ex.data()[expCount];
					sinBuf[i] *= Ex.data()[expCount];
					if((++expCount) >= expSize)
						break;
				}
			}
			pCorrIQ->doCorrect(cosBuf, sinBuf, 96*sampleRateIndex);
			pBufI->write(cosBuf, 96*sampleRateIndex);
			pBufQ->write(sinBuf, 96*sampleRateIndex);
			if(++count > 5)
			{
				count = 0;
				state = CW_PLAY_TONE_WAIT_END;
			}
			delayCount = 0;
		break;
		case CW_PLAY_TONE_WAIT_END:
			keyBuffer(cosBuf, sinBuf, 96*sampleRateIndex);
			if(expCount < expSize)
			{
			   for(quint32 i = 0; i < 96*sampleRateIndex; i++)
			   {
				   cosBuf[i] *= Ex.data()[expCount];
				   sinBuf[i] *= Ex.data()[expCount];
				   if((++expCount) >= expSize) break;
			   }
			}
			pCorrIQ->doCorrect(cosBuf, sinBuf, 96*sampleRateIndex);
			pBufI->write(cosBuf, 96*sampleRateIndex);
			pBufQ->write(sinBuf, 96*sampleRateIndex);
			if(!keyStatus)
			{
				state = CW_PLAY_TONE_END;
				expCount = expSize - 1;
			}
			delayCount = 0;
		break;
		case CW_PLAY_TONE_END:
			keyBuffer(cosBuf, sinBuf, 96*sampleRateIndex);
			if(expCount > 0)
			{
			   for(quint32 i = 0; i < 96*sampleRateIndex; i++)
			   {
				   if(expCount > 0)
				   {
					   cosBuf[i] *= Ex.data()[expCount-1];
					   sinBuf[i] *= Ex.data()[expCount-1];
					   expCount--;
				   }
				   else
				   {
					   cosBuf[i] = 0.0;
					   sinBuf[i] = 0.0;
				   }
			   }
			   if(expCount <= 0) state = CW_READ_KEY;
			}
			pCorrIQ->doCorrect(cosBuf, sinBuf, 96*sampleRateIndex);
			pBufI->write(cosBuf, 96*sampleRateIndex);
			pBufQ->write(sinBuf, 96*sampleRateIndex);
			delayCount = 0;
		break;
		case CW_PLAY_STRING_PTT_DELAY:
trf:		if(pttDelay == 0)
			{
				state = CW_PLAY_STRING;
				goto trf2;
			}
			else
			{
				if(++count > (pttDelay/2))
				{
					count = 0;
					state = CW_PLAY_STRING;
				}
			}
			delayCount = 0;
		break;
		case CW_PLAY_STRING:
trf2:		if(queue.size() > 0)
			{
				code = queue.dequeue();
				count = 0;
				if(code == CW_CHAR_DOT)
				{
					state = CW_PLAY_DOT;
					goto pdot;
				}
				if(code == CW_CHAR_DASH)
				{
					state = CW_PLAY_DASH;
					goto pdash;
				}
				if(code == CW_CHAR_PAUSE)
				{
					state = CW_PLAY_PAUSE;
					goto pp;
				}
				if(code == CW_CHAR_LONG_PAUSE)
				{
					state = CW_PLAY_LONG_PAUSE;
					goto plp;
				}
			}
			else
			{
				transferStat = false;
				state = CW_PLAY_STRING_DELAY_TX2RX;
			}
			delayCount = 0;
		break;
		case CW_PLAY_DOT:
pdot:		pBufI->write(pDotI+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			pBufQ->write(pDotQ+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			if(count == 0) emit cwSignal(true);
			if(count == ((quint32)(dotBlocks*weight)))
				emit cwSignal(false);
			if(++count >= dotBlocks)
			{
				count = 0;
				state = CW_PLAY_STRING;
			}
			delayCount = 0;
		break;
		case CW_PLAY_DASH:
pdash:		pBufI->write(pDashI+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			pBufQ->write(pDashQ+(96*sampleRateIndex)*count, 96*sampleRateIndex);
			if(count == 0) emit cwSignal(true);
			if(count == ((quint32)(dashBlocks*3*weight/2)))
				emit cwSignal(false);
			if(++count >= dashBlocks)
			{
				count = 0;
				state = CW_PLAY_STRING;
			}
			delayCount = 0;
		break;
		case CW_PLAY_PAUSE:
pp:			pBufI->write(pSilence, 96*sampleRateIndex);
			pBufQ->write(pSilence, 96*sampleRateIndex);
			if(++count >= pause)
			{
				count = 0;
				state = CW_PLAY_STRING;
			}
			delayCount = 0;
		break;
		case CW_PLAY_LONG_PAUSE:
plp:		pBufI->write(pSilence, 96*sampleRateIndex);
			pBufQ->write(pSilence, 96*sampleRateIndex);
			if(++count >= longPause)
			{
				count = 0;
				state = CW_PLAY_STRING;
			}
			delayCount = 0;
		break;
		case CW_PLAY_STRING_DELAY_TX2RX:
			if(txToRxDelay == 0)
			{
				count = 0;
				state = CW_READ_KEY;
				if(txMode)
				{
					delayCount = 0;
					txMode = false;
					emit trxChanged(txMode);
				}
			}
			else
			{
				if(++count > (txToRxDelay/2))
				{
					count = 0;
					state = CW_READ_KEY;
					if(txMode)
					{
						delayCount = 0;
						txMode = false;
						emit trxChanged(txMode);
					}
				}
			}
			delayCount = 0;
		break;
		default:
			state = CW_READ_KEY;
		break;
	}
}

void CwCore::onDash(bool stat)
{
	QMutexLocker locker(&mut);
	if(!isStarted)
		return;

	if(autoMode)
		(swapKey) ? dotQueue.enqueue(stat) : dashQueue.enqueue(stat);
	else
		keyQueue.enqueue(stat);
	if(breakInEna)
	{
		if(isStarted)
		if(!txMode)
		{
			txMode = true;
			emit trxChanged(txMode);
		}
	}
}

void CwCore::onDot(bool stat)
{
	QMutexLocker locker(&mut);

	if(!isStarted) return;

	if(autoMode)
		(swapKey) ? dashQueue.enqueue(stat) : dotQueue.enqueue(stat);
	else
		keyQueue.enqueue(stat);

	if(breakInEna)
	{
		if(isStarted)
		if(!txMode)
		{
			txMode = true;
			emit trxChanged(txMode);
		}
	}
}

void CwCore::onKey(bool stat)
{
	QMutexLocker locker(&mut);

	if(!isStarted)
		return;

	keyQueue.enqueue(stat);
}

void CwCore::computeCwBuffer()
{
	quint32 T;
	quint32 tmp = 0;
	qreal cwFreqKey = 0.0;

	Ex.clear();
	Ex.resize(96 * sampleRateIndex * ramp / 2);

	int newDotLen = 96 * sampleRateIndex * tau / 2.0;
	int newDashLen = newDotLen*2;

	NewDotI.resize(newDotLen);
	NewDotQ.resize(newDotLen);
	NewDashI.resize(newDashLen);
	NewDashQ.resize(newDashLen);

	int expLen = 96 * sampleRateIndex * ramp / 2;
	for(int i = 0; i < Ex.size(); i++)
	{
		Ex.data()[i] = 1 - qExp(-(i*5.0/ expLen));
	}

	T = ((float)(newDotLen)) * weight;
	genCos(NewDotI.data(), T, 0, cwFreq/48000.0/sampleRateIndex);
	genSin(NewDotQ.data(), T, 0, cwFreq/48000.0/sampleRateIndex);

	((T/2.0) > Ex.size()) ? tmp = Ex.size() : tmp = T/2;
	for(quint32 i = 0; i < tmp; i++)
	{
		NewDotI.data()[i] *= Ex.at(i);
		NewDotQ.data()[i] *= Ex.at(i);
		NewDotI.data()[T - 1 - i] *= Ex.at(i);
		NewDotQ.data()[T - 1 - i] *= Ex.at(i);
	}

	for(int i = T; i < newDotLen; i++)
	{
		NewDotI.data()[i] = 0.0;
		NewDotQ.data()[i] = 0.0;
	}

	T *= 3;
	genCos(NewDashI.data(), T, 0, cwFreq/48000.0/sampleRateIndex);
	genSin(NewDashQ.data(), T, 0, cwFreq/48000.0/sampleRateIndex);
	((T/2.0) > Ex.size()) ? tmp = Ex.size() : tmp = T/2;
	for(quint32 i = 0; i < tmp; i++)
	{
		NewDashI.data()[i] *= Ex.at(i);
		NewDashQ.data()[i] *= Ex.at(i);
		NewDashI.data()[T - 1 - i] *= Ex.at(i);
		NewDashQ.data()[T - 1 - i] *= Ex.at(i);
	}

	for(int i = T; i < newDashLen; i++)
	{
		NewDashI.data()[i] = 0.0;
		NewDashQ.data()[i] = 0.0;
	}

	pCorrIQ->doCorrect(NewDashI.data(), NewDashQ.data(), newDashLen);
	pCorrIQ->doCorrect(NewDotI.data(), NewDotQ.data(), newDotLen);
	tmp = (48000.0*sampleRateIndex / cwFreq);
	cwFreqKey = 48000.0*sampleRateIndex / tmp;
	mut.lock();
	dotLen = newDotLen;
	dashLen = newDashLen;
	for(int i = 0; i < newDotLen; i++)
	{
		pDotI[i] = NewDotI.data()[i];
		pDotQ[i] = NewDotQ.data()[i];
		pDashI[i] = NewDashI.data()[i];
		pDashQ[i] = NewDashQ.data()[i];
	}
	for(int i = newDotLen; i < newDashLen; i ++)
	{
		pDashI[i] = NewDashI.data()[i];
		pDashQ[i] = NewDashQ.data()[i];
	}
	NewKeyI.resize(tmp);
	NewKeyQ.resize(tmp);
	genCos(NewKeyI.data(), tmp, 0, 1.0/tmp);
	genSin(NewKeyQ.data(), tmp, 0, 1.0/tmp);
	mut.unlock();
}

void CwCore::keyBuffer(float *pBufferI, float *pBufferQ, quint32 size)
{
#if(KEY_DIRECT_COMPUTE==0)
	quint32 cnt = (quint32)NewKeyQ.size();
	if(keyCnt >= cnt)
		keyCnt = 0;
	for(quint32 i = 0; i < size; i++)
	{
		pBufferI[i] = NewKeyI.data()[keyCnt];
		Buffer[i] = pBufferI[i];
		pBufferQ[i] = NewKeyQ.data()[keyCnt];
		keyCnt++;
		if(keyCnt >= cnt)
			keyCnt = 0;
	}
#else
	static float phase = 0.0;
	quint32 tmp = (48000.0*sampleRateIndex / cwFreq);
	phase = genCos(pBufferI, size, phase, 1.0/tmp);
	genSin(pBufferQ, size, phase, 1.0/tmp);
#endif
}

double CwCore::genSin(float *pBuf, quint32 size, double phase, double freq)
{
	double phase_step = freq*2*DSP_PI;
	double cosval = cos(phase);
	double sinval = sin(phase);
	double cosdelta = cos(phase_step);
	double sindelta = sin(phase_step);
	double tmpval;

	for(quint32 i = 0; i < size; i++ )
	{
		tmpval = cosval*cosdelta - sinval*sindelta;
		sinval = cosval*sindelta + sinval*cosdelta;
		cosval = tmpval;

		pBuf[i] = (float)(sinval);
		phase += phase_step;
	}
	return phase;
}

double CwCore::genCos(float *pBuf, quint32 size, double phase, double freq)
{
	double phase_step = freq*2*DSP_PI;
	double cosval = cos(phase);
	double sinval = sin(phase);
	double cosdelta = cos(phase_step);
	double sindelta = sin(phase_step);
	double tmpval;

	for(quint32 i = 0; i < size; i++ )
	{
		tmpval = cosval*cosdelta - sinval*sindelta;
		sinval = cosval*sindelta + sinval*cosdelta;
		cosval = tmpval;
		pBuf[i] = (float)(cosval);
		phase += phase_step;
	}
	return phase;
}

quint32 CwCore::charToCw(QChar chr, quint8 *pCwBuf)
{
	quint32 retVal = 0;

	if(chr == 'A' || chr == 'a')		// .-
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_PAUSE, retVal = 3;
	else if(chr == 'B' || chr == 'b')	// -...
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'C' || chr == 'c')	// -.-.
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'D' || chr == 'd')	// -..
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'E' || chr == 'e')	// .
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_PAUSE, retVal = 2;
	else if(chr == 'F' || chr == 'f') 	// ..-.
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'G' || chr == 'g')	// --.
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'H' || chr == 'h')	// ....
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'I' || chr == 'i')	// ..
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_PAUSE, retVal = 3;
	else if(chr == 'J' || chr == 'j')	// .---
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'K' || chr == 'k')	// -.-
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'L' || chr == 'l')	// .-..
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'M' || chr == 'm')	// --
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_PAUSE, retVal = 3;
	else if(chr == 'N' || chr == 'n')	// -.
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_PAUSE, retVal = 3;
	else if(chr == 'O' || chr == 'o')	// ---
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'P' || chr == 'p')	// .--.
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'Q' || chr == 'q')	// --.-
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'R' || chr == 'r')	// .-.
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'S' || chr == 's')	// ...
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'T' || chr == 't')	// -
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_PAUSE, retVal = 2;
	else if(chr == 'U' || chr == 'u')	// ..-
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'V' || chr == 'v')	// ...-
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'W' || chr == 'w')	// .--
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_PAUSE, retVal = 4;
	else if(chr == 'X' || chr == 'x')	// -..-
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'Y' || chr == 'y')	// -.--
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == 'Z' || chr == 'z')	// --..
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_PAUSE, retVal = 5;
	else if(chr == ' ')					// пауза, равная четырем точкам
		pCwBuf[0] = CW_CHAR_LONG_PAUSE, retVal = 1;
	// цифры
	else if(chr == '0')	// -----
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '1')	// .----
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '2')	// ..---
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '3')	// ...--
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '4')	// ....-
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '5')	// .....
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '6')	// -....
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '7')	// --...
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '8')	// ---..
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '9')	// ----.
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	// знаки препинания
	else if(chr == '?')	// ..--..
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_DOT, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else if(chr == '/')	// -..-.
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_PAUSE, retVal = 6;
	else if(chr == '!')	// --..--
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_DASH, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else if(chr == ',')	// .-.-.-
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DASH, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_DASH, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else if(chr == ':')	// ---...
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DOT, pCwBuf[5] = CW_CHAR_DOT, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else if(chr == ';')	// -.-.-.
		pCwBuf[0] = CW_CHAR_DASH, pCwBuf[1] = CW_CHAR_DOT, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_DOT, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else if(chr == '@')	// .--.-.
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DASH, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_DOT, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else if(chr == '"')	// .-..-.
		pCwBuf[0] = CW_CHAR_DOT, pCwBuf[1] = CW_CHAR_DASH, pCwBuf[2] = CW_CHAR_DOT, pCwBuf[3] = CW_CHAR_DOT, pCwBuf[4] = CW_CHAR_DASH, pCwBuf[5] = CW_CHAR_DOT, pCwBuf[6] = CW_CHAR_PAUSE, retVal = 7;
	else 				// пауза, равная четырем точкам
		pCwBuf[0] = CW_CHAR_LONG_PAUSE, retVal = 1;

	return retVal;
}

