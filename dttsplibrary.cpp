#include "dttsplibrary.h"
#include <QDebug>

DttSPLibrary::DttSPLibrary()
{
    is_loaded = false;
    memset(&routs, 0, sizeof(DttSPRouts));
}

DttSPLibrary::~DttSPLibrary()
{
    if(is_loaded)
    {
        pLibrary->unload();
        delete pLibrary;
    }
}

bool DttSPLibrary::load()
{
     pLibrary = new QLibrary("DttSP");

    pLibrary->load();

    is_loaded = pLibrary->isLoaded();

    if(!is_loaded)
    {
        qDebug() << "Cannot load library.";
        delete pLibrary;
        return false;
    }

    // resolve funcs
    routs.Setup_SDR = (DllFunc0)pLibrary->resolve("Setup_SDR");
    routs.Destroy_SDR = (DllFunc0)pLibrary->resolve("Destroy_SDR");
    routs.SetMode = (DllFunc1)pLibrary->resolve("SetMode");
    routs.AudioReset = (DllFunc0)pLibrary->resolve("AudioReset");
    routs.SetDCBlock = (DllFunc2)pLibrary->resolve("SetDCBlock");
    routs.SetFilter = (DllFunc3)pLibrary->resolve("SetFilter");
    routs.Release_Update = (DllFunc0)pLibrary->resolve("Release_Update");
    routs.SetRXOutputGain = (DllFunc5)pLibrary->resolve("SetRXOutputGain");
    routs.SetOsc = (DllFunc5)pLibrary->resolve("SetOsc");
    routs.SetTXOsc = (DllFunc5)pLibrary->resolve("SetTXOsc");
    routs.SetSampleRate = (DllFunc5)pLibrary->resolve("SetSampleRate");
    routs.SetNR = (DllFunc2)pLibrary->resolve("SetNR");
    routs.SetBlkNR = (DllFunc2)pLibrary->resolve("SetBlkNR");
    routs.SetNRvals = (DllFunc6)pLibrary->resolve("SetNRvals");
    routs.SetTXCompandSt = (DllFunc7)pLibrary->resolve("SetTXCompandSt");
    routs.SetTXCompand = (DllFunc4)pLibrary->resolve("SetTXCompand");
    routs.SetTXSquelchSt = (DllFunc2)pLibrary->resolve("SetTXSquelchSt");
    routs.SetTXSquelchVal = (DllFunc8)pLibrary->resolve("SetTXSquelchVal");
    routs.SetANF = (DllFunc2)pLibrary->resolve("SetANF");
    routs.SetBlkANF = (DllFunc2)pLibrary->resolve("SetBlkANF");
    routs.SetANFvals = (DllFunc6)pLibrary->resolve("SetANFvals");
    routs.SetNB = (DllFunc2)pLibrary->resolve("SetNB");
    routs.SetNBvals = (DllFunc4)pLibrary->resolve("SetNBvals");
    routs.SetSDROM = (DllFunc2)pLibrary->resolve("SetSDROM");
    routs.SetSDROMvals = (DllFunc4)pLibrary->resolve("SetSDROMvals");
    routs.SetBIN = (DllFunc2)pLibrary->resolve("SetBIN");
    routs.SetRXAGC = (DllFunc19)pLibrary->resolve("SetRXAGC");
    routs.SetRXAGCAttack = (DllFunc7)pLibrary->resolve("SetRXAGCAttack");
    routs.SetRXAGCDecay = (DllFunc7)pLibrary->resolve("SetRXAGCDecay");
    routs.SetRXAGCHang = (DllFunc7)pLibrary->resolve("SetRXAGCHang");
    routs.SetRXAGCSlope = (DllFunc7)pLibrary->resolve("SetRXAGCSlope");
    routs.SetRXAGCHangThreshold = (DllFunc7)pLibrary->resolve("SetRXAGCHangThreshold");
    routs.SetTXALCAttack = (DllFunc7)pLibrary->resolve("SetTXALCAttack");
    routs.SetTXCarrierLevel = (DllFunc5)pLibrary->resolve("SetTXCarrierLevel");
    routs.SetTXALCDecay = (DllFunc7)pLibrary->resolve("SetTXALCDecay");
    routs.SetTXALCBot = (DllFunc5)pLibrary->resolve("SetTXALCBot");
    routs.SetTXALCHang = (DllFunc7)pLibrary->resolve("SetTXALCHang");
    routs.SetTXLevelerSt = (DllFunc2)pLibrary->resolve("SetTXLevelerSt");
    routs.SetTXLevelerAttack = (DllFunc7)pLibrary->resolve("SetTXLevelerAttack");
    routs.SetTXLevelerDecay = (DllFunc7)pLibrary->resolve("SetTXLevelerDecay");
    routs.SetTXLevelerTop = (DllFunc4)pLibrary->resolve("SetTXLevelerTop");
    routs.SetTXLevelerHang = (DllFunc7)pLibrary->resolve("SetTXLevelerHang");
    routs.SetFixedAGC = (DllFunc4)pLibrary->resolve("SetFixedAGC");
    routs.SetRXAGCTop = (DllFunc4)pLibrary->resolve("SetRXAGCTop");
    routs.SetCorrectIQ = (DllFunc9)pLibrary->resolve("SetCorrectIQ");
    routs.SetCorrectIQPhase = (DllFunc4)pLibrary->resolve("SetCorrectIQPhase");
    routs.SetCorrectIQGain = (DllFunc4)pLibrary->resolve("SetCorrectIQGain");
    routs.SetCorrectTXIQ = (DllFunc9)pLibrary->resolve("SetCorrectTXIQ");
    routs.SetCorrectTXIQPhase = (DllFunc4)pLibrary->resolve("SetCorrectTXIQPhase");
    routs.SetCorrectTXIQGain = (DllFunc4)pLibrary->resolve("SetCorrectTXIQGain");
    routs.SetPWSmode = (DllFunc7)pLibrary->resolve("SetPWSmode");
    routs.SetWindow = (DllFunc20)pLibrary->resolve("SetWindow");
    routs.SetSpectrumPolyphase = (DllFunc2)pLibrary->resolve("SetSpectrumPolyphase");
    routs.SetTXEQ = (DllFunc10)pLibrary->resolve("SetTXEQ");
    routs.SetGrphTXEQ = (DllFunc10)pLibrary->resolve("SetGrphTXEQ");
    routs.SetGrphTXEQcmd = (DllFunc2)pLibrary->resolve("SetGrphTXEQcmd");
    routs.SetNotch160 = (DllFunc2)pLibrary->resolve("SetNotch160");
    routs.SetGrphRXEQ = (DllFunc10)pLibrary->resolve("SetGrphRXEQ");
    routs.SetGrphRXEQcmd = (DllFunc2)pLibrary->resolve("SetGrphRXEQcmd");
    routs.SetTXAGCFF = (DllFunc2)pLibrary->resolve("SetTXAGCFF");
    routs.SetTXAGCFFCompression = (DllFunc4)pLibrary->resolve("SetTXAGCFFCompression");
    routs.SetSquelchVal = (DllFunc8)pLibrary->resolve("SetSquelchVal");
    routs.SetSquelchState = (DllFunc2)pLibrary->resolve("SetSquelchState");
    routs.SetTRX = (DllFunc11)pLibrary->resolve("SetTRX");
    routs.SetDSPBuflen = (DllFunc7)pLibrary->resolve("SetDSPBuflen");
    routs.SetAudioSize = (DllFunc12)pLibrary->resolve("SetAudioSize");
    routs.SetTXAGCLimit = (DllFunc4)pLibrary->resolve("SetTXAGCLimit");
    routs.Process_Spectrum = (DllFunc13)pLibrary->resolve("Process_Spectrum");
    routs.Process_Panadapter = (DllFunc13)pLibrary->resolve("Process_Panadapter");
    routs.Process_Phase = (DllFunc14)pLibrary->resolve("Process_Phase");
    routs.Process_Scope = (DllFunc14)pLibrary->resolve("Process_Scope");
    routs.SetRingBufferOffset = (DllFunc10)pLibrary->resolve("SetRingBufferOffset");
    routs.Calculate_Meters = (DllFunc15)pLibrary->resolve("Calculate_Meters");
    routs.NewResampler = (DllFunc16)pLibrary->resolve("NewResampler");
    routs.DoResampler = (DllFunc17)pLibrary->resolve("DoResampler");
    routs.DelPolyPhaseFIR = (DllFunc18)pLibrary->resolve("DelPolyPhaseFIR");
    routs.NewResamplerF = (DllFunc16)pLibrary->resolve("NewResamplerF");
    routs.DoResamplerF = (DllFunc21)pLibrary->resolve("DoResamplerF");
    routs.DelPolyPhaseFIRF = (DllFunc18)pLibrary->resolve("DelPolyPhaseFIRF");
    routs.SetRXListen = (DllFunc7)pLibrary->resolve("SetRXListen");
    routs.SetRXOn = (DllFunc7)pLibrary->resolve("SetRXOn");
    routs.SetRXOff = (DllFunc7)pLibrary->resolve("SetRXOff");
    routs.SetRXPan = (DllFunc8)pLibrary->resolve("SetRXPan");
    routs.ProcessSamplesThread = (DllFunc0)pLibrary->resolve("process_samples_thread");
    routs.AudioCallback = (DllFunc22)pLibrary->resolve("Audio_Callback");
    routs.SetTXFilter = (DllFunc29)pLibrary->resolve("SetTXFilter");

    return true;
}

