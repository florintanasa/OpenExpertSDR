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
#include "AudioThread.h"

AudioThread::AudioThread(void *UsrData, QObject *parent): QThread(parent)
{
	pInR  = new ringBuf(4096*2);
	pInL  = new ringBuf(4096*2);
	pOutR = new ringBuf(4096*2);
	pOutL = new ringBuf(4096*2);
	userData = UsrData;
	pDsp = (DttSP *)userData;
    pExpSdr = (ExpertSDR_vA2_1*)pDsp->UserData;
	start(QThread::TimeCriticalPriority);
	bufSize = 4096;
}

AudioThread::~AudioThread()
{
	terminate();
	wait();

	delete pInR;
	delete pInL;
	delete pOutR;
	delete pOutL;
}

void AudioThread::setFrameSize(int size)
{
	float zeroBuf[size];
	bufSize = size;
	pInL->clear();
	pInR->clear();
	pOutL->clear();
	pOutR->clear();

	for(int i = 0; i < size; i++)
		zeroBuf[0] = 0.0;

	pOutL->write(zeroBuf, size);
	pOutR->write(zeroBuf, size);
}

void AudioThread::process(void *pIn, void *pOut, int size)
{
	int* ArrayPtr = (int *)pOut;
	float* pOutRight;
    float* pOutLeft;
	pOutLeft = (float *)ArrayPtr[0];
	pOutRight = (float *)ArrayPtr[1];

    ArrayPtr = (int *)pIn;
	float* pInLeft;
	float* pInRight;
	pInLeft = (float *)ArrayPtr[0];
	pInRight = (float *)ArrayPtr[1];

	pInL->write(pInLeft, size);
	pInR->write(pInRight, size);
	pOutL->read(pOutLeft, size);
	pOutR->read(pOutRight, size);
	sem.release(1);
}

