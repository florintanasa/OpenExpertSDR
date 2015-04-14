#ifndef DTTSPLIBRARY_H
#define DTTSPLIBRARY_H

#include <QLibrary>
#include "Defines.h"
#ifdef Q_OS_WIN // Windows
#include "qt_windows.h"
#endif /*Q_OS_WIN*/

typedef enum _swchstate {
  SWCH_FALL,
  SWCH_STDY,
  SWCH_RISE
} SWCHSTATE;

typedef enum _agcmode
{ agcOFF, agcLONG, agcSLOW, agcMED, agcFAST } AGCMODE;

typedef enum _windowtype
{
  RECTANGULAR_WINDOW,
  HANNING_WINDOW,
  WELCH_WINDOW,
  PARZEN_WINDOW,
  BARTLETT_WINDOW,
  HAMMING_WINDOW,
  BLACKMAN2_WINDOW,
  BLACKMAN3_WINDOW,
  BLACKMAN4_WINDOW,
  EXPONENTIAL_WINDOW,
  RIEMANN_WINDOW,
  BLACKMANHARRIS_WINDOW,
  NUTTALL_WINDOW,
} Windowtype;

typedef enum
{
  SIGNAL_STRENGTH,
  AVG_SIGNAL_STRENGTH,
  ADC_REAL,
  ADC_IMAG,
  AGC_GAIN,
  MIC,
  PWR,
  ALC,
  EQtap,
  LEVELER,
  COMP,
  CPDR,
  ALC_G,
  LVL_G
} METERTYPE;

typedef struct _COMPLEX
{
  float re, im;
} COMPLEX;

typedef enum
{
  FIR_Undef, FIR_Lowpass, FIR_Bandpass, FIR_Highpass, FIR_Hilbert,
  FIR_Bandstop
} FIR_response_type;

typedef struct _real_FIR
{
  float *coef;
  int size;
  FIR_response_type type;
  BOOLEAN cplx;
  struct
  {
    float lo, hi;
  } freq;
} RealFIRDesc, *RealFIR;

typedef struct resample_state
{
  COMPLEX *input, *output, *filterMemoryBuff;
  RealFIR filter;
  int filterMemoryBuffLength, inputArrayLength, numFiltTaps,
    indexfiltMemBuf, interpFactor, filterPhaseNum, deciFactor,
    numOutputSamples;
  int MASK;
} resampler, *ResSt;

typedef struct resampleF_state
{
  float *input, *output, *filterMemoryBuff;
  RealFIR filter;
  int filterMemoryBuffLength, inputArrayLength, numFiltTaps,
    indexfiltMemBuf, interpFactor, filterPhaseNum, deciFactor,
    numOutputSamples;
  int MASK;
} resamplerF, *ResStF;

typedef void (*DllFunc0)();
typedef int (*DllFunc1)(SDRMODE);
typedef void (*DllFunc2)(BOOLEAN);
typedef int (*DllFunc3)(double , double, int, TRXMODE);
typedef void (*DllFunc4)(double);
typedef int (*DllFunc5)(double);
typedef void (*DllFunc6)(int, int, double, double);
typedef void (*DllFunc7)(int);
typedef void (*DllFunc8)(float);
typedef void (*DllFunc9)(double, double);
typedef void (*DllFunc10)(int*);
typedef void (*DllFunc11)(TRXMODE);
typedef void (*DllFunc12)(unsigned int);
typedef void (*DllFunc13)(float*);
typedef void (*DllFunc14)(float*, int);
typedef float (*DllFunc15)(METERTYPE);
typedef void* (*DllFunc16)(int, int);
typedef void (*DllFunc17)(COMPLEX *, COMPLEX *, int, int *, ResSt);
typedef void (*DllFunc18)(ResSt);
typedef void (*DllFunc19)(AGCMODE);
typedef void (*DllFunc20)(Windowtype);
typedef void (*DllFunc21)(float *, float *, int, int*,ResStF);
typedef void (*DllFunc22)(float*, float*, float*, float*, unsigned int);
typedef void (*DllFunc23)(float, bool, bool, bool);
typedef void (*DllFunc24)(float, bool, float, float, float, float);
typedef void (*DllFunc25)(bool);
typedef bool (*DllFunc26)();
typedef void (*DllFunc27)(BYTE);
typedef void (*DllFunc28)(float*, float*, int);
typedef void (*DllFunc29)(uint, double, double);