void DttSPLibrary::Setup_SDR()
{
    if(routs.Setup_SDR)
        routs.Setup_SDR();
}

void DttSPLibrary::Destroy_SDR()
{
    if(routs.Destroy_SDR)
        routs.Destroy_SDR();
}

int DttSPLibrary::SetMode(SDRMODE Mode)
{
    if(routs.SetMode)
        return routs.SetMode(Mode);
    return -1;
}

void DttSPLibrary::AudioReset()
{
    if(routs.AudioReset)
        routs.AudioReset();
}

void DttSPLibrary::SetDCBlock(bool Status)
{
    if(routs.SetDCBlock)
        routs.SetDCBlock(Status);
}

int DttSPLibrary::SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx)
{
    if(routs.SetFilter)
        return routs.SetFilter(low_frequency, high_frequency, taps, trx);
    return -1;
}

void DttSPLibrary::Release_Update()
{
    if(routs.Release_Update)
        routs.Release_Update();
}

int DttSPLibrary::SetRXOutputGain(double Gain)
{
    if(routs.SetRXOutputGain)
        routs.SetRXOutputGain(Gain);
    return -1;
}

int DttSPLibrary::SetOsc(double Freq)
{
    if(routs.SetOsc)
        return routs.SetOsc(Freq);
    return -1;
}

