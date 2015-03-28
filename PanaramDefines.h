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

#ifndef PANARAMDEFINES_H_
#define PANARAMDEFINES_H_

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>


#define RIT_LIMIT 10000

#define STEP_GRID_SIZE 9
static const int StepGridHz[STEP_GRID_SIZE] = {1, 5, 10, 25, 50, 100, 250, 500, 1000};

#define STEP_FREQ_PIX_WIDTH 70
#define STEP_GRID_DDS_SIZE 21
static const int StepGridDDSHz[STEP_GRID_DDS_SIZE] = {1, 5, 10, 25, 50, 100, 250, 500, 1000, 2500, 5000, 10000, 15000, 25000, 50000, 100000, 250000, 500000, 1000000, 5000000, 10000000};
static const int StepDDSHz[19] = {1, 5, 10, 25, 50, 100, 250, 500, 1000, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 1000000, 5000000, 10000000};

#define BAND_SIZE 13
static const QString BandStr[BAND_SIZE] =   {"160M",  "80M",  "60M",  "40M",  "30M",   "20M",   "17M",   "15M",   "12M",   "10M",   "6M",     "2M",    "0.7M"};
static const GLint BandFreq[2][BAND_SIZE]= {{1800000,3500000,5100000,7000000,10100000,14000000,18068000,21000000,24890000,28000000,50000000,144000000,430000000},
										    {2000000,3800000,5500000,7300000,10150000,14350000,18318000,21450000,25140000,29700000,54000000,146000000,436000000}};

#define STEP_DB_SIZE 8
static const int StepDb[STEP_DB_SIZE] = {1, 2, 5, 10, 20, 25, 50, 100};


typedef enum
{
	NO_RANGE,
	RANGE160,
	RANGE80,
	RANGE60,
	RANGE40,
	RANGE30,
	RANGE20,
	RANGE17,
	RANGE15,
	RANGE12,
	RANGE10,
	RANGE6,
	RANGE2,
	RANGE07
}RANGE;

typedef enum
{
	NON_CROSS,
	CROSS,
	HORIZONTAL,
	VERTICAL
}CURSOR;

typedef enum
{
	RULE = 1,
	DB_PANEL,
	GRID,
	MAIN_FILTER,
	BAND_FILTER,
	LEFT_BAND,
	RIGHT_BAND,
	WATERFALL,
	PITCH,
	MAIN_FILTER2,
	LEFT_BAND2,
	RIGHT_BAND2,
	BAND_FILTER2,
	PITCH2,
	ZOOMER
}OBJECTS;

typedef enum
{
	SOLID,
	GRADIENT,
	IMAGE
}BACKGROUND;

typedef enum
{
	GRID_LINE_SOLID,
	GRID_LINE_STIPPLE
}GRID_TYPE;

typedef enum
{
	GRAD,
	LINES,
}SPECTR_MODE;

typedef enum
{
	WATERFALL_AUTO = 0,
	MULTIHAND,
	ALL_AUTO
}AUTO_LEVEL;

typedef enum
{
	kHz50,
	kHz20,
	kHz10,
	kHz5,
	kHz2,
	kHz1,
	Hz500,
	Hz200,
	Hz100

}GRID_SCALE_TYPE;

typedef enum
{
	UNPRESSED,
	PRESS_LEFT_RULE,
	PRESS_RIGHT_RULE,
	PRESS_LEFT_DBM,
	PRESS_RIGHT_DBM,
	PRESS_LEFT_GRID,
	PRESS_RIGHT_GRID,
	PRESS_LEFT_CF,
	PRESS_RIGHT_CF,
	PRESS_LEFT_BF,
	PRESS_RIGHT_BF,
	PRESS_LEFT_BF_LEFT,
	PRESS_LEFT_BF_RIGHT,
	PRESS_RIGHT_BF_RIGHT,
	PRESS_RIGHT_BF_LEFT,
	PRESS_LEFT_WATTERFALL,
	PRESS_RIGHT_WATERFALL,
	MOVE_RULE,
	MOVE_ZOOM,
	MOVE_GRID,
	MOVE_CF,
	MOVE_BF,
	MOVE_BF_LEFT,
	MOVE_BF_RIGHT,
	MOVE_WATTERFALL,
	PRESS_LEFT_CF2,
	PRESS_RIGHT_CF2,
	PRESS_LEFT_BF2,
	PRESS_RIGHT_BF2,
	PRESS_LEFT_BF_LEFT2,
	PRESS_LEFT_BF_RIGHT2,
	PRESS_RIGHT_BF_RIGHT2,
	PRESS_RIGHT_BF_LEFT2,
	ZOOMER_PRESS
}ACTION_OBJECT;

typedef struct
{
	float OffsetDbm;
	float LenDbm;
	int PanMode;
	int PanType;
	bool RigthClickChangeFilter;
	bool SwapLeftRigthButton;
	int StandartColorSetting;
	QColor LineColor;
	QColor SpectrumColor;
	QColor LineBackColor;
	QColor LineBackColor2;
	QColor GradTopColor;
	QColor GradBotColor;
	QColor GradBgrnColor;
	QColor GradBgrnColor2;
	int TrancpSpectrumLine;
	int TrancpSpectrum;
	int TrancpGradTop;
	int TrancpGradBot;
	int WfMode;
	QColor WfGradColor;
	int TrncpLineGrid;
	int BackgroundType;
	int ImgBackgroundType;
	int WfMaxOffset;
	int WfMinOffset;
	int SpMaxOffset;
	int SpMinOffset;
	int Hysteresis;
	int StatePowerChange;
	int AutoSettingType;
	bool AvaregingWhenChangingDDS;
	int TypeCursor;
	float RulePosY;
	bool TRxMode;
	QColor CustomColorGradTop;
	QColor CustomColorGradBot;
	QColor CustomColorGradBack;
	QColor CustomColorLineLines;
	QColor CustomColorLineSpectrum;
	QColor CustomColorLineBack;
	QColor CustomColorWFgrad;
	int CustomPanType;
	int CustomTrancpSpectrumLine;
	int CustomTrancpSpectrum;
	int CustomWfMode;
	int CustomBackgroundType;
	int CustomImgBackgroundType;
	int CustomTrancpGradTop;
	int CustomTrancpGradBot;
	int StyleType;
	bool GridOn;
	bool GridMoving;
	bool GridSlowDown;
	int LevelSlowdown;
	double Zoom;
	double ZoomPos;
	QColor ColorFilter1;
	QColor ColorFilter2;
	QColor ColorBand1;
	QColor ColorBand2;
	QColor ColorPitch1;
	QColor ColorPitch2;
	int TransparentFilter1;
	int TransparentFilter2;
}PAN_OPTION;

#endif /* PANARAMDEFINES_H_ */