typedef struct {
    DllFunc0 Setup_SDR;
    DllFunc0 Destroy_SDR;
    DllFunc1 SetMode;
    DllFunc0 AudioReset;
    DllFunc2 SetDCBlock;
    DllFunc3 SetFilter;
    DllFunc0 Release_Update;
    DllFunc5 SetRXOutputGain;
    DllFunc5 SetOsc;
    DllFunc5 SetTXOsc;
    DllFunc5 SetSampleRate;
    DllFunc2 SetNR;
    DllFunc2 SetBlkNR;
    DllFunc6 SetNRvals;
    DllFunc7 SetTXCompandSt;
    DllFunc4 SetTXCompand;
    DllFunc2 SetTXSquelchSt;
    DllFunc8 SetTXSquelchVal;
    DllFunc2 SetANF;
    DllFunc2 SetBlkANF;
    DllFunc6 SetANFvals;
    DllFunc2 SetNB;
    DllFunc4 SetNBvals;
    DllFunc2 SetSDROM;
    DllFunc4 SetSDROMvals;
    DllFunc2 SetBIN;
    DllFunc19 SetRXAGC;
    DllFunc7 SetRXAGCAttack;
    DllFunc7 SetRXAGCDecay;
    DllFunc7 SetRXAGCHang;
    DllFunc7 SetRXAGCSlope;
    DllFunc7 SetRXAGCHangThreshold;
    DllFunc7 SetTXALCAttack;
    DllFunc5 SetTXCarrierLevel;
    DllFunc7 SetTXALCDecay;
    DllFunc5 SetTXALCBot;
    DllFunc7 SetTXALCHang;
    DllFunc2 SetTXLevelerSt;
    DllFunc7 SetTXLevelerAttack;
    DllFunc7 SetTXLevelerDecay;
    DllFunc4 SetTXLevelerTop;
    DllFunc7 SetTXLevelerHang;
    DllFunc4 SetFixedAGC;
    DllFunc4 SetRXAGCTop;
    DllFunc9 SetCorrectIQ;
    DllFunc4 SetCorrectIQPhase;
    DllFunc4 SetCorrectIQGain;
    DllFunc9 SetCorrectTXIQ;
    DllFunc4 SetCorrectTXIQPhase;
    DllFunc4 SetCorrectTXIQGain;
    DllFunc7 SetPWSmode;
    DllFunc20 SetWindow;
    DllFunc2 SetSpectrumPolyphase;
    DllFunc10 SetTXEQ;
    DllFunc10 SetGrphTXEQ;
    DllFunc2 SetGrphTXEQcmd;
    DllFunc2 SetNotch160;
    DllFunc10 SetGrphRXEQ;
    DllFunc2 SetGrphRXEQcmd;
    DllFunc2 SetTXAGCFF;
    DllFunc4 SetTXAGCFFCompression;
    DllFunc8 SetSquelchVal;
    DllFunc2 SetSquelchState;
    DllFunc11 SetTRX;
    DllFunc7 SetDSPBuflen;
    DllFunc12 SetAudioSize;
    DllFunc4 SetTXAGCLimit;
    DllFunc13 Process_Spectrum;
    DllFunc13 Process_Panadapter;
    DllFunc14 Process_Phase;
    DllFunc14 Process_Scope;
    DllFunc10 SetRingBufferOffset;
    DllFunc15 Calculate_Meters;
    DllFunc16 NewResampler;
    DllFunc17 DoResampler;
    DllFunc18 DelPolyPhaseFIR;
    DllFunc16 NewResamplerF;
    DllFunc21 DoResamplerF;
    DllFunc18 DelPolyPhaseFIRF;
    DllFunc7 SetRXListen;
    DllFunc7 SetRXOn;
    DllFunc7 SetRXOff;
    DllFunc8 SetRXPan;
    DllFunc0 ProcessSamplesThread;
    DllFunc22 AudioCallback;
    DllFunc29 SetTXFilter;
} DttSPRouts;

class DttSPLibrary
{
public:
    DttSPLibrary();
    ~DttSPLibrary();