int DttSPLibrary::SetTXOsc(double Freq)
{
    if(routs.SetTXOsc)
        return routs.SetTXOsc(Freq);
    return -1;
}

int DttSPLibrary::SetSampleRate(double Rate)
{
    if(routs.SetSampleRate)
        return routs.SetSampleRate(Rate);
    return -1;
}

void DttSPLibrary::SetNR(bool Status)
{
    if(routs.SetNR)
        routs.SetNR(Status);
}

void DttSPLibrary::SetBlkNR(bool Status)
{
    if(routs.SetBlkNR)
        routs.SetBlkNR(Status);
}

void DttSPLibrary::SetNRvals(int taps, int delay, double gain, double leak)
{
    if(routs.SetNRvals)
        routs.SetNRvals(taps, delay, gain, leak);
}

void DttSPLibrary::SetTXCompandSt(int Param)
{
    if(routs.SetTXCompandSt)
        routs.SetTXCompandSt(Param);
}

void DttSPLibrary::SetTXCompand(double Param)
{
    if(routs.SetTXCompand)
        routs.SetTXCompand(Param);
}

void DttSPLibrary::SetTXSquelchSt(bool Status)
{
    if(routs.SetTXSquelchSt)
        routs.SetTXSquelchSt(Status);
}

void DttSPLibrary::SetTXSquelchVal(float Val)
{
    if(routs.SetTXSquelchVal)
        routs.SetTXSquelchVal(Val);
}

