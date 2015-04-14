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
 * Copyright (C) Valery Mikhaylovsky
 * The authors can be reached by email at maksimus1210@gmail.com
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <QString>

#ifdef Q_OS_LINUX
typedef unsigned char  BYTE; // 1byte
typedef unsigned short  WORD; // 2bytes
typedef unsigned long  DWORD; //4bytes
typedef int BOOLEAN;
#endif

#define MAX_DSP_BUFFER_SIZE 4096

#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))

#define PI     3.141592653589793238462643383279
#define TWO_PI 6.283185307179586476925286766559

#define UPDATES_MS 20

#define LEVEL_SMETER 			33.3f

#define NUM_MODES 		12
#define NUM_FILTERS		9
#define NUM_BANDS		14

#define BPF160_F		2500000
#define BPF80_F			4000000
#define BPF60_F			6000000
#define BPF40_F			7300000
#define BPF30_F			12000000
#define BPF20_F			14500000
#define BPF17_F			19000000
#define BPF15_F			21500000
#define BPF12_F			25200000
#define BPF10_F			30000000
#define BPF6_F			65000000

#define BPF160_START	1800000
#define BPF160_END		2000000

#define BPF80_START		3500000
#define BPF80_END		3800000

#define BPF60_START		5100000
#define BPF60_END		5500000

#define BPF40_START		7000000
#define BPF40_END		7300000

#define BPF30_START		10100000
#define BPF30_END		10150000

#define BPF20_START		14000000
#define BPF20_END		14350000

#define BPF17_START		18068000
#define BPF17_END		18318000

#define BPF15_START		21000000
#define BPF15_END		21450000

#define BPF12_START		24890000
#define BPF12_END		25140000

#define BPF10_START		26970000
#define BPF10_END		29700000

#define BPF6_START		50000000
#define BPF6_END		54000000

#define BPF2_START		144000000
#define BPF2_END		146000000

#define BPF07_START		430000000
#define BPF07_END		440000000

typedef enum _trxmode
{
	RX,
	TX
} TRXMODE;

#define LOCK_DIDABLE 		0x00000000
#define LOCK_DDS	 		0x00000001
#define LOCK_FILTER 		0x00000002
#define LOCK_FILTER_BAND 	0x00000004
#define LOCK_BAND 			0x00000008
#define LOCK_MODE 			0x00000010
#define LOCK_MOX 			0x00000020
#define LOCK_TONE 			0x00000040
#define LOCK_LOCK 			0x00000080
#define LOCK_RIT			0x00000100
#define LOCK_SPLIT			0x00000200
#define LOCK_PREAMP			0x00000400
#define LOCK_VOICE			0x00000800
#define LOCK_ALL 			0xFFFFFFFF

typedef struct
{
	int FreqLow;
	int FreqHigh;
	int dFreqLow;
	int dFreqHigh;
	double FreqLowGL;
	double FreqHighGL;
	double dFreqLowGL;
	double dFreqHighGL;

}FILTER_BAND;

typedef struct
{
	int StepGridFreq;
	float sFreqGL;
	float dFreqGL;
	int sFreq;
	int dFreq;
	int cFreq;
	int BandWidth;
}FREQ, *pFREQ;

typedef enum
{
  LSB,
  USB,
  DSB,
  CWL,
  CWU,
  FMN,
  AM,
  DIGU,
  SPEC,
  DIGL,
  SAM,
  DRM
} SDRMODE;

typedef enum
{
  BAND160M,
  BAND80M,
  BAND60M,
  BAND40M,
  BAND30M,
  BAND20M,
  BAND17M,
  BAND15M,
  BAND12M,
  BAND10M,
  BAND6M,
  BAND2M,
  BAND07M,
  BANDGEN,
} BAND_MODE;

typedef enum
{
  FILTER0,
  FILTER1,
  FILTER2,
  FILTER3,
  FILTER4,
  FILTER5,
  FILTER6,
  FILTER7,
  FILTER8,
} FILTER;

typedef enum
{
  RADIO,
  SINE,
  SINE_TWO_TONE,
  NOISE,
  TRIANGLE,
  SAWTOOTH,
  SILENCE
} RX_INPUT_SIGNAL;

typedef struct ModeOptions
{
	int  cbPreampIndex;
	int  cbAgcIndex;
	bool BinEnable;
	bool NrEnable;
	bool AnfEnable;
	bool Nb1Enable;
	bool Nb2Enable;

	bool SqlEnable;
	int  SqlValue;
	bool GateEnable;
	int  GateValue;
	bool CompEnable;
	int  CompValue;
	bool CpdrEnable;
	int  CpdrValue;
	int FilterIndex;

	int VarFilterH;
	int VarFilterL;
}OPT_MODE, *pOPT_MODE;

typedef struct BandOptions
{
	int CurrentModeIndex;
	int MainFreq;
	int Pitch;
	int Volume;
	int RfGain;
	OPT_MODE	Mode[NUM_MODES];
}OPT_BAND, *pOPT_BAND;

typedef struct PLUG_OPT
{
	int SdrType;
	int cbPaDriverIndex;
	int cbPaInIndex;
	int cbPaOutIndex;
	int cbPaChannelsIndex;
	int cbPaSampleRate;
	int cbPaBufferSizeIndex;
	int sbPaLattency;
	int sbDdsMul;
}PLUGIN_OPTIONS, *pPLUGIN_OPTIONS;

typedef void StreamCallback(const void *Input, void *Output, unsigned long FrameCount, void *pUserData);

#endif /* DEFINES_H_ */


