void AudioThread::run()
{
	float inL[4096];
	float inR[4096];
	float outL[4096];
	float outR[4096];
    int DspCurrentPos;
    int BufPos;
    static int cntTxDel = 0;

    forever
	{
		sem.acquire(1);
        pInL->read(inL, bufSize);
        pInR->read(inR, bufSize);
        DspCurrentPos = pDsp->DspCurrentPos;
        BufPos = GetCurrentPos(pDsp->DspPos, DspCurrentPos);
        memcpy(pDsp->DspCircleBuf[pDsp->DspPos][0], inL, bufSize*sizeof(float));
        memcpy(pDsp->DspCircleBuf[pDsp->DspPos][1], inR, bufSize*sizeof(float));
     	pDsp->DspPos++;
     	if(pDsp->DspPos >= MAX_DSP_MEMORY_BUF)
     		pDsp->DspPos = 0;

     	if(pExpSdr->CalibrateSoundCard)
     	{
     		pDsp->SineWave(inL, bufSize, pExpSdr->PhaseAccumulator, 50);
     		pExpSdr->PhaseAccumulator = pDsp->CosineWave(inR, bufSize, pExpSdr->PhaseAccumulator, 50);
     		pDsp->AudioCallback(inL, inR, outL, outR, bufSize);
     		pDsp->ScaleBuffer(inL , outL , bufSize, 1.0);
     		pDsp->ScaleBuffer(inR, outR, bufSize, 1.0);
     	}
     	else
     	{
     		if(pDsp->TrxMode == TX)
     		{
     			if((pDsp->SdrMode == CWL) && !pExpSdr->StateTone)
                    pExpSdr->pCwMacro->pCwCore->procSamples(outL, outR, bufSize);
  //              int fake5=0;
                    else if((pDsp->SdrMode == CWU) && !pExpSdr->StateTone)
                    pExpSdr->pCwMacro->pCwCore->procSamples(outR, outL, bufSize);
  //              int fake6=0;
                    else
     			{
     				switch(pExpSdr->TxInputSignal)
     				{
     					case RADIO:
     						pDsp->ScaleBuffer(inL , inL , bufSize, pExpSdr->MicScale * pExpSdr->MicGainScale);
     						pDsp->ScaleBuffer(inR, inR, bufSize, pExpSdr->MicScale * pExpSdr->MicGainScale);
     					break;

     					case SINE:
     						pDsp->SineWave(inL, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
     						pExpSdr->PhaseAccumulator = pDsp->CosineWave(inR, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
     					break;
     					case SINE_TWO_TONE:
     						double dump;
     						pDsp->SineWave2Tone(inL, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &dump, &dump);
     						pDsp->CosineWave2Tone(inR, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &pExpSdr->PhaseAccumulator, &pExpSdr->PhaseAccumulator2);
     					break;
     					case NOISE:
     						pDsp->Noise(inL, bufSize);
     						pDsp->Noise(inR, bufSize);
     					break;
     					case TRIANGLE:
     						pDsp->Triangle(inL, bufSize, pExpSdr->RxFreqSignal);
     						memcpy(inR, inL, bufSize*sizeof(float));
     					break;
     					case SAWTOOTH:
     						pDsp->Sawtooth(inL, bufSize, pExpSdr->RxFreqSignal);
     						memcpy(inR, inL, bufSize*sizeof(float));
     					break;
     					case SILENCE:
     						pDsp->ClearBuffer(inL, bufSize);
     						pDsp->ClearBuffer(inR, bufSize);
     					break;

     					default:
     					break;
     				}
     				pExpSdr->pVac->readBuf(inL, inR, bufSize);
     				if(cntTxDel > pExpSdr->glitchCoeff)
      					pDsp->AudioCallback(inL, inR, outL, outR, bufSize);
     				else if((cntTxDel > (pExpSdr->glitchCoeff/4))||(cntTxDel <= pExpSdr->glitchCoeff))
     				{
     					pDsp->AudioCallback(inL, inR, outL, outR, bufSize);
     					memset(outL, 0, bufSize*sizeof(float));
     					memset(outR, 0, bufSize*sizeof(float));
     					cntTxDel++;
     				}
     				else
     				{
     					memset(outL, 0, bufSize*sizeof(float));
     					memset(outR, 0, bufSize*sizeof(float));
     					cntTxDel++;
     				}
     				memset(inL,0,sizeof(float)*bufSize);
     				memset(inR,0,sizeof(float)*bufSize);
					 pDsp->ScaleBuffer(outL , outL , bufSize, 80.0);
					 pDsp->ScaleBuffer(outR, outR, bufSize, 80.0);
     				if(pExpSdr->DspMute)
     				{
     					memset(outR,0,sizeof(float)*bufSize);
     					memset(outL,0,sizeof(float)*bufSize);
     				}
     			}
     			pExpSdr->pVac->writeBuf(outL, outR, bufSize);
     			switch((BAND_MODE)pExpSdr->BandModeChecked)
     			{
     			case BAND160M:
     				pDsp->ScaleBuffer(outL, outL, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val160);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val160);
     				break;
     			case BAND80M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val80);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val80);
     				break;
     			case BAND60M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val60);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val60);
     				break;
     			case BAND40M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val40);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val40);
     				break;
     			case BAND30M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val30);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val30);
     				break;
     			case BAND20M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val20);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val20);
     				break;
     			case BAND17M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val17);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val17);
     				break;
     			case BAND15M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val15);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val15);
     				break;
     			case BAND12M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val12);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val12);
     				break;
     			case BAND10M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val10);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val10);
     				break;
     			case BAND6M:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val6);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val6);
     				break;
     			default:
     				pDsp->ScaleBuffer(outL , outL , bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard);
     				break;
     			}
     			pExpSdr->rxGlCnt = 0;
     		}
     		else
     		{
     			switch(pExpSdr->RxInputSignal)
     			{
     				case RADIO:
     					pDsp->ScaleBuffer(inL , inL , bufSize, 0);
     					pDsp->ScaleBuffer(inR, inR, bufSize, 0);
     				break;
     				case SINE:
     					pDsp->SineWave(inL, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
     					pExpSdr->PhaseAccumulator = pDsp->CosineWave(inR, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
     				break;
     				case SINE_TWO_TONE:
     					double dump;
     					pDsp->SineWave2Tone(inL, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &dump, &dump);
     					pDsp->CosineWave2Tone(inR, bufSize, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &pExpSdr->PhaseAccumulator, &pExpSdr->PhaseAccumulator2);
     				break;
     				case NOISE:
     					pDsp->Noise(inL, bufSize);
     					pDsp->Noise(inR, bufSize);
     				break;
     				case TRIANGLE:
     					pDsp->Triangle(inL, bufSize, pExpSdr->RxFreqSignal);
     					memcpy(inR, inL, bufSize*sizeof(float));
     				break;
     				case SAWTOOTH:
     					pDsp->Sawtooth(inL, bufSize, pExpSdr->RxFreqSignal);
     					memcpy(inR, inL, bufSize*sizeof(float));
     				break;
     				case SILENCE:
     					pDsp->ClearBuffer(inL, bufSize);
     					pDsp->ClearBuffer(inR, bufSize);
     				break;
     				default:
     				break;
     			}
     			if(pExpSdr->rxGlCnt > pExpSdr->glitchCoeff)
     				pDsp->AudioCallback(pDsp->DspCircleBuf[BufPos][0], pDsp->DspCircleBuf[BufPos][1], outL, outR, bufSize);
     			else if((pExpSdr->rxGlCnt > (pExpSdr->glitchCoeff/2))||(pExpSdr->rxGlCnt <= pExpSdr->glitchCoeff))
     			{
     				pDsp->AudioCallback(pDsp->DspCircleBuf[BufPos][0], pDsp->DspCircleBuf[BufPos][1], outL, outR, bufSize);
     				memset(outL, 0, bufSize*sizeof(float));
     				memset(outR, 0, bufSize*sizeof(float));
     				pExpSdr->rxGlCnt++;
     			}
     			else
     			{
     				memset(outL, 0, bufSize*sizeof(float));
     				memset(outR, 0, bufSize*sizeof(float));
     				pExpSdr->rxGlCnt++;
     			}

     			if(!pExpSdr->pVac->writeBuf(outL, outR, bufSize));
     			{
                    pDsp->ScaleBuffer(outL, outL, bufSize, 10.0);
     				pDsp->ScaleBuffer(outR, outR, bufSize, pExpSdr->Volume);
     			}
     			pExpSdr->pVac->readBuf(inL, inR, bufSize);
     			cntTxDel = 0;
     		}
         }
		pOutL->write(outL, bufSize);
		pOutR->write(outR, bufSize);
	}
}