void DttSPLibrary::SetANF(bool Status)
{
    if(routs.SetANF)
        routs.SetANF(Status);
}

void DttSPLibrary::SetBlkANF(bool Status)
{
    if(routs.SetBlkANF)
        routs.SetBlkANF(Status);
}

void DttSPLibrary::SetANFvals(int taps, int delay, double gain, double leak)
{
    if(routs.SetANFvals)
        routs.SetANFvals(taps, delay, gain, leak);
}

void DttSPLibrary::SetNB(bool Status)
{
    if(routs.SetNB)
        routs.SetNB(Status);
}

void DttSPLibrary::SetNBvals(double Val)
{
    if(routs.SetNBvals)
        routs.SetNBvals(Val);
}

void DttSPLibrary::SetSDROM(bool Status)
{
    if(routs.SetSDROM)
        routs.SetSDROM(Status);
}

void DttSPLibrary::SetSDROMvals(double Val)
{
    if(routs.SetSDROMvals)
        routs.SetSDROMvals(Val);
}

void DttSPLibrary::SetBIN(bool Status)
{
    if(routs.SetBIN)
        routs.SetBIN(Status);
}

void DttSPLibrary::SetRXAGC(AGCMODE Mode)
{
    if(routs.SetRXAGC)
        routs.SetRXAGC(Mode);
}

void DttSPLibrary::SetRXAGCAttack(int Val)
{
    if(routs.SetRXAGCAttack)
        routs.SetRXAGCAttack(Val);
}

void DttSPLibrary::SetRXAGCDecay(int Val)
{
    if(routs.SetRXAGCDecay)
        routs.SetRXAGCDecay(Val);
}

void DttSPLibrary::SetRXAGCHang(int Val)
{
    if(routs.SetRXAGCHang)
        routs.SetRXAGCHang(Val);
}

void DttSPLibrary::SetRXAGCSlope(int Val)
{
    if(routs.SetRXAGCSlope)
        routs.SetRXAGCSlope(Val);
}

void DttSPLibrary::SetRXAGCHangThreshold(int Val)
{
    if(routs.SetRXAGCHangThreshold)
        routs.SetRXAGCHangThreshold(Val);
}

void DttSPLibrary::SetTXALCAttack(int Val)
{
    if(routs.SetTXALCAttack)
        routs.SetTXALCAttack(Val);
}

void DttSPLibrary::SetTXCarrierLevel(float Val)
{
    if(routs.SetTXCarrierLevel)
        routs.SetTXCarrierLevel(Val);
}

void DttSPLibrary::SetTXALCDecay(bool Status)
{
    if(routs.SetTXALCDecay)
        routs.SetTXALCDecay(Status);
}

void DttSPLibrary::SetTXALCBot(double Val)
{
    if(routs.SetTXALCBot)
        routs.SetTXALCBot(Val);
}

void DttSPLibrary::SetTXALCHang(int Val)
{
    if(routs.SetTXALCHang)
        routs.SetTXALCHang(Val);
}

void DttSPLibrary::SetTXLevelerSt(bool Status)
{
    if(routs.SetTXLevelerSt)
        routs.SetTXLevelerSt(Status);
}

void DttSPLibrary::SetTXLevelerAttack(int Val)
{
    if(routs.SetTXLevelerAttack)
        routs.SetTXLevelerAttack(Val);
}

void DttSPLibrary::SetTXLevelerDecay(int Val)
{
    if(routs.SetTXLevelerDecay)
        routs.SetTXLevelerDecay(Val);
}

void DttSPLibrary::SetTXLevelerTop(double Val)
{
    if(routs.SetTXLevelerTop)
        routs.SetTXLevelerTop(Val);
}

