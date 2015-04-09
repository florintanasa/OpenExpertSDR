#include "expertsdr_va2_1.h"
#include "ui_expertsdr_va2_1.h"

#include <QList>
//#include <dbt.h>
#include <search.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QtCore>
#include <QtGlobal>
#include <QFontDatabase>

int crBand = 0;
int crMode = 0;

static bool IsReadyRx = true;
static bool IsReadyTx = true;
QMutex TxRxTimeDelayMutix;
QMutex RxTxTimeDelayMutix;
QString pathTmp(QDir::homePath());

int compare(const void * a, const void * b)
{
  return ( *(float*)a - *(float*)b);
}

double DbToVal(double Db)
{
    return (qPow(10.0,0.1*(48-Db)));
}
double DbToValMic(double Db)
{
    return (qPow(10.0,0.05*(Db)));
}

float DecreaseExp(float Ti)
{
    return (1.0 - qExp(Ti));
}

template< typename T >
T GetMax(T array[], int size)
{
  T t = array[0];
  for(int i = 1; i < size; i++)
  {
      if(t < array[i])
          t = array[i];
  }
  return t;
}

void TimeWaitInInstatnceThread(int mSec)
{
//    Sleep(mSec);
    TxRxTimeDelayMutix.lock();
    IsReadyRx = true;
    TxRxTimeDelayMutix.unlock();
}

void TimeWaitRxTxInInstatnceThread(int mSec)
{
//    Sleep(mSec);
    RxTxTimeDelayMutix.lock();
    IsReadyTx = true;
    RxTxTimeDelayMutix.unlock();
}

const int FilterLowFreq[NUM_MODES][NUM_FILTERS] = 	{	{-1800, -2000, -2400, -2700, -3000, -3300, -3700, -4500},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        { -1250, -1500, -1650, -2000, -2500, -3250, -4000, -5000},
                                                        { -1250, -1500, -1650, -2000, -2500, -3250, -4000, -5000},
                                                        { -1250, -1500, -1650, -2000, -2500, -3250, -4000, -5000},
                                                        { -1250, -1500, -1650, -2000, -2500, -3250, -4000, -5000},
                                                        { -25, 	 -50,   -100,  -200,  -300,  -500,  -750,  -1000},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        {-1800,  -2000, -2400, -2700, -3000, -3300, -3700, -4500},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        { -5000,  -5000,  -5000,  -5000,  -5000,  -5000,  -5000,  -5000}};

const int FilterHighFreq[NUM_MODES][NUM_FILTERS] = 	{	{    0,     0,     0,     0,     0,     0,     0,     0},
                                                        { 1800,  2000,  2400,  2700,  3000,  3300,  3700,  4500},
                                                        { 1250,  1500,  1650,  2000,  2500,  3250,  4000,  5000},
                                                        { 2500,  3000,  3300,  4000,  5000,  6500,  8000,  10000},
                                                        { 2500,  3000,  3300,  4000,  5000,  6500,  8000,  10000},
                                                        { 2500,  3000,  3300,  4000,  5000,  6500,  8000,  10000},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        {   25,	   50,    100,   200,   300,   500,   750,  1000},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        {    0,     0,     0,     0,     0,     0,     0,     0},
                                                        { 1800,   2000,  2400,  2700,  3000,  3300,  3700,  4500},
                                                        { 5000,   5000,  5000,  5000,  5000,  5000,  5000,  5000}};

static float LastToneFilterLow = 0;
static float LastToneFilterHigh = 0;
static bool KeyTrye = false;
static int VolumeTxToRx = 0;
static double tmpMicScale = 1.0;

void audioCallBack4(const void *In, void *Out, unsigned long FrameCount, void *UserData)
{
    ExpertSDR_vA2_1 *pExpSdr = (ExpertSDR_vA2_1*)UserData;
    DttSP *pDsp = pExpSdr->pDsp;
    int DspCurrentPos = pDsp->DspCurrentPos;
    int BufPos = GetCurrentPos(pDsp->DspPos, DspCurrentPos);
    int* ArrayPtr = (int *)Out;
    float* pOutRight0 = (float *)ArrayPtr[0];
    float* pOutLeft0 = (float *)ArrayPtr[1];
    float* pOutRight1 = (float *)ArrayPtr[2];
    float* pOutLeft1 = (float *)ArrayPtr[3];
    ArrayPtr = (int *)In;
    float* pInLeft0 = (float *)ArrayPtr[0];
    float* pInRight0 = (float *)ArrayPtr[1];
    float* pInLeft1 = (float *)ArrayPtr[2];
    float* pInRight1 = (float *)ArrayPtr[3];
    float* pOutRight = 0;
    float* pOutLeft = 0;
    float* pInRight = 0;
    float* pInLeft = 0;

    if(pDsp->TrxMode != TX)
    {
        pOutLeft  = pOutRight0;
        pOutRight = pOutLeft0;
        pInLeft = pInLeft0;
        pInRight = pInRight0;
        memset(pOutLeft1,0, (int)(sizeof(float)*FrameCount));
        memset(pOutRight1,0,sizeof(float)*FrameCount);
    }
    else
    {
        pOutLeft  = pOutRight1;
        pOutRight = pOutLeft1;
        pInLeft = pInLeft1;
        pInRight = pInRight1;
        memset(pOutLeft0,0,sizeof(float)*FrameCount);
        memset(pOutRight0,0,sizeof(float)*FrameCount);
    }

    float *pTmp;
    if(pExpSdr->bSwapLineOut)
    {
        pTmp = pOutLeft;
        pOutLeft = pOutRight;
        pOutRight = pTmp;
    }
    if(pExpSdr->bSwapLineIn)
    {
        pTmp = pInLeft;
        pInLeft = pInRight;
        pInRight = pTmp;
    }

    if(!IsReadyTx)
    {
        memset(pInLeft,0,sizeof(float)*FrameCount);
        memset(pInRight,0,sizeof(float)*FrameCount);
    }
    pDsp->DspPos++;
    if(pDsp->DspPos >= MAX_DSP_MEMORY_BUF)
        pDsp->DspPos = 0;
    if(pExpSdr->CalibrateSoundCard)
    {
        pDsp->SineWave(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, 50);
        pExpSdr->PhaseAccumulator = pDsp->CosineWave(pInRight, FrameCount, pExpSdr->PhaseAccumulator, 50);
        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft1, pOutRight1, FrameCount);
        pDsp->ScaleBuffer(pInLeft , pOutLeft1 , FrameCount, 1.0);
        pDsp->ScaleBuffer(pInRight, pOutRight1, FrameCount, 1.0);
    }
    else
    {
        if(pDsp->TrxMode == TX)
        {
            if((pDsp->SdrMode == CWL) && !pExpSdr->StateTone)
 //               int fake1=0;
                pExpSdr->pCwMacro->pCwCore->procSamples(pOutLeft, pOutRight, FrameCount);
            else if((pDsp->SdrMode == CWU) && !pExpSdr->StateTone)
                pExpSdr->pCwMacro->pCwCore->procSamples(pOutRight, pOutLeft, FrameCount);
 //           int fake2=0;
                else
            {
                switch(pExpSdr->TxInputSignal)
                {
                    case RADIO:
                        pDsp->ScaleBuffer(pInLeft , pInLeft , FrameCount, pExpSdr->MicScale * pExpSdr->MicGainScale);
                        pDsp->ScaleBuffer(pInRight, pInRight, FrameCount, pExpSdr->MicScale * pExpSdr->MicGainScale);
                    break;

                    case SINE:
                        pDsp->SineWave(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                        pExpSdr->PhaseAccumulator = pDsp->CosineWave(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                    break;
                    case SINE_TWO_TONE:
                        double dump;
                        pDsp->SineWave2Tone(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &dump, &dump);
                        pDsp->CosineWave2Tone(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &pExpSdr->PhaseAccumulator, &pExpSdr->PhaseAccumulator2);
                    break;
                    case NOISE:
                        pDsp->Noise(pInLeft, FrameCount);
                        pDsp->Noise(pInRight, FrameCount);
                    break;
                    case TRIANGLE:
                        pDsp->Triangle(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                        memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                    break;
                    case SAWTOOTH:
                        pDsp->Sawtooth(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                        memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                    break;
                    case SILENCE:
                        pDsp->ClearBuffer(pInLeft, FrameCount);
                        pDsp->ClearBuffer(pInRight, FrameCount);
                    break;

                    default:
                    break;
                }
                pExpSdr->pVac->readBuf(pInLeft, pInRight, FrameCount);
                if(pExpSdr->IsRampRelay)
                {
                    int tmp = pExpSdr->cntSwitchRxTx;
                    if(pExpSdr->cntSwitchRxTx >= pExpSdr->vecRampDelay.size())
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
                    else
                    {
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRampDelay.size())
                            {
                                pInLeft[i]  *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                                pInRight[i] *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);

                        pExpSdr->cntSwitchRxTx = tmp;
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRampDelay.size())
                            {
                                pOutLeft[i]  *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                                pOutRight[i] *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                    }
                }
                else
                {
                    int tmp = pExpSdr->cntSwitchRxTx;
                    if(pExpSdr->cntSwitchRxTx >= pExpSdr->vecRamp.size())
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
                    else
                    {
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRamp.size())
                            {
                                pInLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                                pInRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);

                        pExpSdr->cntSwitchRxTx = tmp;
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRamp.size())
                            {
                                pOutLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                                pOutRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                    }
                }
                memset(pInLeft,0,sizeof(float)*FrameCount);
                memset(pInRight,0,sizeof(float)*FrameCount);
                if(pExpSdr->DspMute)
                {
                    memset(pOutRight,0,sizeof(float)*FrameCount);
                    memset(pOutLeft,0,sizeof(float)*FrameCount);
                }
            }
            pExpSdr->pVac->writeBuf(pOutLeft, pOutRight, FrameCount);
            if(pExpSdr->ui->pbMon->isChecked())
            {
                pDsp->ScaleBuffer(pOutLeft , pOutLeft0 , FrameCount, pExpSdr->MonVolume);
                pDsp->ScaleBuffer(pOutRight, pOutRight0, FrameCount, pExpSdr->MonVolume);
            }
            switch((BAND_MODE)pExpSdr->BandModeChecked)
            {
            case BAND160M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val160);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val160);
                break;
            case BAND80M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val80);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val80);
                break;
            case BAND60M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val60);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val60);
                break;
            case BAND40M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val40);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val40);
                break;
            case BAND30M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val30);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val30);
                break;
            case BAND20M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val20);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val20);
                break;
            case BAND17M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val17);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val17);
                break;
            case BAND15M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val15);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val15);
                break;
            case BAND12M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val12);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val12);
                break;
            case BAND10M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val10);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val10);
                break;
            case BAND6M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val6);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val6);
                break;
            case BAND2M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val2);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val2);
                break;
            case BAND07M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val07);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val07);
                break;
            default:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard);
                break;
            }
            pExpSdr->rxGlCnt = 0;
        }
        else
        {
            pExpSdr->pMem->readIqBuf(pInLeft, pInRight, FrameCount);

            memcpy(pDsp->DspCircleBuf[pDsp->DspPos][0], pInLeft, FrameCount*sizeof(float));
            memcpy(pDsp->DspCircleBuf[pDsp->DspPos][1], pInRight, FrameCount*sizeof(float));

            switch(pExpSdr->RxInputSignal)
            {
                case RADIO:
                    pDsp->ScaleBuffer(pInLeft , pInLeft , FrameCount, pExpSdr->LineInGainScale);
                    pDsp->ScaleBuffer(pInRight, pInRight, FrameCount, pExpSdr->LineInGainScale);
                break;
                case SINE:
                    pDsp->SineWave(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                    pExpSdr->PhaseAccumulator = pDsp->CosineWave(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                break;
                case SINE_TWO_TONE:
                    double dump;
                    pDsp->SineWave2Tone(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &dump, &dump);
                    pDsp->CosineWave2Tone(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &pExpSdr->PhaseAccumulator, &pExpSdr->PhaseAccumulator2);
                break;
                case NOISE:
                    pDsp->Noise(pInLeft, FrameCount);
                    pDsp->Noise(pInRight, FrameCount);
                break;
                case TRIANGLE:
                    pDsp->Triangle(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                    memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                break;
                case SAWTOOTH:
                    pDsp->Sawtooth(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                    memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                break;
                case SILENCE:
                    pDsp->ClearBuffer(pInLeft, FrameCount);
                    pDsp->ClearBuffer(pInRight, FrameCount);
                break;
                default:
                break;
            }
            if(pExpSdr->sdrMode == CWL || pExpSdr->sdrMode == CWU)
                pDsp->AudioCallback(pDsp->DspCircleBuf[BufPos][0], pDsp->DspCircleBuf[BufPos][1], pOutLeft, pOutRight, FrameCount);
            else
            {
                int tmp = pExpSdr->cntSwitchTxRx;
                if(pExpSdr->cntSwitchTxRx >= pExpSdr->vecRamp.size())
                    pDsp->AudioCallback(pDsp->DspCircleBuf[BufPos][0], pDsp->DspCircleBuf[BufPos][1], pOutLeft, pOutRight, FrameCount);
                else
                {
                    for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchTxRx++)
                    {
                        if(pExpSdr->cntSwitchTxRx < pExpSdr->vecRamp.size())
                        {
                            pInLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                            pInRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                        }
                    }
                    pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
                    pExpSdr->cntSwitchTxRx = tmp;
                    for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchTxRx++)
                    {
                        if(pExpSdr->cntSwitchTxRx < pExpSdr->vecRamp.size())
                        {
                            pOutLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                            pOutRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                        }
                    }
                }
            }
            pExpSdr->pVac->writeBuf(pOutLeft, pOutRight, FrameCount);
            pDsp->ScaleBuffer(pOutLeft, pOutLeft, FrameCount, pExpSdr->Volume);
            pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->Volume);
            memset(pInLeft,0,sizeof(float)*FrameCount);
            memset(pInRight,0,sizeof(float)*FrameCount);
            pExpSdr->pVac->readBuf(pInLeft, pInRight, FrameCount);
        }
    }
}

void DspCallBack(const void *In, void *Out, unsigned long FrameCount, void *UserData)
{
    ExpertSDR_vA2_1 *pExpSdr = (ExpertSDR_vA2_1*)UserData;
    DttSP *pDsp = pExpSdr->pDsp;
    int DspCurrentPos = pDsp->DspCurrentPos;
    int BufPos = GetCurrentPos(pDsp->DspPos, DspCurrentPos);
    float** ArrayPtr = (float **)Out;
    float* pOutRight;
    float* pOutLeft;
    static int cntTxDel = 0;
    if(!pExpSdr->bSwapLineOut)
    {
        pOutLeft = (float *)(ArrayPtr[0]);
        pOutRight = (float *)(ArrayPtr[1]);
    }
    else
    {
        pOutLeft = (float *)(ArrayPtr[1]);
        pOutRight = (float *)(ArrayPtr[0]);
    }
    ArrayPtr = (float **)In;
    float* pInLeft;
    float* pInRight;

    if(!pExpSdr->bSwapLineIn)
    {
        pInLeft = (float *)(ArrayPtr[0]);
        pInRight = (float *)(ArrayPtr[1]);
    }
    else
    {
        pInLeft = (float *)(ArrayPtr[1]);
        pInRight = (float *)(ArrayPtr[0]);
    }
    pDsp->DspPos++;
    if(pDsp->DspPos >= MAX_DSP_MEMORY_BUF)
        pDsp->DspPos = 0;
    if(pExpSdr->CalibrateSoundCard)
    {
        pDsp->SineWave(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, 50);
        pExpSdr->PhaseAccumulator = pDsp->CosineWave(pInRight, FrameCount, pExpSdr->PhaseAccumulator, 50);
        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
        pDsp->ScaleBuffer(pInLeft , pOutLeft , FrameCount, 1.0);
        pDsp->ScaleBuffer(pInRight, pOutRight, FrameCount, 1.0);
    }
    else
    {
        if(pDsp->TrxMode == TX)
        {
            if((pDsp->SdrMode == CWL) && !pExpSdr->StateTone)
                pExpSdr->pCwMacro->pCwCore->procSamples(pOutLeft, pOutRight, FrameCount);
    //  int fake3=0;
                else if((pDsp->SdrMode == CWU) && !pExpSdr->StateTone)
                pExpSdr->pCwMacro->pCwCore->procSamples(pOutRight, pOutLeft, FrameCount);
    //  int fake4=0;
                else
            {
                if(pExpSdr->pVoiceRec->isRec())
                    pExpSdr->pVoiceRec->BufferIO(pInLeft, pInRight, FrameCount);

                switch(pExpSdr->TxInputSignal)
                {
                    case RADIO:
                        pDsp->ScaleBuffer(pInLeft , pInLeft , FrameCount, pExpSdr->MicScale * pExpSdr->MicGainScale);
                        pDsp->ScaleBuffer(pInRight, pInRight, FrameCount, pExpSdr->MicScale * pExpSdr->MicGainScale);
                    break;

                    case SINE:
                        pDsp->SineWave(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                        pExpSdr->PhaseAccumulator = pDsp->CosineWave(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                    break;
                    case SINE_TWO_TONE:
                        double dump;
                        pDsp->SineWave2Tone(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &dump, &dump);
                        pDsp->CosineWave2Tone(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &pExpSdr->PhaseAccumulator, &pExpSdr->PhaseAccumulator2);
                    break;
                    case NOISE:
                        pDsp->Noise(pInLeft, FrameCount);
                        pDsp->Noise(pInRight, FrameCount);
                    break;
                    case TRIANGLE:
                        pDsp->Triangle(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                        memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                    break;
                    case SAWTOOTH:
                        pDsp->Sawtooth(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                        memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                    break;
                    case SILENCE:
                        pDsp->ClearBuffer(pInLeft, FrameCount);
                        pDsp->ClearBuffer(pInRight, FrameCount);
                    break;

                    default:
                    break;
                }

                if(pExpSdr->pVoiceRec->isPlay())
                    pExpSdr->pVoiceRec->BufferIO(pInLeft, pInRight, FrameCount);
                pExpSdr->pVac->readBuf(pInLeft, pInRight, FrameCount);
                if(pExpSdr->IsRampRelay)
                {
                    int tmp = pExpSdr->cntSwitchRxTx;
                    if(pExpSdr->cntSwitchRxTx >= pExpSdr->vecRampDelay.size())
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
                    else
                    {
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRampDelay.size())
                            {
                                pInLeft[i]  *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                                pInRight[i] *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);

                        pExpSdr->cntSwitchRxTx = tmp;
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRampDelay.size())
                            {
                                pOutLeft[i]  *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                                pOutRight[i] *= pExpSdr->vecRampDelay.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                    }
                }
                else
                {
                    int tmp = pExpSdr->cntSwitchRxTx;
                    if(pExpSdr->cntSwitchRxTx >= pExpSdr->vecRamp.size())
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
                    else
                    {
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRamp.size())
                            {
                                pInLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                                pInRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                        pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);

                        pExpSdr->cntSwitchRxTx = tmp;
                        for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchRxTx++)
                        {
                            if(pExpSdr->cntSwitchRxTx < pExpSdr->vecRamp.size())
                            {
                                pOutLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                                pOutRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchRxTx];
                            }
                        }
                    }
                }
                memset(pInLeft,0,sizeof(float)*FrameCount);
                memset(pInRight,0,sizeof(float)*FrameCount);
                if(pExpSdr->DspMute)
                {
                    memset(pOutRight,0,sizeof(float)*FrameCount);
                    memset(pOutLeft,0,sizeof(float)*FrameCount);
                }
            }
            pExpSdr->pVac->writeBuf(pOutLeft, pOutRight, FrameCount);
            switch((BAND_MODE)pExpSdr->BandModeChecked)
            {
            case BAND160M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val160);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val160);
                break;
            case BAND80M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val80);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val80);
                break;
            case BAND60M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val60);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val60);
                break;
            case BAND40M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val40);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val40);
                break;
            case BAND30M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val30);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val30);
                break;
            case BAND20M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val20);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val20);
                break;
            case BAND17M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val17);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val17);
                break;
            case BAND15M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val15);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val15);
                break;
            case BAND12M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val12);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val12);
                break;
            case BAND10M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val10);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val10);
                break;
            case BAND6M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val6);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val6);
                break;
            case BAND2M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val2);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val2);
                break;
            case BAND07M:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val07);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard*pExpSdr->Val07);
                break;
            default:
                pDsp->ScaleBuffer(pOutLeft , pOutLeft , FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard);
                pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->DriveScale*pExpSdr->ScaleOutBuffSCard);
                break;
            }
            pExpSdr->rxGlCnt = 0;
        }
        else
        {
            pExpSdr->pMem->readIqBuf(pInLeft, pInRight, FrameCount);
            qWarning() << "pDsp =" << pDsp << endl;
            qWarning() << "pDsp->DspPos =" << pDsp->DspPos << endl;
            qWarning() << "pDsp->DspCircleBuf[pDsp->DspPos][0] =" << pDsp->DspCircleBuf[pDsp->DspPos][0] << endl;
            qWarning() << "pDsp->DspCircleBuf[pDsp->DspPos][1] =" << pDsp->DspCircleBuf[pDsp->DspPos][1] << endl;
            qWarning() << "pInLeft =" << pInLeft << endl;
            qWarning() << "pInRight =" << pInRight << endl;

            memcpy(pDsp->DspCircleBuf[pDsp->DspPos][0], pInLeft, FrameCount*sizeof(float));
            memcpy(pDsp->DspCircleBuf[pDsp->DspPos][1], pInRight, FrameCount*sizeof(float));

            switch(pExpSdr->RxInputSignal)
            {
                case RADIO:
                    pDsp->ScaleBuffer(pInLeft , pInLeft , FrameCount, pExpSdr->LineInGainScale);
                    pDsp->ScaleBuffer(pInRight, pInRight, FrameCount, pExpSdr->LineInGainScale);
                break;
                case SINE:
                    pDsp->SineWave(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                    pExpSdr->PhaseAccumulator = pDsp->CosineWave(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->RxFreqSignal);
                break;
                case SINE_TWO_TONE:
                    double dump;
                    pDsp->SineWave2Tone(pInLeft, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &dump, &dump);
                    pDsp->CosineWave2Tone(pInRight, FrameCount, pExpSdr->PhaseAccumulator, pExpSdr->PhaseAccumulator2, pExpSdr->RxFreqSignal, 5000, &pExpSdr->PhaseAccumulator, &pExpSdr->PhaseAccumulator2);
                break;
                case NOISE:
                    pDsp->Noise(pInLeft, FrameCount);
                    pDsp->Noise(pInRight, FrameCount);
                break;
                case TRIANGLE:
                    pDsp->Triangle(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                    memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                break;
                case SAWTOOTH:
                    pDsp->Sawtooth(pInLeft, FrameCount, pExpSdr->RxFreqSignal);
                    memcpy(pInRight, pInLeft, FrameCount*sizeof(float));
                break;
                case SILENCE:
                    pDsp->ClearBuffer(pInLeft, FrameCount);
                    pDsp->ClearBuffer(pInRight, FrameCount);
                break;
                default:
                break;
            }
            if(pExpSdr->sdrMode == CWL || pExpSdr->sdrMode == CWU)
                pDsp->AudioCallback(pDsp->DspCircleBuf[BufPos][0], pDsp->DspCircleBuf[BufPos][1], pOutLeft, pOutRight, FrameCount);
            else
            {
                int tmp = pExpSdr->cntSwitchTxRx;
                if(pExpSdr->cntSwitchTxRx >= pExpSdr->vecRamp.size())
                    pDsp->AudioCallback(pDsp->DspCircleBuf[BufPos][0], pDsp->DspCircleBuf[BufPos][1], pOutLeft, pOutRight, FrameCount);
                else
                {
                    for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchTxRx++)
                    {
                        if(pExpSdr->cntSwitchTxRx < pExpSdr->vecRamp.size())
                        {
                            pInLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                            pInRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                        }
                    }
                    pDsp->AudioCallback(pInLeft, pInRight, pOutLeft, pOutRight, FrameCount);
                    pExpSdr->cntSwitchTxRx = tmp;
                    for(uint i = 0; i < FrameCount; i++, pExpSdr->cntSwitchTxRx++)
                    {
                        if(pExpSdr->cntSwitchTxRx < pExpSdr->vecRamp.size())
                        {
                            pOutLeft[i]  *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                            pOutRight[i] *= pExpSdr->vecRamp.data()[pExpSdr->cntSwitchTxRx];
                        }
                    }
                }
            }
            pExpSdr->pVac->writeBuf(pOutLeft, pOutRight, FrameCount);
            pExpSdr->pVoiceRec->BufferIO(pOutLeft, pOutRight, FrameCount);
            pDsp->ScaleBuffer(pOutLeft, pOutLeft, FrameCount, pExpSdr->Volume);
            pDsp->ScaleBuffer(pOutRight, pOutRight, FrameCount, pExpSdr->Volume);
            memset(pInLeft,0,sizeof(float)*FrameCount);
            memset(pInRight,0,sizeof(float)*FrameCount);
            pExpSdr->pVac->readBuf(pInLeft, pInRight, FrameCount);
            cntTxDel = 0;
        }
    }
}

void VacCallBack(const void *In, void *Out, unsigned long FrameCount, void *UserData)
{
    ExpertSDR_vA2_1	*pExpSdr = (ExpertSDR_vA2_1*)UserData;
    pExpSdr->pVac->vacCallBack(In, Out, FrameCount, UserData);
}


ExpertSDR_vA2_1::ExpertSDR_vA2_1(QWidget *parent) :
    //QMainWindow(parent),
    QWidget(parent),
    ui(new Ui::ExpertSDR_vA2_1)

{
    ui->setupUi(this);

    IsUtcTime = false;
    EqTxOn 	 = true;
    EqRxOn 	 = false;
    EqTxMode = false;
    Volume = 1.0;
    for(int i = 0; i < 5; i++)
        CalibrateLevel[i] = 0;
    CalibrateFreq = 1.0;
    FreqNum = 1600000;
    saveVfoA = FreqNum;
    CurrentBandIndex = 0;
    panTxScale = 1.0;
    for(int iBand = 0; iBand < NUM_BANDS; iBand++)
    {
        OptBands[iBand].CurrentModeIndex = 0;
        OptBands[iBand].Pitch = 750;
        OptBands[iBand].Volume = 100;
        OptBands[iBand].RfGain = 80;
        for(int iMode = 0; iMode < NUM_MODES; iMode++)
        {
            OptBands[iBand].Mode[iMode].cbPreampIndex = 0;
            OptBands[iBand].Mode[iMode].cbAgcIndex = 0;
            OptBands[iBand].Mode[iMode].BinEnable = false;
            OptBands[iBand].Mode[iMode].NrEnable = false;
            OptBands[iBand].Mode[iMode].AnfEnable = false;
            OptBands[iBand].Mode[iMode].Nb1Enable = false;
            OptBands[iBand].Mode[iMode].Nb2Enable = false;
            OptBands[iBand].Mode[iMode].SqlEnable = false;
            OptBands[iBand].Mode[iMode].SqlValue = 0;
            OptBands[iBand].Mode[iMode].GateEnable = false;
            OptBands[iBand].Mode[iMode].GateValue = 0;
            OptBands[iBand].Mode[iMode].CompEnable = false;
            OptBands[iBand].Mode[iMode].CompValue = 0;
            OptBands[iBand].Mode[iMode].CpdrEnable = false;
            OptBands[iBand].Mode[iMode].CpdrValue = 0;
            OptBands[iBand].Mode[iMode].FilterIndex = 4;
        }
    }
    FilterPosFreq = 0;
    OptBands[BAND160M].MainFreq = BPF160_START;
    OptBands[BAND80M].MainFreq = BPF80_START;
    OptBands[BAND60M].MainFreq = BPF60_START;
    OptBands[BAND40M].MainFreq = BPF40_START;
    OptBands[BAND30M].MainFreq = BPF30_START;
    OptBands[BAND20M].MainFreq = BPF20_START;
    OptBands[BAND17M].MainFreq = BPF17_START;
    OptBands[BAND15M].MainFreq = BPF15_START;
    OptBands[BAND12M].MainFreq = BPF12_START;
    OptBands[BAND10M].MainFreq = BPF10_START;
    OptBands[BAND6M].MainFreq  = BPF6_START;
    OptBands[BAND2M].MainFreq  = 144000000;
    OptBands[BAND07M].MainFreq = 433000000;
    OptBands[BANDGEN].MainFreq = 1800000;
    Val160= 0.1;
    Val80 = 0.1;
    Val60 = 0.1;
    Val40 = 0.1;
    Val30 = 0.1;
    Val20 = 0.1;
    Val17 = 0.1;
    Val15 = 0.1;
    Val12 = 0.1;
    Val10 = 0.1;
    Val6  = 0.1;
    Val2  = 0.1;
    Val07 = 0.1;
    IsRampRelay = false;
    saveStateBrakeIn = true;
    isStarted = false;
    VacEnable = false;
    timerClickMem.start();
    timerClickMode.start();
    timerClickFilter.start();
    timerChangePanelIF.start();
    timerChangePanelFilterWidth.start();
    timerChangePanelFilterShift.start();
    timerChangePanelFilterLow.start();
    timerChangePanelFilterHigh.start();
    cntSwitchRxTx = 0;
    cntSwitchTxRx = 0;
    CalibrateSoundCard = false;
    PhaseAccumulator = 0;
    PhaseAccumulator2 = 0;
    RxInputSignal = 0;
    RxFreqSignal = 1000;
    TxInputSignal = 0;
    MicScale = 1.0;
    DriveScale = 1.0;
    LineInGainScale = 1.0;
    MicGainScale = 1.0;
    ScaleOutBuffSCard = 1.0;
    bSwapLineIn = false;
    bSwapLineOut = false;
    StateTone = false;
    DspMute = false;
    glitchCoeff = 0;
    UpdatesTimerID = 0;
    savePreamp = 0;
    smeter2mCorr  = 0.0;
    smeter07mCorr = 0.0;
    pCwMacro = 0;
    m_EvtDispatch = 0;
    pCalibrateSC = 0;
    pTimerTxOutKeyer = 0;
    pTimer = 0;
    pOpt = 0;
    pAbout = 0;
    pVoiceRec = 0;
    pDsp = 0;
    pCatManager = 0;
    pSM = 0;
    pModeBut = 0;
    pMem = 0;
    pGraph = 0;
    pSmeter = 0;
//    pPanel = 0;
    Splitter = 0;
    pBandBut = 0;
    pFiltBut = 0;
    pEqBut = 0;
    pMenuPreamp = 0;
    pAgPreamp = 0;
    pMenuAgc = 0;
    pAgAgc = 0;
    pMenuStep = 0;
    pAgStep = 0;
    pCalibrator = 0;
    pDg0 = 0;
    pDg1 = 0;
    pDg2 = 0;
    pDg3 = 0;
    pDg4 = 0;
    pDg5 = 0;
    pDg6 = 0;
    pDg7 = 0;
    pDg8 = 0;
    pDg9 = 0;
    pPaVac = 0;
    pVac = 0;
    pMnFudTimer = 0;
    pSdrCtrl = 0;
    pVfoB = 0;
    pTxSplitPb = 0;
    pAudThr = 0;
    isGenTx = false;
    flagLock = 0;
    numBandVfoB = 0;
    vfoBVal[0] = 7100000;
    vfoBVal[1] = 145500000;
    vfoBVal[2] = 433000000;
    sdrMode = LSB;


    pCwMacro = new CwMacro;
    m_EvtDispatch = QAbstractEventDispatcher::instance(0);
    pCalibrateSC = new ProgressCalibrate;
    pTimerTxOutKeyer = new QTimer;
    pTimer = new QTimer;
    pOpt = new Options;
    pAbout = new About;
    pVoiceRec = new VoiceRecorder;
    pDsp = new DttSP(this);
    pCatManager = new CatManager(this, pOpt);
    pSM = new S_Meter;
    pModeBut = new QButtonGroup;
    pMem = new WdgMem(&FreqNum, pModeBut);
    pGraph = new WdgGraph(pOpt);
    pSmeter = new SMeterSDR(pGraph->pGl->pPanOpt->ui.wdgSmeterOpt, ui->wdgSmeter);
//    pPanel = new Panel(pOpt->ui.wdgPanel);
    Splitter = new QSplitter(ui->WdgMainGraph);
    pBandBut = new QButtonGroup;
    pFiltBut = new QButtonGroup;
    pEqBut = new QButtonGroup;
    pMenuPreamp = new QMenu;
    pAgPreamp = new QActionGroup(this);
    pMenuAgc = new QMenu;
    pAgAgc = new QActionGroup(this);
    pMenuStep = new QMenu;
    pAgStep = new QActionGroup(this);
    pCalibrator = new Calibrator;


    QString qmenu_style = "QMenu {border: 1px solid #000000; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #333333,  stop: 1 #2e2e2e); color: #EBEBEB;}\n"
                          "QMenu::item:selected {background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #325C32, stop:0.45 #366436, stop:0.55 #366436, stop: 1 #4e9d4e);}";

    pMenuPreamp->setStyleSheet(qmenu_style);
    pMenuAgc->setStyleSheet(qmenu_style);
    pMenuStep->setStyleSheet(qmenu_style);

    pDg0 = new Didgit(ui->pbDg0);
    pDg1 = new Didgit(ui->pbDg1);
    pDg2 = new Didgit(ui->pbDg2);
    pDg3 = new Didgit(ui->pbDg3);
    pDg4 = new Didgit(ui->pbDg4);
    pDg5 = new Didgit(ui->pbDg5);
    pDg6 = new Didgit(ui->pbDg6);
    pDg7 = new Didgit(ui->pbDg7);
    pDg8 = new Didgit(ui->pbDg8);
    pDg9 = new Didgit(ui->pbDg9);
    pPaVac = new pa19(this);
    pVac = new Vac;
    pMenuFreqUpDown = new QMenu;
    pMenuFreqUpDown->setStyleSheet(qmenu_style);
    pFupDownAct = new QActionGroup(this);
    pMnFudTimer = new QTimer;
    pSdrCtrl = new SdrPlugin(pOpt, DspCallBack, audioCallBack4, this);
    pVfoB = new Scale(ui->wdgVfoB);
    pTxSplitPb = new QButtonGroup;
    pAudThr = new AudioThread(pDsp);

    ui->WdgLpanel->setFixedHeight(0);
    pTimer->start(21);
    pVoiceRec->open(pOpt->getWaveFilesDirLocationDefault() + "ExpertSDR_QuickWave.wav");
    pDsp->Open();
    pSM->SetDbm(-100);


    pModeBut->addButton(ui->pbLSB, LSB);
    pModeBut->addButton(ui->pbUSB, USB);
    pModeBut->addButton(ui->pbDSB, DSB);
    pModeBut->addButton(ui->pbAM, AM);
    pModeBut->addButton(ui->pbSAM, SAM);
    pModeBut->addButton(ui->pbFMN, FMN);
    pModeBut->addButton(ui->pbCWL, CWL);
    pModeBut->addButton(ui->pbCWU, CWU);
    pModeBut->addButton(ui->pbSPEC, SPEC);
    pModeBut->addButton(ui->pbDIGL, DIGL);
    pModeBut->addButton(ui->pbDIGU, DIGU);
    pModeBut->addButton(ui->pbDRM, DRM);


    pSmeter->resize(ui->wdgSmeter->size());
    Splitter->setOrientation(Qt::Horizontal);
    Splitter->addWidget(pGraph);
    Splitter->addWidget(pMem);
    Splitter->setStretchFactor(0, 1);
    Splitter->setCollapsible(0, false);
    Splitter->setOpaqueResize(false);


    pBandBut->addButton(ui->pb160M, BAND160M);
    pBandBut->addButton(ui->pb80M, BAND80M);
    pBandBut->addButton(ui->pb40M, BAND40M);
    pBandBut->addButton(ui->pb30M, BAND30M);
    pBandBut->addButton(ui->pb20M, BAND20M);
    pBandBut->addButton(ui->pb17M, BAND17M);
    pBandBut->addButton(ui->pb15M, BAND15M);
    pBandBut->addButton(ui->pb12M, BAND12M);
    pBandBut->addButton(ui->pb10M, BAND10M);
    pBandBut->addButton(ui->pb6M,  BAND6M);
    pBandBut->addButton(ui->pb60M, BAND60M);
    pBandBut->addButton(ui->pb2M,  BAND2M);
    pBandBut->addButton(ui->pb07M, BAND07M);
    pBandBut->addButton(ui->pbGEN, BANDGEN);
    ui->pb07M->setEnabled(false);
    ui->pb2M->setEnabled(false);


    pFiltBut->addButton(ui->pbF0, FILTER0);
    pFiltBut->addButton(ui->pbF1, FILTER1);
    pFiltBut->addButton(ui->pbF2, FILTER2);
    pFiltBut->addButton(ui->pbF3, FILTER3);
    pFiltBut->addButton(ui->pbF4, FILTER4);
    pFiltBut->addButton(ui->pbF5, FILTER5);
    pFiltBut->addButton(ui->pbF6, FILTER6);
    pFiltBut->addButton(ui->pbF7, FILTER7);
    pFiltBut->addButton(ui->pbF8, FILTER8);


    pEqBut->addButton(ui->pbEqRx, 0);
    pEqBut->addButton(ui->pbEqTx, 1);
    ui->pbEqRx->setChecked(true);


    pMenuPreamp->addAction("-30dB");
    pMenuPreamp->addAction("-20dB");
    pMenuPreamp->addAction("-10dB");
    pMenuPreamp->addAction(" 0dB");
    pMenuPreamp->addAction("+10dB");

    for(int i = 0; i < 5; i++)
    {
        pAgPreamp->addAction(pMenuPreamp->actions().at(i));
        pMenuPreamp->actions().at(i)->setCheckable(true);
    }

    pMenuPreamp->actions().at(0)->setChecked(true);
    pMenuAgc->addAction("Off");
    pMenuAgc->addAction("Long");
    pMenuAgc->addAction("Slow");
    pMenuAgc->addAction("Med");
    pMenuAgc->addAction("Fast");

    for(int i = 0; i < 5; i++)
    {
        pAgAgc->addAction(pMenuAgc->actions().at(i));
        pMenuAgc->actions().at(i)->setCheckable(true);
    }

    pMenuAgc->actions().at(0)->setChecked(true);
    pMenuStep->addAction("1Hz");
    pMenuStep->addAction("5Hz");
    pMenuStep->addAction("10Hz");
    pMenuStep->addAction("25Hz");
    pMenuStep->addAction("50Hz");
    pMenuStep->addAction("100Hz");
    pMenuStep->addAction("250Hz");
    pMenuStep->addAction("500Hz");
    pMenuStep->addAction("1kHz");
    pMenuStep->addAction("5kHz");
    pMenuStep->addAction("10kHz");
    pMenuStep->addAction("25kHz");
    pMenuStep->addAction("50kHz");
    pMenuStep->addAction("100kHz");
    pMenuStep->addAction("250kHz");
    pMenuStep->addAction("500kHz");
    pMenuStep->addAction("1MHz");
    pMenuStep->addAction("5MHz");
    pMenuStep->addAction("10MHz");
    for(int i = 0; i < 19; i++)
    {
        pAgStep->addAction(pMenuStep->actions().at(i));
        pMenuStep->actions().at(i)->setCheckable(true);
    }
    pMenuStep->actions().at(0)->setChecked(true);
    ui->pbPreamp->setMenu(pMenuPreamp);
    ui->pbAgc->setMenu(pMenuAgc);
    ui->pbStep->setMenu(pMenuStep);

     QFontDatabase::addApplicationFont(":/images/font/DS-DIGI.TTF");

     pDg0->SetFontType("DS-Digital", 24);
     pDg1->SetFontType("DS-Digital", 24);
     pDg2->SetFontType("DS-Digital", 24);
     pDg3->SetFontType("DS-Digital", 36);
     pDg4->SetFontType("DS-Digital", 36);
     pDg5->SetFontType("DS-Digital", 36);
     pDg6->SetFontType("DS-Digital", 36);
     pDg7->SetFontType("DS-Digital", 36);
     pDg8->SetFontType("DS-Digital", 36);
     pDg9->SetFontType("DS-Digital", 36);

      if(!pDsp->isLoad)
     {
         QMessageBox msgBox;
         msgBox.setText(tr("Can't load DttSP lib!"));
         msgBox.exec();
         qFatal("ExpertSDR: can't load DttSP lib!");
         return;
     }


     pPaVac->open();
     pOpt->ui.cbPaVacDriver->addItems(pPaVac->driverName());
     pOpt->ui.cbPaVacDriver->setCurrentIndex(0);
     pOpt->ui.cbPaVacIn->addItems(pPaVac->inDevName(0));
     pOpt->ui.cbPaVacIn->setCurrentIndex(0);
     pOpt->ui.cbPaVacOut->addItems(pPaVac->outDevName(0));
     pOpt->ui.cbPaVacIn->setCurrentIndex(0);

     pMenuFreqUpDown->addAction(QString::number(FreqNum/1000000.0f, 'f', 6) + " MHz");
     ui->pbFlist->setMenu(pMenuFreqUpDown);
     pFupDownAct->addAction(pMenuFreqUpDown->actions().at(0));
     pMenuFreqUpDown->actions().at(0)->setCheckable(true);
     pMenuFreqUpDown->actions().at(0)->setChecked(true);

     pTxSplitPb->addButton(ui->pbVfoA, 0);
     pTxSplitPb->addButton(ui->pbVfoB, 1);
     this->resize(600, 400);
     onXvtrxEnable(false);
     pGraph->ui.slTime->setMinimum(-MAX_DSP_MEMORY_BUF+1);

     DriveScale = ui->slDrive->value()/100.0;
     LineInGainScale = pOpt->ui.sbLineInGain->value()/100.0;
     MicGainScale = qPow(10.0, pOpt->ui.sbMicInGain->value()/20.0);
     pGraph->pGl->GetBandFilter(RxFilterBandLow, RxFilterBandHigh);
     pGraph->pGl->SetVisibleInfo(ui->cbPanInfo->isChecked());
     MicScale = DbToValMic(ui->slMic->value());

     OnOptChangeNrVals(0);
     OnOptChangeAgcVals(0);
     OnOptChangeNbsVals(0);
     OnOptChangeAnfVals(0);
     OnOptChangeSampleRate(0);
     OnOptChangeDspBufLen(0);
     OnOptChangeDspBufLen(pOpt->ui.cbRxDspBufSize->currentIndex());
     SetTxFilter();
     saveDdsFreq = pGraph->pGl->GetDDSFreq();
     saveSampleRate = pOpt->getSampleRate();
     pMem->pIqList->setWaveDir(pOpt->getWaveFilesDirLocation());
     saveFilter = pGraph->pGl->GetFilter();
     saveFiltId = pFiltBut->checkedId();
     saveMode = pModeBut->checkedId();

     connect(pTimer, SIGNAL(timeout()), this, SLOT(OnUpdateDataTime()));
     connect(ui->pbTime, SIGNAL(clicked()), this, SLOT(OnTimeClick()));
     connect(ui->pbSm, SIGNAL(clicked(bool)), this, SLOT(OnShowSM(bool)));
     connect(pModeBut, SIGNAL(buttonClicked(int)), this, SLOT(OnChangeMode(int)));
     connect(pSM, SIGNAL(ModChange(int)), this, SLOT(OnChangeMode(int)));
     connect(pBandBut, SIGNAL(buttonClicked(int)), this, SLOT(OnChangeBand(int)));
     connect(pSM, SIGNAL(BandChange(int)), this, SLOT(OnChangeBand(int)));
     connect(pFiltBut, SIGNAL(buttonClicked(int)), this, SLOT(OnChangeFilter(int)));
     connect(pEqBut, SIGNAL(buttonClicked(int)), this, SLOT(OnEqModeChanged(int)));
     connect(ui->pbEqOn,  SIGNAL(clicked(bool)), this, SLOT(OnEqOn(bool)));
     connect(ui->pbVac, SIGNAL(toggled(bool)), this, SLOT(OnDspEq(bool)));
     connect(ui->pbVoicePlay, SIGNAL(toggled(bool)), this, SLOT(OnDspEq(bool)));
     connect(ui->pbVoiceRec, SIGNAL(toggled(bool)), this, SLOT(OnDspEq(bool)));
     connect(ui->slTxEq400, SIGNAL(valueChanged(int)), this, SLOT(OnEqTxChanged(int)));
     connect(ui->slTxEq1k5, SIGNAL(valueChanged(int)), this, SLOT(OnEqTxChanged(int)));
     connect(ui->slTxEq6k,  SIGNAL(valueChanged(int)), this, SLOT(OnEqTxChanged(int)));
     connect(ui->slRxEq400, SIGNAL(valueChanged(int)), this, SLOT(OnEqRxChanged(int)));
     connect(ui->slRxEq1k5, SIGNAL(valueChanged(int)), this, SLOT(OnEqRxChanged(int)));
     connect(ui->slRxEq6k,  SIGNAL(valueChanged(int)), this, SLOT(OnEqRxChanged(int)));
     connect(pMenuPreamp, SIGNAL(triggered(QAction*)), this, SLOT(OnPreampChanged(QAction*)));
     connect(pMenuAgc, SIGNAL(triggered(QAction*)), this, SLOT(OnAgcChanged(QAction*)));
     connect(pMenuStep, SIGNAL(triggered(QAction*)), this, SLOT(OnStepChanged(QAction*)));
     connect(pOpt->ui.pbStartCalibProc,  SIGNAL(clicked()), this, SLOT(OnStartCalibration()));
     connect(pCalibrator,  SIGNAL(showCalibration()), this, SLOT(paramForCalibration()));
     connect(this,  SIGNAL(emitFreqDbm(int, float)), pCalibrator, SIGNAL(emitFreqDbm(int, float)));
     connect(pCalibrator,  SIGNAL(getFreqDbm()), this, SLOT(freqDbm()));
     connect(pCalibrator,  SIGNAL(calibrationFreqCoeff(float)), this, SLOT(setCalibrFreqCoeff(float)));
     connect(pCalibrator,  SIGNAL(preamp(int)), this, SLOT(OnChangePreamp(int)));
     connect(pCalibrator,  SIGNAL(mode(int)), this, SLOT(OnChangeMode(int)));
     connect(pCalibrator,  SIGNAL(getDbm()), this, SLOT(getSmeter()));
     connect(this,  SIGNAL(emitDbm(float)), pCalibrator, SIGNAL(setDbm(float)));
     connect(pCalibrator,  SIGNAL(calibrationDbmCoeff(int, float)), this, SLOT(setCalibrDbmCoeff(int, float)));
     connect(pCalibrator, SIGNAL(setWinType(int)), pOpt, SLOT(SetWindowType(int)));
     connect(ui->pbShowFullScreen, SIGNAL(clicked(bool)), this, SLOT(OnShowFullScreen(bool)));
     connect(ui->pbClose,   SIGNAL(clicked()), this, SLOT(OnClose()));
     connect(ui->pbOptions, SIGNAL(clicked()), this, SLOT(OnOptions()));
     connect(ui->pbAbout,   SIGNAL(clicked()), this, SLOT(OnAbout()));
     connect(ui->pbLpan,    SIGNAL(clicked(bool)), this, SLOT(OnLpanel(bool)));
     connect(ui->pbMemPan,  SIGNAL(clicked(bool)), this, SLOT(OnMemPanel(bool)));
     connect(ui->pbSql,  SIGNAL(clicked(bool)), this, SLOT(OnSql(bool)));
     connect(pSM->ui.pbSql,  SIGNAL(clicked(bool)), this, SLOT(OnSql(bool)));
     connect(ui->pbGate, SIGNAL(clicked(bool)), this, SLOT(OnGate(bool)));
     connect(ui->pbComp, SIGNAL(clicked(bool)), this, SLOT(OnComp(bool)));
     connect(ui->pbCpdr, SIGNAL(clicked(bool)), this, SLOT(OnCpdr(bool)));
     connect(ui->slSql,  SIGNAL(valueChanged(int)), this, SLOT(OnSql(int)));
     connect(ui->slGate, SIGNAL(valueChanged(int)), this, SLOT(OnGate(int)));
     connect(ui->slComp, SIGNAL(valueChanged(int)), this, SLOT(OnComp(int)));
     connect(ui->slCpdr, SIGNAL(valueChanged(int)), this, SLOT(OnCpdr(int)));
     connect(ui->sbSql,  SIGNAL(valueChanged(int)), this, SLOT(OnSql(int)));
     connect(ui->sbGate, SIGNAL(valueChanged(int)), this, SLOT(OnGate(int)));
     connect(ui->sbComp, SIGNAL(valueChanged(int)), this, SLOT(OnComp(int)));
     connect(ui->sbCpdr, SIGNAL(valueChanged(int)), this, SLOT(OnCpdr(int)));
     connect(ui->slPitch, SIGNAL(valueChanged(int)), this, SLOT(OnPitch(int)));
     connect(ui->sbPitch, SIGNAL(valueChanged(int)), this, SLOT(OnPitch(int)));
     connect(ui->pbNr, SIGNAL(clicked(bool)), this, SLOT(OnNr(bool)));
     connect(ui->pbAnf, SIGNAL(clicked(bool)), this, SLOT(OnAnf(bool)));
     connect(ui->pbLock, SIGNAL(clicked(bool)), this, SLOT(OnLock(bool)));
     connect(ui->pbNb, SIGNAL(clicked(bool)), this, SLOT(OnNb(bool)));
     connect(ui->pbNb2, SIGNAL(clicked(bool)), this, SLOT(OnNb2(bool)));
     connect(ui->pbBin, SIGNAL(clicked(bool)), this, SLOT(OnBin(bool)));
     connect(ui->pbStart, SIGNAL(clicked(bool)), this, SLOT(OnStart(bool)));
     connect(ui->pbMox, SIGNAL(clicked(bool)), this, SLOT(OnMox(bool)));
     connect(pSdrCtrl, SIGNAL(PttChanged(bool)), this, SLOT(OnMox(bool)));	// todo телеграф
     connect(pSdrCtrl, SIGNAL(Start(bool)), this, SLOT(OnStart(bool)));
     connect(pSdrCtrl, SIGNAL(ChangeMode(int)), this, SLOT(OnChangeMode(int)));
     connect(pSdrCtrl, SIGNAL(TuneChanged(long)), this, SLOT(OnNewTune(long)));
     connect(pSdrCtrl, SIGNAL(DDSChanged(long)), this, SLOT(OnNewDDS(long)));
     connect(this, SIGNAL(TuneChanged(int)), pSdrCtrl, SLOT(OnTuneChanged(int)));
     connect(this, SIGNAL(ModeChanged(int)), pSdrCtrl, SLOT(OnModeChanged(int)));
     connect(this, SIGNAL(SoundCardSampleRateChanged(int)), pSdrCtrl, SLOT(SoundCardSampleRateChanged(int)));
//     connect(pPanel, SIGNAL(KeyPtt(bool)),  pSdrCtrl, SIGNAL(PttChanged(bool)));
     connect(pCatManager, SIGNAL(PttChanged(bool)),  pSdrCtrl, SIGNAL(PttChanged(bool)));
     connect(pOpt, SIGNAL(PttChanged(bool)),  pSdrCtrl, SIGNAL(PttChanged(bool)), Qt::QueuedConnection);
     connect(pCwMacro,  SIGNAL(trxChanged(bool)), this, SLOT(OnMox(bool)));
     connect(ui->pbMox, SIGNAL(clicked(bool)), this, SLOT(IsKeyTrue(bool)));
     connect(ui->pbMute, SIGNAL(clicked(bool)), this, SLOT(OnMute(bool)));
     connect(pSM->ui.pbMute, SIGNAL(clicked(bool)), this, SLOT(OnMute(bool)));
     connect(ui->slVol, SIGNAL(valueChanged(int)), this, SLOT(OnVolume(int)));
     connect(ui->slMonVol, SIGNAL(valueChanged(int)), this, SLOT(OnMonVolume(int)));
     connect(pSM->ui.slVolume, SIGNAL(valueChanged(int)), this, SLOT(OnVolume(int)));
     connect(ui->slAgc, SIGNAL(valueChanged(int)), this, SLOT(OnAgc(int)));
     connect(ui->slDrive, SIGNAL(valueChanged(int)), this, SLOT(OnDrive(int)));
     connect(ui->slMic, SIGNAL(valueChanged(int)), this, SLOT(OnMic(int)));
     connect(pDg0, SIGNAL(Incr()), this, SLOT(HighDig0()));
     connect(pDg1, SIGNAL(Incr()), this, SLOT(HighDig1()));
     connect(pDg2, SIGNAL(Incr()), this, SLOT(HighDig2()));
     connect(pDg3, SIGNAL(Incr()), this, SLOT(HighDig3()));
     connect(pDg4, SIGNAL(Incr()), this, SLOT(HighDig4()));
     connect(pDg5, SIGNAL(Incr()), this, SLOT(HighDig5()));
     connect(pDg6, SIGNAL(Incr()), this, SLOT(HighDig6()));
     connect(pDg7, SIGNAL(Incr()), this, SLOT(HighDig7()));
     connect(pDg8, SIGNAL(Incr()), this, SLOT(HighDig8()));
     connect(pDg9, SIGNAL(Incr()), this, SLOT(HighDig9()));
     connect(pDg0, SIGNAL(Decr()), this, SLOT(LowDig0()));
     connect(pDg1, SIGNAL(Decr()), this, SLOT(LowDig1()));
     connect(pDg2, SIGNAL(Decr()), this, SLOT(LowDig2()));
     connect(pDg3, SIGNAL(Decr()), this, SLOT(LowDig3()));
     connect(pDg4, SIGNAL(Decr()), this, SLOT(LowDig4()));
     connect(pDg5, SIGNAL(Decr()), this, SLOT(LowDig5()));
     connect(pDg6, SIGNAL(Decr()), this, SLOT(LowDig6()));
     connect(pDg7, SIGNAL(Decr()), this, SLOT(LowDig7()));
     connect(pDg8, SIGNAL(Decr()), this, SLOT(LowDig8()));
     connect(pDg9, SIGNAL(Decr()), this, SLOT(LowDig9()));
     connect(pOpt->ui.sbWeight, SIGNAL(valueChanged(int)), pCwMacro->pCwCore, SLOT(setWeight(int)));
     connect(pOpt->ui.sbRamp, SIGNAL(valueChanged(int)), pCwMacro->pCwCore, SLOT(setRamp(int)));
     connect(ui->sbCwDelay, SIGNAL(valueChanged(int)), this, SLOT(onBreakInDelay(int)));
     connect(ui->chbBreakIn, SIGNAL(stateChanged(int)), this, SLOT(onBreakIn(int)));
     connect(ui->sbSWSpeed, SIGNAL(valueChanged(int)), this, SLOT(onCwSpeed(int)));
     connect(ui->sbSWSpeed, SIGNAL(valueChanged(int)), ui->slSpeed, SLOT(setValue(int)));
     connect(ui->slSpeed, SIGNAL(valueChanged(int)), ui->sbSWSpeed, SLOT(setValue(int)));
     connect(pOpt->ui.chbDisMonitor, SIGNAL(stateChanged(int)), this, SLOT(ChangedCWMon(int)));
     connect(ui->pbMon, SIGNAL(clicked(bool)), this, SLOT(ChangedCWMon(bool)));
     connect(pOpt->ui.sbTxCarrier, SIGNAL(valueChanged(int)), this, SLOT(OnAmCarrier(int)));
     connect(pOpt->ui.pbDefault, SIGNAL(clicked()), this, SLOT(defaultSettings()));
     connect(pOpt->ui.pbDefaultCal, SIGNAL(clicked()), this, SLOT(defaultCalibrations()));
     connect(pOpt->ui.pbApply, SIGNAL(clicked()), this, SLOT(onOptionsApply()));
     connect(pGraph->pGl, SIGNAL(ChangeCentrFreq(int)), this, SLOT(OnChangeMainFreq(int)));
     connect(pMem, SIGNAL(FreqMem(int, int)), this, SLOT(OnChangeMainFreqWithWdgMem(int, int)));
     connect(pGraph->pGl, SIGNAL(ChangeFilterLowFreq(int)), this, SLOT(OnChangeLowFilterFreq(int)));
     connect(pGraph->pGl, SIGNAL(ChangeFilterHighFreq(int)), this, SLOT(OnChangeHighFilterFreq(int)));
     connect(ui->pbFup, SIGNAL(clicked()), this, SLOT(OnMnNext()));
     connect(ui->pbFdown, SIGNAL(clicked()), this, SLOT(OnMnPrev()));
     connect(pOpt, SIGNAL(driverVacChanged(int)), this, SLOT(onPaVacDriverChanged(int)));
     connect(pMenuFreqUpDown, SIGNAL(triggered(QAction*)), this, SLOT(OnMenuFreqUpDownChanged(QAction*)));
     connect(pMnFudTimer, SIGNAL(timeout()), this, SLOT(OnMnFudTimeout()));
     connect(pSdrCtrl, SIGNAL(sunsdrConnectInfo(QString)), this, SLOT(setSunSDRState(QString)));
     connect(pTxSplitPb, SIGNAL(buttonClicked(int)), this, SLOT(OnSwitchVfo(int)));
     connect(ui->pbSplit, SIGNAL(clicked(bool)), this, SLOT(OnSplit(bool)));
     connect(ui->pbAtoB, SIGNAL(clicked()), this, SLOT(OnAtoB()));
     connect(ui->pbAfromB, SIGNAL(clicked()), this, SLOT(OnAfromB()));
     connect(ui->pbAswapB, SIGNAL(clicked()), this, SLOT(OnAswapB()));
     connect(ui->pbRx2, SIGNAL(clicked(bool)), this, SLOT(OnRx2(bool)));
     connect(pGraph->pGl, SIGNAL(IsOnFilter2(bool)), this, SLOT(OnFilter2Hide(bool)));
     connect(ui->slRx1Vol, SIGNAL(valueChanged(int)), pDsp, SLOT(SetRx1Volume(int)));
     connect(ui->slRx2Vol, SIGNAL(valueChanged(int)), pDsp, SLOT(SetRx2Volume(int)));
     connect(ui->slRx1Bal, SIGNAL(valueChanged(int)), pDsp, SLOT(SetRx1Balance(int)));
     connect(ui->slRx2Bal, SIGNAL(valueChanged(int)), pDsp, SLOT(SetRx2Balance(int)));
     connect(ui->pbBal1, SIGNAL(clicked()), this, SLOT(OnBal1()));
     connect(ui->pbBal2, SIGNAL(clicked()), this, SLOT(OnBal2()));
     connect(pGraph->pGl, SIGNAL(ChangeFilterFreq2(int)), this, SLOT(OnChangeFilter2Freq(int)));
     connect(pVfoB, SIGNAL(valueChanged(int)), this, SLOT(OnVfoBChanged(int)));
     connect(pOpt, SIGNAL(TxMagRej(double)), this, SLOT(OnChangeTxGainRej(double)));
     connect(pOpt, SIGNAL(TxMagRejAmFm(double)), this, SLOT(OnChangeTxGainRej(double)));
     connect(pOpt, SIGNAL(TxPhaseRej(double)), this, SLOT(OnChangeTxPhaseRej(double)));
     connect(pOpt, SIGNAL(TxPhaseRejAmFm(double)), this, SLOT(OnChangeTxPhaseRej(double)));
     connect(pOpt, SIGNAL(TxMagRejCw(double)), this, SLOT(OnChangeTxGainRejCw(double)));
     connect(pOpt, SIGNAL(TxPhaseRejCw(double)), this, SLOT(OnChangeTxPhaseRejCw(double)));
     connect(pOpt->ui.sbColibrFreqNum, SIGNAL(valueChanged(double)), this, SLOT(ChangedCalibrateNum(double)));
     connect(pOpt, SIGNAL(XvtrxEnable(bool)), this, SLOT(onXvtrxEnable(bool)));    onXvtrxEnable(false);
     connect(pOpt->ui.cbXvtrxAntRx, SIGNAL(currentIndexChanged(int)), this, SLOT(onXvAntSwitch(int)));
     connect(pOpt->ui.cbXvtrxAntTx, SIGNAL(currentIndexChanged(int)), this, SLOT(onXvAntSwitch(int)));
     connect(pOpt->ui.sbVhfOsc, SIGNAL(valueChanged(double)), this, SLOT(SetVhfOsc(double)));
     connect(pOpt->ui.sbUhfOsc, SIGNAL(valueChanged(double)), this, SLOT(SetUhfOsc(double)));
     connect(pOpt->ui.cbCalGen, SIGNAL(clicked(bool)), this, SLOT(onCalibrationGen(bool)));
     connect(pOpt->ui.sb2mSm, SIGNAL(valueChanged(double)), this, SLOT(onSm2mCorr(double)));
     connect(pOpt->ui.sb07mSm, SIGNAL(valueChanged(double)), this, SLOT(onSm07mCorr(double)));
     connect(pGraph->pGl, SIGNAL(ChangeFilterFreq(int)), this, SLOT(OnChangeFilterFreq(int)));
     connect(pGraph->ui.slTime, SIGNAL(sliderReleased()), this, SLOT(OnTimeChange()));
     connect(pGraph->ui.pbTimePlus, SIGNAL(clicked()), this, SLOT(OnTimePlus()));
     connect(pGraph->ui.pbTimeMinus, SIGNAL(clicked()), this, SLOT(OnTimeMinus()));
     connect(pGraph->ui.pbTimeUp, SIGNAL(clicked()), this, SLOT(OnTimeUp()));
     connect(pSM, SIGNAL(FreqChange(int)), this, SLOT(OnChangeStationFreq(int)));
//     connect(pPanel, SIGNAL(mainFreq(int)), this, SLOT(ChangeValcoder(int)));
//     connect(pPanel, SIGNAL(band(int)), this, SLOT(OnPanChangeBand(int)));
//     connect(pPanel, SIGNAL(mode(int)), this, SLOT(OnPanChangeMode(int)));
//     connect(pPanel, SIGNAL(filter(int)), this, SLOT(OnPanChangeFilter(int)));
//     connect(pPanel, SIGNAL(agc(int)), this, SLOT(OnPanChangeAgc(int)));
//     connect(pPanel, SIGNAL(preamp(int)), this, SLOT(OnPanChangePreamp(int)));
//     connect(pPanel, SIGNAL(step(int)), this, SLOT(OnPanChangeStep(int)));
//     connect(pPanel, SIGNAL(bin()), this, SLOT(OnPanChangeBin()));
//     connect(pPanel, SIGNAL(nr()), this, SLOT(OnPanChangeNr()));
//     connect(pPanel, SIGNAL(anf()), this, SLOT(OnPanChangeAnf()));
//     connect(pPanel, SIGNAL(nb()), this, SLOT(OnPanChangeNb()));
//     connect(pPanel, SIGNAL(nb2()), this, SLOT(OnPanChangeNb2()));
//     connect(pPanel, SIGNAL(lock()), this, SLOT(OnPanChangeLock()));
//     connect(pPanel, SIGNAL(memory(int)), this, SLOT(OnPanChangeMemory(int)));
//     connect(pPanel, SIGNAL(sqlOnOff()), this, SLOT(OnPanChangeSqlState()));
//     connect(pPanel, SIGNAL(pa()), this, SLOT(OnPanChangePa()));
//     connect(pPanel, SIGNAL(station(int)), this, SLOT(OnPanChangeStation(int)));
//     connect(pPanel, SIGNAL(mute()), this, SLOT(OnPanChangeMute()));
//     connect(pPanel, SIGNAL(power()), this, SLOT(OnPanChangeStart()));
//     connect(pPanel, SIGNAL(volume(int)), this, SLOT(OnPanVolume(int)));
//     connect(pPanel, SIGNAL(micGain(int)), this, SLOT(OnPanMicGain(int)));
//     connect(pPanel, SIGNAL(squelch(int)), this, SLOT(OnPanSquelch(int)));
//     connect(pPanel, SIGNAL(filterHigh(int)), this, SLOT(OnPanFilterHigh(int)));
//     connect(pPanel, SIGNAL(filterLow(int)), this, SLOT(OnPanFilterLow(int)));
//     connect(pPanel, SIGNAL(filterWidth(int)), this, SLOT(OnPanFilterWidth(int)));
//     connect(pPanel, SIGNAL(filterShift(int)), this, SLOT(OnPanFilterShift(int)));
//     connect(pPanel, SIGNAL(drive(int)), this, SLOT(OnPanDrive(int)));
//     connect(pPanel, SIGNAL(ifVal(int)), this, SLOT(OnPanIF(int)));
//     connect(pPanel, SIGNAL(rfGain(int)), this, SLOT(OnPanRfGain(int)));
//     connect(pPanel, SIGNAL(zoom(int)), this, SLOT(OnPanZoom(int)));
     connect(pCalibrator, SIGNAL(freq(int)), this, SLOT(OnChangeStationFreq(int)));
     connect(pCalibrator, SIGNAL(push()), this, SLOT(pushState()));
     connect(pCalibrator, SIGNAL(pop()), this, SLOT(popState()));
     connect(pCalibrator, SIGNAL(mode(int)), this, SLOT(OnChangeMode(int)));
     connect(pCalibrator, SIGNAL(ifFreq(int)), this, SLOT(OnIF(int)));
     connect(pCalibrator, SIGNAL(preamp(int)), this, SLOT(OnChangePreamp(int)));
     connect(pOpt->ui.cbWinType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnOptChangeWindow(int)));
     connect(ui->slSr, SIGNAL(valueChanged(int)), pGraph->pGl, SLOT(SetSpectrRate(int)));
     connect(ui->slWr, SIGNAL(valueChanged(int)), pGraph->pGl, SLOT(SetWaterfallRate(int)));
     connect(pGraph->pGl, SIGNAL(ChangeStepDDS(int)), this, SLOT(ChangeValcoder(int)));
     connect(pCatManager, SIGNAL(changedIncrDecrVFO_A(int)), this, SLOT(ChangeValcoder(int)));
     connect(pOpt->ui.cbTrTypeSignal, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeTxInputSignal(int)));
     connect(pOpt->ui.cbRcTypeSignal, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeRxInputSignal(int)));
     connect(pOpt->ui.slRxSignalFreq, SIGNAL(valueChanged(int)), this, SLOT(ChangeRxFreqSignal(int)));
     connect(pOpt->ui.sbLineInGain, SIGNAL(valueChanged(int)), this, SLOT(OnLineInGain(int)));
     connect(pOpt->ui.sbMicInGain, SIGNAL(valueChanged(int)), this, SLOT(OnMicInGain(int)));
     connect(ui->pbAutoFreqMaxLevel, SIGNAL(clicked()), pGraph->pGl, SLOT(SetSetupFilterMaxLevel()));
     connect(ui->pbMemUpFreq, SIGNAL(clicked()), this, SLOT(SetMemDownFreq()));
     connect(ui->pbMemDownFreq, SIGNAL(clicked()), this, SLOT(SetMemUpFreq()));
     connect(pOpt->ui.pbStartLevCol_2, SIGNAL(clicked()), this, SLOT(calibrateSCard()));
     connect(pOpt->ui.doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ChangedScardUotVoltage(double)));
     connect(pGraph->ui.slTime, SIGNAL(valueChanged(int)), this, SLOT(ChangedTimeMachine(int)));
     connect(pOpt->ui.chbSwapLineIn, SIGNAL(stateChanged(int)), this, SLOT(ChangeSwapLineIn(int)));
     connect(pOpt->ui.chbSwapLineOut, SIGNAL(stateChanged(int)), this, SLOT(ChangeSwapLineOut(int)));
     connect(ui->cbPanInfo, SIGNAL(stateChanged(int)), pGraph->pGl, SLOT(SetVisibleInfo(int)));
     connect(pSM, SIGNAL(Close()), this, SLOT(SMeterClosed()));
     connect(pOpt, SIGNAL(PowerCorrectChanged160(int)), this, SLOT(PowerCorrect160(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged80(int)), this, SLOT(PowerCorrect80(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged60(int)), this, SLOT(PowerCorrect60(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged40(int)), this, SLOT(PowerCorrect40(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged30(int)), this, SLOT(PowerCorrect30(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged20(int)), this, SLOT(PowerCorrect20(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged17(int)), this, SLOT(PowerCorrect17(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged15(int)), this, SLOT(PowerCorrect15(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged12(int)), this, SLOT(PowerCorrect12(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged10(int)), this, SLOT(PowerCorrect10(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged6(int)), this, SLOT(PowerCorrect6(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged2(int)), this, SLOT(PowerCorrect2(int)));
     connect(pOpt, SIGNAL(PowerCorrectChanged07(int)), this, SLOT(PowerCorrect07(int)));
     connect(pCalibrateSC, SIGNAL(CloseSignal()), this, SLOT(endCalibrateCard()));
     connect(pSdrCtrl, SIGNAL(AdcChanged(int, int)), this, SLOT(adcMeters(int, int)));
//     connect(pPanel, SIGNAL(KeyDash(bool)), pSdrCtrl, SIGNAL(DashChanged(bool)));
//     connect(pPanel, SIGNAL(KeyDot(bool)),  pSdrCtrl, SIGNAL(DotChanged(bool)));
     connect(pOpt, SIGNAL(KeyChanged(bool)),  pCwMacro->pCwCore, SLOT(onKey(bool)), Qt::QueuedConnection);
     connect(pOpt, SIGNAL(DashChanged(bool)),  pSdrCtrl, SIGNAL(DashChanged(bool)));
     connect(pOpt, SIGNAL(DotChanged(bool)),  pSdrCtrl, SIGNAL(DotChanged(bool)));
     connect(pCatManager, SIGNAL(KeyChanged(bool)),  pCwMacro->pCwCore, SLOT(onKey(bool)));
     connect(pSdrCtrl, SIGNAL(DashChanged(bool)),  pCwMacro->pCwCore, SLOT(onDash(bool)));
     connect(pSdrCtrl, SIGNAL(DotChanged(bool)),  pCwMacro->pCwCore, SLOT(onDot(bool)));
     connect(pOpt->ui.chbPoliphase, SIGNAL(stateChanged(int)), this, SLOT(OnSetPolyphase(int)));
     connect(pOpt->ui.sbTxInFilterFreqSsb, SIGNAL(valueChanged(int)), this, SLOT(OnSbTxFilterChanged(int)));
     connect(pOpt->ui.sbTxOutFilterFreqSsb, SIGNAL(valueChanged(int)), this, SLOT(OnSbTxFilterChanged(int)));
     connect(pOpt->ui.sbTxInFilterFreqDig, SIGNAL(valueChanged(int)), this, SLOT(OnSbTxFilterChanged(int)));
     connect(pOpt->ui.sbTxOutFilterFreqDig, SIGNAL(valueChanged(int)), this, SLOT(OnSbTxFilterChanged(int)));
     connect(pOpt->ui.sbTxOutFilterFreqAm, SIGNAL(valueChanged(int)), this, SLOT(OnSbTxFilterChanged(int)));
     connect(pOpt->ui.sbTxOutFilterFreqFmn, SIGNAL(valueChanged(int)), this, SLOT(OnSbTxFilterChanged(int)));
     connect(ui->pbPa, SIGNAL(clicked(bool)), this, SLOT(OnPaOn(bool)));
     connect(pOpt->ui.sbDigL, SIGNAL(valueChanged(int)), this, SLOT(onChangeDigLSb(int)));
     connect(pOpt->ui.sbDigU, SIGNAL(valueChanged(int)), this, SLOT(onChangeDigUSb(int)));
     connect(pOpt->ui.sbNrDelay, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeNrVals(int)));
     connect(pOpt->ui.sbNrGain, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeNrVals(int)));
     connect(pOpt->ui.sbNrLeak, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeNrVals(int)));
     connect(pOpt->ui.sbNrTaps, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeNrVals(int)));
     connect(pOpt->ui.sbAgcAttak, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAgcVals(int)));
     connect(pOpt->ui.sbAgcDecay, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAgcVals(int)));
     connect(pOpt->ui.sbAgcFixGain, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAgcVals(int)));
     connect(pOpt->ui.sbAgcGain, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAgcVals(int)));
     connect(pOpt->ui.sbAgcHang, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAgcVals(int)));
     connect(pOpt->ui.sbAgcHangThresh, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAgcVals(int)));
     connect(pOpt->ui.sbNb1Thr, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeNbsVals(int)));
     connect(pOpt->ui.sbNb2Thr, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeNbsVals(int)));
     connect(pOpt->ui.sbAnfDelay, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAnfVals(int)));
     connect(pOpt->ui.sbAnfGain, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAnfVals(int)));
     connect(pOpt->ui.sbAnfLeak, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAnfVals(int)));
     connect(pOpt->ui.sbAnfTaps, SIGNAL(valueChanged(int)), this, SLOT(OnOptChangeAnfVals(int)));
     connect(pOpt->ui.cbPaSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(OnOptChangeSampleRate(int)));
     connect(pOpt->ui.cbRxDspBufSize, SIGNAL(currentIndexChanged(int)), this, SLOT(OnOptChangeDspBufLen(int)));
     connect(pOpt->ui.sbAgcSlope, SIGNAL(valueChanged(int)), this, SLOT(OnOptAgcSlope(int)));
     connect(pOpt->ui.sbAgcAttak, SIGNAL(valueChanged(int)), this, SLOT(OnOptAgcAttak(int)));
     connect(pOpt->ui.sbAgcDecay, SIGNAL(valueChanged(int)), this, SLOT(OnOptAgcDecay(int)));
     connect(pOpt->ui.sbAgcHang, SIGNAL(valueChanged(int)), this, SLOT(OnOptAgcHang(int)));
     connect(pOpt->ui.sbAgcHangThresh, SIGNAL(valueChanged(int)), this, SLOT(OnOptAgcHangThr(int)));
     connect(pOpt->ui.sbVacRxGain, SIGNAL(valueChanged(int)), this, SLOT(OnVacRxGain(int)));
     connect(pOpt->ui.sbVacTxGain, SIGNAL(valueChanged(int)), this, SLOT(OnVacTxGain(int)));
     connect(pOpt, SIGNAL(SoundCardOptChanged()), this, SLOT(OnSoundOptChanged()));
     connect(ui->pbCwMacro, SIGNAL(clicked()), this, SLOT(onCwMacro()));
     connect(ui->pbVac, SIGNAL(clicked(bool)), this, SLOT(OnPbVacEn(bool)));
     connect(pOpt->ui.chbIambic, SIGNAL(stateChanged(int)), this, SLOT(onKeyAutoMode(int)));
     connect(pOpt->ui.chbRevPaddle, SIGNAL(stateChanged(int)), this, SLOT(onSwapKeys(int)));
     connect(pCwMacro,  SIGNAL(changeMode(int)), this, SLOT(OnChangeMode(int)));
     connect(pCatManager, SIGNAL(txMode()), this, SLOT(onCatTx()));
     connect(pCatManager, SIGNAL(rxMode()), this, SLOT(onCatRx()));
     connect(pOpt->ui.pbLoad, SIGNAL(clicked()), this, SLOT(OnLoad()));
     connect(pOpt->ui.pbSaveAs, SIGNAL(clicked()), this, SLOT(OnSaveAs()));
     connect(pOpt->ui.pbCalibrationLoad, SIGNAL(clicked()), this, SLOT(OnLoadCalibration()));
     connect(pOpt->ui.pbCalibrationSaveAs, SIGNAL(clicked()), this, SLOT(OnSaveAsCalibration()));
     connect(ui->pbTone, SIGNAL(clicked()), this, SLOT(OnTune()));
     connect(pMem->pIqList, SIGNAL(isRec(bool)), this, SLOT(setLockIsRec(bool)));
     connect(pMem->pIqList, SIGNAL(isPlay(bool)), this, SLOT(setLockIsPlay(bool)));
     connect(pMem->pIqList, SIGNAL(isPaused(bool)), this, SLOT(setIsPause(bool)));
     connect(pMem->pIqList, SIGNAL(isChangedPlayFile(bool)), this, SLOT(changedPlayFile(bool)));
     connect(ui->pbRitOn, SIGNAL(toggled(bool)), pGraph->pGl, SLOT(setRit(bool)));
     connect(ui->sbRit, SIGNAL(valueChanged(int)), pGraph->pGl, SLOT(setRitValue(int)));
     connect(pGraph->pGl, SIGNAL(ChangeRitValue(int)), ui->sbRit, SLOT(setValue(int)));
     connect(ui->pbRitReset, SIGNAL(clicked()), this, SLOT(resetRitValue()));
     connect(pOpt, SIGNAL(wavePathChanged(QString)), this, SLOT(setWavePath(QString)));
     saveDdsFreq = pGraph->pGl->GetDDSFreq();
     pMem->pIqList->setWaveDir(pOpt->getWaveFilesDirLocation());
     connect(ui->pbVoicePlay, SIGNAL(toggled(bool)), this, SLOT(onPlayVoice(bool)));
     connect(pVoiceRec, SIGNAL(statePlay(bool)), this, SLOT(onPlayVoice(bool)));
     connect(ui->pbVoiceRec, SIGNAL(toggled(bool)), this, SLOT(onRecVoice(bool)));
     connect(ui->sbVarLowFreq, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBandFilter(int)));
     connect(ui->sbVarHighFreq, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBandFilter(int)));
     connect(pGraph->pGl, SIGNAL(changedFps(int)), this, SLOT(setUpdateTime(int)));
     connect(ui->pbRitOn, SIGNAL(toggled(bool)), this, SLOT(setRxEnable(bool)));
     connect(ui->sbRit, SIGNAL(valueChanged(int)), pDsp, SLOT(SetRitValue(int)));
     connect(ui->pbVfoB_Up_Band, SIGNAL(clicked()), this, SLOT(changedVfoBUp()));
     connect(ui->pbVfoB_Down_Band, SIGNAL(clicked()), this, SLOT(changedVfoBDown()));
     connect(pOpt->ui.sbPaTxDelayValue, SIGNAL(valueChanged(int)), this, SLOT(calcRampDelay(int)));
     connect(pOpt->ui.chbPaTxDelay, SIGNAL(stateChanged(int)), this, SLOT(calcRampDelay(int)));
     connect(pOpt->ui.chbPaTxDelay, SIGNAL(toggled(bool)), this, SLOT(setRampDelayEnable(bool)));
     connect(pOpt->ui.hsVoiceVolumeTx, SIGNAL(valueChanged(int)), pVoiceRec, SLOT(setScaleDb(int)));

     setLock(LOCK_ALL);
     readSettings();
     OnStart(false);

     saveVolume = ui->slVol->value();

     OnChangeBand(crBand);
     setLock(LOCK_MOX | LOCK_TONE | LOCK_VOICE | LOCK_DDS | LOCK_BAND);


}

ExpertSDR_vA2_1::~ExpertSDR_vA2_1()
{
    delete ui;
    delete pVfoB;
    delete pAudThr;
    delete pCalibrator;
   delete pCwMacro;
    delete pCalibrateSC;
    if(pTimer->isActive())
        pTimer->stop();
    delete pTimer;
    delete pAbout;
    delete pDsp;
    delete pCatManager;
    delete pSM;
    delete pMem;
    delete pGraph;
    delete pSmeter;
//    delete pPanel;
    delete Splitter;
    delete pBandBut;
    delete pFiltBut;
    delete pEqBut;
    delete pMenuPreamp;
    delete pAgPreamp;
    delete pMenuAgc;
    delete pAgAgc;
    delete pMenuStep;
    delete pAgStep;
    delete pDg0;
    delete pDg1;
    delete pDg2;
    delete pDg3;
    delete pDg4;
    delete pDg5;
    delete pDg6;
    delete pDg7;
    delete pDg8;
    delete pDg9;
    delete pMenuFreqUpDown;
    delete pFupDownAct;
    if(pMnFudTimer->isActive())
        pMnFudTimer->stop();
    delete pMnFudTimer;
    delete pSdrCtrl;
    delete pTxSplitPb;
    delete pVoiceRec;
    delete pPaVac;
    delete pVac;
}

void ExpertSDR_vA2_1::OnSoundOptChanged()
{
    if(ui->pbStart->isChecked())
    {
        OnStart(false);
 //       Sleep(50);
        OnStart(true);
    }
    ui->pbVac->setChecked(pOpt->ui.chbVacEnable->isChecked());
}

void ExpertSDR_vA2_1::OnShowSM(bool IsShow)
{
    if(IsShow)
        pSM->show();
    else
        pSM->hide();
}

void ExpertSDR_vA2_1::closeEvent(QCloseEvent *event)
{
    killTimer(UpdatesTimerID);
    if(ui->pbStart->isChecked())
        OnStart(false);

    if(!writeSettings())
    {
        event->ignore();
        return;
    }

    if(pOpt->isVisible())
        pOpt->hide();
    if(pAbout->isVisible())
        pAbout->hide();
    if(pSdrCtrl->IsStart())
        pSdrCtrl->Stop();
    if(pSM->isVisible())
        pSM->close();
    if(pCwMacro->isVisible())
        pCwMacro->close();

    pOpt->StopProgs();
    pDsp->terminate();
    pDsp->Close();
    pSdrCtrl->Close();
    pPaVac->close();
//    pPanel->onClose();
}

void ExpertSDR_vA2_1::resizeEvent(QResizeEvent *event)
{
    Splitter->resize(ui->WdgMainGraph->size());
    ui->wdgSmeter->resize(ui->widget_16->size());
    pSmeter->resize(ui->wdgSmeter->size());
}

void ExpertSDR_vA2_1::showEvent(QShowEvent *event)
{
    Splitter->resize(ui->WdgMainGraph->size());
    ui->wdgSmeter->resize(ui->widget_16->size());
    pSmeter->resize(ui->wdgSmeter->size());
}

void ExpertSDR_vA2_1::onPaVacDriverChanged(int Index)
{
    pOpt->ui.cbPaVacIn->clear();
    pOpt->ui.cbPaVacIn->addItems(pPaVac->inDevName(Index));
    pOpt->ui.cbPaVacOut->clear();
    pOpt->ui.cbPaVacOut->addItems(pPaVac->outDevName(Index));
}

void ExpertSDR_vA2_1::onCwSpeed(int speed)
{
    pCwMacro->pCwCore->setSpeed(speed);
}

void ExpertSDR_vA2_1::adcMeters(int Ufwd, int Uref)
{
    double Ufwdf, Ureff, Swr;
    double Pwr;
    Ufwdf = Ufwd*5.0/256.0;
    Ureff = Uref*5.0/256.0;

    if((Ufwdf - Ureff) < 0.01)
        Swr = 11.0;
    else
    {
        Swr = (Ufwdf + Ureff)/(Ufwdf - Ureff);
        Swr = qMin(Swr, 11.0);
    }

    float a5 = -2.733324;
    float a4 = 17.754560;
    float a3 = -42.511845;
    float a2 = 50.504433;
    float a1 = -20.072973;
    float a0 = 3.368461;
    Pwr = a5*qPow(Ufwdf, 5) + a4*qPow(Ufwdf, 4) +a3*qPow(Ufwdf, 3) + a2*Ufwdf*Ufwdf + a1*Ufwdf + a0;
    if(Ufwdf < 0.35)
        Pwr = 0;
    if(Pwr < 0.6)
        Swr = 1.0;
    pSmeter->SetPowerMeterVal(qMax(Pwr, 0.0));
    pSmeter->SetSwrMeterVal(Swr);
}

void ExpertSDR_vA2_1::onXvtrxEnable(bool enable)
{
    ui->pb2M->setEnabled(enable);
    ui->pb07M->setEnabled(enable);
    pOpt->ui.gbXvtrxOpt->setVisible(enable);
    pCalibrator->setXvtrx(enable);
}

void ExpertSDR_vA2_1::onCalibrationGen(bool status)
{
    pSdrCtrl->setCalGen(status);
}

void ExpertSDR_vA2_1::SetVhfOsc(double MHz)
{
    pSdrCtrl->setVhfOsc(MHz*1000000);
    pSdrCtrl->SetDdsFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
}

void ExpertSDR_vA2_1::SetUhfOsc(double MHz)
{
    pSdrCtrl->setUhfOsc(MHz*1000000);
    pSdrCtrl->SetDdsFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
}

void ExpertSDR_vA2_1::pushState()
{
    saveVfoA   = FreqNum;
    saveFilter = pGraph->pGl->GetFilter();
    saveFiltId = pFiltBut->checkedId();
    saveMode   = pModeBut->checkedId();
    savePreamp = pAgPreamp->actions().indexOf(pAgPreamp->checkedAction());
    saveWinType = pOpt->GetWindowType();
}

void ExpertSDR_vA2_1::popState()
{
    OnIF(saveFilter);
    OnChangeStationFreq(saveVfoA);
    OnChangeMode(saveMode);
    pFiltBut->button(saveFiltId)->setChecked(true);
    OnChangeFilter(saveFiltId);
    pOpt->SetWindowType(saveWinType);
    OnChangePreamp(savePreamp);
}

void ExpertSDR_vA2_1::OnUpdateDataTime()
{
    QDateTime dtime = QDateTime::currentDateTime();
    if(IsUtcTime)
    {
        QDateTime UtcTime = dtime.toUTC();
        ui->pbTime->setText(tr(" Time: ") + UtcTime.time().toString(Qt::TextDate)+ " UTC");
        ui->lbDate->setText(tr(" Date: ") + UtcTime.date().toString() + "     ");
    }
    else
    {
        QDateTime UtcTime = dtime;
        ui->pbTime->setText(tr(" Time: ") + UtcTime.time().toString(Qt::TextDate)+ " LOC");
        ui->lbDate->setText(tr(" Date: ") + UtcTime.date().toString() + "     ");
    }
}

bool ExpertSDR_vA2_1::writeSettings()
{
    qDebug() << "ExpertSDR: writeSettings()";
    vfoBVal[numBandVfoB] = pVfoB->value();
    vfoBVal[numBandVfoB] = ui->pbVfoB->isChecked();
    bool state = false;
retry:
    QFileInfo fInfo(QDir::homePath()+"/ExpertSDR/Options.ini");
    if(fInfo.isFile())
    {
        if(!fInfo.isWritable())
        {
            state = QFile::remove(QDir::homePath()+"/ExpertSDR/Options.ini");
            if(!state)
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::homePath() + "/ExpertSDR"));
                 QMessageBox msgBox;
                 msgBox.setText("Write settings error!");
                 msgBox.setInformativeText("Options.ini file is write protected.\nPlease delete the Options.ini file!");
                 msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel | QMessageBox::Close);
                 msgBox.setDefaultButton(QMessageBox::Retry);
                 int ret = msgBox.exec();
                 switch (ret)
                 {
                   case QMessageBox::Retry:
                       goto retry;
                       break;
                   case QMessageBox::Cancel:
                       return false;
                       break;
                   case QMessageBox::Close:
                   default:
                       break;
                 }
            }
        }
    }
    fInfo.setFile(QDir::homePath()+"/ExpertSDR/SunSDR.ini");
        if(fInfo.isFile())
        {
            if(!fInfo.isWritable())
            {
                state = QFile::remove(QDir::homePath()+"/ExpertSDR/SunSDR.ini");
                if(!state)
                {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::homePath() + "/ExpertSDR"));

                     QMessageBox msgBox;
                     msgBox.setText("Write settings error!");
                     msgBox.setInformativeText("SunSDR.ini file is write protected.\nPlease delete the SunSDR.ini file!");
                     msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel | QMessageBox::Close);
                     msgBox.setDefaultButton(QMessageBox::Retry);
                     int ret = msgBox.exec();
                     switch (ret)
                     {
                       case QMessageBox::Retry:
                           goto retry;
                           break;
                       case QMessageBox::Cancel:
                           return false;
                           break;
                       case QMessageBox::Close:
                       default:
                           break;
                     }
                }
            }
        }


        QSettings settings(QDir::homePath()+"/ExpertSDR/Options.ini",  QSettings::IniFormat);
        QSettings setSunSDR(QDir::homePath()+"/ExpertSDR/SunSDR.ini",  QSettings::IniFormat);
        int Index = 0;
        QString Str;
        float Val;
        setSunSDR.beginGroup("SunSDR");
            setSunSDR.beginGroup("FileVersion_1_0_0");
                setSunSDR.setValue("CalibrationPreamp0", CalibrateLevel[0]);
                setSunSDR.setValue("CalibrationPreamp1", CalibrateLevel[1]);
                setSunSDR.setValue("CalibrationPreamp2", CalibrateLevel[2]);
                setSunSDR.setValue("CalibrationPreamp3", CalibrateLevel[3]);
                setSunSDR.setValue("CalibrationPreamp4", CalibrateLevel[4]);
                setSunSDR.setValue("CalibrationFreq", CalibrateFreq);
                pOpt->GetPowerCorrection(BAND160M, &Val);
                setSunSDR.setValue("PowerCorrection160", Val);
                pOpt->GetPowerCorrection(BAND80M, &Val);
                setSunSDR.setValue("PowerCorrection80", Val);
                pOpt->GetPowerCorrection(BAND60M, &Val);
                setSunSDR.setValue("PowerCorrection60", Val);
                pOpt->GetPowerCorrection(BAND40M, &Val);
                setSunSDR.setValue("PowerCorrection40", Val);
                pOpt->GetPowerCorrection(BAND30M, &Val);
                setSunSDR.setValue("PowerCorrection30", Val);
                pOpt->GetPowerCorrection(BAND20M, &Val);
                setSunSDR.setValue("PowerCorrection20", Val);
                pOpt->GetPowerCorrection(BAND17M, &Val);
                setSunSDR.setValue("PowerCorrection17", Val);
                pOpt->GetPowerCorrection(BAND15M, &Val);
                setSunSDR.setValue("PowerCorrection15", Val);
                pOpt->GetPowerCorrection(BAND12M, &Val);
                setSunSDR.setValue("PowerCorrection12", Val);
                pOpt->GetPowerCorrection(BAND10M, &Val);
                setSunSDR.setValue("PowerCorrection10", Val);
                pOpt->GetPowerCorrection(BAND6M, &Val);
                setSunSDR.setValue("PowerCorrection6", Val);
                pOpt->GetPowerCorrection(BAND2M, &Val);
                setSunSDR.setValue("PowerCorrection2", Val);
                pOpt->GetPowerCorrection(BAND07M, &Val);
                setSunSDR.setValue("PowerCorrection07", Val);
                setSunSDR.setValue("LevelOutVoltage", pOpt->ui.doubleSpinBox->value());
                setSunSDR.setValue("Tx_GainIQ", pOpt->ui.slTxImRejMag->value());
                setSunSDR.setValue("TX_PhaseIQ", pOpt->ui.slTxImRejPhase->value());
                setSunSDR.setValue("Tx_GainIQAmFm", pOpt->ui.slTxImRejMagAmFm->value());
                setSunSDR.setValue("TX_PhaseIQAmFm", pOpt->ui.slTxImRejPhaseAmFm->value());
                setSunSDR.setValue("Tx_GainIQ_CW", pOpt->ui.slTxImRejMagCw->value());
                setSunSDR.setValue("TX_PhaseIQ_CW", pOpt->ui.slTxImRejPhaseCw->value());
                setSunSDR.setValue("DdsOscIndex", pOpt->ui.cbDdsOsc->currentIndex());
                setSunSDR.setValue("XvtrxEnable", pOpt->ui.chbXvtrxEnable->isChecked());
                setSunSDR.setValue("cbXvtrxAntTx", pOpt->ui.cbXvtrxAntTx->currentIndex());
                setSunSDR.setValue("cbXvtrxAntRx", pOpt->ui.cbXvtrxAntRx->currentIndex());
                setSunSDR.setValue("sbVhfOsc", pOpt->ui.sbVhfOsc->value());
                setSunSDR.setValue("sbUhfOsc", pOpt->ui.sbUhfOsc->value());
                setSunSDR.setValue("sb2mSm", pOpt->ui.sb2mSm->value());
                setSunSDR.setValue("sb07mSm", pOpt->ui.sb07mSm->value());
            setSunSDR.endGroup();
        setSunSDR.endGroup();

        settings.beginGroup("MainWindow");
            settings.setValue("SMPosition", pSM->pos());
            settings.setValue("Smeter_options", pSM->getSettings());
            settings.setValue("SM_Enable", ui->pbSm->isChecked());
            settings.setValue("PA", ui->pbPa->isChecked());
            if(this->isFullScreen())
            {
                settings.setValue("Geometry", geometry());
                settings.setValue("Position", MainWindowPos);
            }
            else
            {
                settings.setValue("Size", size());
                settings.setValue("Position", pos());
            }
            settings.setValue("MainStep", pMenuStep->actions().indexOf(pAgStep->checkedAction()));
            settings.setValue("FullScreen", this->isFullScreen());
            settings.setValue("Maximized", this->isMaximized());
            settings.setValue("pbSplit", ui->pbSplit->isChecked());
            settings.setValue("pbLock", ui->pbLock->isChecked());
            settings.setValue("slMonVolume", ui->slMonVol->value());
            settings.setValue("slDrive", ui->slDrive->value());
            settings.setValue("slMic", ui->slMic->value());
            settings.setValue("UtcTime", IsUtcTime);
            settings.setValue("L_Pannel", ui->pbLpan->isChecked());
            settings.setValue("EqTxOn", EqTxOn);
            settings.setValue("EqRxOn", EqRxOn);
            settings.setValue("TxEq400Hz", ui->slTxEq400->value());
            settings.setValue("TxEq1k5Hz", ui->slTxEq1k5->value());
            settings.setValue("TxEq6kHz", ui->slTxEq6k->value());
            settings.setValue("RxEq400Hz", ui->slRxEq400->value());
            settings.setValue("RxEq1k5Hz", ui->slRxEq1k5->value());
            settings.setValue("RxEq6kHz", ui->slRxEq6k->value());
            settings.setValue("Rx1Vol", ui->slRx1Vol->value());
            settings.setValue("Rx2Vol", ui->slRx2Vol->value());
            settings.setValue("Rx1Bal", ui->slRx1Bal->value());
            settings.setValue("Rx2Bal", ui->slRx2Bal->value());
            settings.setValue("SqlOn", ui->pbSql->isChecked());
            settings.setValue("slSql", ui->slSql->value());
            settings.setValue("GateOn", ui->pbGate->isChecked());
            settings.setValue("slGate", ui->slGate->value());
            settings.setValue("CompOn", ui->pbComp->isChecked());
            settings.setValue("slComp", ui->slComp->value());
            settings.setValue("CpdrOn", ui->pbCpdr->isChecked());
            settings.setValue("slCpdr", ui->slCpdr->value());
            settings.setValue("CwPitch", ui->slPitch->value());
            if(!ui->chbBreakIn->isChecked() && saveStateBrakeIn)
                settings.setValue("BreakIn", saveStateBrakeIn);
            else
                settings.setValue("BreakIn", ui->chbBreakIn->isChecked());
            settings.setValue("CwMonitor", ui->pbMon->isChecked());
            settings.setValue("CwDelay", ui->sbCwDelay->value());
            settings.setValue("slSpectrumRate", ui->slSr->value());
            settings.setValue("slWfallRate", ui->slWr->value());
            settings.setValue("Frequency", FreqNum);
            settings.setValue("VfoB", pVfoB->value());
            settings.setValue("OnRit", ui->pbRitOn->isChecked());
            settings.setValue("RitValue", ui->sbRit->value());
            Index = pMenuStep->actions().indexOf(pAgStep->checkedAction());
            pGraph->pGl->SetStepDDS(Index);
            settings.setValue("Step", Index);
            QStringList StrList;
            QString n;
            for(int i = 0; i < pMenuFreqUpDown->actions().count(); i++)
                StrList.append(pMenuFreqUpDown->actions().at(i)->iconText());
            settings.setValue("MenuFrequencyMemory", StrList);
            StrList.clear();
            for(int i = 0; i < pMenuFreqUpDown->actions().count(); i++)
            {
                n.setNum(pMenuFreqUpDown->actions().at(i)->data().toInt());
                StrList.append(n);
            }
            settings.setValue("MenuFilterFrequencyMemory", StrList);
            settings.setValue("MenuFrequencyMemory_checked_index", pMenuFreqUpDown->actions().indexOf(pFupDownAct->checkedAction()));
            settings.setValue("Filter", pGraph->pGl->GetFilter());
            if(ui->pbMemPan->isChecked()) splitOpt = Splitter->saveState();
            settings.setValue("GraphSplitterPos", splitOpt);
            settings.setValue("MemPanel", ui->pbMemPan->isChecked());

            settings.beginGroup("BandOptions");
                settings.setValue("CurrentBand", CurrentBandIndex);
                for(int iBand = 0; iBand < NUM_BANDS; iBand++)
                {
                    Str.setNum(iBand);
                    settings.beginGroup("Band" + Str);
                        settings.setValue("CurrentMode", OptBands[iBand].CurrentModeIndex);
                        settings.setValue("BandFreq", OptBands[iBand].MainFreq);
                        settings.setValue("Pitch", OptBands[iBand].Pitch);
                        settings.setValue("slVolume", OptBands[iBand].Volume);
                        settings.setValue("slAgc", OptBands[iBand].RfGain);
                        for(int iMode = 0; iMode < NUM_MODES; iMode++)
                        {
                            Str.setNum(iMode);
                            settings.beginGroup("Mode" + Str);
                                settings.setValue("PreampIndex", OptBands[iBand].Mode[iMode].cbPreampIndex);
                                settings.setValue("AgcIndex", OptBands[iBand].Mode[iMode].cbAgcIndex);
                                settings.setValue("BinEnable", OptBands[iBand].Mode[iMode].BinEnable);
                                settings.setValue("NrEnable", OptBands[iBand].Mode[iMode].NrEnable);
                                settings.setValue("AnfEnable", OptBands[iBand].Mode[iMode].AnfEnable);
                                settings.setValue("Nb1Enable", OptBands[iBand].Mode[iMode].Nb1Enable);
                                settings.setValue("Nb2Enable", OptBands[iBand].Mode[iMode].Nb2Enable);
                                settings.setValue("SqlEnable", OptBands[iBand].Mode[iMode].SqlEnable);
                                settings.setValue("SqlValue", OptBands[iBand].Mode[iMode].SqlValue);
                                settings.setValue("GateEnable", OptBands[iBand].Mode[iMode].GateEnable);
                                settings.setValue("GateValue", OptBands[iBand].Mode[iMode].GateValue);
                                settings.setValue("CompEnable", OptBands[iBand].Mode[iMode].CompEnable);
                                settings.setValue("CompValue", OptBands[iBand].Mode[iMode].CompValue);
                                settings.setValue("CpdrEnable", OptBands[iBand].Mode[iMode].CpdrEnable);
                                settings.setValue("CpdrValue", OptBands[iBand].Mode[iMode].CpdrValue);
                                settings.setValue("FilterIndex", OptBands[iBand].Mode[iMode].FilterIndex);
                                settings.setValue("VarFilterH", OptBands[iBand].Mode[iMode].VarFilterH);
                                settings.setValue("VarFilterL", OptBands[iBand].Mode[iMode].VarFilterL);
                            settings.endGroup();
                        }
                    settings.endGroup();
                }
            settings.endGroup();
            settings.setValue("vfoBval0", vfoBVal[0]);
            settings.setValue("vfoBval1", vfoBVal[1]);
            settings.setValue("vfoBval2", vfoBVal[2]);
            settings.setValue("numBandVfoB", numBandVfoB);
        settings.endGroup();
        settings.beginGroup("SmeterInMainWindow");
            settings.setValue("RxType", pSmeter->GetRxType());
            settings.setValue("TxType", pSmeter->GetTxType());
        settings.endGroup();
        settings.beginGroup("OptionsWindow");
            settings.setValue("Position", pOpt->pos());
            settings.setValue("SDRPluginIndex", pOpt->ui.cbSdrType->currentIndex());
            settings.setValue("Audio_Driver", pOpt->ui.cbPaDriver->currentIndex());
            settings.setValue("Audio_Input", pOpt->ui.cbPaIn->currentIndex());
            settings.setValue("Audio_Output", pOpt->ui.cbPaOut->currentIndex());
            settings.setValue("Audio_Channels", pOpt->ui.cbPaChannels->currentIndex());
            settings.setValue("Audio_BufferSize", pOpt->ui.cbPaBufferSize->currentIndex());
            settings.setValue("Audio_SampleRate", pOpt->ui.cbPaSampleRate->currentIndex());
            settings.setValue("Audio_Lattency", pOpt->ui.sbPaLattency->value());
            settings.setValue("Audio_MicInGain", pOpt->ui.sbMicInGain->value());
            settings.setValue("Audio_LineInGain", pOpt->ui.sbLineInGain->value());
            settings.setValue("Vac_Enable", pOpt->ui.chbVacEnable->isChecked());
            settings.setValue("Vac_Driver", pOpt->ui.cbPaVacDriver->currentIndex());
            settings.setValue("Vac_Input", pOpt->ui.cbPaVacIn->currentIndex());
            settings.setValue("Vac_Output", pOpt->ui.cbPaVacOut->currentIndex());
            settings.setValue("Vac_BufferSize", pOpt->ui.cbPaVacBufferSize->currentIndex());
            settings.setValue("Vac_SampleRate", pOpt->ui.cbPaVacSampleRate->currentIndex());
            settings.setValue("Vac_Lattency", pOpt->ui.sbPaVacLattency->value());
            settings.setValue("Vac_TxGain", pOpt->ui.sbVacTxGain->value());
            settings.setValue("Vac_RxGain", pOpt->ui.sbVacRxGain->value());
            settings.setValue("NR_Taps", pOpt->ui.sbNrTaps->value());
            settings.setValue("NR_Delay", pOpt->ui.sbNrDelay->value());
            settings.setValue("NR_Gain", pOpt->ui.sbNrGain->value());
            settings.setValue("NR_Leak", pOpt->ui.sbNrLeak->value());
            settings.setValue("ANF_Taps", pOpt->ui.sbAnfTaps->value());
            settings.setValue("ANF_Delay", pOpt->ui.sbAnfDelay->value());
            settings.setValue("ANF_Gain", pOpt->ui.sbAnfGain->value());
            settings.setValue("ANF_Leak", pOpt->ui.sbAnfLeak->value());
                    settings.setValue("NB1_Thr", pOpt->ui.sbNb1Thr->value());
                    settings.setValue("NB2_Thr", pOpt->ui.sbNb2Thr->value());
                    settings.setValue("Alc_Attak", pOpt->ui.sbAlcAttak->value());
                    settings.setValue("Alc_Decay", pOpt->ui.sbAlcDecay->value());
                    settings.setValue("Alc_Hang", pOpt->ui.sbAlcHang->value());
                    settings.setValue("Agc_slope", pOpt->ui.sbAgcSlope->value());
                    settings.setValue("Agc_Maxgain", pOpt->ui.sbAgcGain->value());
                    settings.setValue("Agc_Attak", pOpt->ui.sbAgcAttak->value());
                    settings.setValue("Agc_Decay", pOpt->ui.sbAgcDecay->value());
                    settings.setValue("Agc_Hang", pOpt->ui.sbAgcHang->value());
                    settings.setValue("Agc_HangThresh", pOpt->ui.sbAgcHangThresh->value());
                    settings.setValue("Agc_FixGain", pOpt->ui.sbAgcFixGain->value());
                    settings.setValue("Window_type", pOpt->ui.cbWinType->currentIndex());
                    settings.setValue("Polyphase_FFT", pOpt->ui.chbPoliphase->checkState());
                    settings.setValue("TxFilterHighFssb", pOpt->ui.sbTxOutFilterFreqSsb->value());
                    settings.setValue("TxFilterLowFssb", pOpt->ui.sbTxInFilterFreqSsb->value());
                    settings.setValue("TxFilterHighFdig", pOpt->ui.sbTxOutFilterFreqDig->value());
                    settings.setValue("TxFilterLowFdig", pOpt->ui.sbTxInFilterFreqDig->value());
                    settings.setValue("TxFilterHighFam", pOpt->ui.sbTxOutFilterFreqAm->value());
                    settings.setValue("TxFilterHighFfmn", pOpt->ui.sbTxOutFilterFreqFmn->value());
                    settings.setValue("cbTxDspBufSize", pOpt->ui.cbTxDspBufSize->currentIndex());
                    settings.setValue("cbRxDspBufSize", pOpt->ui.cbRxDspBufSize->currentIndex());
                    settings.setValue("sbAmCarrier", pOpt->ui.sbTxCarrier->value());
                    settings.setValue("sbDigL", pOpt->ui.sbDigL->value());
                    settings.setValue("sbDigU", pOpt->ui.sbDigU->value());
                    settings.setValue("chbIambicKeyer", pOpt->ui.chbIambic->isChecked());
                    settings.setValue("chbDisMonitorKeyer", pOpt->ui.chbDisMonitor->isChecked());
                    settings.setValue("chbRevPaddleKeyer", pOpt->ui.chbRevPaddle->isChecked());
                    settings.setValue("chbHighResKeyer", pOpt->ui.chbHighRes->isChecked());
                    settings.setValue("chbModeBKeyer", pOpt->ui.chbModeB->isChecked());
                    settings.setValue("sbWeightKeyer", pOpt->ui.sbWeight->value());
                    settings.setValue("sbRampKeyer", pOpt->ui.sbRamp->value());
                    settings.setValue("sbCWSpeed", ui->sbSWSpeed->value());
                    settings.setValue("sbDelayTxOut", ui->sbCwDelay->value());
                    settings.setValue("SecKeyEnable", pOpt->ui.chbKeyEnable->isChecked());
                    settings.setValue("SecKeyPortName", pOpt->ui.cbKeyPortName->currentIndex());
                    settings.setValue("SecKeyPttLine", pOpt->ui.cbKeyPttLine->currentIndex());
                    settings.setValue("SecKeyKeyLine", pOpt->ui.cbKeyKeyLine->currentIndex());
                    settings.setValue("AddKeyEnable", pOpt->ui.chbAddKeyEnable->isChecked());
                    settings.setValue("AddKeyPortName", pOpt->ui.cbAddKeyPortName->currentIndex());
                    settings.setValue("AddKeyPortDtrRts", pOpt->ui.cbAddKeys->currentIndex());
                    settings.setValue("cbProg0", pOpt->ui.cbProg0->isChecked());
                    settings.setValue("cbProg1", pOpt->ui.cbProg1->isChecked());
                    settings.setValue("cbProg2", pOpt->ui.cbProg2->isChecked());
                    settings.setValue("cbProg3", pOpt->ui.cbProg3->isChecked());
                    settings.setValue("cbProg4", pOpt->ui.cbProg4->isChecked());
                    settings.setValue("Prog0", pOpt->PathProg0);
                    settings.setValue("Prog1", pOpt->PathProg1);
                    settings.setValue("Prog2", pOpt->PathProg2);
                    settings.setValue("Prog3", pOpt->PathProg3);
                    settings.setValue("Prog4", pOpt->PathProg4);
                    settings.setValue("SpRate", ui->slSr->value());
                    settings.setValue("WfRate", ui->slWr->value());
                    settings.setValue("CatOn", pOpt->ui.chbCatEnable->isChecked());
                    settings.setValue("CatPortName", pOpt->ui.cbCatPortName->currentIndex());
                    settings.setValue("CatBoudrate", pOpt->ui.cbBaudRate->currentIndex());
                    settings.setValue("CatStopBits", pOpt->ui.cbCatStopBits->currentIndex());
                    settings.setValue("CatParity", pOpt->ui.cbCatParity->currentIndex());
                    settings.setValue("CatData", pOpt->ui.cbCatData->currentIndex());
                    settings.setValue("CatPttLine", pOpt->ui.cbCatPttLine->currentIndex());
                    settings.setValue("CatKeyLine", pOpt->ui.cbCatKeyLine->currentIndex());
                    settings.setValue("PttEnable", pOpt->ui.chbPttEnable->isChecked());
                    settings.setValue("PttPortName", pOpt->ui.cbPttPortName->currentIndex());
                    settings.setValue("PortDtr", pOpt->ui.chbPttDtr->isChecked());
                    settings.setValue("PortRts", pOpt->ui.chbPttRts->isChecked());
                    settings.setValue("SwapLineIn", pOpt->ui.chbSwapLineIn->isChecked());
                    settings.setValue("SwapLineOut", pOpt->ui.chbSwapLineOut->isChecked());
                    settings.setValue("PanInfo", ui->cbPanInfo->isChecked());
                    settings.setValue("chbExControl", pOpt->ui.chbExtCtrl->isChecked());
                    settings.setValue("chb160Rx1", pOpt->ui.chb160Rx1->isChecked());
                    settings.setValue("chb160Rx2", pOpt->ui.chb160Rx2->isChecked());
                    settings.setValue("chb160Rx3", pOpt->ui.chb160Rx3->isChecked());
                    settings.setValue("chb160Rx4", pOpt->ui.chb160Rx4->isChecked());
                    settings.setValue("chb160Rx5", pOpt->ui.chb160Rx5->isChecked());
                    settings.setValue("chb160Rx6", pOpt->ui.chb160Rx6->isChecked());
                    settings.setValue("chb80Rx1", pOpt->ui.chb80Rx1->isChecked());
                    settings.setValue("chb80Rx2", pOpt->ui.chb80Rx2->isChecked());
                    settings.setValue("chb80Rx3", pOpt->ui.chb80Rx3->isChecked());
                    settings.setValue("chb80Rx4", pOpt->ui.chb80Rx4->isChecked());
                    settings.setValue("chb80Rx5", pOpt->ui.chb80Rx5->isChecked());
                    settings.setValue("chb80Rx6", pOpt->ui.chb80Rx6->isChecked());
                    settings.setValue("chb60Rx1", pOpt->ui.chb60Rx1->isChecked());
                    settings.setValue("chb60Rx2", pOpt->ui.chb60Rx2->isChecked());
                    settings.setValue("chb60Rx3", pOpt->ui.chb60Rx3->isChecked());
                    settings.setValue("chb60Rx4", pOpt->ui.chb60Rx4->isChecked());
                    settings.setValue("chb60Rx5", pOpt->ui.chb60Rx5->isChecked());
                    settings.setValue("chb60Rx6", pOpt->ui.chb60Rx6->isChecked());
                    settings.setValue("chb40Rx1", pOpt->ui.chb40Rx1->isChecked());
                    settings.setValue("chb40Rx2", pOpt->ui.chb40Rx2->isChecked());
                    settings.setValue("chb40Rx3", pOpt->ui.chb40Rx3->isChecked());
                    settings.setValue("chb40Rx4", pOpt->ui.chb40Rx4->isChecked());
                    settings.setValue("chb40Rx5", pOpt->ui.chb40Rx5->isChecked());
                    settings.setValue("chb40Rx6", pOpt->ui.chb40Rx6->isChecked());
                    settings.setValue("chb30Rx1", pOpt->ui.chb30Rx1->isChecked());
                    settings.setValue("chb30Rx2", pOpt->ui.chb30Rx2->isChecked());
                    settings.setValue("chb30Rx3", pOpt->ui.chb30Rx3->isChecked());
                    settings.setValue("chb30Rx4", pOpt->ui.chb30Rx4->isChecked());
                    settings.setValue("chb30Rx5", pOpt->ui.chb30Rx5->isChecked());
                    settings.setValue("chb30Rx6", pOpt->ui.chb30Rx6->isChecked());
                    settings.setValue("chb20Rx1", pOpt->ui.chb20Rx1->isChecked());
                    settings.setValue("chb20Rx2", pOpt->ui.chb20Rx2->isChecked());
                    settings.setValue("chb20Rx3", pOpt->ui.chb20Rx3->isChecked());
                    settings.setValue("chb20Rx4", pOpt->ui.chb20Rx4->isChecked());
                    settings.setValue("chb20Rx5", pOpt->ui.chb20Rx5->isChecked());
                    settings.setValue("chb20Rx6", pOpt->ui.chb20Rx6->isChecked());
                    settings.setValue("chb17Rx1", pOpt->ui.chb17Rx1->isChecked());
                    settings.setValue("chb17Rx2", pOpt->ui.chb17Rx2->isChecked());
                    settings.setValue("chb17Rx3", pOpt->ui.chb17Rx3->isChecked());
                    settings.setValue("chb17Rx4", pOpt->ui.chb17Rx4->isChecked());
                    settings.setValue("chb17Rx5", pOpt->ui.chb17Rx5->isChecked());
                    settings.setValue("chb17Rx6", pOpt->ui.chb17Rx6->isChecked());
                    settings.setValue("chb15Rx1", pOpt->ui.chb15Rx1->isChecked());
                    settings.setValue("chb15Rx2", pOpt->ui.chb15Rx2->isChecked());
                    settings.setValue("chb15Rx3", pOpt->ui.chb15Rx3->isChecked());
                    settings.setValue("chb15Rx4", pOpt->ui.chb15Rx4->isChecked());
                    settings.setValue("chb15Rx5", pOpt->ui.chb15Rx5->isChecked());
                    settings.setValue("chb15Rx6", pOpt->ui.chb15Rx6->isChecked());
                    settings.setValue("chb12Rx1", pOpt->ui.chb12Rx1->isChecked());
                    settings.setValue("chb12Rx2", pOpt->ui.chb12Rx2->isChecked());
                    settings.setValue("chb12Rx3", pOpt->ui.chb12Rx3->isChecked());
                    settings.setValue("chb12Rx4", pOpt->ui.chb12Rx4->isChecked());
                    settings.setValue("chb12Rx5", pOpt->ui.chb12Rx5->isChecked());
                    settings.setValue("chb12Rx6", pOpt->ui.chb12Rx6->isChecked());
                    settings.setValue("chb10Rx1", pOpt->ui.chb10Rx1->isChecked());
                    settings.setValue("chb10Rx2", pOpt->ui.chb10Rx2->isChecked());
                    settings.setValue("chb10Rx3", pOpt->ui.chb10Rx3->isChecked());
                    settings.setValue("chb10Rx4", pOpt->ui.chb10Rx4->isChecked());
                    settings.setValue("chb10Rx5", pOpt->ui.chb10Rx5->isChecked());
                    settings.setValue("chb10Rx6", pOpt->ui.chb10Rx6->isChecked());
                    settings.setValue("chb6Rx1", pOpt->ui.chb6Rx1->isChecked());
                    settings.setValue("chb6Rx2", pOpt->ui.chb6Rx2->isChecked());
                    settings.setValue("chb6Rx3", pOpt->ui.chb6Rx3->isChecked());
                    settings.setValue("chb6Rx4", pOpt->ui.chb6Rx4->isChecked());
                    settings.setValue("chb6Rx5", pOpt->ui.chb6Rx5->isChecked());
                    settings.setValue("chb6Rx6", pOpt->ui.chb6Rx6->isChecked());
                    settings.setValue("chb2Rx1", pOpt->ui.chb2Rx1->isChecked());
                    settings.setValue("chb2Rx2", pOpt->ui.chb2Rx2->isChecked());
                    settings.setValue("chb2Rx3", pOpt->ui.chb2Rx3->isChecked());
                    settings.setValue("chb2Rx4", pOpt->ui.chb2Rx4->isChecked());
                    settings.setValue("chb2Rx5", pOpt->ui.chb2Rx5->isChecked());
                    settings.setValue("chb2Rx6", pOpt->ui.chb2Rx6->isChecked());
                    settings.setValue("chb07Rx1", pOpt->ui.chb07Rx1->isChecked());
                    settings.setValue("chb07Rx2", pOpt->ui.chb07Rx2->isChecked());
                    settings.setValue("chb07Rx3", pOpt->ui.chb07Rx3->isChecked());
                    settings.setValue("chb07Rx4", pOpt->ui.chb07Rx4->isChecked());
                    settings.setValue("chb07Rx5", pOpt->ui.chb07Rx5->isChecked());
                    settings.setValue("chb07Rx6", pOpt->ui.chb07Rx6->isChecked());
                    settings.setValue("chb160Tx1", pOpt->ui.chb160Tx1->isChecked());
                    settings.setValue("chb160Tx2", pOpt->ui.chb160Tx2->isChecked());
                    settings.setValue("chb160Tx3", pOpt->ui.chb160Tx3->isChecked());
                    settings.setValue("chb160Tx4", pOpt->ui.chb160Tx4->isChecked());
                    settings.setValue("chb160Tx5", pOpt->ui.chb160Tx5->isChecked());
                    settings.setValue("chb160Tx6", pOpt->ui.chb160Tx6->isChecked());
                    settings.setValue("chb80Tx1", pOpt->ui.chb80Tx1->isChecked());
                    settings.setValue("chb80Tx2", pOpt->ui.chb80Tx2->isChecked());
                    settings.setValue("chb80Tx3", pOpt->ui.chb80Tx3->isChecked());
                    settings.setValue("chb80Tx4", pOpt->ui.chb80Tx4->isChecked());
                    settings.setValue("chb80Tx5", pOpt->ui.chb80Tx5->isChecked());
                    settings.setValue("chb80Tx6", pOpt->ui.chb80Tx6->isChecked());
                    settings.setValue("chb60Tx1", pOpt->ui.chb60Tx1->isChecked());
                    settings.setValue("chb60Tx2", pOpt->ui.chb60Tx2->isChecked());
                    settings.setValue("chb60Tx3", pOpt->ui.chb60Tx3->isChecked());
                    settings.setValue("chb60Tx4", pOpt->ui.chb60Tx4->isChecked());
                    settings.setValue("chb60Tx5", pOpt->ui.chb60Tx5->isChecked());
                    settings.setValue("chb60Tx6", pOpt->ui.chb60Tx6->isChecked());
                    settings.setValue("chb40Tx1", pOpt->ui.chb40Tx1->isChecked());
                    settings.setValue("chb40Tx2", pOpt->ui.chb40Tx2->isChecked());
                    settings.setValue("chb40Tx3", pOpt->ui.chb40Tx3->isChecked());
                    settings.setValue("chb40Tx4", pOpt->ui.chb40Tx4->isChecked());
                    settings.setValue("chb40Tx5", pOpt->ui.chb40Tx5->isChecked());
                    settings.setValue("chb40Tx6", pOpt->ui.chb40Tx6->isChecked());
                    settings.setValue("chb30Tx1", pOpt->ui.chb30Tx1->isChecked());
                    settings.setValue("chb30Tx2", pOpt->ui.chb30Tx2->isChecked());
                    settings.setValue("chb30Tx3", pOpt->ui.chb30Tx3->isChecked());
                    settings.setValue("chb30Tx4", pOpt->ui.chb30Tx4->isChecked());
                    settings.setValue("chb30Tx5", pOpt->ui.chb30Tx5->isChecked());
                    settings.setValue("chb30Tx6", pOpt->ui.chb30Tx6->isChecked());
                    settings.setValue("chb20Tx1", pOpt->ui.chb20Tx1->isChecked());
                    settings.setValue("chb20Tx2", pOpt->ui.chb20Tx2->isChecked());
                    settings.setValue("chb20Tx3", pOpt->ui.chb20Tx3->isChecked());
                    settings.setValue("chb20Tx4", pOpt->ui.chb20Tx4->isChecked());
                    settings.setValue("chb20Tx5", pOpt->ui.chb20Tx5->isChecked());
                    settings.setValue("chb20Tx6", pOpt->ui.chb20Tx6->isChecked());
                    settings.setValue("chb17Tx1", pOpt->ui.chb17Tx1->isChecked());
                    settings.setValue("chb17Tx2", pOpt->ui.chb17Tx2->isChecked());
                    settings.setValue("chb17Tx3", pOpt->ui.chb17Tx3->isChecked());
                    settings.setValue("chb17Tx4", pOpt->ui.chb17Tx4->isChecked());
                    settings.setValue("chb17Tx5", pOpt->ui.chb17Tx5->isChecked());
                    settings.setValue("chb17Tx6", pOpt->ui.chb17Tx6->isChecked());
                    settings.setValue("chb15Tx1", pOpt->ui.chb15Tx1->isChecked());
                    settings.setValue("chb15Tx2", pOpt->ui.chb15Tx2->isChecked());
                    settings.setValue("chb15Tx3", pOpt->ui.chb15Tx3->isChecked());
                    settings.setValue("chb15Tx4", pOpt->ui.chb15Tx4->isChecked());
                    settings.setValue("chb15Tx5", pOpt->ui.chb15Tx5->isChecked());
                    settings.setValue("chb15Tx6", pOpt->ui.chb15Tx6->isChecked());
                    settings.setValue("chb12Tx1", pOpt->ui.chb12Tx1->isChecked());
                    settings.setValue("chb12Tx2", pOpt->ui.chb12Tx2->isChecked());
                    settings.setValue("chb12Tx3", pOpt->ui.chb12Tx3->isChecked());
                    settings.setValue("chb12Tx4", pOpt->ui.chb12Tx4->isChecked());
                    settings.setValue("chb12Tx5", pOpt->ui.chb12Tx5->isChecked());
                    settings.setValue("chb12Tx6", pOpt->ui.chb12Tx6->isChecked());
                    settings.setValue("chb10Tx1", pOpt->ui.chb10Tx1->isChecked());
                    settings.setValue("chb10Tx2", pOpt->ui.chb10Tx2->isChecked());
                    settings.setValue("chb10Tx3", pOpt->ui.chb10Tx3->isChecked());
                    settings.setValue("chb10Tx4", pOpt->ui.chb10Tx4->isChecked());
                    settings.setValue("chb10Tx5", pOpt->ui.chb10Tx5->isChecked());
                    settings.setValue("chb10Tx6", pOpt->ui.chb10Tx6->isChecked());
                    settings.setValue("chb6Tx1", pOpt->ui.chb6Tx1->isChecked());
                    settings.setValue("chb6Tx2", pOpt->ui.chb6Tx2->isChecked());
                    settings.setValue("chb6Tx3", pOpt->ui.chb6Tx3->isChecked());
                    settings.setValue("chb6Tx4", pOpt->ui.chb6Tx4->isChecked());
                    settings.setValue("chb6Tx5", pOpt->ui.chb6Tx5->isChecked());
                    settings.setValue("chb6Tx6", pOpt->ui.chb6Tx6->isChecked());
                    settings.setValue("chb2Tx1", pOpt->ui.chb2Tx1->isChecked());
                    settings.setValue("chb2Tx2", pOpt->ui.chb2Tx2->isChecked());
                    settings.setValue("chb2Tx3", pOpt->ui.chb2Tx3->isChecked());
                    settings.setValue("chb2Tx4", pOpt->ui.chb2Tx4->isChecked());
                    settings.setValue("chb2Tx5", pOpt->ui.chb2Tx5->isChecked());
                    settings.setValue("chb2Tx6", pOpt->ui.chb2Tx6->isChecked());
                    settings.setValue("chb07Tx1", pOpt->ui.chb07Tx1->isChecked());
                    settings.setValue("chb07Tx2", pOpt->ui.chb07Tx2->isChecked());
                    settings.setValue("chb07Tx3", pOpt->ui.chb07Tx3->isChecked());
                    settings.setValue("chb07Tx4", pOpt->ui.chb07Tx4->isChecked());
                    settings.setValue("chb07Tx5", pOpt->ui.chb07Tx5->isChecked());
                    settings.setValue("chb07Tx6", pOpt->ui.chb07Tx6->isChecked());
                    pGraph->pGl->pPanOpt->writeSettings(&settings);
//                    pPanel->writeSettings(&settings);
                    pCwMacro->writeSetting(&settings);
                    pSmeter->saveSettings(&settings);
                    pMem->writeSettings(&settings);
                    settings.setValue("CalibrationPath", pathTmp);
                    settings.setValue("waveFileDefaultPath", pOpt->getWaveFilesDirLocation());
                    pGraph->pGl->writeSettings(&settings);
                    settings.setValue("GenTx", isGenTx);
                    settings.setValue("RampTxDelayEnable", pOpt->ui.chbPaTxDelay->isChecked());
                    settings.setValue("RampTxDelayValue", pOpt->ui.sbPaTxDelayValue->value());
                    settings.setValue("VolumeVoiceTxValue", pOpt->ui.sbVoiceVolumeTx->value());
                settings.endGroup();
                return true;
}

void ExpertSDR_vA2_1::readSettings()
{
    qDebug() << "ExpertSDR: readSettings()";
    QSettings settings(QDir::homePath()+"/ExpertSDR/Options.ini", QSettings::IniFormat);
    QSettings setSunSDR(QDir::homePath()+"/ExpertSDR/SunSDR.ini",  QSettings::IniFormat);
    QString Str;
    float Val;
    setSunSDR.beginGroup("SunSDR");
        setSunSDR.beginGroup("FileVersion_1_0_0");
            CalibrateLevel[0] = setSunSDR.value("CalibrationPreamp0", 0).toFloat();
            CalibrateLevel[1] = setSunSDR.value("CalibrationPreamp1", 0).toFloat();
            CalibrateLevel[2] = setSunSDR.value("CalibrationPreamp2", 0).toFloat();
            CalibrateLevel[3] = setSunSDR.value("CalibrationPreamp3", 0).toFloat();
            CalibrateLevel[4] = setSunSDR.value("CalibrationPreamp4", 0).toFloat();
            CalibrateFreq = setSunSDR.value("CalibrationFreq", 1.0).toDouble();
            Val = setSunSDR.value("PowerCorrection160", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection160 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND160M, Val);
            PowerCorrect160(Val);
            Val = setSunSDR.value("PowerCorrection80", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection80 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND80M, Val);
            PowerCorrect80(Val);
            Val = setSunSDR.value("PowerCorrection60", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection60 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND60M, Val);
            PowerCorrect60(Val);
            Val = setSunSDR.value("PowerCorrection40", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection40 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND40M, Val);
            PowerCorrect40(Val);
            Val = setSunSDR.value("PowerCorrection30", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection30 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND30M, Val);
            PowerCorrect30(Val);
            Val = setSunSDR.value("PowerCorrection20", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection20 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND20M, Val);
            PowerCorrect20(Val);
            Val = setSunSDR.value("PowerCorrection17", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection17 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND17M, Val);
            PowerCorrect17(Val);
            Val = setSunSDR.value("PowerCorrection15", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection15 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND15M, Val);
            PowerCorrect15(Val);
            Val = setSunSDR.value("PowerCorrection12", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection12 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND12M, Val);
            PowerCorrect12(Val);
            Val = setSunSDR.value("PowerCorrection10", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection10 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND10M, Val);
            PowerCorrect10(Val);
            Val = setSunSDR.value("PowerCorrection6", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection6 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND6M, Val);
            PowerCorrect6(Val);
            Val = setSunSDR.value("PowerCorrection2", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection2 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND2M, Val);
            PowerCorrect2(Val);
            Val = setSunSDR.value("PowerCorrection07", 55).toFloat();
            if((Val < 48) || (Val > 65))
            {
                qWarning() << "ExpertSDR: readSettings(): PowerCorrection0.7 = " << Val;
                Val = 57;
            }
            pOpt->SetPowerCorrection(BAND07M, Val);
            PowerCorrect07(Val);
            volatile double tmpDValue = setSunSDR.value("LevelOutVoltage", 2.2).toDouble();
            if(tmpDValue < 0.1 || tmpDValue > 8.0)
            {
                qWarning() << "ExpertSDR: readSettings(): LevelOutVoltage = " << tmpDValue;
                tmpDValue = 1.0;
            }
            else
                qDebug() << "ExpertSDR: readSettings(): LevelOutVoltage = " << tmpDValue;
            pOpt->ui.doubleSpinBox->setValue(tmpDValue);
            ChangedScardUotVoltage(tmpDValue);
            volatile int tmpIValue = setSunSDR.value("Tx_GainIQ", 0).toInt();
            if(tmpIValue < -40000 || tmpIValue > 40000)
            {
                qWarning() << "ExpertSDR: readSettings(): Tx_GainIQ = " << tmpIValue;
                tmpIValue = 0;
            }
            pOpt->ui.slTxImRejMag->setValue(tmpIValue);
            OnChangeTxGainRej(tmpIValue);
            tmpIValue = setSunSDR.value("TX_PhaseIQ", 0).toInt();
            if(tmpIValue < -50000 || tmpIValue > 50000)
            {
                qWarning() << "ExpertSDR: readSettings(): Tx_GainIQ = " << tmpIValue;
                tmpIValue = 0;
            }
            pOpt->ui.slTxImRejPhase->setValue(tmpIValue);
            OnChangeTxPhaseRej(tmpIValue);
            tmpIValue = setSunSDR.value("Tx_GainIQAmFm", 0).toInt();
            if(tmpIValue < -40000 || tmpIValue > 40000)
            {
                qWarning() << "ExpertSDR: readSettings(): Tx_GainIQAmFm = " << tmpIValue;
                tmpIValue = 0;
            }
            pOpt->ui.slTxImRejMagAmFm->setValue(tmpIValue);
            OnChangeTxGainRej(tmpIValue);
            tmpIValue = setSunSDR.value("TX_PhaseIQAmFm", 0).toInt();
            if(tmpIValue < -50000 || tmpIValue > 50000)
            {
                qWarning() << "ExpertSDR: readSettings(): TX_PhaseIQAmFm = " << tmpIValue;
                tmpIValue = 0;
            }
            pOpt->ui.slTxImRejPhaseAmFm->setValue(tmpIValue);
            OnChangeTxPhaseRej(tmpIValue);
            tmpIValue = setSunSDR.value("Tx_GainIQ_CW", 0).toInt();
            if(tmpIValue < -40000 || tmpIValue > 40000)
            {
                qWarning() << "ExpertSDR: readSettings(): Tx_GainIQ_CW = " << tmpIValue;
                tmpIValue = 0;
            }
            pOpt->ui.slTxImRejMagCw->setValue(tmpIValue);
            OnChangeTxGainRejCw(tmpIValue);
            tmpIValue = setSunSDR.value("TX_PhaseIQ_CW", 0).toInt();
            if(tmpIValue < -50000 || tmpIValue > 50000)
            {
                qWarning() << "ExpertSDR: readSettings(): TX_PhaseIQ_CW = " << tmpIValue;
                tmpIValue = 0;
            }
            pOpt->ui.slTxImRejPhaseCw->setValue(tmpIValue);
            OnChangeTxPhaseRejCw(tmpIValue);

            tmpIValue = setSunSDR.value("DdsOscIndex", 1).toInt();
            pOpt->ui.cbDdsOsc->setCurrentIndex(tmpIValue);
            bool tmpBValue = setSunSDR.value("XvtrxEnable", false).toBool();
            pOpt->ui.chbXvtrxEnable->setChecked(tmpBValue);
            onXvtrxEnable(tmpBValue);
            pOpt->onEnableXvtrx(tmpBValue);

            tmpIValue = setSunSDR.value("cbXvtrxAntTx", 0).toInt();
            pOpt->ui.cbXvtrxAntTx->setCurrentIndex(tmpIValue);

            tmpIValue = setSunSDR.value("cbXvtrxAntRx", 0).toInt();
            pOpt->ui.cbXvtrxAntRx->setCurrentIndex(tmpIValue);

            tmpDValue = setSunSDR.value("sbVhfOsc", 120.0).toDouble();
            if(tmpDValue < 100 || tmpDValue > 140)
            {
                qWarning() << "ExpertSDR: readSettings(): sbVhfOsc = " << tmpDValue;
                tmpDValue = 120;
            }
            pOpt->ui.sbVhfOsc->setValue(tmpDValue);
            pSdrCtrl->setVhfOsc(tmpDValue*1000000);

            tmpDValue = setSunSDR.value("sbUhfOsc", 409.6).toDouble();
            if(tmpDValue < 390 || tmpDValue > 420)
            {
                qWarning() << "ExpertSDR: readSettings(): sbUhfOsc = " << tmpDValue;
                tmpDValue = 409.6;
            }
            pOpt->ui.sbUhfOsc->setValue(tmpDValue);
            pSdrCtrl->setUhfOsc(tmpDValue*1000000);

            tmpDValue = setSunSDR.value("sb2mSm", pOpt->ui.sb2mSm->value()).toDouble();
             if(tmpDValue < -40 || tmpDValue > 40)
                 tmpDValue = 0;
             pOpt->ui.sb2mSm->setValue(tmpDValue);
             onSm2mCorr(tmpDValue);

             tmpDValue = setSunSDR.value("sb07mSm", pOpt->ui.sb07mSm->value()).toDouble();
             if(tmpDValue < -40 || tmpDValue > 40)
                 tmpDValue = 0;
             pOpt->ui.sb07mSm->setValue(tmpDValue);
             onSm07mCorr(tmpDValue);
        setSunSDR.endGroup();
    setSunSDR.endGroup();
    settings.beginGroup("MainWindow");
        pSM->move(settings.value("SMPosition", QPoint(300, 300)).toPoint());
        pSM->setSettings(settings.value("Smeter_options", false));
        ui->pbSm->setChecked(settings.value("SM_Enable", false).toBool());
        ui->pbPa->setChecked(settings.value("PA", true).toBool());
        OnShowSM(ui->pbSm->isChecked());

        MainWindowSize = settings.value("Size", QSize(900, 500)).toSize();
        MainWindowPos = settings.value("Position", QPoint(200, 200)).toPoint();
        if(settings.value("FullScreen", 0).toBool())
            this->showFullScreen();
        else
        {
            if(settings.value("Maximized", 0).toBool())
            {
                this->showMaximized();
            }
            else
            {
                this->showNormal();
            }
            MainWindowSize = settings.value("Size", QSize(900, 500)).toSize();
            resize(MainWindowSize);
            move(MainWindowPos);
        }
        tmpIValue = settings.value("CwPitch", 750).toInt();
        if(tmpIValue < 50 || tmpIValue > 2500)
        {
            qWarning() << "ExpertSDR: readSettings(): CwPitch = " << tmpIValue;
            tmpIValue = 750;
        }
        OnPitch(tmpIValue);

        tmpIValue = settings.value("MainStep", 4).toInt();
        if(tmpIValue < 0 || tmpIValue > 18)
        {
            qWarning() << "ExpertSDR: readSettings(): MainStep = " << tmpIValue;
            tmpIValue = 4;
        }
        OnStepChanged(tmpIValue);

        ui->pbShowFullScreen->setChecked(settings.value("FullScreen", false).toBool());

        tmpIValue = settings.value("FilterID", 4).toInt();
        if(tmpIValue < 0 || tmpIValue > 8)
        {
            qWarning() << "ExpertSDR: readSettings(): FilterID = " << tmpIValue;
            tmpIValue = 4;
        }
        pFiltBut->button(tmpIValue)->setChecked(true);

        tmpBValue = settings.value("pbSplit", false).toBool();
        ui->pbSplit->setChecked(tmpBValue);
        OnSplit(tmpBValue);
        tmpBValue = settings.value("pbLock", false).toBool();
        ui->pbLock->setChecked(tmpBValue);
        OnLock(tmpBValue);

        tmpIValue = settings.value("slMonVolume", 50).toInt();
        if(tmpIValue < 1 || tmpIValue > 60)
        {
            qWarning() << "ExpertSDR: readSettings(): slMonVolume = " << tmpIValue;
            tmpIValue = 50;
        }
        OnMonVolume(tmpIValue);

        tmpIValue = settings.value("slDrive", 40).toInt();
        if(tmpIValue < 0 || tmpIValue > 10000)
        {
            qWarning() << "ExpertSDR: readSettings(): slDrive = " << tmpIValue;
            tmpIValue = 40;
        }
        OnDrive(tmpIValue);

        tmpIValue = settings.value("slMic", 30).toInt();
        if(tmpIValue < 0 || tmpIValue > 70)
        {
            qWarning() << "ExpertSDR: readSettings(): slMic = " << tmpIValue;
            tmpIValue = 30;
        }
        OnMic(tmpIValue);

        IsUtcTime = settings.value("UtcTime", 0).toBool();
        ui->pbLpan->setChecked(settings.value("L_Pannel", false).toBool());
        OnLpanel(ui->pbLpan->isChecked());

        tmpIValue = settings.value("Preamp", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 4)
        {
            qWarning() << "ExpertSDR: readSettings(): Preamp = " << tmpIValue;
            tmpIValue = 0;
        }
        pMenuPreamp->actions().at(tmpIValue)->setChecked(true);
        OnPreampChanged(pMenuPreamp->actions().at(tmpIValue));

        tmpIValue = settings.value("Agc", 2).toInt();
        if(tmpIValue < 0 || tmpIValue > 4)
        {
            qWarning() << "ExpertSDR: readSettings(): Agc = " << tmpIValue;
            tmpIValue = 2;
        }
        pMenuAgc->actions().at(tmpIValue)->setChecked(true);
        OnAgcChanged(pMenuAgc->actions().at(tmpIValue));
        pDsp->SetRxAgc((AGCMODE)tmpIValue);

        EqTxOn = settings.value("EqTxOn", false).toBool();
        EqRxOn = settings.value("EqRxOn", false).toBool();
        ui->pbEqRx->setChecked(true);
        if(EqRxOn)
        {
            ui->pbEqOn->setChecked(true);
            OnEqOn(true);
        }

        tmpIValue = settings.value("TxEq400Hz", 0).toInt();
        if(tmpIValue < -15 || tmpIValue > 15)
        {
            qWarning() << "ExpertSDR: readSettings(): TxEq400Hz = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->slTxEq400->setValue(tmpIValue);

        tmpIValue = settings.value("TxEq1k5Hz", 0).toInt();
        if(tmpIValue < -15 || tmpIValue > 15)
        {
            qWarning() << "ExpertSDR: readSettings(): TxEq1k5Hz = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->slTxEq1k5->setValue(tmpIValue);

        tmpIValue = settings.value("TxEq6kHz", 0).toInt();
        if(tmpIValue < -15 || tmpIValue > 15)
        {
            qWarning() << "ExpertSDR: readSettings(): TxEq6kHz = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->slTxEq6k->setValue(tmpIValue);

        tmpIValue = settings.value("RxEq400Hz", 0).toInt();
        if(tmpIValue < -15 || tmpIValue > 15)
        {
            qWarning() << "ExpertSDR: readSettings(): RxEq400Hz = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->slRxEq400->setValue(tmpIValue);

        tmpIValue = settings.value("RxEq1k5Hz", 0).toInt();
        if(tmpIValue < -15 || tmpIValue > 15)
        {
            qWarning() << "ExpertSDR: readSettings(): RxEq1k5Hz = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->slRxEq1k5->setValue(tmpIValue);

        tmpIValue = settings.value("RxEq6kHz", 0).toInt();
        if(tmpIValue < -15 || tmpIValue > 15)
        {
            qWarning() << "ExpertSDR: readSettings(): RxEq6kHz = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->slRxEq6k->setValue(tmpIValue);

        tmpIValue = settings.value("Rx1Vol", 70).toInt();
        if(tmpIValue < 0 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): Rx1Vol = " << tmpIValue;
            tmpIValue = 70;
        }
        ui->slRx1Vol->setValue(tmpIValue);
        pDsp->SetRx1Volume(tmpIValue);

        tmpIValue = settings.value("Rx2Vol", 70).toInt();
        if(tmpIValue < 0 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): Rx2Vol = " << tmpIValue;
            tmpIValue = 70;
        }
        ui->slRx2Vol->setValue(tmpIValue);
        pDsp->SetRx2Volume(tmpIValue);

        tmpIValue = settings.value("Rx1Bal", 50).toInt();
        if(tmpIValue < 1 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): Rx1Bal = " << tmpIValue;
            tmpIValue = 50;
        }
        ui->slRx1Bal->setValue(tmpIValue);
        pDsp->SetRx1Balance(tmpIValue);

        tmpIValue = settings.value("Rx2Bal", 50).toInt();
        if(tmpIValue < 1 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): Rx2Bal = " << tmpIValue;
            tmpIValue = 50;
        }
        ui->slRx2Bal->setValue(tmpIValue);
        pDsp->SetRx2Balance(tmpIValue);

        ui->pbSql->setChecked(settings.value("SqlOn", false).toBool());
        OnSql(ui->pbSql->isChecked());

        tmpIValue = settings.value("slSql", 0).toInt();
        if(tmpIValue < -160 || tmpIValue > 0)
        {
            qWarning() << "ExpertSDR: readSettings(): slSql = " << tmpIValue;
            tmpIValue = -80;
        }
        ui->slSql->setValue(tmpIValue);
        OnSql(tmpIValue);
        ui->pbGate->setChecked(settings.value("GateOn", false).toBool());
        tmpIValue = settings.value("slGate", 0).toInt();
        if(tmpIValue < -165 || tmpIValue > 0)
        {
            qWarning() << "ExpertSDR: readSettings(): slGate = " << tmpIValue;
            tmpIValue = 0;
        }
        OnGate(tmpIValue);

        tmpBValue = settings.value("CompOn", false).toBool();
        ui->pbComp->setChecked(tmpBValue);
        OnComp(tmpBValue);
        tmpIValue = settings.value("slComp", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 20)
        {
            qWarning() << "ExpertSDR: readSettings(): slComp = " << tmpIValue;
            tmpIValue = 0;
        }
        OnComp(tmpIValue);

        tmpBValue = settings.value("CpdrOn", false).toBool();
        ui->pbCpdr->setChecked(tmpBValue);
        OnCpdr(tmpBValue);
        tmpIValue = settings.value("slCpdr", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 10)
        {
            qWarning() << "ExpertSDR: readSettings(): slCpdr = " << tmpIValue;
            tmpIValue = 0;
        }
        OnCpdr(tmpIValue);

        tmpBValue = settings.value("OnRit", false).toBool();
        ui->pbRitOn->setChecked(tmpBValue);
        tmpIValue = settings.value("RitValue", 0).toInt();
        if(tmpIValue < -10000 || tmpIValue > 10000)
        {
            qWarning() << "ExpertSDR: readSettings(): OnRit = " << tmpIValue;
            tmpIValue = 0;
        }
        ui->sbRit->setValue(tmpIValue);

        ui->chbBreakIn->setChecked(settings.value("BreakIn", true).toBool());
        onBreakIn(ui->chbBreakIn->checkState());

        ui->pbMon->setChecked(settings.value("CwMonitor", false).toBool());
        ChangedCWMon(ui->pbMon->isChecked());

        tmpIValue = settings.value("CwDelay", 25).toInt();
        if(tmpIValue < 10 || tmpIValue > 5000)
        {
            qWarning() << "ExpertSDR: readSettings(): CwDelay = " << tmpIValue;
            tmpIValue = 25;
        }
        ui->sbCwDelay->setValue(tmpIValue);

        tmpIValue = settings.value("slSpectrumRate", 10).toInt();
        if(tmpIValue < 1 || tmpIValue > 30)
        {
            qWarning() << "ExpertSDR: readSettings(): slSpectrumRate = " << tmpIValue;
            tmpIValue = 10;
        }
        ui->slSr->setValue(tmpIValue);

        tmpIValue = settings.value("slWfallRate", 10).toInt();
        if(tmpIValue < 1 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): slWfallRate = " << tmpIValue;
            tmpIValue = 10;
        }
        ui->slWr->setValue(tmpIValue);
        pMenuFreqUpDown->clear();
        tmpIValue = settings.value("MenuFrequencyMemory_checked_index", 0).toInt();
        if(pFupDownAct->actions().count()!= 0)
            pFupDownAct->actions().at(tmpIValue)->setChecked(true);

        QPoint tmpPoint = settings.value("BandFilter", QPoint(300, 1000)).toPoint();
        pGraph->pGl->SetBandFilter(tmpPoint.x(), tmpPoint.y());
        tmpIValue = settings.value("Filter", 0).toInt();
        if(tmpIValue < -96000 || tmpIValue > 96000)
        {
            qWarning() << "ExpertSDR: readSettings(): Filter = " << tmpIValue;
            tmpIValue = 0;
        }
        FilterPosFreq = tmpIValue;
        pGraph->pGl->SetFilter(FilterPosFreq);

        tmpIValue = settings.value("Pitch", 750).toInt();
        if(tmpIValue < 50 || tmpIValue > 2500)
        {
            qWarning() << "ExpertSDR: readSettings(): Pitch = " << tmpIValue;
            tmpIValue = 750;
        }
        pGraph->pGl->SetPitch(tmpIValue);

        tmpIValue = settings.value("Frequency", 1700000).toInt();
        if(tmpIValue < 0 || tmpIValue > 440000000)
        {
            qWarning() << "ExpertSDR: readSettings(): Frequency = " << tmpIValue;
            tmpIValue = 1700000;
        }
        SetFreq(tmpIValue);

        tmpIValue = settings.value("VfoB", 7050000).toInt();
        if(tmpIValue < 0 || tmpIValue > 440000000)
        {
            qWarning() << "ExpertSDR: readSettings(): VfoB = " << tmpIValue;
            tmpIValue = 7050000;
        }
        splitOpt = settings.value("GraphSplitterPos", Splitter->saveState()).toByteArray();
        Splitter->restoreState(splitOpt);
        ui->pbMemPan->setChecked(settings.value("MemPanel", true).toBool());
        OnMemPanel(ui->pbMemPan->isChecked());

        tmpDValue = settings.value("panTxScale", 140.0).toDouble();
        if(tmpDValue < 10.0 || tmpDValue > 200.0)
        {
            qWarning() << "ExpertSDR: readSettings(): panTxScale = " << tmpDValue;
            tmpDValue = 100.0;
        }
        panTxScale = tmpDValue;
        tmpDValue = settings.value("panTxOffset", -100.0).toDouble();
        if(tmpDValue < -200 || tmpDValue > 50.0)
        {
            qWarning() << "ExpertSDR: readSettings():   = " << tmpDValue;
            tmpDValue = -100.0;
        }
        panTxOffset = tmpDValue;

        settings.beginGroup("BandOptions");
        tmpIValue = settings.value("CurrentBand", BAND160M).toInt();
        if(tmpIValue < 0 || tmpIValue > 13)
        {
            qWarning() << "ExpertSDR: readSettings(): CurrentBand = " << tmpIValue;
            tmpIValue = 0;
        }
        CurrentBandIndex = tmpIValue;
        crBand = tmpIValue;
        for(int iBand = 0; iBand < NUM_BANDS; iBand++)
        {
            int TmpBandFreq = 0;
            if(iBand == BAND160M) TmpBandFreq = BPF160_START;
            if(iBand == BAND80M)  TmpBandFreq = BPF80_START;
            if(iBand == BAND60M)  TmpBandFreq = BPF60_START;
            if(iBand == BAND40M)  TmpBandFreq = BPF40_START;
            if(iBand == BAND30M)  TmpBandFreq = BPF30_START;
            if(iBand == BAND20M)  TmpBandFreq = BPF20_START;
            if(iBand == BAND17M)  TmpBandFreq = BPF17_START;
            if(iBand == BAND15M)  TmpBandFreq = BPF15_START;
            if(iBand == BAND12M)  TmpBandFreq = BPF12_START;
            if(iBand == BAND10M)  TmpBandFreq = BPF10_START;
            if(iBand == BAND6M)   TmpBandFreq = BPF6_START;
            if(iBand == BAND2M)   TmpBandFreq = BPF2_START;
            if(iBand == BAND07M)  TmpBandFreq = BPF07_START;
            if(iBand == BANDGEN)  TmpBandFreq = BPF160_START;

            Str.setNum(iBand);
                settings.beginGroup("Band" + Str);
                    tmpIValue = settings.value("CurrentMode", 0).toInt();
                    if(tmpIValue < 0 || tmpIValue > 11)
                    {
                        qWarning() << "ExpertSDR: readSettings(): Band" + Str +": CurrentMode = " << tmpIValue;
                        tmpIValue = 0;
                    }
                    OptBands[iBand].CurrentModeIndex = tmpIValue;

                    tmpIValue = settings.value("BandFreq", TmpBandFreq).toInt();
                    switch((BAND_MODE)iBand)
                    {
                    case BAND160M:
                        if(tmpIValue < BPF160_START || tmpIValue > BPF160_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band160: BandFreq = " << tmpIValue;
                            tmpIValue = BPF160_START;
                        }
                        break;
                    case BAND80M:
                        if(tmpIValue < BPF80_START || tmpIValue > BPF80_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band80: BandFreq = " << tmpIValue;
                            tmpIValue = BPF80_START;
                        }
                        break;
                    case BAND60M:
                        if(tmpIValue < BPF60_START || tmpIValue > BPF60_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band60: BandFreq = " << tmpIValue;
                            tmpIValue = BPF60_START;
                        }
                        break;
                    case BAND40M:
                        if(tmpIValue < BPF40_START || tmpIValue > BPF40_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band40: BandFreq = " << tmpIValue;
                            tmpIValue = BPF40_START;
                        }
                        break;
                    case BAND30M:
                        if(tmpIValue < BPF30_START || tmpIValue > BPF30_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band30: BandFreq = " << tmpIValue;
                            tmpIValue = BPF30_START;
                        }
                        break;
                    case BAND20M:
                        if(tmpIValue < BPF20_START || tmpIValue > BPF20_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band20: BandFreq = " << tmpIValue;
                            tmpIValue = BPF20_START;
                        }
                        break;
                    case BAND17M:
                        if(tmpIValue < BPF17_START || tmpIValue > BPF17_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band17: BandFreq = " << tmpIValue;
                            tmpIValue = BPF17_START;
                        }
                        break;
                    case BAND15M:
                        if(tmpIValue < BPF15_START || tmpIValue > BPF15_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band15: BandFreq = " << tmpIValue;
                            tmpIValue = BPF15_START;
                        }
                        break;
                    case BAND12M:
                        if(tmpIValue < BPF12_START || tmpIValue > BPF12_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band12: BandFreq = " << tmpIValue;
                            tmpIValue = BPF12_START;
                        }
                        break;
                    case BAND10M:
                        if(tmpIValue < BPF10_START || tmpIValue > BPF10_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band10: BandFreq = " << tmpIValue;
                            tmpIValue = BPF10_START;
                        }
                        break;
                    case BAND6M:
                        if(tmpIValue < BPF6_START || tmpIValue > BPF6_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band6: BandFreq = " << tmpIValue;
                            tmpIValue = BPF6_START;
                        }
                        break;
                    case BAND2M:
                        if(tmpIValue < BPF2_START || tmpIValue > BPF2_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band2: BandFreq = " << tmpIValue;
                            tmpIValue = BPF2_START;
                        }
                        break;
                    case BAND07M:
                        if(tmpIValue < BPF07_START || tmpIValue > BPF07_END)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band07: BandFreq = " << tmpIValue;
                            tmpIValue = BPF07_START;
                        }
                        break;
                    case BANDGEN:
                        if(tmpIValue < 1000000 || tmpIValue > 440000000)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band60: BandFreq = " << tmpIValue;
                            tmpIValue = BPF160_START;
                        }
                        break;
                    default:
                        if(tmpIValue < 1000000 || tmpIValue > 440000000)
                        {
                            qWarning() << "ExpertSDR: readSettings(): Band?: BandFreq = " << tmpIValue;
                            tmpIValue = BPF160_START;
                        }
                        break;
                    }
                    OptBands[iBand].MainFreq = tmpIValue;
                    tmpIValue = settings.value("Pitch", 750).toInt();
                    if(tmpIValue < 50 || tmpIValue > 2500)
                    {
                        qWarning() << "ExpertSDR: readSettings(): Pitch = " << tmpIValue;
                        tmpIValue = 750;
                    }
                    OptBands[iBand].Pitch = tmpIValue;
                    tmpIValue = settings.value("slVolume", 70).toInt();
                    if(tmpIValue < 0 || tmpIValue > 100)
                    {
                        qWarning() << "ExpertSDR: readSettings(): slVolume = " << tmpIValue;
                        tmpIValue = 70;
                    }
                    OptBands[iBand].Volume = tmpIValue;
                    tmpIValue = settings.value("slAgc", 80).toInt();
                    if(tmpIValue < -20 || tmpIValue > 120)
                    {
                        qWarning() << "ExpertSDR: readSettings(): slAgc = " << tmpIValue;
                        tmpIValue = 80;
                    }
                    OptBands[iBand].RfGain = tmpIValue;

                    for(int iMode = 0; iMode < NUM_MODES; iMode++)
                    {
                        Str.setNum(iMode);
                        settings.beginGroup("Mode" + Str);
                            tmpIValue = settings.value("PreampIndex", 0).toInt();
                            if(tmpIValue < 0 || tmpIValue > 4)
                            {
                                qWarning() << "ExpertSDR: readSettings(): PreampIndex = " << tmpIValue;
                                tmpIValue = 0;
                            }
                            OptBands[iBand].Mode[iMode].cbPreampIndex = tmpIValue;

                            tmpIValue = settings.value("AgcIndex", 1).toInt();
                            if(tmpIValue < 0 || tmpIValue > 4)
                            {
                                qWarning() << "ExpertSDR: readSettings(): AgcIndex = " << tmpIValue;
                                tmpIValue = 1;
                            }
                            OptBands[iBand].Mode[iMode].cbAgcIndex = tmpIValue;
                            OptBands[iBand].Mode[iMode].BinEnable = settings.value("BinEnable", false).toBool();
                            OptBands[iBand].Mode[iMode].NrEnable = settings.value("NrEnable", false).toBool();
                            OptBands[iBand].Mode[iMode].AnfEnable = settings.value("AnfEnable", false).toBool();
                            OptBands[iBand].Mode[iMode].Nb1Enable = settings.value("Nb1Enable", false).toBool();
                            OptBands[iBand].Mode[iMode].Nb2Enable = settings.value("Nb2Enable",false).toBool();
                            OptBands[iBand].Mode[iMode].SqlEnable = settings.value("SqlEnable", false).toBool();
                            tmpIValue = settings.value("SqlValue", 0).toInt();
                            if(tmpIValue < -165 || tmpIValue > 0)
                            {
                                qWarning() << "ExpertSDR: readSettings(): SqlValue = " << tmpIValue;
                                tmpIValue = 0;
                            }
                            OptBands[iBand].Mode[iMode].SqlValue = tmpIValue;
                            OptBands[iBand].Mode[iMode].GateEnable = settings.value("GateEnable", false).toBool();
                            tmpIValue = settings.value("GateValue", 0).toInt();
                            if(tmpIValue < -165 || tmpIValue > 0)
                            {
                                qWarning() << "ExpertSDR: readSettings(): GateValue = " << tmpIValue;
                                tmpIValue = 0;
                            }
                            OptBands[iBand].Mode[iMode].GateValue = tmpIValue;
                                OptBands[iBand].Mode[iMode].CompEnable = settings.value("CompEnable", false).toBool();
                            tmpIValue = settings.value("CompValue", 0).toInt();
                            if(tmpIValue < 0 || tmpIValue > 20)
                            {
                                qWarning() << "ExpertSDR: readSettings(): CompValue = " << tmpIValue;
                                tmpIValue = 0;
                            }
                            OptBands[iBand].Mode[iMode].CompValue = tmpIValue;
                            OptBands[iBand].Mode[iMode].CpdrEnable = settings.value("CpdrEnable", false).toBool();
                            tmpIValue = settings.value("CpdrValue", 0).toInt();
                            if(tmpIValue < 0 || tmpIValue > 10)
                            {
                                qWarning() << "ExpertSDR: readSettings(): CpdrValue = " << tmpIValue;
                                tmpIValue = 0;
                            }
                            OptBands[iBand].Mode[iMode].CpdrValue = tmpIValue;
                            tmpIValue = settings.value("FilterIndex", 4).toInt();
                            if(tmpIValue < 0 || tmpIValue > 8)
                            {
                                qWarning() << "ExpertSDR: readSettings(): FilterIndex = " << tmpIValue;
                                tmpIValue = 4;
                            }
                            OptBands[iBand].Mode[iMode].FilterIndex = tmpIValue;
                            tmpIValue  = settings.value("VarFilterH", 1000).toInt();
                            if(tmpIValue < -20000 || tmpIValue > 20000)
                            {
                                qWarning() << "ExpertSDR: readSettings(): VarFilterH = " << tmpIValue;
                                tmpIValue = 1000;
                            }
                            OptBands[iBand].Mode[iMode].VarFilterH  = tmpIValue;
                            tmpIValue  = settings.value("VarFilterL", -1000).toInt();
                            if(tmpIValue < -20000 || tmpIValue > 20000)
                            {
                                qWarning() << "ExpertSDR: readSettings(): VarFilterL = " << tmpIValue;
                                tmpIValue = 1000;
                            }
                            OptBands[iBand].Mode[iMode].VarFilterL  = tmpIValue;
                        settings.endGroup();
                    }
                settings.endGroup();
            }
        settings.endGroup();
        vfoBVal[0] = settings.value("vfoBval0", 7100000).toInt();
        vfoBVal[1] = settings.value("vfoBval1", 145500000).toInt();
        vfoBVal[2] = settings.value("vfoBval2", 433000000).toInt();
        pVfoB->setValue(vfoBVal[0]);

        numBandVfoB = settings.value("numBandVfoB", 0).toInt();
        if(numBandVfoB < 0 || numBandVfoB > 2)
        {
            qWarning() << "ExpertSDR: readSettings(): numBandVfoB = " << numBandVfoB;
            numBandVfoB = 0;
        }
        if(numBandVfoB == 1)
        {
            pVfoB->setMinValue(BPF2_START);
            pVfoB->setMaxValue(BPF2_END);
        }
        else if(numBandVfoB == 2)
        {
            pVfoB->setMinValue(BPF07_START);
            pVfoB->setMaxValue(BPF07_END);
        }
        else
        {
            pVfoB->setMinValue(0);
            pVfoB->setMaxValue(65000000);
        }
        pVfoB->setValue(vfoBVal[numBandVfoB]);

    settings.endGroup();

    OnVolume(OptBands[CurrentBandIndex].Volume);
    OnAgc(OptBands[CurrentBandIndex].RfGain);

    settings.beginGroup("SmeterInMainWindow");
        tmpIValue = settings.value("TxType", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 2)
        {
            qWarning() << "ExpertSDR: readSettings(): SmeterTxType = " << tmpIValue;
            tmpIValue = 0;
        }
        pSmeter->SetTxType(tmpIValue);
        tmpIValue = settings.value("RxType", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 2)
        {
            qWarning() << "ExpertSDR: readSettings(): SmeterRxType = " << tmpIValue;
            tmpIValue = 0;
        }
        pSmeter->SetRxType(tmpIValue);
    settings.endGroup();

    settings.beginGroup("OptionsWindow");
        pOpt->move(settings.value("Position", QPoint(200, 200)).toPoint());

        int SDRPluginIndex = settings.value("SDRPluginIndex", 0).toInt();
        pOpt->ui.cbSdrType->setCurrentIndex(SDRPluginIndex);
        pOpt->onSdrTypeChanged(SDRPluginIndex);

        tmpIValue = settings.value("Audio_Driver", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 1)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_Driver = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaDriver->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Audio_Input", 0).toInt();
        if(tmpIValue < 0)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_Input = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaIn->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Audio_Output", 0).toInt();
        if(tmpIValue < 0)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_Output = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaOut->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Audio_Channels", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 1)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_Channels = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaChannels->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Audio_BufferSize", 4).toInt();
        if(tmpIValue < 0 || tmpIValue > 4)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_BufferSize = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaBufferSize->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Audio_SampleRate", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 2)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_SampleRate = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaSampleRate->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Audio_Lattency", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 99)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_Lattency = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbPaLattency->setValue(tmpIValue);
        tmpIValue = settings.value("Audio_MicInGain", 10).toInt();
        if(tmpIValue < 0 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_MicInGain = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbMicInGain->setValue(tmpIValue);
        OnMicInGain(tmpIValue);
        tmpIValue = settings.value("Audio_LineInGain", 100).toInt();
        if(tmpIValue < 0 || tmpIValue > 100)
        {
            qWarning() << "ExpertSDR: readSettings(): Audio_LineInGain = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbLineInGain->setValue(tmpIValue);
        OnLineInGain(tmpIValue);
        pOpt->ui.chbVacEnable->setChecked(settings.value("Vac_Enable", false).toBool());
        ui->pbVac->setChecked(pOpt->ui.chbVacEnable->isChecked());
        tmpIValue = settings.value("Vac_Driver", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 1)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_Driver = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaVacDriver->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Vac_Input", 0).toInt();
        if(tmpIValue < 0)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_Input = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaVacIn->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Vac_Output", 0).toInt();
        if(tmpIValue < 0)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_Output = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.cbPaVacOut->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Vac_BufferSize", 2).toInt();
        if(tmpIValue < 0 || tmpIValue > 3)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_BufferSize = " << tmpIValue;
            tmpIValue = 2;
        }
        pOpt->ui.cbPaVacBufferSize->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Vac_SampleRate", 3).toInt();
        if(tmpIValue < 0 || tmpIValue > 6)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_SampleRate = " << tmpIValue;
            tmpIValue = 3;
        }
        pOpt->ui.cbPaVacSampleRate->setCurrentIndex(tmpIValue);
        tmpIValue = settings.value("Vac_Lattency", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 99)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_Lattency = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbPaVacLattency->setValue(tmpIValue);
        tmpIValue = settings.value("Vac_TxGain", 0).toInt();
        if(tmpIValue < -45 || tmpIValue > 50)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_TxGain = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbVacTxGain->setValue(tmpIValue);
        tmpIValue = settings.value("Vac_RxGain", 0).toInt();
        if(tmpIValue < -45 || tmpIValue > 20)
        {
            qWarning() << "ExpertSDR: readSettings(): Vac_RxGain = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbVacRxGain->setValue(tmpIValue);

        pOpt->ui.sbNrTaps->setValue(settings.value("NR_Taps", 45).toInt());
        pOpt->ui.sbNrDelay->setValue(settings.value("NR_Delay", 64).toInt());
        pOpt->ui.sbNrGain->setValue(settings.value("NR_Gain", 16).toInt());
        pOpt->ui.sbNrLeak->setValue(settings.value("NR_Leak", 10).toInt());
        pOpt->ui.sbAnfTaps->setValue(settings.value("ANF_Taps", 31).toInt());
        pOpt->ui.sbAnfDelay->setValue(settings.value("ANF_Delay", 64).toInt());
        pOpt->ui.sbAnfGain->setValue(settings.value("ANF_Gain", 10).toInt());
        pOpt->ui.sbAnfLeak->setValue(settings.value("ANF_Leak", 1).toInt());
        pOpt->ui.sbNb1Thr->setValue(settings.value("NB1_Thr", 20).toInt());
        pOpt->ui.sbNb2Thr->setValue(settings.value("NB2_Thr", 15).toInt());
        pOpt->ui.sbAlcAttak->setValue(settings.value("Alc_Attak", 2).toInt());
        pOpt->ui.sbAlcDecay->setValue(settings.value("Alc_Decay", 10).toInt());
        pOpt->ui.sbAlcHang->setValue(settings.value("Alc_Hang", 500).toInt());
        pOpt->ui.sbAgcSlope->setValue(settings.value("Agc_slope", 0).toInt());
        OnOptAgcSlope(pOpt->ui.sbAgcSlope->value());
        pOpt->ui.sbAgcGain->setValue(settings.value("Agc_Maxgain", 90).toInt());
        pOpt->ui.sbAgcAttak->setValue(settings.value("Agc_Attak", 2).toInt());
        OnOptAgcAttak(pOpt->ui.sbAgcAttak->value());
        pOpt->ui.sbAgcDecay->setValue(settings.value("Agc_Decay", 500).toInt());
        OnOptAgcDecay(pOpt->ui.sbAgcDecay->value());
        pOpt->ui.sbAgcHang->setValue(settings.value("Agc_Hang", 500).toInt());
        OnOptAgcHang(pOpt->ui.sbAgcHang->value());
        pOpt->ui.sbAgcHangThresh->setValue(settings.value("Agc_HangThresh", 0).toInt());
        OnOptAgcHangThr(pOpt->ui.sbAgcHangThresh->value());
        pOpt->ui.sbAgcFixGain->setValue(settings.value("Agc_FixGain", 70).toInt());
        pOpt->ui.cbWinType->setCurrentIndex(settings.value("Window_type", 11).toInt());
        pOpt->ui.chbPoliphase->setChecked(settings.value("Polyphase_FFT", false).toBool());
        OnSetPolyphase(pOpt->ui.chbPoliphase->checkState());
        pOpt->ui.sbTxOutFilterFreqSsb->setValue(settings.value("TxFilterHighFssb", 3000).toInt());
        pOpt->ui.sbTxInFilterFreqSsb->setValue(settings.value("TxFilterLowFssb", 100).toInt());
        pOpt->ui.sbTxOutFilterFreqDig->setValue(settings.value("TxFilterHighFdig", 3000).toInt());
        pOpt->ui.sbTxInFilterFreqDig->setValue(settings.value("TxFilterLowFdig", 100).toInt());
        pOpt->ui.sbTxOutFilterFreqAm->setValue(settings.value("TxFilterHighFam", 3000).toInt());
        pOpt->ui.sbTxOutFilterFreqFmn->setValue(settings.value("TxFilterHighFfmn", 3000).toInt());
        pOpt->ui.cbTxDspBufSize->setCurrentIndex(settings.value("cbTxDspBufSize", 3).toInt());
        pOpt->ui.cbRxDspBufSize->setCurrentIndex(settings.value("cbRxDspBufSize", 3).toInt());
        pOpt->ui.sbTxCarrier->setValue(settings.value("sbAmCarrier", 90).toInt());
        OnAmCarrier(pOpt->ui.sbTxCarrier->value());
        pOpt->ui.sbDigL->setValue(settings.value("sbDigL", 2200).toInt());
        pOpt->ui.sbDigU->setValue(settings.value("sbDigU", 1200).toInt());
        pOpt->ui.chbIambic->setChecked(settings.value("chbIambicKeyer", true).toBool());
        pOpt->ui.chbDisMonitor->setChecked(settings.value("chbDisMonitorKeyer", false).toBool());
        pOpt->ui.chbRevPaddle->setChecked(settings.value("chbRevPaddleKeyer", false).toBool());
       pCwMacro->pCwCore->swapKeys(pOpt->ui.chbRevPaddle->checkState());
        pOpt->ui.chbHighRes->setChecked(settings.value("chbHighResKeyer", false).toBool());
        pOpt->ui.chbModeB->setChecked(settings.value("chbModeBKeyer", false).toBool());
        pOpt->ui.sbWeight->setValue(settings.value("sbWeightKeyer", 50).toInt());
        pOpt->ui.sbRamp->setValue(settings.value("sbRampKeyer", 5).toInt());
        ui->sbSWSpeed->setValue(settings.value("sbCWSpeed", 25).toInt());
        ui->sbCwDelay->setValue(settings.value("sbDelayTxOut", 200).toInt());
        pCwMacro->pCwCore->setWeight(pOpt->ui.sbWeight->value());
        pCwMacro->pCwCore->setRamp(pOpt->ui.sbRamp->value());
        pCwMacro->pCwCore->setSpeed(ui->sbSWSpeed->value());
        pCwMacro->pCwCore->setBreakInDelay(ui->sbCwDelay->value());
        pOpt->ui.chbKeyEnable->setChecked(settings.value("SecKeyEnable", false).toBool());
        pOpt->ui.cbKeyPortName->setCurrentIndex(settings.value("SecKeyPortName", 0).toInt());
        pOpt->ui.cbKeyPttLine->setCurrentIndex(settings.value("SecKeyPttLine", 0).toInt());
        pOpt->ui.cbKeyKeyLine->setCurrentIndex(settings.value("SecKeyKeyLine", 0).toInt());
        if(pOpt->ui.chbKeyEnable->isChecked())
            pOpt->keyOpen(true);
        pOpt->ui.chbAddKeyEnable->setChecked(settings.value("AddKeyEnable", false).toBool());
        pOpt->ui.cbAddKeyPortName->setCurrentIndex(settings.value("AddKeyPortName", 0).toInt());
        pOpt->ui.cbAddKeys->setCurrentIndex(settings.value("AddKeyPortDtrRts", 0).toInt());
        if(pOpt->ui.chbAddKeyEnable->isChecked())
            pOpt->addKeyOpen(true);
        pCwMacro->pCwCore->setAutoMode(pOpt->ui.chbIambic->checkState());
        pOpt->SetProg0(settings.value("cbProg0", false).toBool(), settings.value("Prog0", "").toString());
        pOpt->SetProg1(settings.value("cbProg1", false).toBool(), settings.value("Prog1", "").toString());
        pOpt->SetProg2(settings.value("cbProg2", false).toBool(), settings.value("Prog2", "").toString());
        pOpt->SetProg3(settings.value("cbProg3", false).toBool(), settings.value("Prog3", "").toString());
        pOpt->SetProg4(settings.value("cbProg4", false).toBool(), settings.value("Prog4", "").toString());
        ui->slSr->setValue(settings.value("SpRate", 10).toInt());
        ui->slWr->setValue(settings.value("WfRate", 30).toInt());
        pGraph->pGl->SetSpectrRate(ui->slSr->value());
        pGraph->pGl->SetWaterfallRate(ui->slWr->value());
        pOpt->ui.chbCatEnable->setChecked(settings.value("CatOn", false).toBool());
        pOpt->ui.cbCatPortName->setCurrentIndex(settings.value("CatPortName", 0).toInt());
        pOpt->ui.cbBaudRate->setCurrentIndex(settings.value("CatBoudrate", 0).toInt());
        pOpt->ui.cbCatStopBits->setCurrentIndex(settings.value("CatStopBits", 0).toInt());
        pOpt->ui.cbCatParity->setCurrentIndex(settings.value("CatParity", 0).toInt());
        pOpt->ui.cbCatData->setCurrentIndex(settings.value("CatData", 3).toInt());
        pOpt->ui.cbCatPttLine->setCurrentIndex(settings.value("CatPttLine", 0).toInt());
        pOpt->ui.cbCatKeyLine->setCurrentIndex(settings.value("CatKeyLine", 0).toInt());
        pCatManager->Open(pOpt->ui.chbCatEnable->isChecked());
        pOpt->ui.chbPttEnable->setChecked(settings.value("PttEnable", false).toBool());
        pOpt->ui.cbPttPortName->setCurrentIndex(settings.value("PttPortName", 0).toInt());
        pOpt->ui.chbPttDtr->setChecked(settings.value("PortDtr", false).toBool());
        pOpt->ui.chbPttRts->setChecked(settings.value("PortRts", false).toBool());
        pOpt->pttOpen(pOpt->ui.chbPttEnable->isChecked());
        pOpt->ui.sbColibrFreqNum->setValue(CalibrateFreq);
        pOpt->ui.chbSwapLineIn->setChecked(settings.value("SwapLineIn", false).toBool());
        pOpt->ui.chbSwapLineOut->setChecked(settings.value("SwapLineOut", false).toBool());
        ChangeSwapLineIn((int)pOpt->ui.chbSwapLineIn->isChecked());
        ChangeSwapLineOut((int)pOpt->ui.chbSwapLineOut->isChecked());
        ui->cbPanInfo->setChecked(settings.value("PanInfo", false).toBool());
        pOpt->ui.chbExtCtrl->setChecked(settings.value("chbExControl", false).toBool());
        pOpt->ui.chb160Rx1->setChecked(settings.value("chb160Rx1", false).toBool());
        pOpt->ui.chb160Rx2->setChecked(settings.value("chb160Rx2", false).toBool());
        pOpt->ui.chb160Rx3->setChecked(settings.value("chb160Rx3", false).toBool());
        pOpt->ui.chb160Rx4->setChecked(settings.value("chb160Rx4", false).toBool());
        pOpt->ui.chb160Rx5->setChecked(settings.value("chb160Rx5", false).toBool());
        pOpt->ui.chb160Rx6->setChecked(settings.value("chb160Rx6", false).toBool());
        pOpt->ui.chb80Rx1->setChecked(settings.value("chb80Rx1", false).toBool());
        pOpt->ui.chb80Rx2->setChecked(settings.value("chb80Rx2", false).toBool());
        pOpt->ui.chb80Rx3->setChecked(settings.value("chb80Rx3", false).toBool());
        pOpt->ui.chb80Rx4->setChecked(settings.value("chb80Rx4", false).toBool());
        pOpt->ui.chb80Rx5->setChecked(settings.value("chb80Rx5", false).toBool());
        pOpt->ui.chb80Rx6->setChecked(settings.value("chb80Rx6", false).toBool());
        pOpt->ui.chb60Rx1->setChecked(settings.value("chb60Rx1", false).toBool());
        pOpt->ui.chb60Rx2->setChecked(settings.value("chb60Rx2", false).toBool());
        pOpt->ui.chb60Rx3->setChecked(settings.value("chb60Rx3", false).toBool());
        pOpt->ui.chb60Rx4->setChecked(settings.value("chb60Rx4", false).toBool());
        pOpt->ui.chb60Rx5->setChecked(settings.value("chb60Rx5", false).toBool());
        pOpt->ui.chb60Rx6->setChecked(settings.value("chb60Rx6", false).toBool());
        pOpt->ui.chb40Rx1->setChecked(settings.value("chb40Rx1", false).toBool());
        pOpt->ui.chb40Rx2->setChecked(settings.value("chb40Rx2", false).toBool());
        pOpt->ui.chb40Rx3->setChecked(settings.value("chb40Rx3", false).toBool());
        pOpt->ui.chb40Rx4->setChecked(settings.value("chb40Rx4", false).toBool());
        pOpt->ui.chb40Rx5->setChecked(settings.value("chb40Rx5", false).toBool());
        pOpt->ui.chb40Rx6->setChecked(settings.value("chb40Rx6", false).toBool());
        pOpt->ui.chb30Rx1->setChecked(settings.value("chb30Rx1", false).toBool());
        pOpt->ui.chb30Rx2->setChecked(settings.value("chb30Rx2", false).toBool());
        pOpt->ui.chb30Rx3->setChecked(settings.value("chb30Rx3", false).toBool());
        pOpt->ui.chb30Rx4->setChecked(settings.value("chb30Rx4", false).toBool());
        pOpt->ui.chb30Rx5->setChecked(settings.value("chb30Rx5", false).toBool());
        pOpt->ui.chb30Rx6->setChecked(settings.value("chb30Rx6", false).toBool());
        pOpt->ui.chb20Rx1->setChecked(settings.value("chb20Rx1", false).toBool());
        pOpt->ui.chb20Rx2->setChecked(settings.value("chb20Rx2", false).toBool());
        pOpt->ui.chb20Rx3->setChecked(settings.value("chb20Rx3", false).toBool());
        pOpt->ui.chb20Rx4->setChecked(settings.value("chb20Rx4", false).toBool());
        pOpt->ui.chb20Rx5->setChecked(settings.value("chb20Rx5", false).toBool());
        pOpt->ui.chb20Rx6->setChecked(settings.value("chb20Rx6", false).toBool());
        pOpt->ui.chb17Rx1->setChecked(settings.value("chb17Rx1", false).toBool());
        pOpt->ui.chb17Rx2->setChecked(settings.value("chb17Rx2", false).toBool());
        pOpt->ui.chb17Rx3->setChecked(settings.value("chb17Rx3", false).toBool());
        pOpt->ui.chb17Rx4->setChecked(settings.value("chb17Rx4", false).toBool());
        pOpt->ui.chb17Rx5->setChecked(settings.value("chb17Rx5", false).toBool());
        pOpt->ui.chb17Rx6->setChecked(settings.value("chb17Rx6", false).toBool());
        pOpt->ui.chb15Rx1->setChecked(settings.value("chb15Rx1", false).toBool());
        pOpt->ui.chb15Rx2->setChecked(settings.value("chb15Rx2", false).toBool());
        pOpt->ui.chb15Rx3->setChecked(settings.value("chb15Rx3", false).toBool());
        pOpt->ui.chb15Rx4->setChecked(settings.value("chb15Rx4", false).toBool());
        pOpt->ui.chb15Rx5->setChecked(settings.value("chb15Rx5", false).toBool());
        pOpt->ui.chb15Rx6->setChecked(settings.value("chb15Rx6", false).toBool());
        pOpt->ui.chb12Rx1->setChecked(settings.value("chb12Rx1", false).toBool());
        pOpt->ui.chb12Rx2->setChecked(settings.value("chb12Rx2", false).toBool());
        pOpt->ui.chb12Rx3->setChecked(settings.value("chb12Rx3", false).toBool());
        pOpt->ui.chb12Rx4->setChecked(settings.value("chb12Rx4", false).toBool());
        pOpt->ui.chb12Rx5->setChecked(settings.value("chb12Rx5", false).toBool());
        pOpt->ui.chb12Rx6->setChecked(settings.value("chb12Rx6", false).toBool());
        pOpt->ui.chb10Rx1->setChecked(settings.value("chb10Rx1", false).toBool());
        pOpt->ui.chb10Rx2->setChecked(settings.value("chb10Rx2", false).toBool());
        pOpt->ui.chb10Rx3->setChecked(settings.value("chb10Rx3", false).toBool());
        pOpt->ui.chb10Rx4->setChecked(settings.value("chb10Rx4", false).toBool());
        pOpt->ui.chb10Rx5->setChecked(settings.value("chb10Rx5", false).toBool());
        pOpt->ui.chb10Rx6->setChecked(settings.value("chb10Rx6", false).toBool());
        pOpt->ui.chb6Rx1->setChecked(settings.value("chb6Rx1", false).toBool());
        pOpt->ui.chb6Rx2->setChecked(settings.value("chb6Rx2", false).toBool());
        pOpt->ui.chb6Rx3->setChecked(settings.value("chb6Rx3", false).toBool());
        pOpt->ui.chb6Rx4->setChecked(settings.value("chb6Rx4", false).toBool());
        pOpt->ui.chb6Rx5->setChecked(settings.value("chb6Rx5", false).toBool());
        pOpt->ui.chb6Rx6->setChecked(settings.value("chb6Rx6", false).toBool());
        pOpt->ui.chb2Rx1->setChecked(settings.value("chb2Rx1", false).toBool());
        pOpt->ui.chb2Rx2->setChecked(settings.value("chb2Rx2", false).toBool());
        pOpt->ui.chb2Rx3->setChecked(settings.value("chb2Rx3", false).toBool());
        pOpt->ui.chb2Rx4->setChecked(settings.value("chb2Rx4", false).toBool());
        pOpt->ui.chb2Rx5->setChecked(settings.value("chb2Rx5", false).toBool());
        pOpt->ui.chb2Rx6->setChecked(settings.value("chb2Rx6", false).toBool());
        pOpt->ui.chb07Rx1->setChecked(settings.value("chb07Rx1", false).toBool());
        pOpt->ui.chb07Rx2->setChecked(settings.value("chb07Rx2", false).toBool());
        pOpt->ui.chb07Rx3->setChecked(settings.value("chb07Rx3", false).toBool());
        pOpt->ui.chb07Rx4->setChecked(settings.value("chb07Rx4", false).toBool());
        pOpt->ui.chb07Rx5->setChecked(settings.value("chb07Rx5", false).toBool());
        pOpt->ui.chb07Rx6->setChecked(settings.value("chb07Rx6", false).toBool());
        pOpt->ui.chb160Tx1->setChecked(settings.value("chb160Tx1", false).toBool());
        pOpt->ui.chb160Tx2->setChecked(settings.value("chb160Tx2", false).toBool());
        pOpt->ui.chb160Tx3->setChecked(settings.value("chb160Tx3", false).toBool());
        pOpt->ui.chb160Tx4->setChecked(settings.value("chb160Tx4", false).toBool());
        pOpt->ui.chb160Tx5->setChecked(settings.value("chb160Tx5", false).toBool());
        pOpt->ui.chb160Tx6->setChecked(settings.value("chb160Tx6", false).toBool());
        pOpt->ui.chb80Tx1->setChecked(settings.value("chb80Tx1", false).toBool());
        pOpt->ui.chb80Tx2->setChecked(settings.value("chb80Tx2", false).toBool());
        pOpt->ui.chb80Tx3->setChecked(settings.value("chb80Tx3", false).toBool());
        pOpt->ui.chb80Tx4->setChecked(settings.value("chb80Tx4", false).toBool());
        pOpt->ui.chb80Tx5->setChecked(settings.value("chb80Tx5", false).toBool());
        pOpt->ui.chb80Tx6->setChecked(settings.value("chb80Tx6", false).toBool());
        pOpt->ui.chb60Tx1->setChecked(settings.value("chb60Tx1", false).toBool());
        pOpt->ui.chb60Tx2->setChecked(settings.value("chb60Tx2", false).toBool());
        pOpt->ui.chb60Tx3->setChecked(settings.value("chb60Tx3", false).toBool());
        pOpt->ui.chb60Tx4->setChecked(settings.value("chb60Tx4", false).toBool());
        pOpt->ui.chb60Tx5->setChecked(settings.value("chb60Tx5", false).toBool());
        pOpt->ui.chb60Tx6->setChecked(settings.value("chb60Tx6", false).toBool());
        pOpt->ui.chb40Tx1->setChecked(settings.value("chb40Tx1", false).toBool());
        pOpt->ui.chb40Tx2->setChecked(settings.value("chb40Tx2", false).toBool());
        pOpt->ui.chb40Tx3->setChecked(settings.value("chb40Tx3", false).toBool());
        pOpt->ui.chb40Tx4->setChecked(settings.value("chb40Tx4", false).toBool());
        pOpt->ui.chb40Tx5->setChecked(settings.value("chb40Tx5", false).toBool());
        pOpt->ui.chb40Tx6->setChecked(settings.value("chb40Tx6", false).toBool());
        pOpt->ui.chb30Tx1->setChecked(settings.value("chb30Tx1", false).toBool());
        pOpt->ui.chb30Tx2->setChecked(settings.value("chb30Tx2", false).toBool());
        pOpt->ui.chb30Tx3->setChecked(settings.value("chb30Tx3", false).toBool());
        pOpt->ui.chb30Tx4->setChecked(settings.value("chb30Tx4", false).toBool());
        pOpt->ui.chb30Tx5->setChecked(settings.value("chb30Tx5", false).toBool());
        pOpt->ui.chb30Tx6->setChecked(settings.value("chb30Tx6", false).toBool());
        pOpt->ui.chb20Tx1->setChecked(settings.value("chb20Tx1", false).toBool());
        pOpt->ui.chb20Tx2->setChecked(settings.value("chb20Tx2", false).toBool());
        pOpt->ui.chb20Tx3->setChecked(settings.value("chb20Tx3", false).toBool());
        pOpt->ui.chb20Tx4->setChecked(settings.value("chb20Tx4", false).toBool());
        pOpt->ui.chb20Tx5->setChecked(settings.value("chb20Tx5", false).toBool());
        pOpt->ui.chb20Tx6->setChecked(settings.value("chb20Tx6", false).toBool());
        pOpt->ui.chb17Tx1->setChecked(settings.value("chb17Tx1", false).toBool());
        pOpt->ui.chb17Tx2->setChecked(settings.value("chb17Tx2", false).toBool());
        pOpt->ui.chb17Tx3->setChecked(settings.value("chb17Tx3", false).toBool());
        pOpt->ui.chb17Tx4->setChecked(settings.value("chb17Tx4", false).toBool());
        pOpt->ui.chb17Tx5->setChecked(settings.value("chb17Tx5", false).toBool());
        pOpt->ui.chb17Tx6->setChecked(settings.value("chb17Tx6", false).toBool());
        pOpt->ui.chb15Tx1->setChecked(settings.value("chb15Tx1", false).toBool());
        pOpt->ui.chb15Tx2->setChecked(settings.value("chb15Tx2", false).toBool());
        pOpt->ui.chb15Tx3->setChecked(settings.value("chb15Tx3", false).toBool());
        pOpt->ui.chb15Tx4->setChecked(settings.value("chb15Tx4", false).toBool());
        pOpt->ui.chb15Tx5->setChecked(settings.value("chb15Tx5", false).toBool());
        pOpt->ui.chb15Tx6->setChecked(settings.value("chb15Tx6", false).toBool());
        pOpt->ui.chb12Tx1->setChecked(settings.value("chb12Tx1", false).toBool());
        pOpt->ui.chb12Tx2->setChecked(settings.value("chb12Tx2", false).toBool());
        pOpt->ui.chb12Tx3->setChecked(settings.value("chb12Tx3", false).toBool());
        pOpt->ui.chb12Tx4->setChecked(settings.value("chb12Tx4", false).toBool());
        pOpt->ui.chb12Tx5->setChecked(settings.value("chb12Tx5", false).toBool());
        pOpt->ui.chb12Tx6->setChecked(settings.value("chb12Tx6", false).toBool());
        pOpt->ui.chb10Tx1->setChecked(settings.value("chb10Tx1", false).toBool());
        pOpt->ui.chb10Tx2->setChecked(settings.value("chb10Tx2", false).toBool());
        pOpt->ui.chb10Tx3->setChecked(settings.value("chb10Tx3", false).toBool());
        pOpt->ui.chb10Tx4->setChecked(settings.value("chb10Tx4", false).toBool());
        pOpt->ui.chb10Tx5->setChecked(settings.value("chb10Tx5", false).toBool());
        pOpt->ui.chb10Tx6->setChecked(settings.value("chb10Tx6", false).toBool());
        pOpt->ui.chb6Tx1->setChecked(settings.value("chb6Tx1", false).toBool());
        pOpt->ui.chb6Tx2->setChecked(settings.value("chb6Tx2", false).toBool());
        pOpt->ui.chb6Tx3->setChecked(settings.value("chb6Tx3", false).toBool());
        pOpt->ui.chb6Tx4->setChecked(settings.value("chb6Tx4", false).toBool());
        pOpt->ui.chb6Tx5->setChecked(settings.value("chb6Tx5", false).toBool());
        pOpt->ui.chb6Tx6->setChecked(settings.value("chb6Tx6", false).toBool());
        pOpt->ui.chb2Tx1->setChecked(settings.value("chb2Tx1", false).toBool());
        pOpt->ui.chb2Tx2->setChecked(settings.value("chb2Tx2", false).toBool());
        pOpt->ui.chb2Tx3->setChecked(settings.value("chb2Tx3", false).toBool());
        pOpt->ui.chb2Tx4->setChecked(settings.value("chb2Tx4", false).toBool());
        pOpt->ui.chb2Tx5->setChecked(settings.value("chb2Tx5", false).toBool());
        pOpt->ui.chb2Tx6->setChecked(settings.value("chb2Tx6", false).toBool());
        pOpt->ui.chb07Tx1->setChecked(settings.value("chb07Tx1", false).toBool());
        pOpt->ui.chb07Tx2->setChecked(settings.value("chb07Tx2", false).toBool());
        pOpt->ui.chb07Tx3->setChecked(settings.value("chb07Tx3", false).toBool());
        pOpt->ui.chb07Tx4->setChecked(settings.value("chb07Tx4", false).toBool());
        pOpt->ui.chb07Tx5->setChecked(settings.value("chb07Tx5", false).toBool());
        pOpt->ui.chb07Tx6->setChecked(settings.value("chb07Tx6", false).toBool());

        pGraph->pGl->pPanOpt->readSettings(&settings);
//        pPanel->readSettings(&settings);
        pCwMacro->readSettings(&settings);
        pSmeter->restoreSettings(&settings);
        pMem->readSettings(&settings);
        pathTmp = settings.value("CalibrationPath", QDir::homePath()).toString();
        pOpt->setWaveFilesDirLocation(settings.value("waveFileDefaultPath", "").toString());
        pGraph->pGl->readSettings(&settings);
        isGenTx = settings.value("GenTx", false).toBool();

        pOpt->ui.chbPaTxDelay->setChecked(settings.value("RampTxDelayEnable", false).toBool());
        tmpIValue = settings.value("RampTxDelayValue", 0).toInt();
        if(tmpIValue < 0 || tmpIValue > 1000)
        {
            qWarning() << "ExpertSDR: readSettings(): RampTxDelayValue = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbPaTxDelayValue->setValue(tmpIValue);
        tmpIValue = settings.value("VolumeVoiceTxValue", 0).toInt();
        if(tmpIValue < pOpt->ui.sbVoiceVolumeTx->minimum() || tmpIValue > pOpt->ui.sbVoiceVolumeTx->maximum())
        {
            qWarning() << "ExpertSDR: readSettings(): VolumeVoiceTxValue = " << tmpIValue;
            tmpIValue = 0;
        }
        pOpt->ui.sbVoiceVolumeTx->setValue(tmpIValue);
    settings.endGroup();

    pGraph->pGl->SetSampleRate(48000<<pOpt->ui.cbPaSampleRate->currentIndex());
    pBandBut->button(CurrentBandIndex)->setChecked(true);
    OnChangeBand(CurrentBandIndex);
    pGraph->pGl->SetFilter2(-pGraph->pGl->GetDDSFreq() + pVfoB->value());
}

void ExpertSDR_vA2_1::defaultSettings()
{
    QMessageBox msgBox(pMem);
    msgBox.setText("The options is set to default.                      ");
    msgBox.setInformativeText("Do you want to set options to default?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.resize(300, 100);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok)
    {
        OnStart(false);
        if(!QFile::remove(QDir::homePath()+"/ExpertSDR/Options.ini"))
        {
            QMessageBox::warning(pMem, tr("ExpertSDR warning"), tr("Can't remove settings file Options.ini.\n"), QMessageBox::Ok);
            qWarning() << "ExpertSDR: defaultSettings(): Can't remove settings file Options.ini!";
            return;
        }
        readSettings();
    }
}

void ExpertSDR_vA2_1::defaultCalibrations()
{
     QMessageBox msgBox(pMem);
     msgBox.setText("The options is set to default.                      ");
     msgBox.setInformativeText("Do you want to set calibrations to default?");
     msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
     msgBox.setDefaultButton(QMessageBox::Cancel);
     msgBox.setIcon(QMessageBox::Information);
     msgBox.resize(300, 100);
     int ret = msgBox.exec();
     if(ret == QMessageBox::Ok)
     {
         if(ui->pbStart->isChecked())
             OnStart(false);
         writeSettings();
         if(!QFile::remove(QDir::homePath()+"/ExpertSDR/SunSDR.ini"))
         {
             QMessageBox::warning(pMem, tr("ExpertSDR warning"), tr("Can't remove calibration file SunSDR.ini.\n"), QMessageBox::Ok);
             qWarning() << "ExpertSDR: defaultCalibrations(): Can't remove calibration file SunSDR.ini!";
             return;
         }
         readSettings();
     }
}

void ExpertSDR_vA2_1::onOptionsApply()
{
    writeSettings();
}

void ExpertSDR_vA2_1::OnLoad()
{
    writeSettings();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Options"), pathTmp+"/Options.ini", tr("ExpertSDR Options (*.ini)"));
    QFileInfo fInfo(fileName);
    if(!fInfo.isFile())
    {
        qWarning() << "ExpertSDR: OnLoad(): File Options.ini not found!";
        return;
    }
    QByteArray br;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    br = file.readLine(13);
    file.close();
    QString str(br);
    if(str != "[MainWindow]")
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("This file is not ExpertSDR options file!");
        msgBox.exec();
        qWarning() << "ExpertSDR: OnLoad(): Loaded file is not ExpertSDR options file!";
        return;
    }
    QFile::remove(QDir::homePath()+"/ExpertSDR/Options.ini");
    QFile::copy(fileName, QDir::homePath()+"/ExpertSDR/Options.ini");
    readSettings();
}

void ExpertSDR_vA2_1::OnSaveAs()
{
    writeSettings();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Options"), pathTmp+"/Options.ini", tr("ExpertSDR Options (*.ini)"));
    if(fileName.isEmpty())
    {
        qWarning() << "ExpertSDR: OnSaveAs(): Destination file not found!";
        return;
    }
    if(!QFile::copy(QDir::homePath()+"/ExpertSDR/Options.ini", fileName))
        qWarning() << "ExpertSDR: OnSaveAs(): Can't copy options file!";
}

void ExpertSDR_vA2_1::OnLoadCalibration()
{
    writeSettings();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open calibration file"), pathTmp+"/SunSDR.ini", tr("SunSDR calibrations file (*.ini)"));
    QFileInfo fInfo(fileName);
    if(!fInfo.isFile())
    {
        qWarning() << "ExpertSDR: OnLoadCalibration(): File SunSDR.ini not found!";
        return;
    }
    QByteArray br;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    br = file.readLine(9);
    file.close();
    QString str(br);
    if(str != "[SunSDR]")
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("This file is not ExpertSDR calibration file!");
        msgBox.exec();
        qWarning() << "ExpertSDR: OnLoadCalibration(): Loaded file is not ExpertSDR calibration file!";
        return;
    }
    if(!QFile::remove(QDir::homePath()+"/ExpertSDR/SunSDR.ini"))
        qWarning() << "ExpertSDR: OnLoadCalibration(): Can't remove old calibration file!";
    if(!QFile::copy(fileName, QDir::homePath()+"/ExpertSDR/SunSDR.ini"))
        qWarning() << "ExpertSDR: OnLoadCalibration(): Can't copy calibration file!";
    readSettings();
}

void ExpertSDR_vA2_1::OnSaveAsCalibration()
{
    writeSettings();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save calibration file"), pathTmp+"/SunSDR.ini", tr("SunSDR calibrations file (*.ini)"));
    if(fileName.isEmpty())
    {
        qWarning() << "ExpertSDR: OnSaveAsCalibration(): Destination file not found!";
        return;
    }
    if(!QFile::copy(QDir::homePath()+"/ExpertSDR/SunSDR.ini", fileName))
        qWarning() << "ExpertSDR: OnSaveAsCalibration(): Can't copy calibration file!";
}

void ExpertSDR_vA2_1::OnStartCalibration()
{
    if(ui->pbStart->isChecked())
        pCalibrator->show();
    else
    {
        QMessageBox::information(pMem, tr("Programm is not started."), tr("Programm must be started before calibration.\nPlease push start button and try again!"));
        qWarning() << "ExpertSDR: OnStartCalibration(): Not started!";
    }
}

void ExpertSDR_vA2_1::OnShowFullScreen(bool Full)
{
    if(Full)
    {
        MainWindowSize = size();
        MainWindowPos = pos();
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
        resize(MainWindowSize);
        move(MainWindowPos);
    }
}

void ExpertSDR_vA2_1::OnClose()
{
    close();
}

void ExpertSDR_vA2_1::OnLpanel(bool Status)
{
    if(Status)
    {
        ui->WdgLpanel->setFixedHeight(55);
        ui->WdgMainGraph->resize(ui->WdgMainGraph->width(), height()-172);
        ui->WdgMainGraph->update();
        pGraph->resize(Splitter->widget(0)->width(), height() - 172);
    }
    else
    {
        ui->WdgLpanel->setFixedHeight(0);
        ui->WdgMainGraph->resize(ui->WdgMainGraph->width(), height()-120);
        ui->WdgMainGraph->update();
        pGraph->resize(Splitter->widget(0)->width(), height() - 120);
    }
    Splitter->resize(ui->WdgMainGraph->size());
    ui->verticalLayout->update();
    ui->horizontalLayout_7->update();
}

void ExpertSDR_vA2_1::OnMemPanel(bool Status)
{
    QList<int> sizes;
    if(!Status)
    {
        ui->pbMemPan->setText(">>");
        splitOpt = Splitter->saveState();
        sizes << Splitter->sizes().at(0) + Splitter->sizes().at(1) << 0;
        Splitter->setSizes(sizes);
        Splitter->widget(1)->setMinimumWidth(0);
        Splitter->widget(1)->setMaximumWidth(0);
    }
    else
    {
        ui->pbMemPan->setText("<<");
        Splitter->widget(1)->setMaximumWidth(1024);
        Splitter->widget(1)->setMinimumWidth(250);
        Splitter->restoreState(splitOpt);
    }
}

void ExpertSDR_vA2_1::OnOptions()
{
    pOpt->show();
    pOpt->activateWindow();
}

void ExpertSDR_vA2_1::OnAbout()
{
    pAbout->show();
    pAbout->activateWindow();
}

void ExpertSDR_vA2_1::OnTimeClick()
{
    IsUtcTime = not IsUtcTime;
}

void ExpertSDR_vA2_1::onCwMacro()
{
    pCwMacro->show();
    pCwMacro->activateWindow();
    if((OptBands[CurrentBandIndex].CurrentModeIndex != CWU) && (OptBands[CurrentBandIndex].CurrentModeIndex != CWL))
        OnChangeMode((CurrentBandIndex > BAND40M) ? CWU : CWL);
}

void ExpertSDR_vA2_1::OnVacRxGain(int Val)
{
    pVac->setInGain(Val);
}

void ExpertSDR_vA2_1::OnVacTxGain(int Val)
{
    pVac->setOutGain(Val);
}

void ExpertSDR_vA2_1::OnPbVacEn(bool Stat)
{
    pOpt->ui.chbVacEnable->setChecked(Stat);
    int Mode = pModeBut->checkedId();
    if((Mode == CWL) || (Mode == CWU) || (Mode == DIGL) || (Mode == DIGU) || (Mode == SPEC) || ui->pbVac->isChecked())
    {
        pVoiceRec->Stop();
        ui->pbVoicePlay->setEnabled(false);
        ui->pbVoiceRec->setEnabled(false);
    }
    else
    {
        ui->pbVoicePlay->setEnabled(true);
        ui->pbVoiceRec->setEnabled(true);
    }
}

void ExpertSDR_vA2_1::onChangeDigLSb(int val)
{
    if(OptBands[CurrentBandIndex].CurrentModeIndex == DIGL)
        OnPitch(pOpt->ui.sbDigL->value());
}

void ExpertSDR_vA2_1::onChangeDigUSb(int val)
{
    if(OptBands[CurrentBandIndex].CurrentModeIndex == DIGU)
        OnPitch(pOpt->ui.sbDigU->value());
}

void ExpertSDR_vA2_1::OnPaOn(bool Status)
{
    ui->pbPa->setChecked(Status);
    ExControl(CurrentBandIndex, (TRXMODE)ui->pbMox->isChecked());
}

void ExpertSDR_vA2_1::OnPreampChanged(QAction * Action)
{
    ui->pbPreamp->setText(Action->iconText() + "    ");
    int Index = pMenuPreamp->actions().indexOf(Action);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].cbPreampIndex = Index;
    pSdrCtrl->SetPreamp(Index);
    pGraph->pGl->SetCalibrateK(6.4);
}

void ExpertSDR_vA2_1::OnAgcChanged(QAction * Action)
{
    ui->pbAgc->setText(Action->iconText() + "    ");
    int Index = pMenuAgc->actions().indexOf(Action);
    indexAgc = Index;
    pDsp->SetRxAgc((AGCMODE)Index);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].cbAgcIndex = Index;
    if(indexAgc == 0)
        pDsp->SetFixedAgc(ui->slAgc->value());
    else
        pDsp->SetRxAgcTop(ui->slAgc->value());
}

void ExpertSDR_vA2_1::OnStepChanged(QAction * Action)
{
    ui->pbStep->setText(Action->iconText() + "    ");
    int Index = pMenuStep->actions().indexOf(Action);
    pGraph->pGl->SetStepDDS(Index);
}

void ExpertSDR_vA2_1::OnStepChanged(int val)
{
    pMenuStep->actions().at(val)->setChecked(true);
    ui->pbStep->setText(pAgStep->checkedAction()->iconText() + "    ");
    pGraph->pGl->SetStepDDS(val);
}

void ExpertSDR_vA2_1::OnChangeBand(int Mode)
{
    if(ui->pbMox->isChecked())
        return;
    qDebug() << "ExpertSDR: OnChangeBand( " << Mode << " )";
    BandModeChecked = Mode;
    CurrentBandIndex = Mode;
    pGraph->pGl->SetBand(CurrentBandIndex);
    if(CurrentBandIndex > BAND07M)
    {
        if(!isGenTx)
        {
            ui->pbMox->setEnabled(false);
            ui->pbTone->setEnabled(false);
        }
    }
    else
    {
        if(!pMem->isPlay())
        {
            ui->pbMox->setEnabled(true);
            ui->pbTone->setEnabled(true);
        }
    }
    OnVolume(OptBands[CurrentBandIndex].Volume);
    OnAgc(OptBands[CurrentBandIndex].RfGain);
    ExControl(CurrentBandIndex, (TRXMODE)TxMode);
    pBandBut->button(Mode)->setChecked(true);
    pSM->SetBand(Mode);
    if(Mode == BAND2M)
    {
        SetFreqMin(BPF2_START);
        SetFreqMax(BPF2_END);
        SetFreq(OptBands[CurrentBandIndex].MainFreq);
        pGraph->pGl->SetDDSFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
        pSdrCtrl->SetDdsFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
        pSmeter->SetVHF(true);
        onXvAntSwitch(0);
    }
    else if(Mode == BAND07M)
    {
        SetFreqMin(BPF07_START);
        SetFreqMax(BPF07_END);
        SetFreq(OptBands[CurrentBandIndex].MainFreq);
        pGraph->pGl->SetDDSFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
        pSdrCtrl->SetDdsFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
        pSmeter->SetVHF(true);
        onXvAntSwitch(0);
    }
    else
    {
        SetFreqMin(100000);
        SetFreqMax(65000000);
        SetFreq(OptBands[CurrentBandIndex].MainFreq);
        pGraph->pGl->SetDDSFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
        pSdrCtrl->SetDdsFreq(OptBands[CurrentBandIndex].MainFreq - FilterPosFreq);
        pSmeter->SetVHF(false);
    }
    pModeBut->button(OptBands[CurrentBandIndex].CurrentModeIndex)->setChecked(true);
    OnChangeMode(OptBands[CurrentBandIndex].CurrentModeIndex);
    pGraph->pGl->SetFilter2(-pGraph->pGl->GetDDSFreq() + pVfoB->value());
    OnChangeFilter(OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex);
}

void ExpertSDR_vA2_1::OnChangeMode(int Mode)
{
    pModeBut->button(Mode)->setChecked(true);
    sdrMode = (SDRMODE)Mode;
    if((Mode == CWL) || (Mode == CWU) || (Mode == DIGL) || (Mode == DIGU) || (Mode == SPEC) || (ui->pbVac->isChecked()))
    {
        pVoiceRec->Stop();
        ui->pbVoicePlay->setEnabled(false);
        ui->pbVoiceRec->setEnabled(false);
    }
    else
    {
        ui->pbVoicePlay->setEnabled(true);
        ui->pbVoiceRec->setEnabled(true);
    }
    SetTxFilter();
    pCwMacro->setMode((SDRMODE)Mode);
    pGraph->pGl->SetMode((SDRMODE)Mode);
    if(Mode == CWL || Mode == CWU)
    {
        ui->swMicSpeed->setCurrentIndex(1);
        ui->StWdg0->setCurrentIndex(1);
        ui->pbF0->setText("25");
        ui->pbF1->setText("50");
        ui->pbF2->setText("100");
        ui->pbF3->setText("200");
        ui->pbF4->setText("300");
        ui->pbF5->setText("500");
        ui->pbF6->setText("750");
        ui->pbF7->setText("1000");
    }
    else
    {
        ui->swMicSpeed->setCurrentIndex(0);
        ui->StWdg0->setCurrentIndex(0);
        if(Mode == LSB || Mode == USB || Mode == SPEC)
        {
            ui->pbF0->setText("1.8K");
            ui->pbF1->setText("2.0K");
            ui->pbF2->setText("2.4K");
            ui->pbF3->setText("2.7K");
            ui->pbF4->setText("3.0K");
            ui->pbF5->setText("3.3K");
            ui->pbF6->setText("3.9K");
            ui->pbF7->setText("4.5K");
        }
        else if(Mode == DIGL || Mode == DIGU )
        {
            ui->pbF0->setText("1.8K");
            ui->pbF1->setText("2.0K");
            ui->pbF2->setText("2.4K");
            ui->pbF3->setText("2.7K");
            ui->pbF4->setText("3.0K");
            ui->pbF5->setText("3.3K");
            ui->pbF6->setText("3.9K");
            ui->pbF7->setText("4.5K");
        }
        else if(Mode == AM || Mode == SAM || Mode == DSB || Mode == FMN)
        {
            ui->pbF0->setText("2.5K");
            ui->pbF1->setText("3.0K");
            ui->pbF2->setText("3.3K");
            ui->pbF3->setText("4.0K");
            ui->pbF4->setText("5.0K");
            ui->pbF5->setText("6.5K");
            ui->pbF6->setText("8.0K");
            ui->pbF7->setText("10.0K");
        }
        else
        {
            ui->pbF0->setText(" ");
            ui->pbF1->setText(" ");
            ui->pbF2->setText(" ");
            ui->pbF3->setText(" ");
            ui->pbF4->setText(" ");
            ui->pbF5->setText(" ");
            ui->pbF6->setText(" ");
            ui->pbF7->setText(" ");
        }
    }
    if(Mode == DRM)
    {
        ui->pbF0->setEnabled(false);
        ui->pbF1->setEnabled(false);
        ui->pbF2->setEnabled(false);
        ui->pbF3->setEnabled(false);
        ui->pbF4->setEnabled(false);
        ui->pbF5->setEnabled(false);
        ui->pbF6->setEnabled(false);
        ui->pbF7->setEnabled(false);
        ui->pbF8->setEnabled(false);
    }
    else
    {
        ui->pbF0->setEnabled(true);
        ui->pbF1->setEnabled(true);
        ui->pbF2->setEnabled(true);
        ui->pbF3->setEnabled(true);
        ui->pbF4->setEnabled(true);
        ui->pbF5->setEnabled(true);
        ui->pbF6->setEnabled(true);
        ui->pbF7->setEnabled(true);
        ui->pbF8->setEnabled(true);
    }
    if(ui->pbMox->isChecked())
        SetTxFilter();
    pModeBut->button(Mode)->setChecked(true);
    pSM->SetMod(Mode);
    OptBands[CurrentBandIndex].CurrentModeIndex = Mode;
    pMenuPreamp->actions().at(OptBands[CurrentBandIndex].Mode[Mode].cbPreampIndex)->setChecked(true);
    OnPreampChanged(pMenuPreamp->actions().at(OptBands[CurrentBandIndex].Mode[Mode].cbPreampIndex));
    pMenuAgc->actions().at(OptBands[CurrentBandIndex].Mode[Mode].cbAgcIndex)->setChecked(true);
    OnAgcChanged(pMenuAgc->actions().at(OptBands[CurrentBandIndex].Mode[Mode].cbAgcIndex));
    pDsp->SetMode((SDRMODE)Mode);
    ui->pbNr->setChecked(OptBands[CurrentBandIndex].Mode[Mode].NrEnable);
    ui->pbAnf->setChecked(OptBands[CurrentBandIndex].Mode[Mode].AnfEnable);
    ui->pbNb->setChecked(OptBands[CurrentBandIndex].Mode[Mode].Nb1Enable);
    ui->pbNb2->setChecked(OptBands[CurrentBandIndex].Mode[Mode].Nb2Enable);
    ui->pbBin->setChecked(OptBands[CurrentBandIndex].Mode[Mode].BinEnable);
    OnNr(OptBands[CurrentBandIndex].Mode[Mode].NrEnable);
    OnAnf(OptBands[CurrentBandIndex].Mode[Mode].AnfEnable);
    OnNb(OptBands[CurrentBandIndex].Mode[Mode].Nb1Enable);
    OnNb2(OptBands[CurrentBandIndex].Mode[Mode].Nb2Enable);
    OnBin(OptBands[CurrentBandIndex].Mode[Mode].BinEnable);
    ui->pbSql->setChecked(OptBands[CurrentBandIndex].Mode[Mode].SqlEnable);
    OnSql(ui->pbSql->isChecked());
    ui->pbGate->setChecked(OptBands[CurrentBandIndex].Mode[Mode].GateEnable);
    ui->pbComp->setChecked(OptBands[CurrentBandIndex].Mode[Mode].CompEnable);
    ui->pbCpdr->setChecked(OptBands[CurrentBandIndex].Mode[Mode].CpdrEnable);
    ui->slSql->setValue(OptBands[CurrentBandIndex].Mode[Mode].SqlValue);
    ui->slGate->setValue(OptBands[CurrentBandIndex].Mode[Mode].GateValue);
    ui->slComp->setValue(OptBands[CurrentBandIndex].Mode[Mode].CompValue);
    ui->slCpdr->setValue(OptBands[CurrentBandIndex].Mode[Mode].CpdrValue);
    if(Mode == DIGL)
        OnPitch(pOpt->ui.sbDigL->value());
    else if(Mode == DIGU)
        OnPitch(pOpt->ui.sbDigU->value());
    else
        OnPitch(OptBands[CurrentBandIndex].Pitch);
    int CurrMode, CurrFilterIdx;
    CurrMode = OptBands[CurrentBandIndex].CurrentModeIndex;
    CurrFilterIdx = OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex;
    OnChangeTxPhaseRej(0);
    OnChangeTxGainRej(0);
    ui->sbVarHighFreq->setValue(OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH);
    ui->sbVarLowFreq->setValue(OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL);
    SetFreq(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter());
    OnChangeFilter(OptBands[CurrentBandIndex].Mode[Mode].FilterIndex);
    emit ModeChanged(Mode);
}

void ExpertSDR_vA2_1::OnChangeFilter(int Filter)
{
    pFiltBut->button(Filter)->setChecked(true);
    int PitchVal;
    int HighFreq = 0, LowFreq = -3000;
    int Foffset = 50;
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex = Filter;
    int CurrMode, CurrFilterIdx;
    CurrMode = OptBands[CurrentBandIndex].CurrentModeIndex;
    CurrFilterIdx = OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex;
    ui->sbVarHighFreq->setValue(OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH);
    ui->sbVarLowFreq->setValue(OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL);
    if(pModeBut->checkedId() == CWL || pModeBut->checkedId() == CWU)
    {
        PitchVal = (pModeBut->checkedId() == CWL) ? -OptBands[CurrentBandIndex].Pitch : OptBands[CurrentBandIndex].Pitch;
        switch(pFiltBut->checkedId())
        {
            case 0:
                HighFreq = PitchVal + 12;
                LowFreq = PitchVal - 12;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 1:
                HighFreq = PitchVal + 25;
                LowFreq = PitchVal - 25;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 2:
                HighFreq = PitchVal + 50;
                LowFreq = PitchVal - 50;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 3:
                HighFreq = PitchVal + 100;
                LowFreq = PitchVal - 100;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 4:
                HighFreq = PitchVal + 150;
                LowFreq = PitchVal - 150;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 5:
                HighFreq = PitchVal + 250;
                LowFreq = PitchVal - 250;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 6:
                HighFreq = PitchVal + 325;
                LowFreq = PitchVal - 325;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 7:
                HighFreq = PitchVal + 500;
                LowFreq = PitchVal - 500;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            case 8:
                HighFreq = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH;
                LowFreq  = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL;
                ui->sbVarHighFreq->setValue(HighFreq);
                ui->sbVarLowFreq->setValue(LowFreq);
            break;
            default:
            break;
        }
    }
    else if(pModeBut->checkedId() == LSB || pModeBut->checkedId() == USB)
    {
        switch(pFiltBut->checkedId())
        {
            case 0:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 1800);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 1800) : Foffset;
            break;
            case 1:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 2000);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 2000) : Foffset;
            break;
            case 2:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 2400);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 2400) : Foffset;
            break;
            case 3:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 2700);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 2700) : Foffset;
            break;
            case 4:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 3000);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 3000) : Foffset;
            break;
            case 5:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 3300);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 3300) : Foffset;
            break;
            case 6:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 3700);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 3700) : Foffset;
            break;
            case 7:
                HighFreq = (pModeBut->checkedId() == LSB) ? - Foffset : (Foffset + 4500);
                LowFreq = (pModeBut->checkedId() == LSB) ? - (Foffset + 4500) : Foffset;
            break;
            case 8:
                HighFreq = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH;
                LowFreq  = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL;
            break;
            default:
            break;
        }
        ui->sbVarHighFreq->setValue(HighFreq);
        ui->sbVarLowFreq->setValue(LowFreq);
    }
    else if(pModeBut->checkedId() == DIGL || pModeBut->checkedId() == DIGU)
    {
        switch(pFiltBut->checkedId())
        {
            case 0:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 1800);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 1800) : Foffset;
            break;
            case 1:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 2000);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 2000) : Foffset;
            break;
            case 2:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 2400);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 2400) : Foffset;
            break;
            case 3:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 2700);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 2700) : Foffset;
            break;
            case 4:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 3000);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 3000) : Foffset;
            break;
            case 5:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 3300);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 3300) : Foffset;
            break;
            case 6:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 3700);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 3700) : Foffset;
            break;
            case 7:
                HighFreq = (pModeBut->checkedId() == DIGL) ? - Foffset : (Foffset + 4500);
                LowFreq = (pModeBut->checkedId() == DIGL) ? - (Foffset + 4500) : Foffset;
            break;
            case 8:
                HighFreq = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH;
                LowFreq  = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL;
            break;
            default:
            break;
        }
        ui->sbVarHighFreq->setValue(HighFreq);
        ui->sbVarLowFreq->setValue(LowFreq);
    }
    else if(pModeBut->checkedId() == AM || pModeBut->checkedId() == FMN || pModeBut->checkedId() == SAM || pModeBut->checkedId() == DSB)
    {
        switch(pFiltBut->checkedId())
        {
            case 0:
                HighFreq = 1250;
                LowFreq = -1250;
            break;
            case 1:
                HighFreq = 1500;
                LowFreq = -1500;
            break;
            case 2:
                HighFreq = 1650;
                LowFreq = -1650;
            break;
            case 3:
                HighFreq = 2000;
                LowFreq = -2000;
            break;
            case 4:
                HighFreq = 2500;
                LowFreq = -2500;
            break;
            case 5:
                HighFreq = 3250;
                LowFreq = -3250;
            break;
            case 6:
                HighFreq = 4000;
                LowFreq = -4000;
            break;
            case 7:
                HighFreq = 5000;
                LowFreq = -5000;
            break;
            case 8:
                HighFreq = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH;
                LowFreq  = OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL;
            break;
            default:
            break;
        }
        ui->sbVarHighFreq->setValue(HighFreq);
        ui->sbVarLowFreq->setValue(LowFreq);
    }
    else if(pModeBut->checkedId() == DRM)
    {
        HighFreq = 5000;
        LowFreq = -5000;
        ui->sbVarHighFreq->setValue(HighFreq);
        ui->sbVarLowFreq->setValue(LowFreq);
    }
    RxFilterHigh = HighFreq;
    RxFilterLow = LowFreq;
    pDsp->SetFilter(LowFreq, HighFreq, 2048, RX);
    pGraph->pGl->SetBandFilter(LowFreq, HighFreq);
    if(ui->pbMox->isChecked())
        SetTxFilter();
}

void ExpertSDR_vA2_1::OnEqOn(bool IsOn)
{
    if(ui->pbEqRx->isChecked())
    {
        EqRxOn = IsOn;
        ui->pbEqOn->setChecked(EqRxOn);
        pDsp->SetGrphRxEQcmd(EqRxOn);
    }
    else
    {
        EqTxOn = IsOn;
        ui->pbEqOn->setChecked(EqTxOn);
        pDsp->SetGrphTXEQcmd(EqTxOn);
    }
    OnDspEq(true);
}

void ExpertSDR_vA2_1::OnDspEq(bool)
{
    if(ui->pbVac->isChecked() || ui->pbVoicePlay->isChecked() || ui->pbVoiceRec->isChecked())
    {
        pDsp->SetGrphRxEQcmd(false);
        pDsp->SetGrphTXEQcmd(false);
        ui->pbEqOn->setEnabled(false);
        ui->pbEqRx->setEnabled(false);
        ui->pbEqTx->setEnabled(false);
        ui->swEq1k5Hz->setEnabled(false);
        ui->swEq400Hz->setEnabled(false);
        ui->swEq6kHz->setEnabled(false);
    }
    else
    {
        pDsp->SetGrphRxEQcmd(EqRxOn);
        pDsp->SetGrphTXEQcmd(EqTxOn);
        ui->pbEqOn->setEnabled(true);
        ui->pbEqRx->setEnabled(true);
        ui->pbEqTx->setEnabled(true);
        ui->swEq1k5Hz->setEnabled(true);
        ui->swEq400Hz->setEnabled(true);
        ui->swEq6kHz->setEnabled(true);
    }
}

void ExpertSDR_vA2_1::OnEqModeChanged(int Number)
{
    if(Number == 0)
        ui->pbEqOn->setChecked(EqRxOn);
    else
        ui->pbEqOn->setChecked(EqTxOn);

    OnEqOn(ui->pbEqOn->isChecked());
    if(Number == 0)
        EqTxMode = false;
    else
        EqTxMode = true;

    ui->swEq400Hz->setCurrentIndex(Number);
    ui->swEq1k5Hz->setCurrentIndex(Number);
    ui->swEq6kHz->setCurrentIndex(Number);
}

void ExpertSDR_vA2_1::OnEqTxChanged(int Value)
{
    int EqBuf[4];
    EqBuf[0] = 0;
    EqBuf[1] = ui->slTxEq400->value();
    EqBuf[2] = ui->slTxEq1k5->value();
    EqBuf[3] = ui->slTxEq6k->value();
    pDsp->SetTxEQ(EqBuf);
}

void ExpertSDR_vA2_1::OnEqRxChanged(int Value)
{
    int EqBuf[4];
    EqBuf[0] = 0;
    EqBuf[1] = ui->slRxEq400->value();
    EqBuf[2] = ui->slRxEq1k5->value();
    EqBuf[3] = ui->slRxEq6k->value();
    pDsp->SetGrphRxEQ(EqBuf);
}

void ExpertSDR_vA2_1::OnSql(bool State)
{
    ui->pbSql->setChecked(State);
    pDsp->SetSquelchState(State);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].SqlEnable = State;
    ui->pbSql->setChecked(State);
    pSM->ui.pbSql->setChecked(State);
}

void ExpertSDR_vA2_1::OnSql(int Value)
{
    ui->slSql->setValue(Value);
    ui->sbSql->setValue(Value);
    pDsp->SetSquelchVal((double)Value);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].SqlValue = Value;
}

void ExpertSDR_vA2_1::OnGate(bool State)
{
    pDsp->SetTxSquelchSt(State);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].GateEnable = State;
    ui->pbGate->setChecked(State);
}

void ExpertSDR_vA2_1::OnGate(int Value)
{
    ui->slGate->setValue(Value);
    ui->sbGate->setValue(Value);
    pDsp->SetTxSquelchVal((double)Value - 7.0);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].GateValue = Value;
}

void ExpertSDR_vA2_1::OnComp(bool State)
{
    ui->pbComp->setChecked(State);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].CompEnable = State;
    if(ui->pbCpdr->isChecked() || ui->pbComp->isChecked())
        pDsp->SetTxCompandSt(1);
    else
        pDsp->SetTxCompandSt(0);
}

void ExpertSDR_vA2_1::OnComp(int Value)
{
    ui->slComp->setValue(Value);
    ui->sbComp->setValue(Value);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].CompValue = Value;
    if(ui->pbComp->isChecked())
        pDsp->SetTxCompand(1.0 + 0.4*(double)Value);
}

void ExpertSDR_vA2_1::OnCpdr(bool State)
{
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].CpdrEnable = State;
    if(ui->pbCpdr->isChecked() || ui->pbComp->isChecked())
        pDsp->SetTxCompandSt(1);
    else
        pDsp->SetTxCompandSt(0);
}

void ExpertSDR_vA2_1::OnCpdr(int Value)
{
    ui->slCpdr->setValue(Value);
    ui->sbCpdr->setValue(Value);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].CpdrValue = Value;
    if(ui->pbCpdr->isChecked() || ui->pbComp->isChecked())
        pDsp->SetTxCompand(0.1*(double)Value);
}

void ExpertSDR_vA2_1::OnNr(bool Val)
{
    ui->pbNr->setChecked(Val);
    pDsp->SetNr(Val);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].NrEnable = Val;
}

void ExpertSDR_vA2_1::OnAnf(bool Val)
{
    ui->pbAnf->setChecked(Val);
    pDsp->SetAnf(Val);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].AnfEnable = Val;
}

void ExpertSDR_vA2_1::OnLock(bool Val)
{
    ui->pbLock->setChecked(Val);
    SetLockBand(!Val);
    SetLockMode(!Val);
    SetEnableFreqPanel(!Val);
    pGraph->pGl->SetLockBandFilter(Val);
    pGraph->pGl->SetLockFilter(Val);
    pGraph->pGl->SetLockGrid(Val);
    pVfoB->setEnabled(!Val);
    ui->pbFdown->setEnabled(!Val);
    ui->pbFlist->setEnabled(!Val);
    ui->pbFup->setEnabled(!Val);
}

void ExpertSDR_vA2_1::OnNb(bool Val)
{
    ui->pbNb->setChecked(Val);
    pDsp->SetNB(Val);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].Nb1Enable = Val;
}

void ExpertSDR_vA2_1::OnNb2(bool Val)
{
    ui->pbNb2->setChecked(Val);
    pDsp->SetSdrOm(Val);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].Nb2Enable = Val;
}

void ExpertSDR_vA2_1::OnBin(bool Val)
{
    ui->pbBin->setChecked(Val);
    pDsp->SetBin(Val);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].BinEnable = Val;
}

void ExpertSDR_vA2_1::OnOptChangeNrVals(int val)
{
    pDsp->SetNrVals((double)pOpt->ui.sbNrTaps->value(), (double)pOpt->ui.sbNrDelay->value(), 0.0001*(double)pOpt->ui.sbNrGain->value(), 0.0000001*(double)pOpt->ui.sbNrLeak->value());
}

void ExpertSDR_vA2_1::OnOptChangeAgcVals(int val)
{
    pDsp->SetRxAgcAttack(pOpt->ui.sbAgcAttak->value());
    pDsp->SetRxAgcDecay(pOpt->ui.sbAgcDecay->value());
    pDsp->SetRxAgcHang(pOpt->ui.sbAgcHang->value());
    pDsp->SetRxAgcHangThreshold(pOpt->ui.sbAgcHangThresh->value());
    pDsp->SetRxAgcSlope(pOpt->ui.sbAgcSlope->value());
}

void ExpertSDR_vA2_1::OnOptChangeNbsVals(int val)
{
    pDsp->SetNBvals(0.165*(double)pOpt->ui.sbNb1Thr->value());
    pDsp->SetSdrOmvals(0.165*(double)pOpt->ui.sbNb2Thr->value());
}

void ExpertSDR_vA2_1::OnOptChangeAnfVals(int val)
{
    pDsp->SetANFvals((double)pOpt->ui.sbAnfTaps->value(), (double)pOpt->ui.sbAnfDelay->value(), 0.0001*(double)pOpt->ui.sbAnfGain->value(), 0.0000001*(double)pOpt->ui.sbAnfLeak->value());
}

void ExpertSDR_vA2_1::OnOptChangeSampleRate(int index)
{
    int tmpRxOsc  = pGraph->pGl->GetDDSFreq();
    int tmpFilter = pGraph->pGl->GetFilter();
    pDsp->SetSampleRate(pOpt->getSampleRate());
    pGraph->pGl->SetSampleRate(pOpt->getSampleRate());
    pVoiceRec->setSampleRate(pOpt->getSampleRate());
    pGraph->pGl->SetDDSFreq(tmpRxOsc);
    pGraph->pGl->SetFilter(tmpFilter);
}

void ExpertSDR_vA2_1::OnOptChangeDspBufLen(int index)
{
    int Dat = pOpt->ui.cbRxDspBufSize->currentText().toInt();
    if(ui->pbStart->isChecked())
    {
        OnStart(false);
        pDsp->SetDSPBuflen(Dat);
        OnStart(true);
    }
    else
        pDsp->SetDSPBuflen(Dat);
}

void ExpertSDR_vA2_1::OnOptAgcSlope(int val)
{
    pDsp->SetRxAgcSlope(val);
}

void ExpertSDR_vA2_1::OnOptAgcAttak(int val)
{
    pDsp->SetRxAgcAttack(val);
}

void ExpertSDR_vA2_1::OnOptAgcDecay(int val)
{
    pDsp->SetRxAgcDecay(val);
}

void ExpertSDR_vA2_1::OnOptAgcHang(int val)
{
    pDsp->SetRxAgcHang(val);
}

void ExpertSDR_vA2_1::OnOptAgcHangThr(int val)
{
    pDsp->SetRxAgcHangThreshold(val);
}

void ExpertSDR_vA2_1::OnOptChangeWindow(int Window)
{
    pDsp->SetWindow((Windowtype)Window);
}

void ExpertSDR_vA2_1::OnStart(bool Start)
{
   if(true)
    {
        ui->pbStart->setChecked(Start);
        if(Start)
        {
            if(isStarted == true)
                return;
            isStarted = true;
            pOpt->ui.cbSdrType->setEnabled(false);

            int plugidx = pOpt->ui.cbSdrType->currentIndex();
            QString plugpath = pOpt->ui.cbSdrType->itemData(plugidx).toString();
            pSdrCtrl->onSdrPluginChanged(plugpath);
            pSdrCtrl->SoundCardSampleRateChanged((int)pOpt->getSampleRate());
            OnLock(ui->pbLock->isChecked());
            if(pMem->isWavPlay())
            {
                if(pMem->sampleRateIndex() < 0)
                {
                    qWarning() << "ExpertSDR: OnStart(): Incorrect sample rate!";
                    goto StopSun1;
                }
            }
            calcVectorRamp(pOpt->getSampleRate(), qRound(280.0/(pOpt->getSampleRate()/48000.0)), 50);
            pOpt->OnGlitchCompChanged(0);
            if(pMem->isPlay())
                setLock(LOCK_MOX | LOCK_TONE);
            else
                setLock(LOCK_DIDABLE);

            ui->pbTone->setEnabled(true);
            onCalibrationGen(pOpt->ui.cbCalGen->isChecked());
            pDsp->SetSampleRate(pOpt->getSampleRate());
            pDsp->SetPanaramaOn(true);
            pDsp->SetDcBlock(true);
            pDsp->SetRx1On(true);
            pDsp->SetRx1Volume(ui->slRx1Vol->value());
            pDsp->AudioReset();
            pDsp->SetSpectrumPolyphase(pOpt->ui.chbPoliphase->isChecked());
            pDsp->SetWindow((Windowtype)pOpt->ui.cbWinType->currentIndex());
            pDsp->SetAudioSize(pOpt->ui.cbPaBufferSize->currentText().toInt());
            pDsp->SetRx1Osc((float)(-pGraph->pGl->GetFilter()));
            pDsp->SetTxLevelerTop(3.162278);

            SetVhfOsc(pOpt->ui.sbVhfOsc->value());
            SetUhfOsc(pOpt->ui.sbUhfOsc->value());

            pSdrCtrl->SetSdrType((SDR_DEVICE)pOpt->ui.cbSdrType->currentIndex());
            pSdrCtrl->setMute(ui->pbMute->isChecked());
            if(pSdrCtrl->Start() != 0)
            {
                qWarning() << "ExpertSDR: OnStart(): SDR control can't started!";
                goto StopSun1;
            }
            if(!pMem->isWavPlay())
            {
                OnChangeBand(CurrentBandIndex);
                OnPitch(ui->slPitch->value());
            }
            pAudThr->setFrameSize(256<<pOpt->ui.cbPaBufferSize->currentIndex());
            if(ui->pbVac->isChecked())
            {
                pVac->open(pOpt->ui.cbPaVacSampleRate->currentText().toInt(),  pOpt->ui.cbPaSampleRate->currentText().toInt());
                pVac->setInGain((float)pOpt->ui.sbVacRxGain->value());
                pVac->setOutGain((float)pOpt->ui.sbVacTxGain->value());
                VacOpt.cbPaDriverIndex = pOpt->ui.cbPaVacDriver->currentIndex();
                VacOpt.cbPaOutIndex = pOpt->ui.cbPaVacOut->currentIndex();
                VacOpt.cbPaInIndex = pOpt->ui.cbPaVacIn->currentIndex();
                VacOpt.cbPaChannelsIndex = 0;
                VacOpt.cbPaSampleRate = pOpt->ui.cbPaVacSampleRate->currentText().toInt();
                VacOpt.cbPaBufferSizeIndex = pOpt->ui.cbPaVacBufferSize->currentIndex()+1;
                VacOpt.sbPaLattency = pOpt->ui.sbPaVacLattency->value();
                VacOpt.sbDdsMul = 0;
                pPaVac->setParam(&VacOpt);
                pPaVac->start(VacCallBack);
                pVac->reset();
            }
            pMem->pIqList->setSampleRate(pOpt->getSampleRate());
            pMem->setCentralFreq(pGraph->pGl->GetDDSFreq());
            pCwMacro->pCwCore->setBufferSize(pOpt->ui.cbPaBufferSize->currentText().toInt());
            pCwMacro->pCwCore->setSampleRate(pOpt->getSampleRate());
            pCwMacro->start();
            pCwMacro->setMode((SDRMODE)pModeBut->checkedId());
            OnMox(RX);
            OnMute(ui->pbMute->isChecked());
            pGraph->pGl->SetFilter2(-pGraph->pGl->GetDDSFreq() + pVfoB->value());
            pGraph->pGl->Start();
            OnPreampChanged(pAgPreamp->checkedAction());
            int Mode = pModeBut->checkedId();
            if((Mode == CWL) || (Mode == CWU) || (Mode == DIGL) || (Mode == DIGU) || (Mode == SPEC) || ui->pbVac->isChecked())
            {
                pVoiceRec->Stop();
                ui->pbVoicePlay->setEnabled(false);
                ui->pbVoiceRec->setEnabled(false);
            }
            else
            {
                ui->pbVoicePlay->setEnabled(true);
                ui->pbVoiceRec->setEnabled(true);
            }
            setUpdateTime(pGraph->pGl->getFps());
        }
        else
        {
StopSun1:
            isStarted = false;
            qDebug() << "ExpertSDR: OnStart(false).";
            pOpt->ui.cbSdrType->setEnabled(true);
            killTimer(UpdatesTimerID);
            pGraph->pGl->Stop();
            UpdatesTimerID = 0;
            if(pMem->isPlay())
            {
                setLockIsPlay(false);
                pMem->Stop();
            }
            pPaVac->stop();
            pVac->close();
            pDsp->SetRx1Osc(-SaveFilter);
            pSdrCtrl->Stop();
            pCwMacro->stop();
            OnMox(RX);
            ui->pbEqRx->setChecked(true);
            ui->pbStart->setChecked(false);
            ui->pbMox->setChecked(false);
            ui->pbTone->setChecked(false);
            setLock(LOCK_MOX | LOCK_TONE | LOCK_VOICE | LOCK_DDS | LOCK_BAND);
            setWindowTitle("ExpertSDR vA2.4.1.21   www.sunsdr.com");
        }
    }
}

void ExpertSDR_vA2_1::ForceStop()
{
    if(ui->pbStart->isChecked())
        OnStart(false);
}

void ExpertSDR_vA2_1::OnSetPolyphase(int Status)
{
    pDsp->SetSpectrumPolyphase((bool)Status);
    pOpt->ui.chbPoliphase->setChecked((bool)Status);
}

void ExpertSDR_vA2_1::onXvAntSwitch(int num)
{
    if(ui->pbStart->isChecked())
    {
        if(ui->pbMox->isChecked())
            pSdrCtrl->setXvAnt(pOpt->ui.cbXvtrxAntTx->currentIndex());
        else
            pSdrCtrl->setXvAnt(pOpt->ui.cbXvtrxAntRx->currentIndex());
    }
}

void ExpertSDR_vA2_1::OnMox(bool Tx)
{
    if(!ui->pbMox->isEnabled())
    {
        ui->pbMox->setChecked(false);
        return;
    }
    if(!ui->pbStart->isChecked())
    {
        ui->pbMox->setChecked(false);
        return;
    }
    if(ui->pbMute->isChecked())
        return;

    if(TrxMode == (TRXMODE)Tx)
        return;

    if(!pOpt->ui.chbPaTxDelay->isChecked())
    {
        QObject *id = sender();
 //       if(((id == ui->pbMox) || (id == pPanel) || (id == pSdrCtrl)) && Tx)
 //           setRampDelayEnable(true);
 //       else
 //           setRampDelayEnable(false);
    }

    pGraph->pGl->SetTRxMode((TRXMODE)Tx);
    ExControl(CurrentBandIndex, (TRXMODE)Tx);
    ui->pbMox->setChecked(Tx);
    TrxMode = (TRXMODE)Tx;
    if(Tx)
    {
        ui->sbVarLowFreq->setEnabled(false);
        ui->sbVarHighFreq->setEnabled(false);
        cntSwitchRxTx = 0;
        OnGate(ui->slGate->value());
        OnComp(ui->slComp->value());
        ui->pbMute->setEnabled(false);
        OnDrive(ui->slDrive->value());
        SetSdrTrx(TX);
        pDsp->SetTrx(TX);
        QPoint band;
        switch(pModeBut->checkedId())
        {
        case LSB:
            band.setX(-pOpt->ui.sbTxOutFilterFreqSsb->value());
            band.setY(-pOpt->ui.sbTxInFilterFreqSsb->value());
            pDsp->SetTxOsc(0.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
            break;
        case DIGL:
            band.setX(-pOpt->ui.sbTxOutFilterFreqDig->value());
            band.setY(-pOpt->ui.sbTxInFilterFreqDig->value());
            pDsp->SetTxOsc(0.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
            break;
        case USB:
            band.setX(pOpt->ui.sbTxInFilterFreqSsb->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqSsb->value());
            pDsp->SetTxOsc(0.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
            break;
        case DIGU:
            band.setX(pOpt->ui.sbTxInFilterFreqDig->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqDig->value());
            pDsp->SetTxOsc(0.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
            break;
        case DSB:
        case AM:
        case SAM:
            band.setX(-pOpt->ui.sbTxOutFilterFreqAm->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqAm->value());
            pDsp->SetTxOsc(-12000.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhaseAmFm->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMagAmFm->value());
            break;
        case FMN:
            band.setX(-pOpt->ui.sbTxOutFilterFreqFmn->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqFmn->value());
            pDsp->SetTxOsc(-12000.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhaseAmFm->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMagAmFm->value());
            break;
        case CWU:
            band.setX(0);
            band.setY(1);
            pGraph->pGl->SpectrumEnable(false);
            pDsp->SetTxOsc(0.0);
            if(!pCwMacro->pCwCore->isTxMode())
            {
                pCwMacro->pCwCore->reset();
            }
            break;
        case CWL:
            band.setX(0);
            band.setY(-1);
            pGraph->pGl->SpectrumEnable(false);
            pDsp->SetTxOsc(0.0);
            if(!pCwMacro->pCwCore->isTxMode())
            {
                pCwMacro->pCwCore->reset();
            }
            break;
        case SPEC:
            band.setY(pOpt->ui.cbPaSampleRate->currentText().toInt()/2.0);
            band.setX(-pOpt->ui.cbPaSampleRate->currentText().toInt()/2.0);
            pDsp->SetTxOsc(0.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
            break;
        case DRM:
            band.setY(5000.0);
            band.setX(-5000.0);
            pDsp->SetTxOsc(0.0);
            pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
            pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
            break;
        default:
            break;
        }
        setLock(LOCK_MODE | LOCK_BAND | LOCK_FILTER | LOCK_RIT | LOCK_DDS | LOCK_FILTER_BAND | LOCK_SPLIT | LOCK_PREAMP);
        ui->pbEqTx->setChecked(true);
        ui->pbPreamp->setEnabled(false);
        pGraph->pGl->GetBandFilter(RxFilterBandLow, RxFilterBandHigh);
        pGraph->pGl->SetBandFilter(band.x(), band.y());
        SetTxFilter();
        tmpMicScale = DbToValMic(ui->slMic->value());
        OnMic(ui->slMic->value());
   //     pPanel->onMox(Tx);
        if(pOpt->ui.cbPaChannels->currentIndex() == 1)
            ui->swVolume->setCurrentIndex(1);
        else
            ui->swVolume->setCurrentIndex(0);
        onXvAntSwitch(0);
        pVoiceRec->setTrxMode(true);
        OnDspEq(true);
    }
    else
    {
        ui->sbVarLowFreq->setEnabled(true);
        ui->sbVarHighFreq->setEnabled(true);
        cntSwitchTxRx = 0;
        OnCpdr(ui->slCpdr->value());
        ui->pbMute->setEnabled(true);
        MicScale = 0.0;
        SetSdrTrx(RX);
        pDsp->SetTrx(RX);
        pDsp->SetRx1Osc(-pGraph->pGl->GetFilter());
        pGraph->pGl->SpectrumEnable(true);
        ui->pbMox->setChecked(false);
        pOpt->ui.cbTrTypeSignal->setCurrentIndex(0);
        StateTone = false;
        ui->pbPreamp->setEnabled(true);
        pGraph->pGl->SetLockBandFilter(false);
        pGraph->pGl->SetLockFilter(false);
        if(!pMem->isWavPlay())
            pGraph->pGl->SetLockGrid(false);
        setLock(LOCK_DIDABLE);
        ui->pbEqRx->setChecked(true);
        ui->pbTone->setChecked(false);
        pGraph->pGl->SpectrumEnable(true);
 //       pPanel->onMox(Tx);
        ui->swVolume->setCurrentIndex(0);
        onXvAntSwitch(0);
        pVoiceRec->setTrxMode(false);
    }
    OnEqModeChanged(Tx);
    pSmeter->SetTx(Tx);
    pGraph->pGl->SetTRxMode((TRXMODE)Tx);
}

void ExpertSDR_vA2_1::SetDspTrx(bool Tx)
{
    (Tx == true) ? pDsp->SetTxOsc(0.0) : pDsp->SetRx1Osc(-pGraph->pGl->GetFilter());
    pDsp->SetTrx((TRXMODE)Tx);
}

void ExpertSDR_vA2_1::SetSdrTrx(bool Tx)
{
    if(ui->pbMute->isChecked())
        return;
    int Mode = OptBands[CurrentBandIndex].CurrentModeIndex;
    if(Tx)
    {
        if(ui->pbVfoA->isChecked())
        {
            switch(pModeBut->checkedId())
            {
                case DSB:
                case AM:
                case SAM:
                case FMN:
                    pSdrCtrl->SetDdsFreq(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter() + 12000.0);
                break;
                default:
                    pSdrCtrl->SetDdsFreq(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter());
                break;
            }
        }
        else
        {
            if(ui->pbRx2->isChecked() || pGraph->pGl->IsFilter2OnPanoram())
            {
                switch(pModeBut->checkedId())
                {
                    case DSB:
                    case AM:
                    case SAM:
                    case FMN:
                        pSdrCtrl->SetDdsFreq(pVfoB->value() + 12000.0);
                    break;
                    default:
                        pSdrCtrl->SetDdsFreq(pVfoB->value());
                    break;
                }
            }
            else
            {
                switch(pModeBut->checkedId())
                {
                    case DSB:
                    case AM:
                    case SAM:
                    case FMN:
                        pGraph->pGl->SetDDSFreq(pVfoB->value() - pGraph->pGl->GetFilter());
                        pSdrCtrl->SetDdsFreq(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter() + 12000.0);
                    break;
                    default:
                        pGraph->pGl->SetDDSFreq(pVfoB->value() - pGraph->pGl->GetFilter());
                        pSdrCtrl->SetDdsFreq(pVfoB->value());
                    break;
                }
            }
        }
        pSdrCtrl->SetTrxMode(true);
        if((Mode == CWL || Mode == CWU) && pOpt->ui.chbDisMonitor->isChecked())
            pSdrCtrl->setMute(false);
        else
            pSdrCtrl->setMute(!ui->pbMon->isChecked());
    }
    else
    {
        pGraph->pGl->SetDDSFreq(FreqNum - pGraph->pGl->GetFilter());
        pSdrCtrl->SetTrxMode(false);
        pSdrCtrl->setMute(ui->pbMute->isChecked());
        pSdrCtrl->SetDdsFreq(pGraph->pGl->GetDDSFreq());
        OnChangeFilter(OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex);
    }
}

void ExpertSDR_vA2_1::OnMute(bool State)
{
    if(State)
    {
        saveVolume = ui->slVol->value();
        ui->slVol->setValue(0);
        ui->slVol->setEnabled(false);
    }
    else
    {
        ui->slVol->setEnabled(true);
        ui->slVol->setValue(saveVolume);
    }
    pSM->ui.pbMute->setChecked(State);
    ui->pbMute->setChecked(State);
    pSdrCtrl->setMute(State);
    DspMute = State;
    if(!State && ui->pbStart->isChecked() && !pMem->isWavPlay() && ((CurrentBandIndex < 12) || isGenTx))
    {
        ui->pbMox->setEnabled(true);
        ui->pbTone->setEnabled(true);
    }
    else
    {
        ui->pbMox->setEnabled(false);
        ui->pbTone->setEnabled(false);
    }
}

void ExpertSDR_vA2_1::OnVolume(int Val)
{
    ui->slVol->setValue(Val);
    pSM->ui.slVolume->setValue(Val);
    VolumeTxToRx = Val;
    Volume = Val/100.0;
    OptBands[CurrentBandIndex].Volume = Val;
}

void ExpertSDR_vA2_1::OnMonVolume(int Val)
{
    ui->slMonVol->setValue(Val);
    MonVolume = Val/100.0;
}

void ExpertSDR_vA2_1::OnAgc(int Val)
{
    if(indexAgc == 0)
        pDsp->SetFixedAgc(Val);
    else
        pDsp->SetRxAgcTop(Val);
    ui->slAgc->setValue(Val);
    OptBands[CurrentBandIndex].RfGain = Val;
}

void ExpertSDR_vA2_1::OnDrive(int Val)
{
    ui->slDrive->setValue(Val);
    DriveScale = qSqrt(Val)/100.0;
}

void ExpertSDR_vA2_1::OnMic(int Val)
{
    ui->slMic->setValue(Val);
    MicScale = DbToValMic(Val);
    tmpMicScale = MicScale;
}

void ExpertSDR_vA2_1::OnPitch(int Val)
{
    int PitchVal = 0;
    if(pModeBut->checkedId() == CWL)
        PitchVal = -Val;
    else if(pModeBut->checkedId() == DIGL)
        PitchVal = -pOpt->ui.sbDigL->value();
    else if(pModeBut->checkedId() == CWU)
        PitchVal = Val;
    else if(pModeBut->checkedId() == DIGU)
        PitchVal = pOpt->ui.sbDigU->value();//Val;
    pGraph->pGl->SetPitch(PitchVal);
    if((pModeBut->checkedId() != DIGL) && (pModeBut->checkedId() != DIGU))
    {
        OptBands[CurrentBandIndex].Pitch = Val;
        ui->slPitch->setValue(Val);
        ui->sbPitch->setValue(Val);
        pCwMacro->pCwCore->setFreq(Val);
    }
    OnChangeFilter(OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex);
    pSM->SetPitch(pGraph->pGl->GetPitch());
    SetFreq(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter());
}

void ExpertSDR_vA2_1::timerEvent(QTimerEvent *e)
{
    float *SpBuffer;
    static float GraphBuffer[NUMS_SAMPLES_FOR_MEANE][4096];
    static int SmeterCnt = 0;
    static float Smeter = 0;
    float tmp = 0;
    SpBuffer = GraphBuffer[0];
    switch(pGraph->pGl->pPanOpt->ui.cbPanMode->currentIndex())
    {
        case 1:
            pDsp->ProcessSpectrum(SpBuffer);
            pDsp->SetPwsMode(1);
        break;
        case 2:
            pDsp->ProcessSpectrum(SpBuffer);
            pDsp->SetPwsMode(0);
        break;
        case 3:
            pDsp->ProcessScope(SpBuffer, 4096);
        break;
        case 4:
            pDsp->ProcessPhase(SpBuffer, 4096);
        break;
        default:  // PANADAPTER
            pDsp->ProcessPanadapter(SpBuffer);
        break;
    }
    if(pBandBut->checkedId() == BAND2M)
        tmp += smeter2mCorr;
    else if(pBandBut->checkedId() == BAND07M)
        tmp += smeter07mCorr;

    bool Mox = ui->pbMox->isChecked();
    float pre_calc = Mox ? CalibrateLevel[0] : CalibrateLevel[pMenuPreamp->actions().indexOf(pAgPreamp->checkedAction())] + tmp - 3.0f;

    for(int i = 0; i< 4096; i++)
    {
        if(!Mox)
            SpBuffer[i] += pre_calc;
        else
            SpBuffer[i] += pre_calc + 6.0f + 3.0f;
    }
    pGraph->pGl->SetSpectrumBuffer(SpBuffer, 4096);
    float tmp1 = pDsp->CalculateMeters(SIGNAL_STRENGTH);
    pSM->SetDbm(tmp1 + CalibrateLevel[pMenuPreamp->actions().indexOf(pAgPreamp->checkedAction())] + LEVEL_SMETER);
    double Level = pDsp->CalculateMeters(ADC_REAL);
    pSmeter->SetAdcLMeterVal(Level);
    Level = pDsp->CalculateMeters(ADC_IMAG);
    pSmeter->SetAdcRMeterVal(Level);
    if(ui->pbMox->isChecked())
    {
        Smeter += qMax(-55.0f, - pDsp->CalculateMeters(MIC) + 3.0f);
        SmeterCnt++;
        if(SmeterCnt > 12)
        {
            pSmeter->SetMicMeterVal(Smeter/SmeterCnt);
            SmeterCnt = 0;
            Smeter = 0;
        }
    }
    else
    {
        Smeter += pDsp->CalculateMeters(SIGNAL_STRENGTH);
        SmeterCnt++;
        if(SmeterCnt > 6)
        {
            Smeter = (Smeter/SmeterCnt) + LEVEL_SMETER + CalibrateLevel[pMenuPreamp->actions().indexOf(pAgPreamp->checkedAction())] + tmp;
            pSmeter->SetSMeterVal(Smeter);
            SmeterCnt = 0;
            Smeter = 0;
        }
    }
}

void ExpertSDR_vA2_1::OnChangeFilterFreq(int Freq)
{
    FilterPosFreq = Freq;
    pDsp->SetRx1Osc(-Freq);
    pDsp->SetTxOsc(-Freq);
    SetFreq(Freq + pGraph->pGl->GetDDSFreq());
    MainFreqChange(Freq + pGraph->pGl->GetDDSFreq());
}

void ExpertSDR_vA2_1::SetScaleNum(long int Num)
{
    Num += pGraph->pGl->GetPitch();
    pSM->SetFreq((int)Num);
    if(Num > 2000000000)
        Num = 2000000000;
    int Digits[10];
    bool IsEnable = false;
    int Temp = Num;
    QString StrTemp;
    StrTemp.setNum(Num);
    Digits[9] = Num/1000000000;
    Temp -= Digits[9]*1000000000;
    Digits[8] = Temp/100000000;
    Temp -= Digits[8]*100000000;
    Digits[7] = Temp/10000000;
    Temp -= Digits[7]*10000000;
    Digits[6] = Temp/1000000;
    Temp -= Digits[6]*1000000;
    Digits[5] = Temp/100000;
    Temp -= Digits[5]*100000;
    Digits[4] = Temp/10000;
    Temp -= Digits[4]*10000;
    Digits[3] = Temp/1000;
    Temp -= Digits[3]*1000;
    Digits[2] = Temp/100;
    Temp -= Digits[2]*100;
    Digits[1] = Temp/10;
    Temp -= Digits[1]*10;
    Digits[0] = Temp;
    pDg0->Set(Digits[0]);
    pDg1->Set(Digits[1]);
    pDg2->Set(Digits[2]);
    pDg3->Set(Digits[3]);
    pDg4->Set(Digits[4]);
    pDg5->Set(Digits[5]);
    pDg6->Set(Digits[6]);
    pDg7->Set(Digits[7]);
    pDg8->Set(Digits[8]);
    pDg9->Set(Digits[9]);

    if(Digits[9] == 0)
    {
        IsEnable = false;
        pDg9->SetEnableNum(false);
    }
    else
    {
        IsEnable = true;
        pDg9->SetEnableNum(true);
    }

    if(Digits[8] == 0 && !IsEnable)
    {
        IsEnable = false;
        pDg8->SetEnableNum(false);
    }
    else
    {
        IsEnable = true;
        pDg8->SetEnableNum(true);
    }

    if(Digits[7] == 0 && !IsEnable)
    {
        IsEnable = false;
        pDg7->SetEnableNum(false);
    }
    else
    {
        IsEnable = true;
        pDg7->SetEnableNum(true);
    }

    if(Digits[6] == 0 && !IsEnable)
    {
        IsEnable = false;
        pDg6->SetEnableNum(false);
    }
    else
    {
        IsEnable = true;
        pDg6->SetEnableNum(true);
    }
}

int ExpertSDR_vA2_1::LoadNum()
{
    int x = 0;
    x += pDg0->Get();
    x += pDg1->Get()*10;
    x += pDg2->Get()*100;
    x += pDg3->Get()*1000;
    x += pDg4->Get()*10000;
    x += pDg5->Get()*100000;
    x += pDg6->Get()*1000000;
    x += pDg7->Get()*10000000;
    x += pDg8->Get()*100000000;
    x += pDg9->Get()*1000000000;
    x -= pGraph->pGl->GetPitch();
    return x;
}

void ExpertSDR_vA2_1::LoadChangeNum()
{
    FreqNum = LoadNum();
    pGraph->pGl->SetDDSFreq(FreqNum - pGraph->pGl->GetFilter());
    pGraph->pGl->SetFilter2(-pGraph->pGl->GetDDSFreq() + pVfoB->value());
    MainFreqChange(FreqNum);
}

void ExpertSDR_vA2_1::LowDig0()
{
    if(FreqNum - 1 >= FreqMin)
        FreqNum -= 1;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig1()
{
    if(FreqNum - 10 >= FreqMin)
        FreqNum -= 10;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig2()
{
    if(FreqNum - 100 >= FreqMin)
        FreqNum -= 100;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig3()
{
    if(FreqNum - 1000 >= FreqMin)
        FreqNum -= 1000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig4()
{
    if(FreqNum - 10000 >= FreqMin)
        FreqNum -= 10000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig5()
{
    if(FreqNum - 100000 >= FreqMin)
        FreqNum -= 100000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig6()
{
    if(FreqNum - 1000000 >= FreqMin)
        FreqNum -= 1000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig7()
{
    if(FreqNum - 10000000 >= FreqMin)
        FreqNum -= 10000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig8()
{
    if(FreqNum - 100000000 >= FreqMin)
        FreqNum -= 100000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::LowDig9()
{
    if(FreqNum - 1000000000 >= FreqMin)
        FreqNum -= 1000000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig0()
{
    if(FreqNum + 1 <= FreqMax)
        FreqNum += 1;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig1()
{
    if(FreqNum + 10 <= FreqMax)
        FreqNum += 10;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig2()
{
    if(FreqNum + 100 <= FreqMax)
        FreqNum += 100;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig3()
{
    if(FreqNum + 1000 <= FreqMax)
        FreqNum += 1000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig4()
{
    if(FreqNum + 10000 <= FreqMax)
        FreqNum += 10000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig5()
{
    if(FreqNum + 100000 <= FreqMax)
        FreqNum += 100000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig6()
{
    if(FreqNum + 1000000 <= FreqMax)
        FreqNum += 1000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig7()
{
    if(FreqNum + 10000000 <= FreqMax)
        FreqNum += 10000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig8()
{
    if(FreqNum + 100000000 <= FreqMax)
        FreqNum += 100000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::HighDig9()
{
    if(FreqNum + 1000000000.0 <= (double)FreqMax)
        FreqNum += 1000000000;
    SetScaleNum(FreqNum);
    LoadChangeNum();
}

void ExpertSDR_vA2_1::OnChangeTxGainRej(double Rej)
{
    SDRMODE m = pDsp->GetMode();
    if((m == DSB) || (m == AM) || (m == SAM) || (m == FMN))
        pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMagAmFm->value());
    else
        pDsp->SetCorrectTXIQGain(pOpt->ui.sbTxImRejMag->value());
}

void ExpertSDR_vA2_1::OnChangeTxPhaseRej(double Rej)
{
    SDRMODE m = pDsp->GetMode();
    if((m == DSB) || (m == AM) || (m == SAM) || (m == FMN))
        pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhaseAmFm->value());
    else
        pDsp->SetCorrectTXIQPhase(pOpt->ui.sbTxImRejPhase->value());
}

void ExpertSDR_vA2_1::OnChangeTxGainRejCw(double Rej)
{
    pCwMacro->pCwCore->correctGain(Rej/100.0);
}

void ExpertSDR_vA2_1::OnChangeTxPhaseRejCw(double Rej)
{
    pCwMacro->pCwCore->correctPhase(Rej/100.0);
}

void ExpertSDR_vA2_1::SetFreqMin(int Value)
{
    FreqMin = Value;
}

void ExpertSDR_vA2_1::SetFreqMax(int Value)
{
    FreqMax = Value;
}

void ExpertSDR_vA2_1::SetFreq(int Freq)
{
    FreqNum = Freq;
    SetScaleNum(Freq);
}

void ExpertSDR_vA2_1::MnAddNewFreq(int Freq, int FilterFreq)
{
    QString Str;
    int Counts;
    double TempFreq = Freq;
    TempFreq /= 1000000.0;
    Str.setNum(TempFreq, 'f', 6);
    Counts = pMenuFreqUpDown->actions().count();
    if(pMenuFreqUpDown->actions().indexOf(pFupDownAct->checkedAction()) < (pMenuFreqUpDown->actions().count()-1))
    {
        int Temp = pMenuFreqUpDown->actions().count()-1 - pMenuFreqUpDown->actions().indexOf(pFupDownAct->checkedAction());
        for(int i = 0 ; i < Temp; i++)
             pMenuFreqUpDown->removeAction(pMenuFreqUpDown->actions().last());
    } else if(Counts >= MAX_FREQ_MEMORIES)
        pMenuFreqUpDown->removeAction(pMenuFreqUpDown->actions().at(0));
    pMenuFreqUpDown->addAction(Str + " MHz");
    pMenuFreqUpDown->actions().last()->setData(FilterFreq);
    pFupDownAct->addAction(pMenuFreqUpDown->actions().last());
    pMenuFreqUpDown->actions().last()->setCheckable(true);
    pMenuFreqUpDown->actions().last()->setChecked(true);
}

void ExpertSDR_vA2_1::OnMnNext()
{
    int Index;
    Index = pMenuFreqUpDown->actions().indexOf(pFupDownAct->checkedAction());
    if(Index < pMenuFreqUpDown->actions().count()-1)
        pMenuFreqUpDown->actions().at(++Index)->setChecked(true);
    OnMenuFreqUpDownChanged(pFupDownAct->checkedAction());
}

void ExpertSDR_vA2_1::OnMnPrev()
{
    int Index;
    Index = pMenuFreqUpDown->actions().indexOf(pFupDownAct->checkedAction());
    if(Index > 0)
        pMenuFreqUpDown->actions().at(--Index)->setChecked(true);
    OnMenuFreqUpDownChanged(pFupDownAct->checkedAction());
}

void ExpertSDR_vA2_1::OnMenuFreqUpDownChanged(QAction* pAct)
{
    QString Str;
    int DdsFreqVal, FilterFreqVal;
    Str = pAct->iconText();
    Str.replace(QString(" MHz"), QString(" "));
    double Freq = Str.toDouble();
    Freq *= 1000000.0f;
    DdsFreqVal = (int)Freq - pGraph->pGl->GetFilter();
    FilterFreqVal = pAct->data().toInt();
    pGraph->pGl->SetFilter(pAct->data().toInt());
    FilterFreqVal = pGraph->pGl->GetFilter();
    pGraph->pGl->SetDDSFreq((int)Freq - pGraph->pGl->GetFilter());
    SetFreq(Freq);
    MnMainFreq = FreqNum;
    MainFreqChange((int)Freq);
    FilterFreqVal++;
    FilterFreqVal++;
}

void ExpertSDR_vA2_1::OnMnFudTimeout()
{
    if((FreqNum != MnMainFreq) && (FreqNum >= 1000000))
    {
        MnAddNewFreq(FreqNum, pGraph->pGl->GetFilter());
        MnMainFreq = FreqNum;
    }
    pMnFudTimer->stop();
}

void ExpertSDR_vA2_1::OnChangeMainFreq(int Freq)
{
    MainFreqChange(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter());
    SetFreq(pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter());

}

void ExpertSDR_vA2_1::OnChangeStationFreq(int Freq)
{
    pGraph->pGl->SetDDSFreq(Freq - pGraph->pGl->GetFilter());
    SetFreq(Freq);
    MainFreqChange(Freq);
}

void ExpertSDR_vA2_1::OnChangeMainFreqWithWdgMem(int Freq, int Mode)
{
    SetFreq(Freq);
    MainFreqChange(Freq);
    if(pModeBut->checkedId() != Mode)
    {
        pModeBut->button(Mode)->setChecked(true);
        OnChangeMode(Mode);
    }
    pGraph->pGl->SetDDSFreq(Freq - pGraph->pGl->GetFilter());
}

void ExpertSDR_vA2_1::OnChangeLowFilterFreq(int FLow)
{
    double Low, High;
    pGraph->pGl->GetBandFilter(Low, High);
    Low = FLow;
    if(ui->pbMox->isChecked())
    {
        switch(pModeBut->checkedId())
        {
        case LSB:
        case CWL:
            if(Low < -pOpt->ui.sbTxOutFilterFreqSsb->maximum())
            {
                Low = -pOpt->ui.sbTxOutFilterFreqSsb->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqSsb->setValue(qAbs(High));
            pOpt->ui.sbTxOutFilterFreqSsb->setValue(qAbs(Low));
            break;
        case DIGL:
            if(Low < -pOpt->ui.sbTxOutFilterFreqDig->maximum())
            {
                Low = -pOpt->ui.sbTxOutFilterFreqDig->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqDig->setValue(qAbs(High));
            pOpt->ui.sbTxOutFilterFreqDig->setValue(qAbs(Low));
            break;
        case USB:
        case CWU:
            if(Low < pOpt->ui.sbTxInFilterFreqSsb->minimum())
            {
                Low = pOpt->ui.sbTxOutFilterFreqSsb->minimum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqSsb->setValue(Low);
            pOpt->ui.sbTxOutFilterFreqSsb->setValue(High);
            break;
        case DIGU:
            if(Low < pOpt->ui.sbTxInFilterFreqDig->minimum())
            {
                Low = pOpt->ui.sbTxOutFilterFreqDig->minimum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqDig->setValue(Low);
            pOpt->ui.sbTxOutFilterFreqDig->setValue(High);
            break;
        case DSB:
        case AM:
        case SAM:
            if(Low < -pOpt->ui.sbTxOutFilterFreqAm->maximum())
            {
                Low = -pOpt->ui.sbTxOutFilterFreqAm->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            High = -Low;
            pGraph->pGl->SetHighBand((int)High);
            pOpt->ui.sbTxOutFilterFreqAm->setValue(High);
            break;
        case FMN:
            if(Low < -pOpt->ui.sbTxOutFilterFreqFmn->maximum())
            {
                Low = -pOpt->ui.sbTxOutFilterFreqFmn->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            High = -Low;
            pGraph->pGl->SetHighBand((int)High);
            pOpt->ui.sbTxOutFilterFreqFmn->setValue(High);
            break;
        case SPEC:
            break;
        case DRM:
            break;
        default:
            break;
        }
        SetTxFilter();
        return;
    }
    int CurrMode, CurrFilterIdx;
    CurrMode = OptBands[CurrentBandIndex].CurrentModeIndex;
    CurrFilterIdx = OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex;
    pFiltBut->button(8)->setChecked(true);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex = 8;
    pDsp->SetFilter(Low, High, 2048, RX);
    OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL = Low;
    OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH = High;
    if(ui->pbMox->isChecked())
        SetTxFilter();

    disconnect(ui->sbVarLowFreq, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBandFilter(int)));
    ui->sbVarLowFreq->setValue(FLow);
    connect(ui->sbVarLowFreq, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBandFilter(int)));
}

void ExpertSDR_vA2_1::OnChangeHighFilterFreq(int Fhigh)
{
    double Low, High;
    pGraph->pGl->GetBandFilter(Low, High);
    High = Fhigh;
    if(ui->pbMox->isChecked())
    {
        switch(pModeBut->checkedId())
        {
        case LSB:
        case CWL:
            pOpt->ui.sbTxInFilterFreqSsb->setMaximum(qAbs(Low) - 100);
            if(High > -pOpt->ui.sbTxInFilterFreqSsb->minimum())
            {
                High = -pOpt->ui.sbTxInFilterFreqSsb->minimum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqSsb->setValue(qAbs(High));
            pOpt->ui.sbTxOutFilterFreqSsb->setValue(qAbs(Low));
            break;
        case DIGL:
            pOpt->ui.sbTxInFilterFreqDig->setMaximum(qAbs(Low) - 100);
            if(High > -pOpt->ui.sbTxInFilterFreqDig->minimum())
            {
                High = -pOpt->ui.sbTxInFilterFreqDig->minimum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqDig->setValue(qAbs(High));
            pOpt->ui.sbTxOutFilterFreqDig->setValue(qAbs(Low));
            break;
        case USB:
        case CWU:
            pOpt->ui.sbTxInFilterFreqSsb->setMaximum(Fhigh - 100);
            if(High > pOpt->ui.sbTxOutFilterFreqSsb->maximum())
            {
                High = pOpt->ui.sbTxOutFilterFreqSsb->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqSsb->setValue(Low);
            pOpt->ui.sbTxOutFilterFreqSsb->setValue(High);
            break;
        case DIGU:
            pOpt->ui.sbTxInFilterFreqDig->setMaximum(Fhigh - 100);
            if(High > pOpt->ui.sbTxOutFilterFreqDig->maximum())
            {
                High = pOpt->ui.sbTxOutFilterFreqDig->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            pOpt->ui.sbTxInFilterFreqDig->setValue(Low);
            pOpt->ui.sbTxOutFilterFreqDig->setValue(High);
            break;
        case DSB:
        case AM:
        case SAM:
            if(High > pOpt->ui.sbTxOutFilterFreqAm->maximum())
            {
                High = pOpt->ui.sbTxOutFilterFreqAm->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            Low = -High;
            pGraph->pGl->SetLowBand((int)Low);
            pOpt->ui.sbTxOutFilterFreqAm->setValue(High);
            break;
        case FMN:
            if(High > pOpt->ui.sbTxOutFilterFreqFmn->maximum())
            {
                High = pOpt->ui.sbTxOutFilterFreqFmn->maximum();
                pGraph->pGl->SetBandFilter(Low, High);
            }
            Low = -High;
            pGraph->pGl->SetLowBand((int)Low);
            pOpt->ui.sbTxOutFilterFreqFmn->setValue(High);
            break;
        case SPEC:
            break;
        case DRM:
            break;
        default:
            break;
        }
        SetTxFilter();
        return;
    }
    int CurrMode, CurrFilterIdx;
    CurrMode = OptBands[CurrentBandIndex].CurrentModeIndex;
    CurrFilterIdx = OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex;
    pFiltBut->button(8)->setChecked(true);
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex = 8;
    pDsp->SetFilter(Low, High, 2048, RX);
    OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterL = Low;
    OptBands[CurrentBandIndex].Mode[CurrMode].VarFilterH = High;
    if(ui->pbMox->isChecked())
        SetTxFilter();

    disconnect(ui->sbVarHighFreq, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBandFilter(int)));
    ui->sbVarHighFreq->setValue(Fhigh);
    connect(ui->sbVarHighFreq, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBandFilter(int)));
}

void ExpertSDR_vA2_1::OnSbTxFilterChanged(int Param)
{
    double Low, High;
    if(ui->pbMox->isChecked())
    {
        switch(pModeBut->checkedId())
        {
        case LSB:
        case CWL:
            Low = -pOpt->ui.sbTxOutFilterFreqSsb->value();
            High = -pOpt->ui.sbTxInFilterFreqSsb->value();
            pOpt->ui.sbTxInFilterFreqSsb->setMaximum(qAbs(Low) - 100);
            pGraph->pGl->SetBandFilter(Low, High);
        break;
        case DIGL:
            Low = -pOpt->ui.sbTxOutFilterFreqDig->value();
            High = -pOpt->ui.sbTxInFilterFreqDig->value();
            pOpt->ui.sbTxInFilterFreqDig->setMaximum(qAbs(Low) - 100);
            pGraph->pGl->SetBandFilter(Low, High);
        break;
        case USB:
        case CWU:
            High = pOpt->ui.sbTxOutFilterFreqSsb->value();
            Low = pOpt->ui.sbTxInFilterFreqSsb->value();
            pOpt->ui.sbTxInFilterFreqSsb->setMaximum(High - 100);
            pGraph->pGl->SetBandFilter(Low, High);
        break;
        case DIGU:
            High = pOpt->ui.sbTxOutFilterFreqDig->value();
            Low = pOpt->ui.sbTxInFilterFreqDig->value();
            pOpt->ui.sbTxInFilterFreqDig->setMaximum(High - 100);
            pGraph->pGl->SetBandFilter(Low, High);
        break;
        case DSB:
        case AM:
        case SAM:
            High = pOpt->ui.sbTxOutFilterFreqAm->value();
            Low = -High;
            pGraph->pGl->SetBandFilter(-High, High);
        break;
        case FMN:
            High = pOpt->ui.sbTxOutFilterFreqFmn->value();
            Low = -High;
            pGraph->pGl->SetBandFilter(-High, High);
        break;
        default:
        break;
        }
        SetTxFilter();
        return;
    }
    else
    {
        QPoint band;
        switch(pModeBut->checkedId())
        {
        case LSB:
        case CWL:
            band.setX(-pOpt->ui.sbTxOutFilterFreqSsb->value());
            band.setY(-pOpt->ui.sbTxInFilterFreqSsb->value());
            break;
        case DIGL:
            band.setX(-pOpt->ui.sbTxOutFilterFreqDig->value());
            band.setY(-pOpt->ui.sbTxInFilterFreqDig->value());
            break;
        case USB:
        case CWU:
            band.setX(pOpt->ui.sbTxInFilterFreqSsb->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqSsb->value());
            break;
        case DIGU:
            band.setX(pOpt->ui.sbTxInFilterFreqDig->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqDig->value());
            break;
        case DSB:
        case AM:
        case SAM:
            band.setX(-pOpt->ui.sbTxOutFilterFreqAm->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqAm->value());
            break;
        case FMN:
            band.setX(-pOpt->ui.sbTxOutFilterFreqFmn->value());
            band.setY(pOpt->ui.sbTxOutFilterFreqFmn->value());
            break;
        case SPEC:
            band.setY(pOpt->ui.cbPaSampleRate->currentText().toInt()/2.0);
            band.setX(-pOpt->ui.cbPaSampleRate->currentText().toInt()/2.0);
            break;
        case DRM:
            band.setY(5000.0);
            band.setX(-5000.0);
            break;
        default:
            break;
        }
        double high = band.y();
        double low = band.x();
        pGraph->pGl->SetTxBandFilter(low, high);
    }
}

void ExpertSDR_vA2_1::OnTimeChange()
{
    pDsp->DspCurrentPos = pGraph->ui.slTime->value();
    OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].FilterIndex = 8;
}

void ExpertSDR_vA2_1::OnTimePlus()
{
    pGraph->ui.slTime->setValue(pGraph->ui.slTime->value() + 100);
    pDsp->DspCurrentPos = pGraph->ui.slTime->value();
}

void ExpertSDR_vA2_1::OnTimeMinus()
{
    pGraph->ui.slTime->setValue(pGraph->ui.slTime->value() - 100);
    pDsp->DspCurrentPos = pGraph->ui.slTime->value();
}

void ExpertSDR_vA2_1::OnTimeUp()
{
    pGraph->ui.slTime->setValue(0);
    pDsp->DspCurrentPos = pGraph->ui.slTime->value();
}

void ExpertSDR_vA2_1::MainFreqChange(int Freq)
{
    pMnFudTimer->stop();
    pMnFudTimer->start(1000);
    FilterPosFreq = pGraph->pGl->GetFilter();
    int DdsFreq = Freq - FilterPosFreq;
    pSdrCtrl->SetDdsFreq(DdsFreq);
    pMem->setCentralFreq(DdsFreq);
    if(DdsFreq <= BPF160_F)
    {
        if ((Freq >= BPF160_START) && (Freq <= BPF160_END))
        {
            if(CurrentBandIndex != BAND160M)
            {
                OptBands[BAND160M].MainFreq = Freq;
                OnChangeBand(BAND160M);
                pBandBut->button(BAND160M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF160_F) && (DdsFreq <= BPF80_F))
    {
        if ((Freq >= BPF80_START) && (Freq <= BPF80_END))
        {
            if(CurrentBandIndex != BAND80M)
            {
                OptBands[BAND80M].MainFreq = Freq;
                OnChangeBand(BAND80M);
                pBandBut->button(BAND80M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF80_F) && (DdsFreq <= BPF60_F))
    {
        if ((Freq >= BPF60_START) && (Freq <= BPF60_END))
        {
            if(CurrentBandIndex != BAND60M)
            {
                OptBands[BAND60M].MainFreq = Freq;
                OnChangeBand(BAND60M);
                pBandBut->button(BAND60M)->setChecked(true);
            };
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF60_F) && (DdsFreq <= BPF40_F))
    {
        if ((Freq >= BPF40_START) && (Freq <= BPF40_END))
        {
            if(CurrentBandIndex != BAND40M)
            {
                OptBands[BAND40M].MainFreq = Freq;
                OnChangeBand(BAND40M);
                pBandBut->button(BAND40M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF40_F) && (DdsFreq <= BPF30_F))
    {
        if ((Freq >= BPF30_START) && (Freq <= BPF30_END))
        {
            if(CurrentBandIndex != BAND30M)
            {
                OptBands[BAND30M].MainFreq = Freq;
                OnChangeBand(BAND30M);
                pBandBut->button(BAND30M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF30_F) && (DdsFreq <= BPF20_F))
    {
        if ((Freq >= BPF20_START) && (Freq <= BPF20_END))
        {
            if(CurrentBandIndex != BAND20M)
            {
                OptBands[BAND20M].MainFreq = Freq;
                OnChangeBand(BAND20M);
                pBandBut->button(BAND20M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF20_F) && (DdsFreq <= BPF17_F))
    {
        if ((Freq >= BPF17_START) && (Freq <= BPF17_END))
        {
            if(CurrentBandIndex != BAND17M)
            {
                OptBands[BAND17M].MainFreq = Freq;
                OnChangeBand(BAND17M);
                pBandBut->button(BAND17M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF17_F) && (DdsFreq <= BPF15_F))
    {
        if ((Freq >= BPF15_START) && (Freq <= BPF15_END))
        {
            if(CurrentBandIndex != BAND15M)
            {
                OptBands[BAND15M].MainFreq = Freq;
                OnChangeBand(BAND15M);
                pBandBut->button(BAND15M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF15_F) && (DdsFreq <= BPF12_F))
    {
        if ((Freq >= BPF12_START) && (Freq <= BPF12_END))
        {
            if(CurrentBandIndex != BAND12M)
            {
                OptBands[BAND12M].MainFreq = Freq;
                OnChangeBand(BAND12M);
                pBandBut->button(BAND12M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF12_F) && (DdsFreq <= BPF10_F))
    {
        volatile int a = BPF10_START;
        volatile int b = BPF10_END;
        a = b;

        if ((Freq >= BPF10_START) && (Freq <= BPF10_END))
        {
            if(CurrentBandIndex != BAND10M)
            {
                OptBands[BAND10M].MainFreq = Freq;
                OnChangeBand(BAND10M);
                pBandBut->button(BAND10M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((DdsFreq > BPF10_F) && (DdsFreq <= BPF6_F))
    {
        if ((Freq >= BPF6_START) && (Freq <= BPF6_END))
        {
            if(CurrentBandIndex != BAND6M)
            {
                OptBands[BAND6M].MainFreq = Freq;
                OnChangeBand(BAND6M);
                pBandBut->button(BAND6M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else if((Freq >= BPF2_START) && (Freq <= BPF2_END))
    {
        if ((Freq >= BPF2_START) && (Freq <= BPF2_END))
        {
            if(CurrentBandIndex != BAND2M)
            {
                OptBands[BAND2M].MainFreq = Freq;
                OnChangeBand(BAND2M);
                pBandBut->button(BAND2M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    else
    {
        if ((Freq >= BPF07_START) && (Freq <= BPF07_END))
        {
            if(CurrentBandIndex != BAND07M)
            {
                OptBands[BAND07M].MainFreq = Freq;
                OnChangeBand(BAND07M);
                pBandBut->button(BAND07M)->setChecked(true);
            }
        }
        else
        {
            if(CurrentBandIndex != BANDGEN)
            {
                OptBands[BANDGEN].MainFreq = Freq;
                OnChangeBand(BANDGEN);
                pBandBut->button(BANDGEN)->setChecked(true);
            }
        }
    }
    OptBands[CurrentBandIndex].MainFreq = Freq;
    emit TuneChanged(Freq);
}

void ExpertSDR_vA2_1::OnAmCarrier(int Val)
{
    pDsp->SetTxCarrierLevel(Val*0.005);
}

void ExpertSDR_vA2_1::paramForCalibration()
{
    pCalibrator->setParam(FreqNum, pGraph->pGl->GetFilter(), pModeBut->checkedId(), pFiltBut->checkedId(), OptBands[CurrentBandIndex].Mode[OptBands[CurrentBandIndex].CurrentModeIndex].cbPreampIndex);
}

void ExpertSDR_vA2_1::ChangeValcoder(int val)
{
    if(ui->pbLock->isChecked())
        return;

    int Index =  pMenuStep->actions().indexOf(pAgStep->checkedAction());
    int StepFreq = 0;
    int IntStepFreq = 0;
    switch(Index)
    {
    case 0:
        StepFreq = 1*val;
        IntStepFreq = 1;
        break;
    case 1:
        StepFreq = 5*val;
        IntStepFreq = 5;
        break;
    case 2:
        StepFreq = 10*val;
        IntStepFreq = 10;
        break;
    case 3:
        StepFreq = 25*val;
        IntStepFreq = 25;
        break;
    case 4:
        StepFreq = 50*val;
        IntStepFreq = 50;
        break;
    case 5:
        StepFreq = 100*val;
        IntStepFreq = 100;
        break;
    case 6:
        StepFreq = 250*val;
        IntStepFreq = 250;
        break;
    case 7:
        StepFreq = 500*val;
        IntStepFreq = 500;
        break;
    case 8:
        StepFreq = 1000*val;
        IntStepFreq = 1000;
        break;
    case 9:
        StepFreq = 5000*val;
        IntStepFreq = 5000;
        break;
    case 10:
        StepFreq = 10000*val;
        IntStepFreq = 10000;
        break;
    case 11:
        StepFreq = 25000*val;
        IntStepFreq = 25000;
        break;
    case 12:
        StepFreq = 50000*val;
        IntStepFreq = 50000;
        break;
    case 13:
        StepFreq = 100000*val;
        IntStepFreq = 100000;
        break;
    case 14:
        StepFreq = 250000*val;
        IntStepFreq = 250000;
        break;
    case 15:
        StepFreq = 500000*val;
        IntStepFreq = 500000;
        break;
    case 16:
        StepFreq = 1000000*val;
        IntStepFreq = 1000000;
        break;
    case 17:
        StepFreq = 5000000*val;
        IntStepFreq = 5000000;
        break;
    case 18:
        StepFreq = 10000000*val;
        IntStepFreq = 10000000;
        break;
    default:
        StepFreq = 250*val;
        IntStepFreq = 250;
        break;
    }
    int filter = pGraph->pGl->GetFilter();
    int freq = FreqNum - filter;
    int delta = filter + freq;
    delta /= IntStepFreq;
    delta *= IntStepFreq;
    delta -= filter;
    delta += StepFreq;
    pGraph->pGl->SetDDSFreq(delta);
    pGraph->pGl->SetFilter(filter);
    OnChangeMainFreq(delta);
}

/*bool ExpertSDR_vA2_1::winEvent(MSG *msg,long *result)
{
    int msgType = msg->message;
    if(msgType == WM_DEVICECHANGE)
    {
        switch(msg->wParam)
        {
            case DBT_DEVNODES_CHANGED:
               pPanel->onWinEvent();
            break;
            case DBT_DEVICEREMOVECOMPLETE:
                pPanel->onWinEvent();
            break;
            default:
            break;
        }
    }
    return false;
} */

int ExpertSDR_vA2_1::MainStep()
{
    return pMenuStep->actions().indexOf(pAgStep->checkedAction());
}

int ExpertSDR_vA2_1::MainMod()
{
    return pModeBut->checkedId();
}

int ExpertSDR_vA2_1::MainAGC()
{
    return pMenuAgc->actions().indexOf(pAgAgc->checkedAction());
}

int ExpertSDR_vA2_1::MainSmeter()
{
    return (int)pDsp->CalculateMeters(SIGNAL_STRENGTH);
}

void ExpertSDR_vA2_1::ChangeRxInputSignal(int index)
{
    RxInputSignal = index;
}

void ExpertSDR_vA2_1::ChangeRxFreqSignal(int val)
{
    RxFreqSignal = val;
}

void ExpertSDR_vA2_1::onBreakInDelay(int val)
{
    pCwMacro->pCwCore->setBreakInDelay(val);
}

void ExpertSDR_vA2_1::onBreakIn(int en)
{
    pCwMacro->pCwCore->setBreakIn((bool)en);
}

void ExpertSDR_vA2_1::onSwapKeys(int en)
{
    pCwMacro->pCwCore->swapKeys(en);
}

void ExpertSDR_vA2_1::onKeyAutoMode(int en)
{
    pCwMacro->pCwCore->setAutoMode(en);
}

void ExpertSDR_vA2_1::onCatTx()
{
    if(ui->pbMox->isChecked()) return;
    OnMox(true);
}

void ExpertSDR_vA2_1::onCatRx()
{
    if(!ui->pbMox->isChecked()) return;
    OnMox(false);
}

void ExpertSDR_vA2_1::ChangeTxInputSignal(int index)
{
    TxInputSignal = index;
}

void ExpertSDR_vA2_1::OnLineInGain(int Val)
{
    LineInGainScale = Val/100.0;
}

void ExpertSDR_vA2_1::OnMicInGain(int Val)
{
    MicGainScale = qPow(10.0, pOpt->ui.sbMicInGain->value()/20.0);
}

void ExpertSDR_vA2_1::SetTxFilter()
{
    QPoint band;
    switch(pModeBut->checkedId())
    {
    case LSB:
        band.setX(-pOpt->ui.sbTxOutFilterFreqSsb->value());
        band.setY(-pOpt->ui.sbTxInFilterFreqSsb->value());
        break;
    case CWL:
        band.setX(-3000);
        band.setY(0);
        break;
    case DIGL:
        band.setX(-pOpt->ui.sbTxOutFilterFreqDig->value());
        band.setY(-pOpt->ui.sbTxInFilterFreqDig->value());
        break;
    case USB:
        band.setX(pOpt->ui.sbTxInFilterFreqSsb->value());
        band.setY(pOpt->ui.sbTxOutFilterFreqSsb->value());
        break;
    case CWU:
        band.setX(0);
        band.setY(3000);
        break;
    case DIGU:
        band.setX(pOpt->ui.sbTxInFilterFreqDig->value());
        band.setY(pOpt->ui.sbTxOutFilterFreqDig->value());
        break;
    case DSB:
    case AM:
    case SAM:
        band.setX(-pOpt->ui.sbTxOutFilterFreqAm->value());
        band.setY(pOpt->ui.sbTxOutFilterFreqAm->value());
        break;
    case FMN:
        band.setX(-pOpt->ui.sbTxOutFilterFreqFmn->value());
        band.setY(pOpt->ui.sbTxOutFilterFreqFmn->value());
        break;
    case SPEC:
        band.setY(pOpt->ui.cbPaSampleRate->currentText().toInt()/2.0);
        band.setX(-pOpt->ui.cbPaSampleRate->currentText().toInt()/2.0);
        break;
    case DRM:
        band.setY(5000.0);
        band.setX(-5000.0);
        break;
    default:
        break;
    }
    double high = band.y();
    double low = band.x();
    pGraph->pGl->SetTxBandFilter(low, high);
    pDsp->SetFilter(low, high, 2048, TX);
}

void ExpertSDR_vA2_1::SetLockMode(bool state)
{
    ui->pbLSB->setEnabled(state);
    ui->pbUSB->setEnabled(state);
    ui->pbDSB->setEnabled(state);
    ui->pbAM->setEnabled(state);
    ui->pbSAM->setEnabled(state);
    ui->pbFMN->setEnabled(state);
    ui->pbCWU->setEnabled(state);
    ui->pbCWL->setEnabled(state);
    ui->pbSPEC->setEnabled(state);
    ui->pbDIGL->setEnabled(state);
    ui->pbDIGU->setEnabled(state);
    ui->pbDRM->setEnabled(state);
}

void ExpertSDR_vA2_1::SetLockBand(bool state)
{
    ui->pb160M->setEnabled(state);
    ui->pb80M->setEnabled(state);
    ui->pb60M->setEnabled(state);
    ui->pb40M->setEnabled(state);
    ui->pb30M->setEnabled(state);
    ui->pb20M->setEnabled(state);
    ui->pb17M->setEnabled(state);
    ui->pb15M->setEnabled(state);
    ui->pb12M->setEnabled(state);
    ui->pb10M->setEnabled(state);
    ui->pb6M->setEnabled(state);
    if(pOpt->ui.chbXvtrxEnable->isChecked())
    {
        ui->pb2M->setEnabled(state);
        ui->pb07M->setEnabled(state);
    }
    else
    {
        ui->pb2M->setEnabled(false);
        ui->pb07M->setEnabled(false);
    }
    ui->pbGEN->setEnabled(state);
}

void ExpertSDR_vA2_1::SetLockFilters(bool state)
{
    ui->pbF0->setEnabled(state);
    ui->pbF1->setEnabled(state);
    ui->pbF2->setEnabled(state);
    ui->pbF3->setEnabled(state);
    ui->pbF4->setEnabled(state);
    ui->pbF5->setEnabled(state);
    ui->pbF6->setEnabled(state);
    ui->pbF7->setEnabled(state);
    ui->pbF8->setEnabled(state);
}

void ExpertSDR_vA2_1::SetMemUpFreq()
{
    int tmp = pMem->SelectedRowIndex() + 1;
    if(tmp >= pMem->ui.twMem->rowCount())
        tmp = 0;
    pMem->SetAllocateRow(tmp);
}

void ExpertSDR_vA2_1::SetMemDownFreq()
{
    int tmp = pMem->SelectedRowIndex() - 1;
    if(tmp < 0)
        tmp = pMem->ui.twMem->rowCount() -1;
    pMem->SetAllocateRow(tmp);
}

void ExpertSDR_vA2_1::SetEnableFreqPanel(bool state)
{
    ui->pbDg9->setEnabled(state);
    ui->pbDg8->setEnabled(state);
    ui->pbDg7->setEnabled(state);
    ui->pbDg6->setEnabled(state);
    ui->pbDg5->setEnabled(state);
    ui->pbDg4->setEnabled(state);
    ui->pbDg3->setEnabled(state);
    ui->pbDg2->setEnabled(state);
    ui->pbDg1->setEnabled(state);
    ui->pbDg0->setEnabled(state);
    ui->pushButton_15->setEnabled(state);
}

void ExpertSDR_vA2_1::calibrateSCard()
{
    if(pOpt->ui.pbStartLevCol_2->isChecked())
    {
        if(!ui->pbStart->isChecked())
        {
            qWarning() << "ExpertSDR: calibrateSCard(): ExpertSDR not started!";
            QMessageBox::information(pMem, tr("Programm is not started."), tr("Programm must be started before calibration.\nPlease push start button and try again!"));
            pOpt->ui.pbStartLevCol_2->setChecked(false);
            return;
        }
        QString str = tr("Is the Line Out Cable unplugged?");
        QString str2 = tr("Running this test with the plug in the normal position plugged the SunSDR could cause damage to the device");
        QMessageBox msgBox;
        msgBox.setText(str);
        msgBox.setInformativeText(str2);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Yes:
            CalibrateSoundCard = true;
            this->setEnabled(false);
            pCalibrateSC->show();
            break;
        case QMessageBox::No:
            pOpt->ui.pbStartLevCol_2->setChecked(false);
            break;
        default:
            pOpt->ui.pbStartLevCol_2->setChecked(false);
            pCalibrateSC->close();
            break;
        }
    }
    else
        CalibrateSoundCard = false;
}

void ExpertSDR_vA2_1::endCalibrateCard()
{
    pOpt->ui.pbStartLevCol_2->setChecked(false);
    this->setEnabled(true);
    calibrateSCard();
}

void ExpertSDR_vA2_1::ChangedScardUotVoltage(double val)
{
    if(val < 0.001)
        val = 0.001;
    ScaleOutBuffSCard = NOMINAL_OUT_VOLTAGE/val;
}

void ExpertSDR_vA2_1::ExControl(int Band, TRXMODE Mode)
{
    BYTE Ctrl = 0;
    if(Mode == TX)
    {
        if(pOpt->ui.chbExtCtrl->isChecked())
        {
            switch((BAND_MODE)Band)
            {
            case BAND160M:
                Ctrl = (pOpt->ui.chb160Tx1->isChecked()<<0)|(pOpt->ui.chb160Tx2->isChecked()<<1)|(pOpt->ui.chb160Tx3->isChecked()<<2)|(pOpt->ui.chb160Tx4->isChecked()<<3)|(pOpt->ui.chb160Tx5->isChecked()<<4)|(pOpt->ui.chb160Tx6->isChecked()<<5);
                break;
            case BAND80M:
                Ctrl = (pOpt->ui.chb80Tx1->isChecked()<<0)|(pOpt->ui.chb80Tx2->isChecked()<<1)|(pOpt->ui.chb80Tx3->isChecked()<<2)|(pOpt->ui.chb80Tx4->isChecked()<<3)|(pOpt->ui.chb80Tx5->isChecked()<<4)|(pOpt->ui.chb80Tx6->isChecked()<<5);
                break;
            case BAND60M:
                Ctrl = (pOpt->ui.chb60Tx1->isChecked()<<0)|(pOpt->ui.chb60Tx2->isChecked()<<1)|(pOpt->ui.chb60Tx3->isChecked()<<2)|(pOpt->ui.chb60Tx4->isChecked()<<3)|(pOpt->ui.chb60Tx5->isChecked()<<4)|(pOpt->ui.chb60Tx6->isChecked()<<5);
                break;
            case BAND40M:
                Ctrl = (pOpt->ui.chb40Tx1->isChecked()<<0)|(pOpt->ui.chb40Tx2->isChecked()<<1)|(pOpt->ui.chb40Tx3->isChecked()<<2)|(pOpt->ui.chb40Tx4->isChecked()<<3)|(pOpt->ui.chb40Tx5->isChecked()<<4)|(pOpt->ui.chb40Tx6->isChecked()<<5);
                break;
            case BAND30M:
                Ctrl = (pOpt->ui.chb30Tx1->isChecked()<<0)|(pOpt->ui.chb30Tx2->isChecked()<<1)|(pOpt->ui.chb30Tx3->isChecked()<<2)|(pOpt->ui.chb30Tx4->isChecked()<<3)|(pOpt->ui.chb30Tx5->isChecked()<<4)|(pOpt->ui.chb30Tx6->isChecked()<<5);
                break;
            case BAND20M:
                Ctrl = (pOpt->ui.chb20Tx1->isChecked()<<0)|(pOpt->ui.chb20Tx2->isChecked()<<1)|(pOpt->ui.chb20Tx3->isChecked()<<2)|(pOpt->ui.chb20Tx4->isChecked()<<3)|(pOpt->ui.chb20Tx5->isChecked()<<4)|(pOpt->ui.chb20Tx6->isChecked()<<5);
                break;
            case BAND17M:
                Ctrl = (pOpt->ui.chb17Tx1->isChecked()<<0)|(pOpt->ui.chb17Tx2->isChecked()<<1)|(pOpt->ui.chb17Tx3->isChecked()<<2)|(pOpt->ui.chb17Tx4->isChecked()<<3)|(pOpt->ui.chb17Tx5->isChecked()<<4)|(pOpt->ui.chb17Tx6->isChecked()<<5);
                break;
            case BAND15M:
                Ctrl = (pOpt->ui.chb15Tx1->isChecked()<<0)|(pOpt->ui.chb15Tx2->isChecked()<<1)|(pOpt->ui.chb15Tx3->isChecked()<<2)|(pOpt->ui.chb15Tx4->isChecked()<<3)|(pOpt->ui.chb15Tx5->isChecked()<<4)|(pOpt->ui.chb15Tx6->isChecked()<<5);
                break;
            case BAND12M:
                Ctrl = (pOpt->ui.chb12Tx1->isChecked()<<0)|(pOpt->ui.chb12Tx2->isChecked()<<1)|(pOpt->ui.chb12Tx3->isChecked()<<2)|(pOpt->ui.chb12Tx4->isChecked()<<3)|(pOpt->ui.chb12Tx5->isChecked()<<4)|(pOpt->ui.chb12Tx6->isChecked()<<5);
                break;
            case BAND10M:
                Ctrl = (pOpt->ui.chb10Tx1->isChecked()<<0)|(pOpt->ui.chb10Tx2->isChecked()<<1)|(pOpt->ui.chb10Tx3->isChecked()<<2)|(pOpt->ui.chb10Tx4->isChecked()<<3)|(pOpt->ui.chb10Tx5->isChecked()<<4)|(pOpt->ui.chb10Tx6->isChecked()<<5);
                break;
            case BAND6M:
                Ctrl = (pOpt->ui.chb6Tx1->isChecked()<<0)|(pOpt->ui.chb6Tx2->isChecked()<<1)|(pOpt->ui.chb6Tx3->isChecked()<<2)|(pOpt->ui.chb6Tx4->isChecked()<<3)|(pOpt->ui.chb6Tx5->isChecked()<<4)|(pOpt->ui.chb6Tx6->isChecked()<<5);
                break;
            case BAND2M:
                Ctrl = (pOpt->ui.chb2Tx1->isChecked()<<0)|(pOpt->ui.chb2Tx2->isChecked()<<1)|(pOpt->ui.chb2Tx3->isChecked()<<2)|(pOpt->ui.chb2Tx4->isChecked()<<3)|(pOpt->ui.chb2Tx5->isChecked()<<4)|(pOpt->ui.chb2Tx6->isChecked()<<5);
                break;
            case BAND07M:
                Ctrl = (pOpt->ui.chb07Tx1->isChecked()<<0)|(pOpt->ui.chb07Tx2->isChecked()<<1)|(pOpt->ui.chb07Tx3->isChecked()<<2)|(pOpt->ui.chb07Tx4->isChecked()<<3)|(pOpt->ui.chb07Tx5->isChecked()<<4)|(pOpt->ui.chb07Tx6->isChecked()<<5);
                break;
            default:
                break;
            }
        }
        if(ui->pbPa->isChecked())
            Ctrl |= 1<<6;
    }
    else
    {
        if(pOpt->ui.chbExtCtrl->isChecked())
        {
            switch((BAND_MODE)Band)
            {
            case BAND160M:
                Ctrl = (pOpt->ui.chb160Rx1->isChecked()<<0)|(pOpt->ui.chb160Rx2->isChecked()<<1)|(pOpt->ui.chb160Rx3->isChecked()<<2)|(pOpt->ui.chb160Rx4->isChecked()<<3)|(pOpt->ui.chb160Rx5->isChecked()<<4)|(pOpt->ui.chb160Rx6->isChecked()<<5);
                break;
            case BAND80M:
                Ctrl = (pOpt->ui.chb80Rx1->isChecked()<<0)|(pOpt->ui.chb80Rx2->isChecked()<<1)|(pOpt->ui.chb80Rx3->isChecked()<<2)|(pOpt->ui.chb80Rx4->isChecked()<<3)|(pOpt->ui.chb80Rx5->isChecked()<<4)|(pOpt->ui.chb80Rx6->isChecked()<<5);
                break;
            case BAND60M:
                Ctrl = (pOpt->ui.chb60Rx1->isChecked()<<0)|(pOpt->ui.chb60Rx2->isChecked()<<1)|(pOpt->ui.chb60Rx3->isChecked()<<2)|(pOpt->ui.chb60Rx4->isChecked()<<3)|(pOpt->ui.chb60Rx5->isChecked()<<4)|(pOpt->ui.chb60Rx6->isChecked()<<5);
                break;
            case BAND40M:
                Ctrl = (pOpt->ui.chb40Rx1->isChecked()<<0)|(pOpt->ui.chb40Rx2->isChecked()<<1)|(pOpt->ui.chb40Rx3->isChecked()<<2)|(pOpt->ui.chb40Rx4->isChecked()<<3)|(pOpt->ui.chb40Rx5->isChecked()<<4)|(pOpt->ui.chb40Rx6->isChecked()<<5);
                break;
            case BAND30M:
                Ctrl = (pOpt->ui.chb30Rx1->isChecked()<<0)|(pOpt->ui.chb30Rx2->isChecked()<<1)|(pOpt->ui.chb30Rx3->isChecked()<<2)|(pOpt->ui.chb30Rx4->isChecked()<<3)|(pOpt->ui.chb30Rx5->isChecked()<<4)|(pOpt->ui.chb30Rx6->isChecked()<<5);
                break;
            case BAND20M:
                Ctrl = (pOpt->ui.chb20Rx1->isChecked()<<0)|(pOpt->ui.chb20Rx2->isChecked()<<1)|(pOpt->ui.chb20Rx3->isChecked()<<2)|(pOpt->ui.chb20Rx4->isChecked()<<3)|(pOpt->ui.chb20Rx5->isChecked()<<4)|(pOpt->ui.chb20Rx6->isChecked()<<5);
                break;
            case BAND17M:
                Ctrl = (pOpt->ui.chb17Rx1->isChecked()<<0)|(pOpt->ui.chb17Rx2->isChecked()<<1)|(pOpt->ui.chb17Rx3->isChecked()<<2)|(pOpt->ui.chb17Rx4->isChecked()<<3)|(pOpt->ui.chb17Rx5->isChecked()<<4)|(pOpt->ui.chb17Rx6->isChecked()<<5);
                break;
            case BAND15M:
                Ctrl = (pOpt->ui.chb15Rx1->isChecked()<<0)|(pOpt->ui.chb15Rx2->isChecked()<<1)|(pOpt->ui.chb15Rx3->isChecked()<<2)|(pOpt->ui.chb15Rx4->isChecked()<<3)|(pOpt->ui.chb15Rx5->isChecked()<<4)|(pOpt->ui.chb15Rx6->isChecked()<<5);
                break;
            case BAND12M:
                Ctrl = (pOpt->ui.chb12Rx1->isChecked()<<0)|(pOpt->ui.chb12Rx2->isChecked()<<1)|(pOpt->ui.chb12Rx3->isChecked()<<2)|(pOpt->ui.chb12Rx4->isChecked()<<3)|(pOpt->ui.chb12Rx5->isChecked()<<4)|(pOpt->ui.chb12Rx6->isChecked()<<5);
                break;
            case BAND10M:
                Ctrl = (pOpt->ui.chb10Rx1->isChecked()<<0)|(pOpt->ui.chb10Rx2->isChecked()<<1)|(pOpt->ui.chb10Rx3->isChecked()<<2)|(pOpt->ui.chb10Rx4->isChecked()<<3)|(pOpt->ui.chb10Rx5->isChecked()<<4)|(pOpt->ui.chb10Rx6->isChecked()<<5);
                break;
            case BAND6M:
                Ctrl = (pOpt->ui.chb6Rx1->isChecked()<<0)|(pOpt->ui.chb6Rx2->isChecked()<<1)|(pOpt->ui.chb6Rx3->isChecked()<<2)|(pOpt->ui.chb6Rx4->isChecked()<<3)|(pOpt->ui.chb6Rx5->isChecked()<<4)|(pOpt->ui.chb6Rx6->isChecked()<<5);
                break;
            case BAND2M:
                Ctrl = (pOpt->ui.chb2Rx1->isChecked()<<0)|(pOpt->ui.chb2Rx2->isChecked()<<1)|(pOpt->ui.chb2Rx3->isChecked()<<2)|(pOpt->ui.chb2Rx4->isChecked()<<3)|(pOpt->ui.chb2Rx5->isChecked()<<4)|(pOpt->ui.chb2Rx6->isChecked()<<5);
                break;
            case BAND07M:
                Ctrl = (pOpt->ui.chb07Rx1->isChecked()<<0)|(pOpt->ui.chb07Rx2->isChecked()<<1)|(pOpt->ui.chb07Rx3->isChecked()<<2)|(pOpt->ui.chb07Rx4->isChecked()<<3)|(pOpt->ui.chb07Rx5->isChecked()<<4)|(pOpt->ui.chb07Rx6->isChecked()<<5);
                break;
            default:
                break;
            }
        }
    }
    pSdrCtrl->SetExtCtrl(Ctrl);
}

void ExpertSDR_vA2_1::ChangedTimeMachine(int value)
{
    if(value != 0)
    {
        SetLockMode(false);
        SetLockBand(false);
        SetEnableFreqPanel(false);
        pGraph->pGl->SetLockGrid(true);
    }
    else
    {
        SetLockMode(true);
        SetLockBand(true);
        SetEnableFreqPanel(true);
        pGraph->pGl->SetLockGrid(false);
    }
}

void ExpertSDR_vA2_1::ChangeSwapLineIn(int val)
{
    bSwapLineIn = (bool)val;
}

void ExpertSDR_vA2_1::ChangeSwapLineOut(int val)
{
    bSwapLineOut = (bool)val;
}

void ExpertSDR_vA2_1::SMeterClosed()
{
    ui->pbSm->setChecked(false);
}

void ExpertSDR_vA2_1::onSm2mCorr(double val)
{
    smeter2mCorr = val;
}

void ExpertSDR_vA2_1::onSm07mCorr(double val)
{
    smeter07mCorr = val;
}

void ExpertSDR_vA2_1::PowerCorrect160(int Val)
{
    Val160 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect80(int Val)
{
    Val80 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect60(int Val)
{
    Val60 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect40(int Val)
{
    Val40 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect30(int Val)
{
    Val30 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect20(int Val)
{
    Val20 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect17(int Val)
{
    Val17 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect15(int Val)
{
    Val15 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect12(int Val)
{
    Val12 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect10(int Val)
{
    Val10 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect6(int Val)
{
    Val6 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect2(int Val)
{
    Val2 = DbToVal(Val);
}

void ExpertSDR_vA2_1::PowerCorrect07(int Val)
{
    Val07 = DbToVal(Val);
}

void ExpertSDR_vA2_1::ChangedCalibrateNum(double Num)
{
    CalibrateFreq = Num;
    pSdrCtrl->SetDdsCalibrate(CalibrateFreq);
    OnChangeStationFreq(FreqNum);
}

void ExpertSDR_vA2_1::IsKeyTrue(bool State)
{
    if(ui->pbMute->isChecked())
        return;
    KeyTrye = State;
}

void ExpertSDR_vA2_1::OnTune()
{
    int Mode = pModeBut->checkedId();
    if((Mode == CWL) || (Mode == CWU) || (Mode == DIGL) || (Mode == DIGU) || (Mode == SPEC) || ui->pbVac->isChecked())
    {
        pVoiceRec->Stop();
        ui->pbVoicePlay->setEnabled(false);
        ui->pbVoiceRec->setEnabled(false);
    }
    else
    {
        ui->pbVoicePlay->setEnabled(true);
        ui->pbVoiceRec->setEnabled(true);
    }
    Mode = OptBands[CurrentBandIndex].CurrentModeIndex;
    if(ui->pbTone->isChecked())
    {
        ui->pbMox->setChecked(true);
        LastToneFilterLow = pOpt->ui.sbTxInFilterFreqSsb->value();
        LastToneFilterHigh = pOpt->ui.sbTxOutFilterFreqSsb->value();
        OnMox(true);
        switch(Mode)
        {
        case USB:
        case DIGU:
            RxFreqSignal = 1000.0;
            pDsp->SetFilter(RxFreqSignal - 100, RxFreqSignal + 100, 2048, TX);
            break;

        case CWU:
            RxFreqSignal = ui->slPitch->value();
            pDsp->SetFilter(RxFreqSignal - 100, RxFreqSignal + 100, 2048, TX);
            break;
        case LSB:
        case DIGL:
            RxFreqSignal = 1000.0;
            pDsp->SetFilter(-RxFreqSignal - 100, -RxFreqSignal + 100, 2048, TX);
            break;
        case CWL:
            RxFreqSignal = ui->slPitch->value();
            pDsp->SetFilter(-RxFreqSignal - 100, -RxFreqSignal + 100, 2048, TX);
            break;
        case DSB:
        case AM:
        case SAM:
        case FMN:
            RxFreqSignal = 1000.0;
            pDsp->SetFilter(RxFreqSignal - 100, RxFreqSignal + 100, 2048, TX);
            break;
        default:
            break;
        }
        pOpt->ui.cbTrTypeSignal->setCurrentIndex(1);
        StateTone = true;
    }
    else
    {
        RxFreqSignal = pOpt->ui.slRxSignalFreq->value();
        ui->pbMox->setChecked(false);
        pOpt->ui.cbTrTypeSignal->setCurrentIndex(0);
        OnMox(false);
        StateTone = false;
    }
}

void ExpertSDR_vA2_1::ChangedCWMon(int State)
{
    ChangedCWMon((bool)State);
}

void ExpertSDR_vA2_1::ChangedCWMon(bool State)
{
    int Mode = OptBands[CurrentBandIndex].CurrentModeIndex;
    ui->pbMon->setChecked((bool)State);
    pOpt->ui.chbDisMonitor->setChecked((bool)State);
    if((Mode == CWL || Mode == CWU) && pOpt->ui.chbDisMonitor->isChecked())
        pSdrCtrl->setMute(false);
    else
    {
        if(ui->pbMox->isChecked())
            pSdrCtrl->setMute(!State);
        else if(!ui->pbMon->isChecked())
            pSdrCtrl->setMute(false);
    }
}

void ExpertSDR_vA2_1::OnPanChangeBand(int Val)
{
    if(ui->pbLock->isChecked())
        return;

    int Band = CurrentBandIndex + Val;
    if(Band > 13)
        Band = 0;
    if(Band < 0)
        Band = 13;
    OnChangeBand(Band);
}

void ExpertSDR_vA2_1::OnPanChangeMode(int Val)
{
    if(timerClickMode.getMilliSeconds() < 100)
        return;
    timerClickMode.start();

    qDebug() << "ExpertSDR: OnPanChangeMode( " << Val << " )";
    int Mode = pModeBut->checkedId();
    switch((SDRMODE)Mode)
    {
    case LSB:
        if(Val > 0)
            Mode = USB;
        else
            Mode = DRM;
        break;
    case USB:
        if(Val > 0)
            Mode = CWL;
        else
            Mode = LSB;
        break;
    case DSB:
        if(Val > 0)
            Mode = AM;
        else
            Mode = CWU;
        break;
    case CWL:
        if(Val > 0)
            Mode = CWU;
        else
            Mode = USB;
        break;
    case CWU:
        if(Val > 0)
            Mode = DSB;
        else
            Mode = CWL;
        break;
    case FMN:
        if(Val > 0)
            Mode = SPEC;
        else
            Mode = SAM;
        break;
    case AM:
        if(Val > 0)
            Mode = SAM;
        else
            Mode = DSB;
        break;
    case DIGU:
        if(Val > 0)
            Mode = DRM;
        else
            Mode = DIGL;
        break;
    case SPEC:
        if(Val > 0)
            Mode = DIGL;
        else
            Mode = FMN;
        break;
    case DIGL:
        if(Val > 0)
            Mode = DIGU;
        else
            Mode = SPEC;
        break;
    case SAM:
        if(Val > 0)
            Mode = FMN;
        else
            Mode = AM;
        break;
    case DRM:
        if(Val > 0)
            Mode = LSB;
        else
            Mode = DIGU;
        break;
    default:
        if(Val > 0)
            Mode = USB;
        else
            Mode = LSB;
        break;
    }
    OnChangeMode(Mode);
}

void ExpertSDR_vA2_1::OnPanChangeFilter(int Val)
{
    if(timerClickFilter.getMilliSeconds() < 100)
        return;
    timerClickFilter.start();

    int Index = pFiltBut->checkedId();
    Index += Val;
    if(Index > 8)
        Index = 0;
    if(Index < 0)
        Index = 8;
    OnChangeFilter(Index);
}

void ExpertSDR_vA2_1::OnPanChangeAgc(int Val)
{
    int Index = pAgAgc->actions().indexOf(pAgAgc->checkedAction());
    Index += Val;
    if(Index > 4)
        Index = 0;
    if(Index < 0)
        Index = 4;
    pMenuAgc->actions().at(Index)->setChecked(true);
    OnAgcChanged(pMenuAgc->actions().at(Index));
}

void ExpertSDR_vA2_1::OnPanChangePreamp(int Val)
{
    int Index = pAgPreamp->actions().indexOf(pAgPreamp->checkedAction());
    Index += Val;
    if(Index > 4)
        Index = 0;
    if(Index < 0)
        Index = 4;
    pMenuPreamp->actions().at(Index)->setChecked(true);
    OnPreampChanged(pMenuPreamp->actions().at(Index));
}

void ExpertSDR_vA2_1::OnPanChangeStep(int Val)
{
    int Index = pAgStep->actions().indexOf(pAgStep->checkedAction());
    Index += Val;
    if(Index > 18)
        Index = 0;
    if(Index < 0)
        Index = 18;
    pMenuStep->actions().at(Index)->setChecked(true);
    OnStepChanged(pMenuStep->actions().at(Index));
}

void ExpertSDR_vA2_1::OnPanChangeBin()
{
    OnBin(!ui->pbBin->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeNr()
{
    OnNr(!ui->pbNr->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeAnf()
{
    OnAnf(!ui->pbAnf->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeNb()
{
    OnNb(!ui->pbNb->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeNb2()
{
    OnNb2(!ui->pbNb2->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeLock()
{
    OnLock(!ui->pbLock->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeMemory(int val)
{
    if(timerClickMem.getMilliSeconds() < 100)
        return;

    timerClickMem.start();
    if(val > 0)
        SetMemUpFreq();
    else if(val < 0)
        SetMemDownFreq();
}

void ExpertSDR_vA2_1::OnPanChangeSqlState()
{
    OnSql(!ui->pbSql->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangePa()
{
    OnPaOn(!ui->pbPa->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeStation(int val)
{
    if(val > 0)
        OnMnNext();
    else if(val < 0)
        OnMnPrev();
}

void ExpertSDR_vA2_1::OnPanChangeMute()
{
    if(ui->pbMute->isChecked())
        ui->pbMute->setChecked(false);
    else
        ui->pbMute->setChecked(true);
    OnMute(ui->pbMute->isChecked());
}

void ExpertSDR_vA2_1::OnPanChangeStart()
{
    OnStart(!ui->pbStart->isChecked());
}

void ExpertSDR_vA2_1::OnPanVolume(int Val)
{
    int Vol = ui->slVol->value();
    Vol += Val;
    if(Vol > 100)
        Vol = 100;
    if(Vol < 0)
        Vol = 0;
    OnVolume(Vol);
}

void ExpertSDR_vA2_1::OnPanMicGain(int Val)
{
    int Vol = ui->slMic->value();
    Vol += Val;
    if(Vol > 70)
        Vol = 70;
    if(Vol < 0)
        Vol = 0;
    OnMic(Vol);
}

void ExpertSDR_vA2_1::OnPanSquelch(int Val)
{
    int SqlVal = ui->slSql->value();
    SqlVal += Val;
    if(SqlVal > 0)
        SqlVal = 0;
    if(SqlVal < -160)
        SqlVal = -160;
    OnSql(SqlVal);
}

void ExpertSDR_vA2_1::OnPanFilterHigh(int Val)
{
    if(timerChangePanelFilterHigh.getMilliSeconds() < 10)
        return;
    timerChangePanelFilterHigh.start();
    double Low, High;
    pGraph->pGl->GetBandFilter(Low, High);
    High += Val;
    if(High < Low)
        High = Low + 20;
    pGraph->pGl->SetBandFilter(Low, High);
    OnChangeHighFilterFreq(High);
}

void ExpertSDR_vA2_1::OnPanFilterLow(int Val)
{
    if(timerChangePanelFilterLow.getMilliSeconds() < 10)
        return;
    timerChangePanelFilterLow.start();
    double Low, High;
    pGraph->pGl->GetBandFilter(Low, High);
    Low += Val;
    if(High < Low)
        Low = High - 20;
    pGraph->pGl->SetBandFilter(Low, High);
    OnChangeLowFilterFreq(Low);
}

void ExpertSDR_vA2_1::OnPanFilterWidth(int Val)
{
    if(timerChangePanelFilterWidth.getMilliSeconds() < 10)
        return;
    timerChangePanelFilterWidth.start();
    double Low, High;
    float Tmp = Val/2.0f;
    pGraph->pGl->GetBandFilter(Low, High);
    double centre = (High - Low) / 2.0;
    Low -= Tmp;
    High += Tmp;
    if(High < Low)
    {
        Low = centre - 20.0;
        High = centre + 20.0;
    }
    else if(qAbs(High - Low) < 20.0)
    {
        Low = High - Low;
        High = High - Low;
        Low -= 10.0;
        High += 10.0;
    }
    pGraph->pGl->SetBandFilter(Low, High);
    OnChangeHighFilterFreq(High);
    OnChangeLowFilterFreq(Low);
}

void ExpertSDR_vA2_1::OnPanFilterShift(int Val)
{
    if(timerChangePanelFilterShift.getMilliSeconds() < 10)
        return;
    timerChangePanelFilterShift.start();
    double Low, High;
    pGraph->pGl->GetBandFilter(Low, High);
    Low += Val;
    High += Val;
    pGraph->pGl->SetBandFilter(Low, High);
    OnChangeHighFilterFreq(High);
    OnChangeLowFilterFreq(Low);
}

void ExpertSDR_vA2_1::OnPanDrive(int Val)
{
    int Drv = ui->slDrive->value();
    Drv += Val*20;
    if(Drv > ui->slDrive->maximum())
        Drv = ui->slDrive->maximum();
    if(Drv < ui->slDrive->minimum())
        Drv = ui->slDrive->minimum();
    OnDrive(Drv);
}

void ExpertSDR_vA2_1::OnPanIF(int Val)
{
    if(timerChangePanelIF.getMilliSeconds() < 10)
        return;
    timerChangePanelIF.start();
    if(ui->pbLock->isChecked())
        return;
    int FiltFreq = pGraph->pGl->GetFilter();
    int MFreq = 0;
    int Index =  pMenuStep->actions().indexOf(pAgStep->checkedAction());
    int StepFreq = 0;
    switch(Index)
    {
    case 0:
        StepFreq = 1;
        break;
    case 1:
        StepFreq = 5;
        break;
    case 2:
        StepFreq = 10;
        break;
    case 3:
        StepFreq = 25;
        break;
    case 4:
        StepFreq = 50;
        break;
    case 5:
        StepFreq = 100;
        break;
    case 6:
        StepFreq = 250;
        break;
    case 7:
        StepFreq = 500;
        break;
    case 8:
        StepFreq = 1000;
        break;
    case 9:
        StepFreq = 5000;
        break;
    case 10:
        StepFreq = 10000;
        break;
    case 11:
        StepFreq = 25000;
        break;
    case 12:
        StepFreq = 50000;
        break;
    case 13:
        StepFreq = 100000;
        break;
    case 14:
        StepFreq = 250000;
        break;
    case 15:
        StepFreq = 500000;
        break;
    case 16:
        StepFreq = 1000000;
        break;
    case 17:
        StepFreq = 5000000;
        break;
    case 18:
        StepFreq = 10000000;
        break;
    default:
        StepFreq = 50;
        break;
    }
    int step = Val*StepFreq;
    FiltFreq += step;
    MFreq = FiltFreq + pGraph->pGl->GetDDSFreq();
    MFreq /= StepFreq;
    MFreq *= StepFreq;
    FiltFreq = MFreq - pGraph->pGl->GetDDSFreq();
    int sr = (int)pOpt->getSampleRate()/2;
    if(FiltFreq > sr)
        FiltFreq = sr;
    else if(FiltFreq < -sr)
        FiltFreq = -sr;
    pGraph->pGl->SetFilter(FiltFreq);
    OnChangeFilterFreq(FiltFreq);
}

void ExpertSDR_vA2_1::OnPanRfGain(int Val)
{
    int AgcVal = ui->slAgc->value();
    AgcVal += Val;
    if(AgcVal > 120)
        AgcVal = 120;
    else if(AgcVal < -20)
        AgcVal = -20;
    OnAgc(AgcVal);
}

void ExpertSDR_vA2_1::OnPanZoom(int Val)
{
    pGraph->pGl->SetScaleGrid(pGraph->pGl->GetScaleGrid() + Val);
}

void ExpertSDR_vA2_1::OnIF(int Val)
{
    if((Val > ((int)pOpt->getSampleRate()/2)))
        Val = pOpt->getSampleRate()/2 - 1000;
    else if((Val < -((int)pOpt->getSampleRate()/2)))
        Val = 1000-pOpt->getSampleRate()/2;

    pGraph->pGl->SetFilter(Val);
    OnChangeFilterFreq(Val);
}

void ExpertSDR_vA2_1::OnChangePreamp(int Val)
{
    if(Val > 4)
        Val = 0;
    else if(Val < 0)
        Val = 4;
    pMenuPreamp->actions().at(Val)->setChecked(true);
    OnPreampChanged(pMenuPreamp->actions().at(Val));
}

void ExpertSDR_vA2_1::OnSplit(bool stat)
{
    if(!stat)
    {
        if(!ui->pbVfoA->isChecked())
        {
            ui->pbVfoA->setChecked(true);
            OnSwitchVfo(0);
        }
    }
    else
    {
        if(!ui->pbVfoB->isChecked())
        {
            ui->pbVfoB->setChecked(true);
            OnSwitchVfo(1);
        }
    }
}

void ExpertSDR_vA2_1::OnAtoB()
{
    if((pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter()) <= 65000000)
    {
        vfoBVal[numBandVfoB] = pVfoB->value();
        numBandVfoB = 0;
        pVfoB->setMinValue(0);
        pVfoB->setMaxValue(65000000);

        pVfoB->setValue(FreqNum + pGraph->pGl->GetPitch());
        OnVfoBChanged(pVfoB->value());
    }
    else if(((pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter()) <= BPF2_END) && ((pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter()) >= BPF2_START))
    {
        vfoBVal[numBandVfoB] = pVfoB->value();
        numBandVfoB = 1;
        pVfoB->setMinValue(BPF2_START);
        pVfoB->setMaxValue(BPF2_END);

        pVfoB->setValue(FreqNum + pGraph->pGl->GetPitch());
        OnVfoBChanged(pVfoB->value());
    }
    else if(((pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter()) <= BPF07_END) && ((pGraph->pGl->GetDDSFreq() + pGraph->pGl->GetFilter()) >= BPF07_START))
    {
        vfoBVal[numBandVfoB] = pVfoB->value();
        numBandVfoB = 2;
        pVfoB->setMinValue(BPF07_START);
        pVfoB->setMaxValue(BPF07_END);

        pVfoB->setValue(FreqNum + pGraph->pGl->GetPitch());
        OnVfoBChanged(pVfoB->value());
    }
}

void ExpertSDR_vA2_1::OnAfromB()
{
    SetFreq(pVfoB->value() - pGraph->pGl->GetPitch());
    MainFreqChange(pVfoB->value());
    pGraph->pGl->SetDDSFreq(FreqNum - pGraph->pGl->GetFilter());
    OnVfoBChanged(pVfoB->value());
}

void ExpertSDR_vA2_1::OnAswapB()
{
    qlonglong Val = FreqNum + pGraph->pGl->GetPitch();
    SetFreq(pVfoB->value() - pGraph->pGl->GetPitch());
    MainFreqChange(pVfoB->value());
    pGraph->pGl->SetDDSFreq(FreqNum - pGraph->pGl->GetFilter());
    pVfoB->setValue(Val);
    OnVfoBChanged(Val);
}

void ExpertSDR_vA2_1::OnRx2(bool stat)
{
    pGraph->pGl->Filter2On(stat);
    pDsp->SetRx2On(stat);
    pDsp->SetRx2Volume(ui->slRx2Vol->value());
}

void ExpertSDR_vA2_1::OnFilter2Hide(bool stat)
{
    ui->pbRx2->setChecked(stat);
    OnRx2(stat);
}

void ExpertSDR_vA2_1::OnChangeFilter2Freq(int Freq)
{
    pDsp->SetRx2Osc(-Freq + pGraph->pGl->GetPitch());
    disconnect(pVfoB, SIGNAL(valueChanged(int)), this, SLOT(OnVfoBChanged(int)));
    pVfoB->setValue(Freq + pGraph->pGl->GetDDSFreq());
    connect(pVfoB, SIGNAL(valueChanged(int)), this, SLOT(OnVfoBChanged(int)));
}

void ExpertSDR_vA2_1::OnVfoBChanged(int Freq)
{
    pGraph->pGl->SetFilter2(-pGraph->pGl->GetDDSFreq() + Freq);
}

void ExpertSDR_vA2_1::OnSwitchVfo(int VfoNum)
{
    pGraph->pGl->SetTxVfo(VfoNum);
    pGraph->pGl->SetFilter2(-pGraph->pGl->GetDDSFreq() + pVfoB->value());
    if(VfoNum > 0)
        ui->pbSplit->setChecked(true);
    else ui->pbSplit->setChecked(false);
}

void ExpertSDR_vA2_1::OnBal1()
{
    ui->slRx1Bal->setValue(50);
}

void ExpertSDR_vA2_1::OnBal2()
{
    ui->slRx2Bal->setValue(50);
}

void ExpertSDR_vA2_1::freqDbm()
{
    int f;
    float d;
    pGraph->pGl->GetPeak(&f, &d);
    emit emitFreqDbm(f, d);
}

void ExpertSDR_vA2_1::setCalibrFreqCoeff(float val)
{
    pOpt->ui.sbColibrFreqNum->setValue(val);
    pSdrCtrl->SetDdsCalibrate(val);
}

void ExpertSDR_vA2_1::getSmeter()
{
    float val = pDsp->CalculateMeters(SIGNAL_STRENGTH);
    emit emitDbm(val);
}

void ExpertSDR_vA2_1::setCalibrDbmCoeff(int num, float val)
{
    CalibrateLevel[num] = val;
}

void ExpertSDR_vA2_1::setLockIsRec(bool state)
{
    OnLock(state);
    pGraph->pGl->SetLockFilter(false);
    ui->pbLock->setEnabled(!state);
}

void ExpertSDR_vA2_1::setLockIsPlay(bool state)
{
    OnLock(state);
    ui->pbMox->setDisabled(state);
    ui->pbTone->setDisabled(state);
    ui->pbLock->setDisabled(state);
    pSM->SetLockFreq(state);
    if(state)
    {
        saveDdsFreq = pGraph->pGl->GetDDSFreq();
        saveSampleRate = pOpt->getSampleRate();
        MainFreqChange(pMem->getCentralFreq() + pGraph->pGl->GetFilter());
        SetFreq(pMem->getCentralFreq() + pGraph->pGl->GetFilter());
        pGraph->pGl->SetDDSFreq(pMem->getCentralFreq());
        OnChangeMainFreq(pMem->getCentralFreq());
        if(pMem->sampleRateIndex() < 0)
        {
            qWarning() << "ExpertSDR: setLockIsPlay(): Incorrect sample rate!";
            OnStart(false);
            setLock(LOCK_DIDABLE);
            connect(pMem, SIGNAL(FreqMem(int, int)), this, SLOT(OnChangeMainFreqWithWdgMem(int, int)));
            return;
        }
        int cb_idx = pMem->sampleRateIndex();
        pOpt->ui.cbPaSampleRate->setCurrentIndex(cb_idx);
        disconnect(pMem, SIGNAL(FreqMem(int, int)), this, SLOT(OnChangeMainFreqWithWdgMem(int, int)));
        if(UpdatesTimerID == 0)
            OnStart(true);
        if(ui->pbStart->isChecked())
            setLock(LOCK_DDS | LOCK_BAND | LOCK_MOX | LOCK_TONE | LOCK_LOCK);
    }
    else
    {
        setLock(LOCK_DIDABLE);
        MainFreqChange(saveDdsFreq + pGraph->pGl->GetFilter());
        SetFreq(saveDdsFreq + pGraph->pGl->GetFilter());
        pGraph->pGl->SetDDSFreq(saveDdsFreq);
        OnChangeMainFreq(saveDdsFreq);
        pOpt->setSampleRate((uint)saveSampleRate);
        pGraph->pGl->SetLockGrid(false);
        connect(pMem, SIGNAL(FreqMem(int, int)), this, SLOT(OnChangeMainFreqWithWdgMem(int, int)));
    }
    SetLockMode(true);
    pGraph->pGl->SetLockFilter(false);
    pGraph->pGl->SetLockBandFilter(false);
}

void ExpertSDR_vA2_1::setIsPause(bool state)
{
    if(state)
        pGraph->pGl->Stop();
    else
        pGraph->pGl->Start();
}

void ExpertSDR_vA2_1::changedPlayFile(bool state)
{
    if(pMem->sampleRateIndex() < 0)
    {
        qWarning() << "ExpertSDR: changedPlayFile(): Incorrect sample rate!";
        OnStart(false);
        return;
    }
    pOpt->ui.cbPaSampleRate->setCurrentIndex(pMem->sampleRateIndex());
    if(UpdatesTimerID != 0)
    {
        pGraph->pGl->SetDDSFreq(pMem->getCentralFreq());
        OnChangeMainFreq(pMem->getCentralFreq());
    }
    if(ui->pbStart->isChecked())
    {
        ui->pbMox->setDisabled(state);
        ui->pbTone->setDisabled(state);
        ui->pbLock->setDisabled(state);
    }
}

void ExpertSDR_vA2_1::setSunSDRState(QString ms)
{
    setWindowTitle("ExpertSDR vA2.4.1.21   www.sunsdr.com         " + ms);
}

void ExpertSDR_vA2_1::setWavePath(QString path)
{
    pMem->pIqList->setWaveDir(path);
}

void ExpertSDR_vA2_1::onPlayVoice(bool state)
{
    if(!pVoiceRec->onPlay(state))
    {
        if(state)
        {
            state = false;
            ui->pbVoicePlay->setChecked(false);
            connect(pSM, SIGNAL(FreqChange(int)), this, SLOT(OnChangeStationFreq(int)));
        }
    }
    ui->pbVoicePlay->setChecked(state);
    ui->pbVoiceRec->setEnabled(!ui->pbVoicePlay->isChecked());
}

void ExpertSDR_vA2_1::onRecVoice(bool state)
{
    if(!pVoiceRec->onRec(state))
    {
        if(state)
        {
            state = false;
            ui->pbVoiceRec->setChecked(false);
        }
    }
    ui->pbVoiceRec->setChecked(state);
    ui->pbVoicePlay->setEnabled(!ui->pbVoiceRec->isChecked());
}

void ExpertSDR_vA2_1::resetRitValue()
{
    ui->sbRit->setValue(0);
}

void ExpertSDR_vA2_1::OnChangeBandFilter(int value)
{
    disconnect(pGraph->pGl, SIGNAL(ChangeFilterLowFreq(int)), this, SLOT(OnChangeLowFilterFreq(int)));
    disconnect(pGraph->pGl, SIGNAL(ChangeFilterHighFreq(int)), this, SLOT(OnChangeHighFilterFreq(int)));
    pGraph->pGl->SetBandFilter(ui->sbVarLowFreq->value(), ui->sbVarHighFreq->value());
    connect(pGraph->pGl, SIGNAL(ChangeFilterLowFreq(int)), this, SLOT(OnChangeLowFilterFreq(int)));
    connect(pGraph->pGl, SIGNAL(ChangeFilterHighFreq(int)), this, SLOT(OnChangeHighFilterFreq(int)));
}

void ExpertSDR_vA2_1::setUpdateTime(int ms)
{
    killTimer(UpdatesTimerID);
    UpdatesTimerID = startTimer(ms);
}

void ExpertSDR_vA2_1::calcVectorRamp(int samplerate, int silenceTime, int rampTime)
{
    int size1 = qFloor((float)silenceTime/(1.0f/(samplerate/1000.0f)));
    int size2 = qFloor((float)rampTime/(1.0f/(samplerate/1000.0f)));
    vecRamp.clear();
    vecRamp.resize(size1 + size2);
    memset(vecRamp.data(), 0.0f, sizeof(float)*(size1 + size2));
    for(int i = 0; i < size2; i++)
        vecRamp.data()[i+size1] = 1.0 - qExp((float)i / (size2/(-5.0)));
}

void ExpertSDR_vA2_1::calcRampDelay(int value)
{
    IsRampRelay = pOpt->ui.chbPaTxDelay->isChecked();
    int sizemin = qFloor((float)(280/(pOpt->getSampleRate()/48000))/(1.0f/(pOpt->getSampleRate()/1000.0f)));
    int size1 = qFloor((float)pOpt->ui.sbPaTxDelayValue->value()/(1.0f/(pOpt->getSampleRate()/1000.0f)));
    int size2 = qFloor(50.0f/(1.0f/(pOpt->getSampleRate()/1000.0f)));

    if(size1 < sizemin)
        size1 = sizemin;
    vecRampDelay.clear();
    vecRampDelay.resize(size1 + size2);
    memset(vecRampDelay.data(), 0.0f, sizeof(float)*(size1 + size2));
    for(int i = 0; i < size2; i++)
        vecRampDelay.data()[i+size1] = 1.0f - qExp((float)i / (size2/(-5.0f)));
}

void ExpertSDR_vA2_1::setRampDelayEnable(bool state)
{
    if(state)
    {
        saveStateBrakeIn = ui->chbBreakIn->isChecked();
        ui->chbBreakIn->setChecked(false);
        ui->chbBreakIn->setEnabled(false);
    }
    else
    {
        ui->chbBreakIn->setEnabled(true);
        ui->chbBreakIn->setChecked(saveStateBrakeIn);
    }
}

void ExpertSDR_vA2_1::setRxEnable(bool state)
{
    pDsp->SetRitEnable(state);
    pDsp->SetRx1Osc((float)(-pGraph->pGl->GetFilter()));
}

void ExpertSDR_vA2_1::OnNewTune(long freq)
{
    int filter_freq = freq - pGraph->pGl->GetDDSFreq();
    pGraph->pGl->SetFilter(filter_freq);
    SetFreq(freq);
    MainFreqChange(freq);
}

void ExpertSDR_vA2_1::OnNewDDS(long freq)
{
    pGraph->pGl->SetDDSFreq(freq);
    int tune = freq + pGraph->pGl->GetFilter();
    SetFreq(tune);
    MainFreqChange(tune);
}

void ExpertSDR_vA2_1::setLock(int flags)
{
    flagLock = flags;
    if(flags & LOCK_DDS)
    {
        ui->pbDg9->setEnabled(false);
        ui->pbDg8->setEnabled(false);
        ui->pbDg7->setEnabled(false);
        ui->pbDg6->setEnabled(false);
        ui->pbDg5->setEnabled(false);
        ui->pbDg4->setEnabled(false);
        ui->pbDg3->setEnabled(false);
        ui->pbDg2->setEnabled(false);
        ui->pbDg1->setEnabled(false);
        ui->pbDg0->setEnabled(false);
        pSM->setLockBand(true);
        pSM->SetLockFreq(true);
        pGraph->pGl->SetLockGrid(true);
        ui->pbFdown->setEnabled(false);
        ui->pbFlist->setEnabled(false);
        ui->pbFup->setEnabled(false);
//        disconnect(pPanel, SIGNAL(mainFreq(int)), this, SLOT(ChangeValcoder(int)));
//        disconnect(pPanel, SIGNAL(memory(int)), this, SLOT(OnPanChangeMemory(int)));
    }
    else
    {
        ui->pbDg9->setEnabled(true);
        ui->pbDg8->setEnabled(true);
        ui->pbDg7->setEnabled(true);
        ui->pbDg6->setEnabled(true);
        ui->pbDg5->setEnabled(true);
        ui->pbDg4->setEnabled(true);
        ui->pbDg3->setEnabled(true);
        ui->pbDg2->setEnabled(true);
        ui->pbDg1->setEnabled(true);
        ui->pbDg0->setEnabled(true);
        pSM->setLockBand(false);
        pSM->SetLockFreq(false);
        pGraph->pGl->SetLockGrid(false);
        ui->pbFdown->setEnabled(true);
        ui->pbFlist->setEnabled(true);
        ui->pbFup->setEnabled(true);
//        connect(pPanel, SIGNAL(mainFreq(int)), this, SLOT(ChangeValcoder(int)));
//        connect(pPanel, SIGNAL(memory(int)), this, SLOT(OnPanChangeMemory(int)));
    }
    if(flags & LOCK_FILTER)
    {
        pGraph->pGl->SetLockFilter(true);
//        disconnect(pPanel, SIGNAL(filter(int)), this, SLOT(OnPanChangeFilter(int)));
//        disconnect(pPanel, SIGNAL(ifVal(int)), this, SLOT(OnPanIF(int)));
    }
    else
    {
        pGraph->pGl->SetLockFilter(false);
//        connect(pPanel, SIGNAL(filter(int)), this, SLOT(OnPanChangeFilter(int)));
//        connect(pPanel, SIGNAL(ifVal(int)), this, SLOT(OnPanIF(int)));
    }
    if(flags & LOCK_FILTER_BAND)
    {
        pGraph->pGl->SetLockBandFilter(true);
        ui->sbVarHighFreq->setEnabled(false);
        ui->sbVarLowFreq->setEnabled(false);
        ui->pbF0->setEnabled(false);
        ui->pbF1->setEnabled(false);
        ui->pbF2->setEnabled(false);
        ui->pbF3->setEnabled(false);
        ui->pbF4->setEnabled(false);
        ui->pbF5->setEnabled(false);
        ui->pbF6->setEnabled(false);
        ui->pbF7->setEnabled(false);
        ui->pbF8->setEnabled(false);
//        disconnect(pPanel, SIGNAL(filterHigh(int)), this, SLOT(OnPanFilterHigh(int)));
//        disconnect(pPanel, SIGNAL(filterLow(int)), this, SLOT(OnPanFilterLow(int)));
//        disconnect(pPanel, SIGNAL(filterWidth(int)), this, SLOT(OnPanFilterWidth(int)));
//        disconnect(pPanel, SIGNAL(filterShift(int)), this, SLOT(OnPanFilterShift(int)));
    }
    else
    {
        pGraph->pGl->SetLockBandFilter(false);
        ui->sbVarHighFreq->setEnabled(true);
        ui->sbVarLowFreq->setEnabled(true);
        ui->pbF0->setEnabled(true);
        ui->pbF1->setEnabled(true);
        ui->pbF2->setEnabled(true);
        ui->pbF3->setEnabled(true);
        ui->pbF4->setEnabled(true);
        ui->pbF5->setEnabled(true);
        ui->pbF6->setEnabled(true);
        ui->pbF7->setEnabled(true);
        ui->pbF8->setEnabled(true);
//        connect(pPanel, SIGNAL(filterHigh(int)), this, SLOT(OnPanFilterHigh(int)));
 //       connect(pPanel, SIGNAL(filterLow(int)), this, SLOT(OnPanFilterLow(int)));
//        connect(pPanel, SIGNAL(filterWidth(int)), this, SLOT(OnPanFilterWidth(int)));
//        connect(pPanel, SIGNAL(filterShift(int)), this, SLOT(OnPanFilterShift(int)));
    }
    if(flags & LOCK_BAND)
    {
        ui->widget_6->setEnabled(false);
        pSM->setLockBand(true);
        pSM->SetLockFreq(true);
//        disconnect(pPanel, SIGNAL(band(int)), this, SLOT(OnPanChangeBand(int)));
 //       disconnect(pPanel, SIGNAL(memory(int)), this, SLOT(OnPanChangeMemory(int)));
    }
    else
    {
        ui->widget_6->setEnabled(true);
        pSM->setLockBand(false);
        pSM->SetLockFreq(false);
//        connect(pPanel, SIGNAL(band(int)), this, SLOT(OnPanChangeBand(int)));
//        connect(pPanel, SIGNAL(memory(int)), this, SLOT(OnPanChangeMemory(int)));
    }
    if(flags & LOCK_MODE)
    {
        ui->widget_5->setEnabled(false);
        pSM->setLockMode(true);
//        disconnect(pPanel, SIGNAL(mode(int)), this, SLOT(OnPanChangeMode(int)));
    }
    else
    {
        ui->widget_5->setEnabled(true);
        pSM->setLockMode(false);
//        connect(pPanel, SIGNAL(mode(int)), this, SLOT(OnPanChangeMode(int)));
    }
    if(flags & LOCK_MOX)
        ui->pbMox->setEnabled(false);
    else if(ui->pbStart->isChecked())
        ui->pbMox->setEnabled(true);
    if(flags & LOCK_TONE)
        ui->pbTone->setEnabled(false);
    else if(ui->pbStart->isChecked())
        ui->pbTone->setEnabled(true);
    if(flags & LOCK_LOCK)
        ui->pbLock->setEnabled(false);
    else
        ui->pbLock->setEnabled(true);
    if(flags & LOCK_RIT)
    {
        ui->pbRitOn->setEnabled(false);
        ui->pbRitReset->setEnabled(false);
        ui->sbRit->setEnabled(false);
    }
    else
    {
        ui->pbRitOn->setEnabled(true);
        ui->pbRitReset->setEnabled(true);
        ui->sbRit->setEnabled(true);
    }
    if(flags & LOCK_SPLIT)
    {
        ui->pbSplit->setEnabled(false);
        ui->pbAtoB->setEnabled(false);
        ui->pbAfromB->setEnabled(false);
        ui->pbAswapB->setEnabled(false);
        ui->wdgVfoB->setEnabled(false);
        ui->pbVfoB_Up_Band->setEnabled(false);
        ui->pbVfoB_Down_Band->setEnabled(false);
    }
    else
    {
        ui->pbSplit->setEnabled(true);
        ui->pbAtoB->setEnabled(true);
        ui->pbAfromB->setEnabled(true);
        ui->pbAswapB->setEnabled(true);
        ui->wdgVfoB->setEnabled(true);
        ui->pbVfoB_Up_Band->setEnabled(true);
        ui->pbVfoB_Down_Band->setEnabled(true);
    }
    if(flags & LOCK_PREAMP)
    {
        ui->pbPreamp->setEnabled(false);
 //       disconnect(pPanel, SIGNAL(preamp(int)), this, SLOT(OnPanChangePreamp(int)));
    }
    else
    {
        ui->pbPreamp->setEnabled(true);
//        connect(pPanel, SIGNAL(preamp(int)), this, SLOT(OnPanChangePreamp(int)));
    }
    if(flags & LOCK_VOICE)
    {
        ui->pbVoicePlay->setEnabled(false);
        ui->pbVoiceRec->setEnabled(false);
    }
    else  if(ui->pbStart->isChecked())
    {
        ui->pbVoicePlay->setEnabled(true);
        ui->pbVoiceRec->setEnabled(true);
    }
}

void ExpertSDR_vA2_1::changedVfoBUp()
{
    vfoBVal[numBandVfoB] = pVfoB->value();
    if(++numBandVfoB > 2)
        numBandVfoB = 2;
    if(numBandVfoB == 1)
    {
        pVfoB->setMinValue(BPF2_START);
        pVfoB->setMaxValue(BPF2_END);
    }
    else if(numBandVfoB == 2)
    {
        pVfoB->setMinValue(BPF07_START);
        pVfoB->setMaxValue(BPF07_END);
    }
    else
    {
        pVfoB->setMinValue(0);
        pVfoB->setMaxValue(65000000);
    }
    pVfoB->setValue(vfoBVal[numBandVfoB]);
}

void ExpertSDR_vA2_1::changedVfoBDown()
{
    vfoBVal[numBandVfoB] = pVfoB->value();
    if(--numBandVfoB < 0)
        numBandVfoB = 0;
    if(numBandVfoB == 1)
    {
        pVfoB->setMinValue(BPF2_START);
        pVfoB->setMaxValue(BPF2_END);
    }
    else if(numBandVfoB == 2)
    {
        pVfoB->setMinValue(BPF07_START);
        pVfoB->setMaxValue(BPF07_END);
    }
    else
    {
        pVfoB->setMinValue(0);
        pVfoB->setMaxValue(65000000);
    }
    pVfoB->setValue(vfoBVal[numBandVfoB]);
}