    bool load();

    void Setup_SDR();
    void Destroy_SDR();
    int SetMode(SDRMODE Mode);
    void AudioReset();
    void SetDCBlock(bool Status);
    int SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx);
    void Release_Update();
    int SetRXOutputGain(double Gain);
    int SetOsc(double Freq);
    int SetTXOsc(double Freq);
    int SetSampleRate(double Rate);
    void SetNR(bool Status);
    void SetBlkNR(bool Status);
    void SetNRvals(int taps, int delay, double gain, double leak);
    void SetTXCompandSt(int Param);
    void SetTXCompand(double Param);
    void SetTXSquelchSt(bool Status);
    void SetTXSquelchVal(float Val);
    void SetANF(bool Status);
    void SetBlkANF(bool Status);
    void SetANFvals(int taps, int delay, double gain, double leak);
    void SetNB(bool Status);
    void SetNBvals(double Val);
    void SetSDROM(bool Status);
    void SetSDROMvals(double Val);
    void SetBIN(bool Status);
    void SetRXAGC(AGCMODE Mode);
    void SetRXAGCAttack(int Val);
    void SetRXAGCDecay(int Val);
    void SetRXAGCHang(int Val);
    void SetRXAGCSlope(int Val);
    void SetRXAGCHangThreshold(int Val);
    void SetTXALCAttack(int Val);
    void SetTXCarrierLevel(float Val);
    void SetTXALCDecay(bool Status);
    void SetTXALCBot(double Val);
    void SetTXALCHang(int Val);
    void SetTXLevelerSt(bool Status);
    void SetTXLevelerAttack(int Val);
    void SetTXLevelerDecay(int Val);
    void SetTXLevelerTop(double Val);
    void SetTXLevelerHang(int Val);
    void SetFixedAGC(double Val);
    void SetRXAGCTop(double Val);
    void SetCorrectIQ(double phase, double gain);
    void SetCorrectIQPhase(double phase);
    void SetCorrectIQGain(double gain);
    void SetCorrectTXIQ(double Gain, double Phase);
    void SetCorrectTXIQPhase(double Phase);
    void SetCorrectTXIQGain(double Gain);
    void SetPWSmode(int Val);
    void SetWindow(Windowtype Wtype);
    void SetSpectrumPolyphase(bool Status);
    void SetTXEQ(int *txeq);
    void SetGrphTXEQ(int *p);
    void SetGrphTXEQcmd(bool Status);
    void SetNotch160(bool Status);
    void SetGrphRXEQ(int *rxeq);
    void SetGrphRXEQcmd(bool Status);
    void SetTXAGCFF(bool Status);
    void SetTXAGCFFCompression(double Val);
    void SetSquelchVal(double Val);
    void SetSquelchState(bool setit);
    void SetTRX(TRXMODE Mode);
    void SetDSPBuflen(int BufLen);
    void SetAudioSize(unsigned int Size);
    void SetTXAGCLimit(double Val);
    void Process_Spectrum(float *pBuf);
    void Process_Panadapter(float *pBuf);
    void Process_Phase(float *pBuf, int Param);
    void Process_Scope(float *pBuf, int Param);
    void SetRingBufferOffset(int *Param);
    float Calculate_Meters(METERTYPE Meter);
    void *NewResampler(int samplerate_in, int samplerate_out);
    void DoResampler(COMPLEX *input, COMPLEX *output, int numsamps, int *outsamps, ResSt ptr);
    void DelPolyPhaseFIR(ResSt p);
    void *NewResamplerF(int samplerate_in, int samplerate_out);
    void DoResamplerF(float *input, float *output, int numsamps, int *outsamps, ResStF ptr);
    void DelPolyPhaseFIRF(ResSt p);
    void SetRXListen(int Val);
    void SetRXOn(int Val);
    void SetRXOff(int Val);
    void SetRXPan(float Val);
    void ProcessSamplesThread();
    void AudioCallback(float* pInL, float* pInR, float* pOutL, float* pOutR, unsigned int FrameCount);
    void SetTXFilter(uint Thread, double Low, double High);

private:
    QLibrary *pLibrary;
    bool is_loaded;
    DttSPRouts routs;
};

#endif // DTTSPLIBRARY_H