void DttSPLibrary::SetTXLevelerHang(int Val)
{
    if(routs.SetTXLevelerHang)
        routs.SetTXLevelerHang(Val);
}

void DttSPLibrary::SetFixedAGC(double Val)
{
    if(routs.SetFixedAGC)
        routs.SetFixedAGC(Val);
}

void DttSPLibrary::SetRXAGCTop(double Val)
{
    if(routs.SetRXAGCTop)
        routs.SetRXAGCTop(Val);
}

void DttSPLibrary::SetCorrectIQ(double phase, double gain)
{
    if(routs.SetCorrectIQ)
        routs.SetCorrectIQ(phase, gain);
}

void DttSPLibrary::SetCorrectIQPhase(double phase)
{
    if(routs.SetCorrectIQPhase)
        routs.SetCorrectIQPhase(phase);
}

void DttSPLibrary::SetCorrectIQGain(double gain)
{
    if(routs.SetCorrectIQGain)
        routs.SetCorrectIQGain(gain);
}

void DttSPLibrary::SetCorrectTXIQ(double Gain, double Phase)
{
    if(routs.SetCorrectTXIQ)
        routs.SetCorrectTXIQ(Gain, Phase);
}

void DttSPLibrary::SetCorrectTXIQPhase(double Phase)
{
    if(routs.SetCorrectTXIQPhase)
        routs.SetCorrectTXIQPhase(Phase);
}

void DttSPLibrary::SetCorrectTXIQGain(double Gain)
{
    if(routs.SetCorrectTXIQGain)
        routs.SetCorrectTXIQGain(Gain);
}

void DttSPLibrary::SetPWSmode(int Val)
{
    if(routs.SetPWSmode)
        routs.SetPWSmode(Val);
}

void DttSPLibrary::SetWindow(Windowtype Wtype)
{
    if(routs.SetWindow)
        routs.SetWindow(Wtype);
}

void DttSPLibrary::SetSpectrumPolyphase(bool Status)
{
    if(routs.SetSpectrumPolyphase)
        routs.SetSpectrumPolyphase(Status);
}

void DttSPLibrary::SetTXEQ(int *txeq)
{
    if(routs.SetTXEQ)
        routs.SetTXEQ(txeq);
}

void DttSPLibrary::SetGrphTXEQ(int *p)
{
    if(routs.SetGrphTXEQ)
        routs.SetGrphTXEQ(p);
}

void DttSPLibrary::SetGrphTXEQcmd(bool Status)
{
    if(routs.SetGrphTXEQcmd)
        routs.SetGrphTXEQcmd(Status);
}

void DttSPLibrary::SetNotch160(bool Status)
{
    if(routs.SetNotch160)
        routs.SetNotch160(Status);
}

void DttSPLibrary::SetGrphRXEQ(int *rxeq)
{
    if(routs.SetGrphRXEQ)
        routs.SetGrphRXEQ(rxeq);
}

void DttSPLibrary::SetGrphRXEQcmd(bool Status)
{
    if(routs.SetGrphRXEQcmd)
        routs.SetGrphRXEQcmd(Status);
}

void DttSPLibrary::SetTXAGCFF(bool Status)
{
    if(routs.SetTXAGCFF)
        routs.SetTXAGCFF(Status);
}

void DttSPLibrary::SetTXAGCFFCompression(double Val)
{
    if(routs.SetTXAGCFFCompression)
        routs.SetTXAGCFFCompression(Val);
}

void DttSPLibrary::SetSquelchVal(double Val)
{
    if(routs.SetSquelchVal)
        routs.SetSquelchVal(Val);
}

void DttSPLibrary::SetSquelchState(bool setit)
{
    if(routs.SetSquelchState)
        routs.SetSquelchState(setit);
}

void DttSPLibrary::SetTRX(TRXMODE Mode)
{
    if(routs.SetTRX)
        routs.SetTRX(Mode);
}

void DttSPLibrary::SetDSPBuflen(int BufLen)
{
    if(routs.SetDSPBuflen)
        routs.SetDSPBuflen(BufLen);
}

void DttSPLibrary::SetAudioSize(unsigned int Size)
{
    if(routs.SetAudioSize)
        routs.SetAudioSize(Size);
}

void DttSPLibrary::SetTXAGCLimit(double Val)
{
    if(routs.SetTXAGCLimit)
        routs.SetTXAGCLimit(Val);
}

void DttSPLibrary::Process_Spectrum(float *pBuf)
{
    if(routs.Process_Spectrum)
        routs.Process_Spectrum(pBuf);
}

void DttSPLibrary::Process_Panadapter(float *pBuf)
{
    if(routs.Process_Panadapter)
        routs.Process_Panadapter(pBuf);
}

void DttSPLibrary::Process_Phase(float *pBuf, int Param)
{
    if(routs.Process_Phase)
        routs.Process_Phase(pBuf, Param);
}

void DttSPLibrary::Process_Scope(float *pBuf, int Param)
{
    if(routs.Process_Scope)
        routs.Process_Scope(pBuf, Param);
}

void DttSPLibrary::SetRingBufferOffset(int *Param)
{
    if(routs.SetRingBufferOffset)
        routs.SetRingBufferOffset(Param);
}

float DttSPLibrary::Calculate_Meters(METERTYPE Meter)
{
    if(routs.Calculate_Meters)
        return routs.Calculate_Meters(Meter);
    return 0;
}

void* DttSPLibrary::NewResampler(int samplerate_in, int samplerate_out)
{
    if(routs.NewResampler)
        return routs.NewResampler(samplerate_in, samplerate_out);
    return NULL;
}

void DttSPLibrary::DoResampler(COMPLEX * input, COMPLEX * output, int numsamps, int *outsamps,
                               ResSt ptr)
{
    if(routs.DoResampler)
        routs.DoResampler(input, output, numsamps, outsamps, ptr);
}

void DttSPLibrary::DelPolyPhaseFIR(ResSt p)
{
    if(routs.DelPolyPhaseFIR)
        routs.DelPolyPhaseFIR(p);
}

void *DttSPLibrary::NewResamplerF(int samplerate_in, int samplerate_out)
{
    if(routs.NewResamplerF)
        return routs.NewResamplerF(samplerate_in, samplerate_out);
    return NULL;
}

void DttSPLibrary::DoResamplerF(float *input, float *output, int numsamps, int *outsamps,
                                ResStF ptr)
{
    if(routs.DoResamplerF)
        routs.DoResamplerF(input, output, numsamps, outsamps, ptr);
}

void DttSPLibrary::DelPolyPhaseFIRF(ResSt p)
{
    if(routs.DelPolyPhaseFIRF)
        routs.DelPolyPhaseFIRF(p);
}

void DttSPLibrary::SetRXListen(int Val)
{
    if(routs.SetRXListen)
        routs.SetRXListen(Val);
}

void DttSPLibrary::SetRXOn(int Val)
{
    if(routs.SetRXOn)
        routs.SetRXOn(Val);
}

void DttSPLibrary::SetRXOff(int Val)
{
    if(routs.SetRXOff)
        routs.SetRXOff(Val);
}

void DttSPLibrary::SetRXPan(float Val)
{
    if(routs.SetRXPan)
        routs.SetRXPan(Val);
}

void DttSPLibrary::ProcessSamplesThread()
{
    if(routs.ProcessSamplesThread)
        routs.ProcessSamplesThread();
}

void DttSPLibrary::AudioCallback(float* pInL, float* pInR, float* pOutL, float* pOutR, unsigned int FrameCount)
{
    if(routs.AudioCallback)
        routs.AudioCallback(pInL, pInR, pOutL, pOutR, FrameCount);
}

void DttSPLibrary::SetTXFilter(uint Thread, double Low, double High)
{
    if(routs.SetTXFilter)
        routs.SetTXFilter(Thread, Low, High);
}
