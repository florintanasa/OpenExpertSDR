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

#include "Panarama.h"
#include <QtGlobal>

#define GL_CLAMP_TO_EDGE 0x812F

int pancompare(const void * a, const void * b)
{
  return ( *(float*)a - *(float*)b);
}

static float MemTmp = 0;

Panarama::Panarama(QWidget *pOpt, QWidget *parent)
	: QGLWidget(QGLFormat::defaultFormat(), parent)
{
	setMouseTracking(true);
	setAutoBufferSwap(false);

	pPanOpt = new PanoramOpt(pOpt);
	pPanOpt->show();

	info_font = NULL;
	dbm_font = NULL;
	band_font = NULL;
	last_update = 0;
	waterfall_mode = 1;
	fps = 30;
	Vis = 0.01f;
	Cursor = CROSS;
	Mode = AM;
	TRxModeOld = RX;
	TRxMode = RX;
	isUpdate = false;
	TrxCnt = 1000;

	currentBand = 13;
	DbmTxState.offset = -120;
	DbmTxState.scale = 100;
	for(int i = 0; i < NUM_BANDS+1; i++)
	{
		DbmRxState[i].offset = -120;
		DbmRxState[i].scale  =  100;
	}

	IsXvrt = false;
	IsFilter2 = false;
	IsCrossF1 = false;
	CntBegin = 0;
	sPosZoomPan = 0.0;
	dPosZoomPan = 0.0;
	IsMoveGrid = true;
	IsDamping = true;
	StepMoveGrid = 0.0;
	StepDamping = 0.96;
	IsMoveUpdate = false;
	LockGrid = false;
	LockScreen = false;
	LockBand = false;
	IsChangedFilter = false;
	LockFilter = false;
	IsEnableSp = true;
	IsStart = false;
	MeanVal = 2;
	minDbm = 0;
	maxDbm = 100;
	CntUpdateBuff = 0;
	IsPeriodGetMax = false;
	CalibrateLevel = 0;
	CntMeaneBuff = 0;
	LineSpeedMax = 100;
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	HeighRule = RULE_HEIGH*2.0/height();
	IsWindow = false;
	CntLineSpeed = 0;
	IsChangedDbmLevel = false;
	NumLineTextures = 0;
	CntLinesTexture = 0;
	PresPosX = 0.0;
	PresPosY = 0.0;
	MovePosX = 0.0;
	MovePosY = 0.0;
	PresPosX2 = 0.0;
	PresPosY2 = 0.0;
	MovePosX2 = 0.0;
	MovePosY2 = 0.0;
	ScaleWindowX = 1.0;
	ScaleWindowY = 1.0;
	oy1 = ScaleWindowY;
	oy2_5 = 2.5*ScaleWindowY;
	oy5 = 5*ScaleWindowY;
	oy10 = 10*ScaleWindowY;
	oy20 = 20*ScaleWindowY;
	oy30 = 30*ScaleWindowY;
	ox1 = ScaleWindowX;
	ox2_5 = 2.5*ScaleWindowX;
	ox5 = 5*ScaleWindowX;
	ox10 = 10*ScaleWindowX;
	ox20 = 20*ScaleWindowX;
	ox30 = 30*ScaleWindowX;
	sRuleYPos = 0.0;
	dRuleYPos = 0.0;
	sScaleRuleX = 1.0;
	dScaleRuleX = 0.0;
	numStepGridDDS = 0;
	numStepGridFilter = 0;
	sFreqDDS = dFreqDDS = 0;
	sFreqFilter1 = dFreqFilter1 = 0;
	SampleRate = 48000;
	sDDSFreq = 0.0;
	dDDSFreq = 0.0;
	sFilter = 0.0;
	dFilter = 0.0;
	sRit = 0.0f;
	dRit = 0.0f;
	sFilter2 = 0.0;
	dFilter2 = 0.0;
	sPich = 100*2.0/SampleRate;
	dPich = 0;
	sRitValue = 0;
	dRitValue = 0;
	signalRitValue = 0;
	isOutOfRit = false;
	sBandPassLow = -5e3*2.0/SampleRate;
	dBandPassLow =  0.0;
	sBandPassHigh = 5e3*2.0/SampleRate;
	dBandPassHigh = 0.0;
	sBandPassLow2 = -5e3*2.0/SampleRate;
	dBandPassLow2 =  0.0;
	sBandPassHigh2 = 5e3*2.0/SampleRate;
	dBandPassHigh2 = 0.0;
	BandTxLow = -5000;
	BandTxHigh = 5000;
	ColorFilter2 = QColor(72, 176, 220, 255);
	BandLow = (sBandPassLow + dBandPassLow)*SampleRate/2.0;
	BandHigh = (sBandPassHigh + dBandPassHigh)*SampleRate/2.0;
	BandLow2 = (sBandPassLow + dBandPassLow)*SampleRate/2.0;
	BandHigh2 = (sBandPassHigh + dBandPassHigh)*SampleRate/2.0;
	LimitMinLowBand = -20000*2.0/SampleRate;
	LimitMaxHighBand = 20000*2.0/SampleRate;
	pRulePixmap = new QPixmap(":images/rule.png");
	pBackgroundPixmap = new QPixmap(":images/Panorama/bg/3.png");
	pBackgroundPixmap2 = new QPixmap(":images/Panorama/bg/4.png");
	pBackgroundPixmap3 = new QPixmap(":images/Panorama/bg/5.png");
	pBackgroundPixmap4 = new QPixmap(":images/Panorama/bg/www.png");
	pLeftRange = new QPixmap(":images/Panorama/agt_back-3.png");
	pRightRange = new QPixmap(":images/Panorama/agt_forward-5.png");
	BackgroundType = IMAGE;
	GridScaleType = kHz50;
	ScaleXPanoram = 1.0;
	TrxCnt = TX_TO_RX_TIME;
	CircleCnt = 0;
	MeanBuff = 5;
	SizeSpBuff = 4096;
	pBuff = new float[BUFF_SIZE];
	pSpBuff = new float[BUFF_SIZE];
	BufWf = new float[BUFF_SIZE];
	for(int i = 0; i < BUFF_SIZE; i++)
	{
		pBuff[i] = -200.0f;
		pSpBuff[i] = -200.0f;
		BufWf[i] = -120.0f;
	}
	pCircleBuff = new float *[CIRCLE_BUFF_SIZE];
	for(int i = 0; i < CIRCLE_BUFF_SIZE; i++)
	{
		pCircleBuff[i] = new float[BUFF_SIZE];
	}
	for(int i = 0; i < CIRCLE_BUFF_SIZE; i++)
		for(int j = 0; j < BUFF_SIZE; j++)
			pCircleBuff[i][j] = -200;

	for(int j = 0; j < NUM_MEANE_BUFF; j++)
	{
		MeanMin[j] = -130;
		MeanMax[j] = 20;
	}
	memset(MedianTimeBuff[0], 0.0f, sizeof(MedianTimeBuff[0]));
	memset(MedianTimeBuff[1], 0.0f, sizeof(MedianTimeBuff[1]));
	memset(MedianTimeBuff[2], 0.0f, sizeof(MedianTimeBuff[2]));

	for(int i = 0; i < 7; i++)
	{
		for(int j = 0; j < BUFF_SIZE; j++)
			MedianTimeBuff[i][j] = -200.0;
	}
	TimerMeanBuff.start(PAN_UPDATE);
	TimerId = startTimer(30);

	connect(&TimerDrawFilterEdges, SIGNAL(timeout()), this, SLOT(FilterEdges()));
	connect(&TimerDrawFilterEdges2, SIGNAL(timeout()), this, SLOT(FilterEdges2()));
	connect(&TimerDbmPos, SIGNAL(timeout()), this, SLOT(DbmPos()));
	connect(&TimerFilterPos, SIGNAL(timeout()), this, SLOT(FilterPos()));
	connect(&TimerFilter2Pos, SIGNAL(timeout()), this, SLOT(Filter2Pos()));
	connect(&TimerAutosetDbm, SIGNAL(timeout()), this, SLOT(AutosetDbm()));
	connect(pPanOpt->ui.cbPowerChange, SIGNAL(currentIndexChanged(int)), this, SLOT(SetPowerComp(int)));
	connect(&TimerChangedFilter, SIGNAL(timeout()), this, SLOT(SlotChangeFilter()));
	connect(&TimerPichPos, SIGNAL(timeout()), this, SLOT(PichPos()));
	connect(&TimerPichPos2, SIGNAL(timeout()), this, SLOT(PichPos2()));
	connect(&TimerMousePos, SIGNAL(timeout()), this, SLOT(SlotMousePos()));
	connect(pPanOpt->ui.cbStepFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(SetStepFilter(int)));
	connect(&TimerTxToRx, SIGNAL(timeout()), this, SLOT(SlotTxToRx()));
	connect(pPanOpt->ui.slSpGradSaturation, SIGNAL(valueChanged(int)), this, SLOT(ChangedAlphaColorSp(int)));
	connect(pPanOpt->ui.slSpGradBrightness, SIGNAL(valueChanged(int)), this, SLOT(ChangedAlphaColorSp(int)));
	connect(pPanOpt->ui.slTrancpSpectrum, SIGNAL(valueChanged(int)), this, SLOT(ChangedAlphaColorSp(int)));
	connect(pPanOpt->ui.slTrancpTop, SIGNAL(valueChanged(int)), this, SLOT(ChangedAlphaColorSp(int)));
	connect(pPanOpt->ui.cbAutoLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTypeAutomatic(int)));
	connect(pPanOpt, SIGNAL(SignalChangeStyle(int)), this, SLOT(ChangedAlphaColorSp(int)));

	ChangedAlphaColorSp(0);
	resize(800, 600);
	BeginX = -1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	EndX   = 1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	Len = BeginX + 1;
	Step = 2.0/SizeSpBuff;
	StepCount = ((int)(Len/Step));
	GridStep = 20000*2.0/SampleRate;
	BeginGrid = -1.0;
	IsDrawFilterEdges = false;
	IsPressFilterEdges = false;
	IsDrawFilterEdges2 = false;
	IsPressFilterEdges2 = false;
	AlphaColor = 0;
	AlphaColor2 = 0;
	AlphaColorDbm = 0.4;
	sLenDbmY = 100.0;
	dLenDbmY = 0.0;
	sOffsetDbm = -120.0;
	dOffsetDbm = 0.0;
	AlphaColorCF = 0.0;
	AlphaColorCF2 = 0.0;
	alphaRitLim = 0.0;
	offsetDbm = sOffsetDbm;
	scaleDbm = sLenDbmY;
	LowDb = -150;
	HighDb = 50;
	IndxSubTexture = 0;
	SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
	SaveLenDbmY = sLenDbmY + dLenDbmY;
	PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
	LastPosObject = (OBJECTS)faceAtPosition(QPoint(width()-10, height()-10));
	ActionObject = UNPRESSED;
	IsVisibleInfo = true;
	SignalDds = sFreqDDS;
	SignalFilter = sFreqFilter1 + dFreqFilter1;
	SignalBandHigh = (sBandPassHigh + dBandPassHigh);
	SignalBandLow = (sBandPassLow + dBandPassLow);
	SignalFilter2 = (sFilter2 + dFilter2);
	SignalBandHigh2 = (sBandPassHigh2 + dBandPassHigh2);
	SignalBandLow2 = (sBandPassLow2 + dBandPassLow2);
	TimerSignalDds.start(PAN_UPDATE);
	timerAlphaColor.start(20);
	connect(&TimerSignalDds, SIGNAL(timeout()), this, SLOT(SlotChangedParametersFreq()));
	connect(&timerAlphaColor, SIGNAL(timeout()), this, SLOT(SlotRitColor()));
	connect(pPanOpt, SIGNAL(ChangeSettings()), this,SLOT(SlotChangedSettings()));
	connect(this, SIGNAL(ChangeZoom(double)), pPanOpt, SLOT(setZoom(double)));
	connect(this, SIGNAL(ChangeZoomPos(double)), pPanOpt, SLOT(setZoomPos(double)));

	pitchVal = 0;
	isRitOn = false;
	ritValue = 0;
	ritLimLow = 0;
	ritLimHigh = 0;
	glRitLimLow = 0.0;
	glRitLimHigh = 0.0;

	SetPowerComp(pPanOpt->ui.cbPowerChange->currentIndex());
	SetStepDbm();
	SetStepGrid();
}

Panarama::~Panarama()
{
	delete pRulePixmap;
	delete pBackgroundPixmap;

	delete[] BufWf;
	delete[] pBuff;
	delete[] pSpBuff;

	for(int i = 0; i < CIRCLE_BUFF_SIZE; i++)
		delete[] pCircleBuff[i];
	delete[] pCircleBuff;

	delete info_font;
	delete dbm_font;
	delete band_font;
	delete cursor_font;
}

void Panarama::SetBand(int band)
{
	if(band > 13)
		bandNum = 13;
	else if(band < 0)
		bandNum = 0;
	else
		bandNum = band;
}

void Panarama::initializeGL()
{
	makeCurrent();
	qglClearColor(Qt::black);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LeftRange = bindTexture((*pLeftRange),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, LeftRange);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	RightRange = bindTexture((*pRightRange),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, RightRange);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	RuleTexture = bindTexture((*pRulePixmap),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, RuleTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	BackgroundTexture = bindTexture((*pBackgroundPixmap),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, BackgroundTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	BackgroundTexture2 = bindTexture((*pBackgroundPixmap2),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, BackgroundTexture2);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	BackgroundTexture3 = bindTexture((*pBackgroundPixmap3),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, BackgroundTexture3);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	BackgroundTexture4 = bindTexture((*pBackgroundPixmap4),GL_TEXTURE_2D, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, BackgroundTexture4);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	BeginX = -1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	EndX   = 1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);

	Len = BeginX + 1;
	StepCount = ((int)(Len/Step));

	size_t img_sz = IMAGE_LEN * IMAGE_LEN * NUM_COLOR;
	GLvoid *pImg = malloc(img_sz);
	memset(pImg, 0xFF000000, img_sz);

	for(int i = 0; i < WF_TEX_H_CNT; i++)
		for(int k = 0; k < WF_TEX_W_CNT; k++)
		{
			glGenTextures(1, &WFTextures[i][k]);
			glBindTexture(GL_TEXTURE_2D, WFTextures[i][k]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_LEN, IMAGE_LEN, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImg);
		}

	free(pImg);

	glEnable(GL_MULTISAMPLE);
	StepMoveGrid = 0;
	sPosRule = 250;
	dPosRule = 0;
	posRule = 1.0 - sPosRule*2.0/height();
	lenRule = RULE_HEIGH*2.0f/height();

	info_font = new GLFont(this, QFont("Verdana", 8));
	dbm_font = new GLFont(this, QFont("Verdana", 8));
	cursor_font = new GLFont(this, QFont("Verdana", 10));
	band_font = new GLFont(this, QFont("Calibri", 12, QFont::Bold));
}

void Panarama::paintGL()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(height() == 0 || width() == 0)
		return;

	BeginX = -1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	EndX   = 1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	DrawBackground();
	DrawRule();
	if(IsEnableSp)
		DrawSpectr();

	DrawZoomer();
	DrawGrid();
	if((1.0-posRule)/ScaleWindowY > 50)
		DrawDbm();

	DrawFilter();
	DrawWaterfall();
	if(IsWindow)
		DrawCursor();

	glFlush();
	swapBuffers();
}
void Panarama::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(width) / height;
	glFrustum(-x, x, -1.0, 1.0, 0, 10);
	glMatrixMode(GL_MODELVIEW);

	HeighRule = RULE_HEIGH*2.0/height;
	ScaleWindowX = 2.0/width;
	ScaleWindowY = 2.0/height;

	oy1 = ScaleWindowY;
	oy2_5 = 2.5*ScaleWindowY;
	oy5 = 5*ScaleWindowY;
	oy10 = 10*ScaleWindowY;
	oy20 = 20*ScaleWindowY;
	oy30 = 30*ScaleWindowY;

	ox1 = ScaleWindowX;
	ox2_5 = 2.5*ScaleWindowX;
	ox5 = 5*ScaleWindowX;
	ox10 = 10*ScaleWindowX;
	ox20 = 20*ScaleWindowX;
	ox30 = 30*ScaleWindowX;

	BeginX = -1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	EndX   = 1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);

	Len = BeginX + 1;
	StepCount = ((int)(Len/Step));

	OffsetNum = (1.5*ox10)/(sScaleRuleX + dScaleRuleX);
	PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;

	if(sPosRule > (height-RULE_HEIGH))
		posRule = 1.0 - (height - RULE_HEIGH)*2.0/height;
	else
		posRule = 1.0 - sPosRule*2.0/height;
	lenRule = RULE_HEIGH*2.0f/height;
	isUpdate = true;

	SetStepGrid();
	SetStepDbm();
}
void Panarama::mousePressEvent(QMouseEvent *event)
{
	posPress = event->pos();
	PresPosX = event->x()*ScaleWindowX - 1.0;
	PresPosY = 1.0 - event->y()*ScaleWindowY;
	PresPosX2 = PresPosX;
	PresPosY2 = PresPosY;

	isUpdate = true;

	Object = (OBJECTS)faceAtPosition(event->pos());
	if(event->buttons() & Qt::LeftButton)
	{
		if(pPanOpt->ui.chbSwapLeftRight->isChecked())
		{
			switch(Object)
			{
			case WATERFALL:
			case GRID:
				IsMoveGrid = false;
				//
				if(!pPanOpt->ui.chbChangeDDS->isChecked())
				{
					if(LockGrid)
						return;

					if(!IsFilter2 || (IsFilter2 && IsCrossF1))
					{
						sDDSFreq -= ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan - sFilter-dFilter) - sPich;
						sFreqDDS = qRound((sDDSFreq)*SampleRate/2.0);
						if((sFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
							sFreqDDS -= sFreqDDS - sFreqDDS%StepGridDDSHz[numStepGridDDS] + StepGridDDSHz[numStepGridDDS];
						else
							sFreqDDS -= sFreqDDS%StepGridDDSHz[numStepGridDDS];
						sDDSFreq = sFreqDDS*2.0/SampleRate;
						dDDSFreq = 0.0;
					}
					else if(IsFilter2 && !IsCrossF1)
					{
							sDDSFreq -= ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan - sFilter2-dFilter2) - sPich;
						sFreqDDS = qRound((sDDSFreq)*SampleRate/2.0);
						if((sFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
							sFreqDDS -= sFreqDDS - sFreqDDS%StepGridDDSHz[numStepGridDDS] + StepGridDDSHz[numStepGridDDS];
						else
							sFreqDDS -= sFreqDDS%StepGridDDSHz[numStepGridDDS];
						sDDSFreq = sFreqDDS*2.0/SampleRate;
						dDDSFreq = 0.0;
					}
				}
				else
				{
					if(LockFilter)
						return;

					if(!IsFilter2 || (IsFilter2 && IsCrossF1))
					{
						if(!isRitOn)
						{
							sFilter = ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan) - sPich;
							sFreqFilter1 = qRound((sFilter)*SampleRate/2.0);
							if((sFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
								sFreqFilter1 = sFreqFilter1 - sFreqFilter1%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
							else
								sFreqFilter1 -= sFreqFilter1%StepGridHz[numStepGridFilter];
							sFilter = sFreqFilter1*2.0/SampleRate;
							dFilter = 0.0;
							dFreqFilter1 = 0;
						}
						else
						{
							float tmpPos = ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan) - sPich;
							if(tmpPos > (sFilter + RIT_LIMIT*2.0/SampleRate) || tmpPos < (sFilter - RIT_LIMIT*2.0/SampleRate))
							{
								isOutOfRit = true;
								return;
							}
							sRit = tmpPos - sFilter;
							sRitValue = sRit*SampleRate/2.0;
						}
					}
					else if(IsFilter2 && !IsCrossF1)
					{
						sFilter2 = ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan) - sPich;
						dFilter2 = 0.0;
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			if((Object == WATERFALL) || (Object == GRID))
			{
				if(pPanOpt->ui.chbGridMoving->isChecked() && !LockGrid)
				{
					freqStart = event->x();
					gridTimer.start();
					timeStart = gridTimer.getMilliSeconds();
					autoStepFreq = 0;
				}
			}
		}
	}
	else if(event->buttons() & Qt::RightButton)
	{
		if(!pPanOpt->ui.chbSwapLeftRight->isChecked())
		{
			switch(Object)
			{
			case WATERFALL:
			case GRID:
				IsMoveGrid = false;
				if(!pPanOpt->ui.chbChangeDDS->isChecked())
				{
					if(Object == GRID)
						StepMoveGrid = 0.0;
					if(LockGrid)
						return;

					if(!IsFilter2 || (IsFilter2 && IsCrossF1))
					{
						sDDSFreq -= ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan - sFilter-dFilter) - sPich;
						sFreqDDS = qRound((sDDSFreq)*SampleRate/2.0);
						if((sFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
							sFreqDDS -= sFreqDDS - sFreqDDS%StepGridDDSHz[numStepGridDDS] + StepGridDDSHz[numStepGridDDS];
						else
							sFreqDDS -= sFreqDDS%StepGridDDSHz[numStepGridDDS];
						sDDSFreq = sFreqDDS*2.0/SampleRate;
						dDDSFreq = 0.0;
					}
					else if(IsFilter2 && !IsCrossF1)
					{
						sDDSFreq -= ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan - sFilter2-dFilter2) - sPich;
						sFreqDDS = qRound((sDDSFreq)*SampleRate/2.0);
						if((sFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
							sFreqDDS -= sFreqDDS - sFreqDDS%StepGridDDSHz[numStepGridDDS] + StepGridDDSHz[numStepGridDDS];
						else
							sFreqDDS -= sFreqDDS%StepGridDDSHz[numStepGridDDS];
						sDDSFreq = sFreqDDS*2.0/SampleRate;
						dDDSFreq = 0.0;
					}
				}
				else
				{
					if(LockFilter)
						return;

					if(!IsFilter2 || (IsFilter2 && IsCrossF1))
					{
						if(!isRitOn)
						{
							sFilter = ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan) - sPich;//(MovePosX - sFilter-dFilter)/(sScaleRuleX + dScaleRuleX) + sFilter+dFilter - sPich;
							sFreqFilter1 = qRound((sFilter)*SampleRate/2.0);
							if((sFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
								sFreqFilter1 = sFreqFilter1 - sFreqFilter1%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
							else
								sFreqFilter1 -= sFreqFilter1%StepGridHz[numStepGridFilter];
							sFilter = sFreqFilter1*2.0/SampleRate;
							dFilter = 0.0;
							dFreqFilter1 = 0;
						}
					}
					else if(IsFilter2 && !IsCrossF1)
					{
						sFilter2 = ((MovePosX - sPosZoomPan - dPosZoomPan))/(sScaleRuleX + dScaleRuleX) + (sPosZoomPan+dPosZoomPan) - sPich;
						dFilter2 = 0.0;
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			if((Object == WATERFALL) || (Object == GRID))
			{
				if(pPanOpt->ui.chbGridMoving->isChecked() && !LockGrid)
				{
					freqStart = event->x();
					gridTimer.start();
					timeStart = gridTimer.getMilliSeconds();
					autoStepFreq = 0;
				}
			}
		}
	}
	else if(event->buttons() & Qt::MidButton)
	{
		if(!IsFilter2)
		{
			if(Object == GRID)
			{
				switch(Cursor)
				{
				case NON_CROSS:
					Cursor = CROSS;
					break;
				case CROSS:
					Cursor = HORIZONTAL;
					break;
				case HORIZONTAL:
					Cursor = VERTICAL;
					break;
				case VERTICAL:
					Cursor = NON_CROSS;
					break;
				default:
					Cursor = CROSS;
					break;
				}
				pPanOpt->PanOption.TypeCursor = (int)Cursor;
			}
		}
		else
			IsCrossF1 = !IsCrossF1;
	}
}
void Panarama::mouseMoveEvent(QMouseEvent *event)
{
	posMove = event->pos();
	LastPosObject = (OBJECTS)faceAtPosition(event->pos());
	MovePosX = event->x()*ScaleWindowX - 1.0;
	MovePosY = 1.0 - event->y()*ScaleWindowY;
	LastPos = event->pos();
	float tmpD = 0.0;
	int tmp;
	int h = height();
	if(event->buttons() & Qt::LeftButton)
	{
		isUpdate = true;
		switch(Object)
		{
		case ZOOMER:
			ActionObject = ZOOMER_PRESS;
			dPosZoomPan = MovePosX - PresPosX;
			if((sPosZoomPan + dPosZoomPan) > 1.0)
				dPosZoomPan = 1.0 - sPosZoomPan;
			else if((sPosZoomPan + dPosZoomPan) < -1.0)
				dPosZoomPan = -1.0 - sPosZoomPan;

			emit ChangeZoomPos(sPosZoomPan + dPosZoomPan);

			break;
		case RULE:
			ActionObject = PRESS_LEFT_RULE;
			tmp = qRound((1-posRule)*h/2);
			if((sPosRule+dPosRule) > tmp)
			{
				sPosRule = tmp;
				dPosRule = 0;
			}
			dPosRule = posMove.y() - posPress.y();
			if((sPosRule+dPosRule) < 40)
				dPosRule = 40 - sPosRule;
			else if((sPosRule+dPosRule) > (h-RULE_HEIGH))
				dPosRule = (h-RULE_HEIGH) - sPosRule;
			posRule = 1.0f - (sPosRule+dPosRule)*2.0f/h;
			PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
			//
			SetStepDbm();
			break;
		case WATERFALL:
		case GRID:
			if(!pPanOpt->ui.chbSwapLeftRight->isChecked())
			{
				IsMoveGrid = false;
				if(LockScreen | LockGrid)
					return;
				ActionObject = PRESS_LEFT_GRID;
				dFilter2 = (MovePosX2 - PresPosX2)/(sScaleRuleX + dScaleRuleX);
				if(event->x() > width()-1)
				{
					int x = event->globalX() - width();
					freqStart -= width();
					cursor.setPos(x, event->globalY());
					PresPosX = -1.0;
					sDDSFreq = sDDSFreq + dDDSFreq;
					if(IsFilter2 || TxVfo==1)
					{
						sFilter2 += dDDSFreq;
						dFilter2 = 0;
					}
					dDDSFreq = 0;
					return;
				}
				else if(event->x() < 1)
				{
					int x = event->globalX() + width();
					freqStart += width();
					cursor.setPos(x, event->globalY());
					PresPosX = 1.0;
					sDDSFreq = sDDSFreq + dDDSFreq;
					if(IsFilter2 || TxVfo==1)
					{
						sFilter2 += dDDSFreq;
						dFilter2 = 0;
					}
					dDDSFreq = 0;
					return;
				}
				dDDSFreq = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				sFreqDDS = qRound((sDDSFreq)*SampleRate/2.0);
				if((sFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
					sFreqDDS -= sFreqDDS - sFreqDDS%StepGridDDSHz[numStepGridDDS] + StepGridDDSHz[numStepGridDDS];
				else
					sFreqDDS -= sFreqDDS%StepGridDDSHz[numStepGridDDS];
				sDDSFreq = sFreqDDS*2.0/SampleRate;
				dFreqDDS = qRound(dDDSFreq*SampleRate/2.0);
				if((dFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
					dFreqDDS = dFreqDDS - (dFreqDDS%StepGridDDSHz[numStepGridDDS]) + StepGridDDSHz[numStepGridDDS];
				else
					dFreqDDS = dFreqDDS - (dFreqDDS%StepGridDDSHz[numStepGridDDS]);
				dDDSFreq = dFreqDDS*2.0/SampleRate;
				if(sDDSFreq + dDDSFreq > 0)
				{
					dDDSFreq = -sDDSFreq;
					dFreqDDS = -sFreqDDS;
				}
				if(bandNum == BAND2M)
				{
					double limLow = -(BPF2_START)*2.0/SampleRate;
					double limHig = -(BPF2_END - SampleRate)*2.0/SampleRate;

					if((sDDSFreq + dDDSFreq) > limLow)
					{
						dDDSFreq = limLow - sDDSFreq;
						sFreqDDS = -BPF2_START;
						dFreqDDS = 0;
					}
					else if((sDDSFreq + dDDSFreq) < limHig)
					{
						dDDSFreq = limHig - sDDSFreq;
						sFreqDDS = -BPF2_END+SampleRate;
						dFreqDDS = 0;
					}
				}
				else if(bandNum == BAND07M)
				{
					double limLow = -(BPF07_START)*2.0/SampleRate;
					double limHig = -(BPF07_END - SampleRate)*2.0/SampleRate;

					if((sDDSFreq + dDDSFreq) > limLow)
					{
						dDDSFreq = limLow - sDDSFreq;
						sFreqDDS = -BPF07_START;
						dFreqDDS = 0;
					}
					else if((sDDSFreq + dDDSFreq) < limHig)
					{
						dDDSFreq = limHig - sDDSFreq;
						sFreqDDS = -BPF07_END+SampleRate;
						dFreqDDS = 0;
					}
				}
				if(IsFilter2 || TxVfo==1)
				{
					dFilter2 = dDDSFreq;
					if((sFilter2+dFilter2) < -1.0 || (sFilter2+dFilter2) > 1.0)
					{
						IsFilter2 = false;
						emit IsOnFilter2(IsFilter2);
					}
				}
				if(!pPanOpt->ui.chbAverange->isChecked())
				{
					IsChangedFilter = true;
					TimerChangedFilter.start(200);
				}
				IsGridChanged = true;
			}
			else
			{
				if(LockScreen | LockGrid)
					return;
				ActionObject = PRESS_RIGHT_GRID;
			}
			if(!TimerMousePos.isActive())
				TimerMousePos.start(1);
			break;
		case PITCH:
		case MAIN_FILTER:
			if(LockScreen)
				return;
			if(LockFilter)
				return;
			ActionObject = PRESS_LEFT_CF;
			if(!isRitOn)
			{
				dFilter = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				sFreqFilter1 = qRound((sFilter)*SampleRate/2.0);
				if((sFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					sFreqFilter1 = sFreqFilter1 - sFreqFilter1%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
				else
					sFreqFilter1 -= sFreqFilter1%StepGridHz[numStepGridFilter];
				sFilter = sFreqFilter1*2.0/SampleRate;
				dFreqFilter1 = qRound(dFilter*SampleRate/2.0);		//
				if((dFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					dFreqFilter1 = dFreqFilter1 - (dFreqFilter1%StepGridHz[numStepGridFilter]) + StepGridHz[numStepGridFilter];
				else
					dFreqFilter1 = dFreqFilter1 - (dFreqFilter1%StepGridHz[numStepGridFilter]);
				dFilter = dFreqFilter1*2.0/SampleRate;
				if(sFilter + dFilter > 1.0)
				{
					dFilter = 1.0 - sFilter;
					dFreqFilter1 = SampleRate/2 - sFreqFilter1;
				}
				else if(sFilter + dFilter < -1.0)
				{
					dFilter = -1.0 - sFilter;
					dFreqFilter1 = -SampleRate/2 - sFreqFilter1;
				}
			}
			else
			{
				dRit = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				sRitValue = qRound(sRit*SampleRate/2.0);
				if((sRitValue%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					sRitValue = sRitValue - sRitValue%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
				else
					sRitValue -= sRitValue%StepGridHz[numStepGridFilter];
				sRit = sRitValue*2.0/SampleRate;
				dRitValue = qRound(dRit*SampleRate/2.0);		//
				if((dRitValue%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					dRitValue = dRitValue - (dRitValue%StepGridHz[numStepGridFilter]) + StepGridHz[numStepGridFilter];
				else
					dRitValue = dRitValue - (dRitValue%StepGridHz[numStepGridFilter]);
				dRit = dRitValue*2.0/SampleRate;
				float tmpLim = RIT_LIMIT*2.0f/SampleRate;
				if((sRit + dRit) > tmpLim)
				{
					dRit = tmpLim - sRit;
					dRitValue = RIT_LIMIT - sRitValue;
				}
				else if((sRit + dRit) < -tmpLim)
				{
					dRit = -tmpLim - sRit;
					dRitValue = -RIT_LIMIT - sRitValue;
				}
				if((sRit + dRit) > 1.0)
				{
					dRit = 1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
				else if((sRit + dRit) < -1.0)
				{
					dRit = -1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
			}
			StepMoveGrid = 0.0;
			break;
		case PITCH2:
		case MAIN_FILTER2:
			if(LockScreen | LockFilter)
				return;
			ActionObject = PRESS_LEFT_CF2;
			if(IsFilter2)
			{
				dFilter2 = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);

				if(TxVfo == 0)
				{
					if(sFilter2 + dFilter2 > 1.0)
						dFilter2 = 1.0 - sFilter2;
					else if(sFilter2 + dFilter2 < -1.0)
						dFilter2 = -1.0 - sFilter2;
				}
			}
			StepMoveGrid = 0.0;
			break;
		case BAND_FILTER:
			if(LockScreen | LockFilter)
				return;
			ActionObject = PRESS_LEFT_BF;
			if(!isRitOn)
			{
				dFilter = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				sFreqFilter1 = qRound((sFilter)*SampleRate/2.0);
				if((sFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					sFreqFilter1 = sFreqFilter1 - sFreqFilter1%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
				else
					sFreqFilter1 -= sFreqFilter1%StepGridHz[numStepGridFilter];
				sFilter = sFreqFilter1*2.0/SampleRate;
				dFreqFilter1 = qRound(dFilter*SampleRate/2.0);		//
				if((dFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					dFreqFilter1 = dFreqFilter1 - (dFreqFilter1%StepGridHz[numStepGridFilter]) + StepGridHz[numStepGridFilter];
				else
					dFreqFilter1 = dFreqFilter1 - (dFreqFilter1%StepGridHz[numStepGridFilter]);
				dFilter = dFreqFilter1*2.0/SampleRate;
				if(sFilter + dFilter > 1.0)
				{
					dFilter = 1.0 - sFilter;
					dFreqFilter1 = SampleRate/2 - sFreqFilter1;
				}
				else if(sFilter + dFilter < -1.0)
				{
					dFilter = -1.0 - sFilter;
					dFreqFilter1 = -SampleRate/2 - sFreqFilter1;
				}
			}
			else
			{
				dRit = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				sRitValue = qRound(sRit*SampleRate/2.0);
				if((sRitValue%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					sRitValue = sRitValue - sRitValue%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
				else
					sRitValue -= sRitValue%StepGridHz[numStepGridFilter];
				sRit = sRitValue*2.0/SampleRate;
				dRitValue = qRound(dRit*SampleRate/2.0);
				if((dRitValue%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
					dRitValue = dRitValue - (dRitValue%StepGridHz[numStepGridFilter]) + StepGridHz[numStepGridFilter];
				else
					dRitValue = dRitValue - (dRitValue%StepGridHz[numStepGridFilter]);
				dRit = dRitValue*2.0/SampleRate;
				float tmpLim = RIT_LIMIT*2.0f/SampleRate;
				if((sRit + dRit) > tmpLim)
				{
					dRit = tmpLim - sRit;
					dRitValue = RIT_LIMIT - sRitValue;
				}
				else if((sRit + dRit) < -tmpLim)
				{
					dRit = -tmpLim - sRit;
					dRitValue = -RIT_LIMIT - sRitValue;
				}
				if((sRit + dRit) > 1.0)
				{
					dRit = 1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
				else if((sRit + dRit) < -1.0)
				{
					dRit = -1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
			}
			IsPressFilterEdges = true;
			StepMoveGrid = 0.0;
			break;
		case BAND_FILTER2:
			if(LockScreen)
				return;
			if(LockFilter)
				return;
			ActionObject = PRESS_LEFT_BF2;
			dFilter2 = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);

			if(TxVfo == 0)
			{
				if(sFilter2 + dFilter2 > 1.0)
					dFilter2 = 1.0 - sFilter2;
				else if(sFilter2 + dFilter2 < -1.0)
					dFilter2 = -1.0 - sFilter2;
			}
			IsPressFilterEdges2 = true;
			StepMoveGrid = 0.0;
			break;
		case LEFT_BAND:
			if(LockScreen)
				return;
			if(LockBand)
				return;
			ActionObject = PRESS_LEFT_BF_LEFT;
			dBandPassLow = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
			if(sBandPassLow + dBandPassLow > sBandPassHigh)
				dBandPassLow = sBandPassHigh - sBandPassLow;
			else if(sBandPassLow + dBandPassLow < LimitMinLowBand)
				dBandPassLow = LimitMinLowBand - sBandPassLow;
			IsPressFilterEdges = true;
			StepMoveGrid = 0.0;
			break;
		case LEFT_BAND2:
			if(LockScreen)
				return;
			if(LockBand)
				return;
			ActionObject = PRESS_LEFT_BF_LEFT;
			dBandPassLow = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
			if(sBandPassLow + dBandPassLow > sBandPassHigh)
				dBandPassLow = sBandPassHigh - sBandPassLow;
			else if(sBandPassLow + dBandPassLow < LimitMinLowBand)
				dBandPassLow = LimitMinLowBand - sBandPassLow;
			IsPressFilterEdges2 = true;
			StepMoveGrid = 0.0;

			break;
		case RIGHT_BAND:
			if(LockScreen)
				return;
			if(LockBand)
				return;
			ActionObject = PRESS_LEFT_BF_RIGHT;
			dBandPassHigh = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
			if(sBandPassHigh + dBandPassHigh > LimitMaxHighBand)
				dBandPassHigh = LimitMaxHighBand - sBandPassHigh;
			else if(sBandPassHigh + dBandPassHigh < sBandPassLow)
				dBandPassHigh = sBandPassLow - sBandPassHigh;
			IsPressFilterEdges = true;
			StepMoveGrid = 0.0;
			break;
		case RIGHT_BAND2:
			if(LockScreen)
				return;
			if(LockBand)
				return;
			ActionObject = PRESS_LEFT_BF_RIGHT;
			dBandPassHigh = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
			if(sBandPassHigh + dBandPassHigh > LimitMaxHighBand)
				dBandPassHigh = LimitMaxHighBand - sBandPassHigh;
			else if(sBandPassHigh + dBandPassHigh < sBandPassLow)
				dBandPassHigh = sBandPassLow - sBandPassHigh;
			IsPressFilterEdges2 = true;
			StepMoveGrid = 0.0;
			break;
		case DB_PANEL:
			ActionObject = PRESS_LEFT_DBM;
			dOffsetDbm = -(MovePosY - PresPosY)*sLenDbmY/(1.0 - (posRule));
			if(sOffsetDbm+dOffsetDbm < -200)
				dOffsetDbm = -200 - sOffsetDbm;
			else if(sOffsetDbm+dOffsetDbm > 50)
				dOffsetDbm = 50 - sOffsetDbm;
			SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
			if(pPanOpt->ui.cbAutoLevel->currentIndex() == ALL_AUTO)
				IsChangedDbmLevel = true;
			break;
		default:
			break;
		}
	}
	else if(event->buttons() & Qt::RightButton)
	{
		isUpdate = true;
		switch(Object)
		{
		case RULE:
			ActionObject = PRESS_RIGHT_RULE;
			dScaleRuleX = (MovePosX - PresPosX)*35;
			if(sScaleRuleX + dScaleRuleX < 1.0)
				dScaleRuleX = 1.0 - sScaleRuleX;
			else if(sScaleRuleX + dScaleRuleX > 35.0)
				dScaleRuleX = 35.0 - sScaleRuleX;

			OffsetNum = (1.1*ox10)/(sScaleRuleX + dScaleRuleX);
			emit ChangeZoom(sScaleRuleX + dScaleRuleX);
			SetStepGrid();
			break;
		case WATERFALL:
		case GRID:
			if(pPanOpt->ui.chbSwapLeftRight->isChecked())
			{
				IsMoveGrid = false;
				if(LockScreen | LockGrid)
					return;
				ActionObject = PRESS_LEFT_GRID;
				if(event->x() > width()-1)
				{
					int x = event->globalX() - width();
					freqStart -= width();

					cursor.setPos(x, event->globalY());
					PresPosX = -1.0;
					sDDSFreq = sDDSFreq + dDDSFreq;
					if(IsFilter2 || TxVfo == 1)
					{
						sFilter2 += dDDSFreq;
						dFilter2 = 0;
					}
					dDDSFreq = 0;
					return;
				}
				else if(event->x() < 1)
				{
					int x = event->globalX() + width();
					freqStart += width();
					cursor.setPos(x, event->globalY());
					PresPosX = 1.0;
					sDDSFreq = sDDSFreq + dDDSFreq;
					if(IsFilter2 || TxVfo == 1)
					{
						sFilter2 += dDDSFreq;
						dFilter2 = 0;
					}
					dDDSFreq = 0;
					return;
				}
				dDDSFreq = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				sFreqDDS = qRound((sDDSFreq)*SampleRate/2.0);
				if((sFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
					sFreqDDS -= sFreqDDS - sFreqDDS%StepGridDDSHz[numStepGridDDS] + StepGridDDSHz[numStepGridDDS];
				else
					sFreqDDS -= sFreqDDS%StepGridDDSHz[numStepGridDDS];
				sDDSFreq = sFreqDDS*2.0/SampleRate;
				dFreqDDS = qRound(dDDSFreq*SampleRate/2.0);
				if((dFreqDDS%StepGridDDSHz[numStepGridDDS]) > (StepGridDDSHz[numStepGridDDS]/2))
					dFreqDDS = dFreqDDS - (dFreqDDS%StepGridDDSHz[numStepGridDDS]) + StepGridDDSHz[numStepGridDDS];
				else
					dFreqDDS = dFreqDDS - (dFreqDDS%StepGridDDSHz[numStepGridDDS]);
				dDDSFreq = dFreqDDS*2.0/SampleRate;
				if(sDDSFreq + dDDSFreq > 0)
				{
					dDDSFreq = -sDDSFreq;
					dFreqDDS = -sFreqDDS;
				}
				if(IsFilter2 || TxVfo == 1)
				{
					dFilter2 = dDDSFreq;
					if((sFilter2+dFilter2) < -1.0 || (sFilter2+dFilter2) > 1.0)
					{
						IsFilter2 = false;
						emit IsOnFilter2(IsFilter2);
					}
				}
				if(bandNum == BAND2M)
				{
					double limLow = -(BPF2_START)*2.0/SampleRate;
					double limHig = -(BPF2_END - SampleRate)*2.0/SampleRate;
					if((sDDSFreq + dDDSFreq) > limLow)
					{
						dDDSFreq = limLow - sDDSFreq;
						sFreqDDS = -BPF2_START;
						dFreqDDS = 0;
					}
					else if((sDDSFreq + dDDSFreq) < limHig)
					{
						dDDSFreq = limHig - sDDSFreq;
						sFreqDDS = -BPF2_END+SampleRate;
						dFreqDDS = 0;
					}
				}
				else if(bandNum == BAND07M)
				{
					double limLow = -(BPF07_START)*2.0/SampleRate;
					double limHig = -(BPF07_END - SampleRate)*2.0/SampleRate;
					if((sDDSFreq + dDDSFreq) > limLow)
					{
						dDDSFreq = limLow - sDDSFreq;
						sFreqDDS = -BPF07_START;
						dFreqDDS = 0;
					}
					else if((sDDSFreq + dDDSFreq) < limHig)
					{
						dDDSFreq = limHig - sDDSFreq;
						sFreqDDS = -BPF07_END+SampleRate;
						dFreqDDS = 0;
					}
				}

				if(!pPanOpt->ui.chbAverange->isChecked())
				{
					IsChangedFilter = true;
					TimerChangedFilter.start(PAN_UPDATE);
				}
				IsGridChanged = true;
				if(pPanOpt->ui.chbGridMoving->isChecked() && !LockGrid)
				{
					if(!TimerMousePos.isActive())
					{
						FirstPos = event->pos();
						TimerMousePos.start(50);
						CntLenMove = 0;
					}
					else
					{
						if(FirstPos == event->pos())
						{
							FirstPos = event->pos();
							CntLenMove = 0;
						}
					}
				}
			}
			else
			{
				if(LockScreen | LockGrid)
					return;

				ActionObject = PRESS_RIGHT_GRID;
			}
			break;
		case MAIN_FILTER:
		case PITCH:
		case BAND_FILTER:
			if(LockScreen | LockFilter | LockGrid)
				return;
			ActionObject = PRESS_LEFT_CF;
			dFilter = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
			sFreqFilter1 = qRound((sFilter)*SampleRate/2.0);
			if((sFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
				sFreqFilter1 = sFreqFilter1 - sFreqFilter1%StepGridHz[numStepGridFilter] + StepGridHz[numStepGridFilter];
			else
				sFreqFilter1 -= sFreqFilter1%StepGridHz[numStepGridFilter];
			sFilter = sFreqFilter1*2.0/SampleRate;
			dFreqFilter1 = qRound(dFilter*SampleRate/2.0);
			if((dFreqFilter1%StepGridHz[numStepGridFilter]) > (StepGridHz[numStepGridFilter]/2))
				dFreqFilter1 = dFreqFilter1 - (dFreqFilter1%StepGridHz[numStepGridFilter]) + StepGridHz[numStepGridFilter];
			else
				dFreqFilter1 = dFreqFilter1 - (dFreqFilter1%StepGridHz[numStepGridFilter]);
			dFilter = dFreqFilter1*2.0/SampleRate;
			if(isRitOn)
			{
				tmpD = RIT_LIMIT*2.0/SampleRate;
				if((sFilter + dFilter + tmpD) > 1.0)
				{
					dFilter = 1.0 - sFilter - tmpD;
					dFreqFilter1 = SampleRate/2 - sFreqFilter1 - RIT_LIMIT;
				}
				else if((sFilter + dFilter - tmpD) < -1.0)
				{
					dFilter = -1.0 - sFilter + tmpD;
					dFreqFilter1 = -SampleRate/2 - sFreqFilter1 + RIT_LIMIT;
				}
			}
			else
			{
				if(sFilter + dFilter > 1.0)
				{
					dFilter = 1.0 - sFilter;
					dFreqFilter1 = SampleRate/2 - sFreqFilter1;
				}
				else if(sFilter + dFilter < -1.0)
				{
					dFilter = -1.0 - sFilter;
					dFreqFilter1 = -SampleRate/2 - sFreqFilter1;
				}
			}
			dFreqDDS = dFreqFilter1;
			dDDSFreq = dFreqDDS*2.0/SampleRate;
			if(bandNum == BAND2M)
			{
				double limLow = -(BPF2_START)*2.0/SampleRate;
				double limHig = -(BPF2_END - SampleRate)*2.0/SampleRate;

				if((sDDSFreq + dDDSFreq) > limLow)
				{
					dDDSFreq = limLow - sDDSFreq;
					sFreqDDS = -BPF2_START;
					dFreqDDS = 0;
				}
				else if((sDDSFreq + dDDSFreq) < limHig)
				{
					dDDSFreq = limHig - sDDSFreq;
					sFreqDDS = -BPF2_END+SampleRate;
					dFreqDDS = 0;
				}
			}
			else if(bandNum == BAND07M)
			{
				double limLow = -(BPF07_START)*2.0/SampleRate;
				double limHig = -(BPF07_END - SampleRate)*2.0/SampleRate;

				if((sDDSFreq + dDDSFreq) > limLow)
				{
					dDDSFreq = limLow - sDDSFreq;
					sFreqDDS = -BPF07_START;
					dFreqDDS = 0;
				}
				else if((sDDSFreq + dDDSFreq) < limHig)
				{
					dDDSFreq = limHig - sDDSFreq;
					sFreqDDS = -BPF07_END+SampleRate;
					dFreqDDS = 0;
				}
			}
			break;
		case LEFT_BAND:
			if(LockScreen)
				return;
			if(LockBand)
				return;

			if(Mode==AM || Mode==FMN || Mode==DSB || Mode==SAM)
			{
				ActionObject = PRESS_RIGHT_BF_LEFT;
				dBandPassLow = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				if(sBandPassLow + dBandPassLow > 0)
					dBandPassLow = -sBandPassLow;
				else if(sBandPassLow + dBandPassLow < LimitMinLowBand)
					dBandPassLow = LimitMinLowBand - sBandPassLow;

				sBandPassHigh = -sBandPassLow;
				dBandPassHigh = -dBandPassLow;

				IsPressFilterEdges = true;
				StepMoveGrid = 0.0;
			}
			break;
		case RIGHT_BAND:
			if(LockScreen)
				return;
			if(LockBand)
				return;

			if(Mode==AM || Mode==FMN || Mode==DSB || Mode==SAM)
			{
				ActionObject = PRESS_RIGHT_BF_RIGHT;
				dBandPassHigh = (MovePosX - PresPosX)/(sScaleRuleX + dScaleRuleX);
				if(sBandPassHigh + dBandPassHigh > LimitMaxHighBand)
					dBandPassHigh = LimitMaxHighBand - sBandPassHigh;
				else if(sBandPassHigh + dBandPassHigh < 0)
					dBandPassHigh = -sBandPassHigh;

				sBandPassLow = -sBandPassHigh;
				dBandPassLow = -dBandPassHigh;

				IsPressFilterEdges = true;
				StepMoveGrid = 0.0;
			}
			break;
		case DB_PANEL:
			ActionObject = PRESS_RIGHT_DBM;
			dLenDbmY = -(MovePosY - PresPosY)*200;
			if(sLenDbmY + dLenDbmY < 10)
				dLenDbmY = 10 - sLenDbmY;
			else if(sLenDbmY + dLenDbmY > 200.0)
				dLenDbmY = 200.0 - sLenDbmY;

			SaveLenDbmY = sLenDbmY + dLenDbmY;
			PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
			SetStepDbm();
			if(pPanOpt->ui.cbAutoLevel->currentIndex() == ALL_AUTO || pPanOpt->ui.cbAutoLevel->currentIndex() == WATERFALL_AUTO)
				IsChangedDbmLevel = true;
			break;
		default:
			break;
		}
	}
	isUpdate = true;
}
void Panarama::mouseReleaseEvent(QMouseEvent *event)
{
	ReleasePos = event->pos();
	if(pPanOpt->ui.chbGridMoving->isChecked() && !LockGrid)
	{
		freqEnd = event->x();
		timeEnd = gridTimer.getMilliSeconds();

		autoStepFreq = (15*(freqEnd*SampleRate/width() - freqStart*SampleRate/width())/(timeEnd - timeStart))/(sScaleRuleX);
		if((qAbs(autoStepFreq) < 25) || (timeEnd > 300))
			autoStepFreq = 0;
	}
	else
		autoStepFreq = 0;
	freqEnd = freqStart;

	if(IsGridChanged)
	{
		IsMoveGrid = true;
		IsGridChanged = false;
	}
	sScaleRuleX += dScaleRuleX;
	dScaleRuleX = 0;
	sPosRule += dPosRule;
	dPosRule = 0;
	sPosZoomPan += dPosZoomPan;
	dPosZoomPan = 0.0;
	IsPressFilterEdges = false;
	IsPressFilterEdges2 = false;
	sRuleYPos += dRuleYPos;
	dRuleYPos = 0.0;
	pPanOpt->PanOption.RulePosY = (float)sRuleYPos;
	sDDSFreq += dDDSFreq;
	dDDSFreq = 0.0;
	sFreqDDS += dFreqDDS;
	dFreqDDS = 0;
	sFilter += dFilter;
	dFilter = 0;
	sFilter2 += dFilter2;
	dFilter2 = 0;
	sFreqFilter1 += dFreqFilter1;
	dFreqFilter1 = 0;
	sBandPassLow += dBandPassLow;
	dBandPassLow = 0.0;
	sBandPassHigh += dBandPassHigh;
	dBandPassHigh = 0.0;
	sBandPassLow2 += dBandPassLow2;
	dBandPassLow2 = 0.0;
	sBandPassHigh2 += dBandPassHigh2;
	dBandPassHigh2 = 0.0;
	sRit += dRit;
	dRit = 0.0f;
	sRitValue += dRitValue;
	dRitValue = 0;
	sOffsetDbm += dOffsetDbm;
	dOffsetDbm = 0.0;
	sLenDbmY += dLenDbmY;
	dLenDbmY = 0.0;
	pPanOpt->PanOption.OffsetDbm = dOffsetDbm + sOffsetDbm;
	pPanOpt->PanOption.LenDbm = dLenDbmY + sLenDbmY;
	ActionObject = UNPRESSED;
	isUpdate = true;

	if((TRxMode == RX) && (TRxModeOld == RX))
	{
		DbmRxState[currentBand].offset = sOffsetDbm;
		DbmRxState[currentBand].scale = sLenDbmY;
	}
	else if((TRxMode == TX) && (TRxModeOld == TX))
	{
		DbmTxState.offset = sOffsetDbm;
		DbmTxState.scale = sLenDbmY;
	}
}
void Panarama::timerEvent(QTimerEvent *event)
{
	UpdateLevels();
	bool ssst = false;

	if(pPanOpt->ui.chbGridMoving->isChecked() && !LockGrid)
	{
		sFreqDDS += autoStepFreq;
		sDDSFreq += autoStepFreq*2.0/SampleRate;
		sFilter2 += autoStepFreq*2.0/SampleRate;

		ssst = true;
		if(sFreqDDS > 0)
		{
			sFreqDDS = 0;
			sDDSFreq = 0;
			autoStepFreq = 0;
		}
	}
	else
		autoStepFreq = 0;

	if(IsMoveGrid && pPanOpt->ui.chbGridMoving->isChecked())
	{
		if(bandNum == BAND2M)
		{
			double limLow = -(BPF2_START)*2.0/SampleRate;
			double limHig = -(BPF2_END - SampleRate)*2.0/SampleRate;

			if((sDDSFreq + dDDSFreq) > limLow)
			{
				dDDSFreq = limLow - sDDSFreq;
				sFreqDDS = -BPF2_START;
				dFreqDDS = 0;
			}
			else if((sDDSFreq + dDDSFreq) < limHig)
			{
				dDDSFreq = limHig - sDDSFreq;
				sFreqDDS = -BPF2_END+SampleRate;
				dFreqDDS = 0;
			}
		}
		else if(bandNum == BAND07M)
		{
			double limLow = -(BPF07_START)*2.0/SampleRate;
			double limHig = -(BPF07_END - SampleRate)*2.0/SampleRate;

			if((sDDSFreq + dDDSFreq) > limLow)
			{
				dDDSFreq = limLow - sDDSFreq;
				sFreqDDS = -BPF07_START;
				dFreqDDS = 0;
			}
			else if((sDDSFreq + dDDSFreq) < limHig)
			{
				dDDSFreq = limHig - sDDSFreq;
				sFreqDDS = -BPF07_END+SampleRate;
				dFreqDDS = 0;
			}
		}
		if(IsFilter2 || TxVfo==1)
		{
			sFilter2 += StepMoveGrid;
			if((sFilter2+dFilter2) < -1.0 || (sFilter2+dFilter2) > 1.0)
			{
				IsFilter2 = false;
				emit IsOnFilter2(IsFilter2);
			}
		}

		if(pPanOpt->ui.chbGridSlowDown->isChecked())
		{
			StepMoveGrid *= pPanOpt->ui.slLevelSlowdown->value()*0.0001;
			if(StepMoveGrid < 50*2.0/SampleRate && StepMoveGrid > -50*2.0/SampleRate)
				StepMoveGrid = 0.0;
		}
	}
	else
		StepMoveGrid = 0.0;
	double Step1 = (2.0/width())/(sScaleRuleX + dScaleRuleX);
	double Step2 = 2.0/4096.0;
	double Begin1 = -1.0;
	double Begin2 = -1.0;
	if(Step1 < Step2)
		Step1 = Step2;
	int  k = 0;
	for(int i = 0; i < 4096; k++, Begin1 += Step1)
	{
		double tmpData = pBuff[i];
		int Cnt = 0;
		while(Begin2 <= Begin1)
		{
			if(tmpData < pBuff[i])
				tmpData = pBuff[i];
			i++;
			Cnt++;
			Begin2 += Step2;
		}
		pSpBuff[k] = tmpData;
	}
	SizeSpBuff = k;
	Step = 2.0/SizeSpBuff;

	if(TrxCnt>TX_TO_RX_TIME && TRxMode==RX)
	{
		if((pPanOpt->ui.cbAutoLevel->currentIndex() != MULTIHAND && IsMoveUpdate)&& (!TimerAutosetDbm.isActive()))
			TimerAutosetDbm.start(PAN_UPDATE);
	}
	if(isUpdate)
	{
		updateGL();
		isUpdate = false;
	}

	if(ssst)
		isUpdate = true;
}
void Panarama::wheelEvent(QWheelEvent *event)
{
	int k = 0;
	k = event->delta();
	if(k > 0)
	{
		if(LastPosObject == GRID || LastPosObject == WATERFALL)
		{
			if(!LockGrid)
            {
                if(pPanOpt->ui.chbWheelChangeFilter->isChecked())
                {
                    SetFilter(GetFilter() + StepDDSHz[numStepGridDDS]);
                }
                else
                {
                    emit ChangeStepDDS(1);
                }
            }
		}
		else if(LastPosObject == MAIN_FILTER || LastPosObject == BAND_FILTER)
		{
			if(LockFilter || LockBand)
				return;
			if(isRitOn)
			{
				sRitValue += StepGridHz[numStepGridFilter];
				sRit = sRitValue*2.0/SampleRate;
				float tmpLim = RIT_LIMIT*2.0f/SampleRate;
				if((sRit + dRit) > tmpLim)
				{
					dRit = tmpLim - sRit;
					dRitValue = RIT_LIMIT - sRitValue;
				}
				else if((sRit + dRit) < -tmpLim)
				{
					dRit = -tmpLim - sRit;
					dRitValue = -RIT_LIMIT - sRitValue;
				}
				if((sRit + dRit) > 1.0)
				{
					dRit = 1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
				else if((sRit + dRit) < -1.0)
				{
					dRit = -1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
			}
			else
			{
				sFreqFilter1 += StepGridHz[numStepGridFilter];
				sFilter = sFreqFilter1*2.0/SampleRate;
			}
		}
		else if(LastPosObject == MAIN_FILTER2 || LastPosObject == BAND_FILTER2)
		{
			if(LockFilter || LockBand)
				return;
			sFilter2 += ScaleWindowX/(sScaleRuleX + dScaleRuleX);
		}
	}
	if(k < 0)
	{
		if(LastPosObject == GRID || LastPosObject == WATERFALL)
		{
			if(!LockGrid)
            {
                if(pPanOpt->ui.chbWheelChangeFilter->isChecked())
                {
                    SetFilter(GetFilter() - StepDDSHz[numStepGridDDS]);
                }
                else
                {
                    emit ChangeStepDDS(-1);
                }
            }
		}
		else if(LastPosObject == MAIN_FILTER || LastPosObject == BAND_FILTER)
		{
			if(LockFilter || LockBand)
				return;
			if(isRitOn)
			{
				sRitValue -= StepGridHz[numStepGridFilter];
				sRit = sRitValue*2.0/SampleRate;
				float tmpLim = RIT_LIMIT*2.0f/SampleRate;
				if((sRit + dRit) > tmpLim)
				{
					dRit = tmpLim - sRit;
					dRitValue = RIT_LIMIT - sRitValue;
				}
				else if((sRit + dRit) < -tmpLim)
				{
					dRit = -tmpLim - sRit;
					dRitValue = -RIT_LIMIT - sRitValue;
				}
				if((sRit + dRit) > 1.0)
				{
					dRit = 1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
				else if((sRit + dRit) < -1.0)
				{
					dRit = -1.0 - sRit;
					dRitValue = SampleRate/2 - sRitValue;
				}
			}
			else
			{
				sFreqFilter1 -= StepGridHz[numStepGridFilter];
				sFilter = sFreqFilter1*2.0/SampleRate;
			}
		}
		else if(LastPosObject == MAIN_FILTER2 || LastPosObject == BAND_FILTER2)
		{
			if(LockFilter || LockBand)
				return;
			sFilter2 -= ScaleWindowX/(sScaleRuleX + dScaleRuleX);
		}
	}
	event->accept();
}
void Panarama::enterEvent(QEvent *event)
{
	setMouseTracking(true);
	IsWindow = true;
	isUpdate = true;
}
void Panarama::leaveEvent(QEvent *event)
{
	setMouseTracking(false);
	IsWindow = false;
	isUpdate = true;
}

int Panarama::faceAtPosition(const QPoint &pos)
{
	  const int MaxSize = 512;
	  GLuint buffer[MaxSize];
	  GLint viewport[4];
	  glGetIntegerv(GL_VIEWPORT, viewport);
	  glSelectBuffer(MaxSize, buffer);
	  glRenderMode(GL_SELECT);
	  glInitNames();
	  glPushName(0);
	  glMatrixMode(GL_PROJECTION);
	  glPushMatrix();
	  glLoadIdentity();
	  gluPickMatrix((GLdouble)pos.x(), (GLdouble)(viewport[3] - pos.y()), 3.0, 3.0, viewport);
	  glFrustum(-1, 1, -1.0, 1.0, -10, 10);
	  DrawBaseElement();
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();
	  if (!glRenderMode(GL_RENDER))
		return -1;
	  return buffer[3];
}

void Panarama::DrawBaseElement()
{
	register float tmpX1 = sFilter+ dFilter-sPosZoomPan - dPosZoomPan;
	if(isRitOn)
		tmpX1 += sRit + dRit;
	register float tmpX2 = sPosZoomPan + dPosZoomPan;
	register float tmpX3 = posRule;
	register float tmpX4 = sFilter2+ dFilter2-sPosZoomPan - dPosZoomPan;
	register float tmpX5 = sBandPassLow + dBandPassLow;
	register float tmpX6 = sBandPassHigh + dBandPassHigh;

	glLoadName((int)ZOOMER);
	glBegin(GL_TRIANGLES);
		glVertex3f(tmpX2 -ox10, tmpX3-1.5*oy10,  -0.7);
		glVertex3f(tmpX2 +ox10, tmpX3-1.5*oy10,  -0.7);
		glVertex3f(tmpX2, tmpX3, -0.7);
	glEnd();
	glPushMatrix();
	glTranslated(tmpX2, tmpX3, 0.0);
	glScaled(sScaleRuleX + dScaleRuleX, 1.0 - (tmpX3), 1.0);
	if(IsDrawFilterEdges)
	{
		glLineWidth(1.5);
		glLoadName((int)LEFT_BAND);
		glBegin(GL_LINES);
			glVertex3d(tmpX1 + tmpX5, 1.0, -0.37);
			glVertex3d(tmpX1 + tmpX5, 0.0, -0.37);
		glEnd();
		glLoadName((int)RIGHT_BAND);
		glBegin(GL_LINES);
			glVertex3d(tmpX1 + tmpX6, 0.0, -0.37);
			glVertex3d(tmpX1 + tmpX6, 1.0, -0.37);
		glEnd();
	}
	glLineWidth(1.5);
	glLoadName((int)MAIN_FILTER);
	glBegin(GL_LINES);
		glVertex3d(tmpX1, 1.0, -0.4);
		glVertex3d(tmpX1, 0.0, -0.4);
	glEnd();
	glLineWidth(1.0);
	glPopMatrix();
	glLoadName((int)DB_PANEL);
	glBegin(GL_QUADS);
		glVertex3d(-1.0, 1.0, -0.35);
		glVertex3d(-1.0, tmpX3, -0.35);
		glVertex3d(-1.0 + ox30 + ox5, tmpX3, -0.35);
		glVertex3d(-1.0 + ox30 + ox5, 1.0, -0.35);
	glEnd();
	glPushMatrix();
	glTranslated(tmpX2, tmpX3, 0.0);
	glScaled(sScaleRuleX + dScaleRuleX, 1.0 - (tmpX3), 1.0);
	if(sPich != 0.0)
	{
		glLoadName((int)PITCH);
		glBegin(GL_LINES);
			glVertex3d(tmpX1 + sPich, 1.0, -0.4);
			glVertex3d(tmpX1 + sPich, 0, -0.4);
		glEnd();
		glLoadName((int)PITCH2);
		glBegin(GL_LINES);
			glVertex3d(tmpX4 + sPich, 1.0, -0.4);
			glVertex3d(tmpX4 + sPich, 0, -0.4);
		glEnd();
	}
	glLoadName((int)BAND_FILTER);
	glBegin(GL_QUADS);
		glVertex3d(tmpX1 + tmpX5, 1.0, -0.35);
		glVertex3d(tmpX1 + tmpX5, 0.0, -0.35);
		glVertex3d(tmpX1 + tmpX6, 0.0, -0.35);
		glVertex3d(tmpX1 + tmpX6, 1.0, -0.35);
	glEnd();
	glPopMatrix();
	if(IsFilter2)
	{
		glPushMatrix();
		glTranslated(tmpX2, tmpX3, 0.0);
		glScaled(sScaleRuleX + dScaleRuleX, 1.0 - (tmpX3), 1.0);
		if(IsDrawFilterEdges2)
		{
			glLineWidth(1.5);
			glLoadName((int)LEFT_BAND2);
			glBegin(GL_LINES);
				glVertex3d(tmpX4 + tmpX5, 1.0, -0.33);
				glVertex3d(tmpX4 + tmpX5, 0.0, -0.33);
			glEnd();
			glLoadName((int)RIGHT_BAND2);
			glBegin(GL_LINES);
				glVertex3d(tmpX4 + tmpX6, 0.0, -0.33);
				glVertex3d(tmpX4 + tmpX6, 1.0, -0.33);
			glEnd();
		}
		glLineWidth(1.5);
		glLoadName((int)MAIN_FILTER2);
		glBegin(GL_LINES);
			glVertex3d(tmpX4, 1.0, -0.34);
			glVertex3d(tmpX4, 0.0, -0.34);
		glEnd();
		glLineWidth(1.0);
		glLoadName((int)BAND_FILTER2);
		glBegin(GL_QUADS);
			glVertex3d(tmpX4 + tmpX5, 1.0, -0.33);
			glVertex3d(tmpX4 + tmpX5, 0.0, -0.33);
			glVertex3d(tmpX4 + tmpX6, 0.0, -0.33);
			glVertex3d(tmpX4 + tmpX6, 1.0, -0.33);
		glEnd();
		glPopMatrix();
	}
	glLoadName((int)RULE);
	glBegin(GL_QUADS);
		glVertex3f(-1.0f, posRule, 0.0f);
		glVertex3f(-1.0f, posRule - lenRule, 0.0f);
		glVertex3f( 1.0f, posRule - lenRule, 0.0f);
		glVertex3f( 1.0f, posRule, 0.0f);
	glEnd();


	glLoadName((int)GRID);
	glBegin(GL_QUADS);
		glVertex3d(-1.0, 1.0 , 0.0);
		glVertex3d(-1.0, tmpX3, 0.0);
		glVertex3d( 1.0, tmpX3, 0.0);
		glVertex3d( 1.0, 1.0 , 0.0);
	glEnd();
	glLoadName((int)WATERFALL);
	glBegin(GL_QUADS);
		glVertex3d(-1.0, tmpX3, 0.0);
		glVertex3d(-1.0, -1.0, 0.0);
		glVertex3d( 1.0, -1.0, 0.0);
		glVertex3d( 1.0, tmpX3, 0.0);
	glEnd();
}

void Panarama::DrawBackground()
{
	if(pPanOpt->ui.comboBox_2->currentIndex() == SOLID)
	{
		if(pPanOpt->ui.cbPanType->currentIndex() == LINES)
			qglColor(pPanOpt->pColorSolidLineBack->getColor());
		else
			qglColor(pPanOpt->pColorSolidBack->getColor());
		glBegin(GL_QUADS);
			glVertex3d(-1.0, 1.0 , 0.0);
			glVertex3d(-1.0, posRule, 0.0);
			glVertex3d( 1.0, posRule, 0.0);
			glVertex3d( 1.0, 1.0 , 0.0);
		glEnd();
	}
	else if(pPanOpt->ui.comboBox_2->currentIndex() == GRADIENT)
	{
		glBegin(GL_QUADS);
			qglColor(QColor(0, 0, 0, 255));
			glVertex3d(-1.0, 1.0 , 0.0);
			if(pPanOpt->ui.cbPanType->currentIndex() == LINES)
				qglColor(pPanOpt->pColorLineBack->getColor());
			else
				qglColor(pPanOpt->pColorGradBack->getColor());
			glVertex3d(-1.0, posRule, 0.0);
			glVertex3d( 1.0, posRule, 0.0);
			qglColor(QColor(0, 0, 0, 255));
			glVertex3d( 1.0, 1.0 , 0.0);
		glEnd();
	}
	else if(pPanOpt->ui.comboBox_2->currentIndex() == IMAGE)
	{
		glEnable(GL_TEXTURE_2D);
		glColor4d(1.0, 1.0, 1.0, 1.0);
		if(pPanOpt->ui.comboBox_3->currentIndex() == 0)
			drawTexture(QRectF(-1.0, 2.0 + posRule, 2.0 , -2.0), BackgroundTexture, GL_TEXTURE_2D);
		else if(pPanOpt->ui.comboBox_3->currentIndex() == 1)
			drawTexture(QRectF(-1.0, 2.0 + posRule, 2.0 , -2.0), BackgroundTexture2, GL_TEXTURE_2D);
		else if(pPanOpt->ui.comboBox_3->currentIndex() == 2)
			drawTexture(QRectF(-1.0, 2.0 + posRule, 2.0 , -2.0), BackgroundTexture3, GL_TEXTURE_2D);
		else if(pPanOpt->ui.comboBox_3->currentIndex() == 3)
			drawTexture(QRectF(-1.0, 2.0 + posRule, 2.0 , -2.0), BackgroundTexture4, GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		glColor4d(1.0, 1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glColor4d(0.0, 0.0, 0.0, 0.2);
			glVertex3d(-1.0, 1.0 , 0.0);
			glVertex3d(-1.0, posRule, -0.001);
			glVertex3d( 1.0, posRule, -0.001);
			glVertex3d( 1.0, 1.0 , 0.0);
		glEnd();
	}
}

void Panarama::DrawRule()
{
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(0.0, 0.0, -0.4);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	drawTexture(QRectF(-1.0, posRule, 2, -HeighRule), RuleTexture, GL_TEXTURE_2D);
	glPopMatrix();
	//
	glPushMatrix();
	glTranslated(sPosZoomPan + dPosZoomPan, 0.0, -0.2);
	glScaled(sScaleRuleX + dScaleRuleX, 1.0, 1.0);
	qglColor(QColor(2, 84, 212,255));
	float Min;
	float Max;
	int pixelsWide;
	float WidthImgL = pLeftRange->width()*ScaleWindowX/(sScaleRuleX + dScaleRuleX);
	float WidthImgR = pRightRange->width()*ScaleWindowX/(sScaleRuleX + dScaleRuleX);
	float FpixelsWide = 0, Fox20 = (ox30+ox5)/(sScaleRuleX + dScaleRuleX), Fox5 = ox5/(sScaleRuleX + dScaleRuleX);
	register float tmpX1 = -pLeftRange->height()*ScaleWindowY*0.8;
	register float tmpX2 = -pRightRange->height()*ScaleWindowY*0.8;
	register float tmpX3 = sPosZoomPan + dPosZoomPan;
	register float tmpX4 = EndX -WidthImgR - WidthImgL - ox10/(sScaleRuleX + dScaleRuleX);
	register float tmpX5 = BeginX + 2*Fox20 + WidthImgR + WidthImgL + ox10/(sScaleRuleX + dScaleRuleX);
	register float tmpX6 = BeginX + 2*Fox20 + WidthImgR + WidthImgL + Fox20;
	register float tmpX7 = EndX - WidthImgR - WidthImgL - ox10/(sScaleRuleX + dScaleRuleX);

	for(int i = 0; i < BAND_SIZE; i++)
	{
		Min = (BandFreq[0][i] - SampleRate/2)*2.0f/SampleRate + (sDDSFreq + dDDSFreq);
		Max = (BandFreq[1][i] - SampleRate/2)*2.0f/SampleRate + (sDDSFreq + dDDSFreq);
		if(Max >= BeginX && Min < EndX)
		{
			pixelsWide = band_font->width(BandStr[i]);
			FpixelsWide = pixelsWide*ScaleWindowX/(sScaleRuleX + dScaleRuleX);
			if(Max >= EndX && Min <= (BeginX + Fox20))
			{
				glColor4f(1,1,1,1);
				drawTexture(QRectF(BeginX + Fox20 - tmpX3, 1.0 - oy2_5, WidthImgL, tmpX1), LeftRange, GL_TEXTURE_2D);
				drawTexture(QRectF(EndX - WidthImgR - tmpX3, 1.0 - oy2_5, WidthImgR, tmpX2), RightRange, GL_TEXTURE_2D);
				band_font->draw(BeginX + 10*Fox5 + WidthImgL - tmpX3, 1.0 - 1.5*oy10, -0.08, BandStr[i], QColor(30, 181, 252,255));
			}
			else if(Max >= EndX && Min > (BeginX + Fox20) && Min < tmpX7)
			{
				glColor4f(1,1,1,1);
				drawTexture(QRectF(Min - tmpX3, 1.0 - oy2_5, WidthImgL, tmpX1), LeftRange, GL_TEXTURE_2D);
				drawTexture(QRectF(EndX - tmpX3 - WidthImgR, 1.0 - oy2_5, WidthImgR, tmpX2), RightRange, GL_TEXTURE_2D);
				if(Min < (EndX - WidthImgR - WidthImgL - Fox20 - FpixelsWide))
					band_font->draw(Min - tmpX3 + 2*Fox5 + WidthImgL, 1.0 - 1.5*oy10, -0.08, BandStr[i], QColor(30, 181, 252,255));
			}
			else if(Max < EndX && Min <= (BeginX + Fox20) && Max > tmpX5)
			{
				glColor4f(1,1,1,1);
				drawTexture(QRectF(BeginX - tmpX3 + Fox20, 1.0 - oy2_5, WidthImgL, tmpX1), LeftRange, GL_TEXTURE_2D);
				drawTexture(QRectF( Max - tmpX3 - WidthImgR, 1.0 - oy2_5, WidthImgR, tmpX2), RightRange, GL_TEXTURE_2D);
				if(Max > tmpX6)
					band_font->draw(BeginX + 10*Fox5 + WidthImgL - tmpX3, 1.0 - 1.5*oy10, -0.08, BandStr[i], QColor(30, 181, 252,255));
			}
			else if(Min >= tmpX4)
			{
				glColor4f(1,1,1,1);
				drawTexture(QRectF(Min - tmpX3, 1.0 - oy2_5, WidthImgL, tmpX1), LeftRange, GL_TEXTURE_2D);
			}
			else if(Max <= tmpX5)
			{
				glColor4f(1,1,1,1);
				drawTexture(QRectF(Max - tmpX3 - WidthImgR, 1.0 - oy2_5, WidthImgR, tmpX2), RightRange, GL_TEXTURE_2D);
			}
			else
			{
				glColor4f(1,1,1,1);
				drawTexture(QRectF( Min - tmpX3, 1.0 - oy2_5, WidthImgL, tmpX1), LeftRange, GL_TEXTURE_2D);
				drawTexture(QRectF( Max - tmpX3 - WidthImgR, 1.0 - oy2_5, WidthImgR, tmpX2), RightRange, GL_TEXTURE_2D);
				band_font->draw(Min - tmpX3 + Fox5 + WidthImgL, 1.0 - 1.5*oy10, -0.08, BandStr[i], QColor(30, 181, 252,255));
			}
			break;
		}
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	info_font->draw(1.0-ox30, posRule - oy10-oy5, -1.0, "Hz");
}

void Panarama::DrawGrid()
{
	glColor4d(1.0, 1.0, 1.0, pPanOpt->ui.horizontalSlider->value()/255.0);
	BeginGrid = (sDDSFreq + dDDSFreq)/GridStep;
	int CntStepGrid = (int)BeginGrid;
	BeginGrid -= CntStepGrid;
	BeginGrid *= GridStep;
	int CntD = (Len-(BeginGrid + 1))/GridStep;
	BeginGrid += CntD*GridStep;
	glPushMatrix();
	glTranslated(sPosZoomPan + dPosZoomPan, 0.0, 0.0);
	glScaled(sScaleRuleX + dScaleRuleX, 1.0, 1.0);
	glPushMatrix();
	glTranslated(0.0, posRule, 0.0);
	glScaled(1.0, 1.0 - (posRule), 1.0);

	if(pPanOpt->ui.comboBox_5->currentIndex() == 1)
	{
		glLineStipple(1,0xAAAA);
		glEnable(GL_LINE_STIPPLE);
	}
	else
		glLineStipple(1,0xFFFF);
	if(pPanOpt->ui.chbGridOn->isChecked())
	{
		glBegin(GL_LINES);
			for(GLdouble i = BeginGrid - 1.0; i < EndX; i += GridStep)
			{
				glVertex3d(i - (sPosZoomPan + dPosZoomPan), 1.0, -0.2);
				glVertex3d(i - (sPosZoomPan + dPosZoomPan), 0.0, -0.2);
			}
		glEnd();
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.0, -1.0);

	glColor4d(1.0, 1.0, 1.0, 1.0);
	BeginGrid -= CntD*GridStep;
	QString StrNum;
	double Num = qAbs((CntStepGrid)*StepNum);
	while((BeginGrid - 1.0) <= BeginX)
	{
		BeginGrid += GridStep;
		Num += StepNum;
	}
	glPointSize(4);
	glEnable(GL_POINT_SMOOTH);
	for(GLdouble i = BeginGrid - 1.0; i < (EndX - 2*ox30/(sScaleRuleX + dScaleRuleX)); i += GridStep)
	{
        StrNum = freqToStr(qRound(Num*1000000));
		OffsetNum = info_font->width(StrNum)/(double)width()/(sScaleRuleX + dScaleRuleX);

		info_font->draw(i - OffsetNum -(sPosZoomPan + dPosZoomPan), posRule -oy10-oy5, 0.02, StrNum);
		Num += StepNum;
		glBegin(GL_POINTS);
			glVertex3d(i - (sPosZoomPan + dPosZoomPan), posRule  - oy2_5, 0.02);
		glEnd();

	}
	glDisable(GL_POINT_SMOOTH);
	glPopMatrix();
	glPopMatrix();
}

void Panarama::draw_filter_rect(int freq, int low, int high, QColor color, bool draw_edge, QColor edge_color, GLdouble z_pos)
{
	GLdouble scale_x = sScaleRuleX + dScaleRuleX;
	GLdouble filter_x_offset = freq * 2.0 / SampleRate - (sPosZoomPan + dPosZoomPan);

	GLfloat tmpPosX1 = filter_x_offset + low * 2.0 / SampleRate;
	GLfloat tmpPosX2 = filter_x_offset + high * 2.0 / SampleRate;

	glPushMatrix();
	glTranslated(sPosZoomPan + dPosZoomPan, posRule, 0.0);
	glScaled(scale_x, 1.0 - (posRule), 1.0);

	qglColor(color);

	glBegin(GL_QUADS);
	glVertex3d(tmpPosX1, 1.0, z_pos);
	glVertex3d(tmpPosX1, 0.0, z_pos);
	glVertex3d(tmpPosX2, 0.0, z_pos);
	glVertex3d(tmpPosX2, 1.0, z_pos);
	glEnd();

	if(draw_edge && edge_color.alpha() != 0)
	{
		qglColor(edge_color);

		glLineWidth(1.5);
		glBegin(GL_LINES);
		glVertex3d(tmpPosX1, 1.0, z_pos);
		glVertex3d(tmpPosX1, 0.0, z_pos);
		glVertex3d(tmpPosX2, 0.0, z_pos);
		glVertex3d(tmpPosX2, 1.0, z_pos);
		glEnd();
	}

	glPopMatrix();
}

void Panarama::draw_filter_center(int freq, QColor color, QColor higlight_color, GLdouble z_pos)
{
	GLdouble scale_x = sScaleRuleX + dScaleRuleX;
	GLdouble pans = sPosZoomPan + dPosZoomPan;
	GLdouble filter_x_offset = freq * 2.0 / SampleRate - pans;

	glLineWidth(2);
	glPushMatrix();
	glTranslated(sPosZoomPan + dPosZoomPan, posRule, 0.0);
	glScaled(scale_x, 1.0 - (posRule), 1.0);
	qglColor(color);
	glBegin(GL_LINES);
		glVertex3d(filter_x_offset, 1.0, z_pos);
		glVertex3d(filter_x_offset, 0.0, z_pos);
	glEnd();

	if(higlight_color.alpha() != 0)
	{
		qglColor(higlight_color);
		GLdouble shift = 2 * ScaleWindowX / scale_x; // 2px
		glBegin(GL_QUADS);
		glVertex3d(filter_x_offset + shift, 1.0, z_pos);
		glVertex3d(filter_x_offset + shift, 0.0, z_pos);
		glVertex3d(filter_x_offset - shift, 0.0, z_pos);
		glVertex3d(filter_x_offset - shift, 1.0, z_pos);
		glEnd();
	}
	glPopMatrix();
}

void Panarama::draw_filter_info(int vfo, int freq, int low, int high, bool show_rxtx_mark, bool is_tx, bool visible_filter_info, bool draw_on_left_side, QString vfo_name)
{
	GLdouble z_pos = -0.75;
	QString head_str;
	QColor h_color;
	QString vfo_str = vfo_name + ": " + freqToStr(vfo) + " Hz";

	if(is_tx)
	{
		head_str = "TX";
		h_color = QColor(255, 0, 0, 255);
	}
	else
	{
		head_str = "RX";
		h_color = QColor(150, 150, 150, 255);
	}

	GLdouble x_pos;
	GLdouble y_pos = 1; // top max

	GLdouble filters = freq * 2.0 / SampleRate;
	GLdouble pans = (sPosZoomPan + dPosZoomPan);
	GLdouble scale_x = sScaleRuleX + dScaleRuleX;
	GLdouble filter_x_offset = (filters - pans) * scale_x + pans;

	GLdouble vfo_text_width = info_font->width(vfo_str) * ScaleWindowX;

	GLdouble from_center_padding = 7 * ScaleWindowX; // 7px
	GLdouble left_padding = from_center_padding + vfo_text_width;
	GLdouble right_padding = from_center_padding;

	bool can_draw_left = (filter_x_offset - left_padding) > -1;
	bool can_draw_right = (filter_x_offset + right_padding + vfo_text_width) < 1;
	bool is_left_side;

	if(draw_on_left_side)
		is_left_side = can_draw_left;
	else
		is_left_side = !can_draw_right;

	GLdouble rxtx_mark_x_pos;
	if(is_left_side)
	{
		x_pos = filter_x_offset - left_padding;
		rxtx_mark_x_pos = filter_x_offset - from_center_padding - band_font->width(head_str) * ScaleWindowX;
	}
	else
	{
		x_pos = filter_x_offset + right_padding;
		rxtx_mark_x_pos = x_pos;
	}

	if(show_rxtx_mark)
	{
		y_pos -= band_font->height() * ScaleWindowY;
		band_font->draw(rxtx_mark_x_pos, y_pos, z_pos, head_str, h_color);
	}

	GLdouble info_h = info_font->height() * ScaleWindowY;
	y_pos -= info_h;
	info_font->draw(x_pos, y_pos, z_pos, vfo_str);

	y_pos -= info_h;

	if(visible_filter_info)
	{
		QString low_str = "L: " + freqToStr(low) + " Hz";
		QString high_str = "H: " + freqToStr(high) + " Hz";
		QString freq_str = "F: " + freqToStr(freq) + " Hz";

		y_pos -= info_h;
		info_font->draw(x_pos, y_pos, z_pos, low_str);
		y_pos -= info_h;
		info_font->draw(x_pos, y_pos, z_pos, high_str);
		y_pos -= info_h;
		info_font->draw(x_pos, y_pos, z_pos, freq_str);
	}
}

void Panarama::DrawFilter()
{
	double fvfob = ((sFilter2 + dFilter2) - (sDDSFreq + dDDSFreq) + 1.0) * SampleRate/2.0;
	double fvfoa = ((sFilter + dFilter) - (sDDSFreq + dDDSFreq) + 1.0) * SampleRate/2.0;

	int vfob = qRound(fvfob);
	int vfoa = qRound(fvfoa);
	bool vfo_ab = vfoa < vfob;

	QColor tx_color(254, 0, 0, 47);
	QColor tx_edge_color(255, 0, 0, 127);

	GLdouble z_tx_f = -0.38;
	GLdouble z_tx_c = -0.39;
	GLdouble z_rx2_f = -0.40;
	GLdouble z_rx2_c = -0.41;
	GLdouble z_rx1_f = -0.42;
	GLdouble z_rx1_c = -0.43;

	bool rx1_filter_is_tx = false;

	int freq = GetFilter();
	double band_tx_low = BandTxLow;
	double band_tx_high = BandTxHigh;

	if(Mode == CWL || Mode == CWU)
	{
		int pitch = GetPitch();
		band_tx_low = -10 + pitch;
		band_tx_high = 10 + pitch;
	}

	if(TxVfo == 0)
	{
		draw_filter_rect(freq, band_tx_low, band_tx_high, tx_color, true, tx_edge_color, z_tx_f);
	}
	else
	{
		GLdouble filter2 = sFilter2 + dFilter2;

		if((filter2 > 1.0 || filter2 < -1.0) && (TRxMode == TX))
		{
			draw_filter_rect(freq, band_tx_low, band_tx_high, tx_color, true, tx_edge_color, z_tx_f);
			rx1_filter_is_tx = true;
		}
		else
		{
			int freq2 = GetFilter2();
			draw_filter_rect(freq2, band_tx_low, band_tx_high, tx_color, true, tx_edge_color, z_tx_f);

			if(!IsFilter2)
			{
				draw_filter_center(freq2, Qt::red, Qt::transparent, z_tx_c);
				draw_filter_info(vfob, freq2, band_tx_low, band_tx_high, true, true, false, !vfo_ab, "B");
			}
		}
	}

	int rit = isRitOn ? getRitValue() : 0;

	QColor filter_col = pPanOpt->pColorFilter1->getColor();
	QColor filter_hi(Qt::white);
	filter_hi.setAlpha(AlphaColorCF * 255);

	draw_filter_center(freq + rit, filter_col, filter_hi, z_rx1_c);

	if(Mode == CWL || Mode == CWU || Mode == DIGU || Mode == DIGL)
	{
		QColor cw_col = pPanOpt->pColorPitch1->getColor();
		QColor cw_hi(Qt::white);
		cw_hi.setAlpha(AlphaColorPich);
		int pitch = GetPitch();
		draw_filter_center(freq + rit + pitch, cw_col, cw_hi, z_rx1_c);
	}

	double bflow, bfhigh;
	GetBandFilter(bflow, bfhigh);

	QColor edge_color(194, 252, 200, AlphaColor);
//	qDebug()<<"AlphaColor:"<<AlphaColor;
//	qDebug()<<"AlphaColor2:"<<AlphaColor2;
//	qDebug()<<"AlphaColorCF:"<<AlphaColorCF;
//	qDebug()<<"AlphaColorCF2:"<<AlphaColorCF2;
//	qDebug()<<"AlphaColorDbm:"<<AlphaColorDbm;
//	qDebug()<<"AlphaColorPich:"<<AlphaColorPich;
//	qDebug()<<"AlphaColorPich2:"<<AlphaColorPich2;
	
	QColor f_color = pPanOpt->pColorBandFilter1->getColor();
	f_color.setAlpha(pPanOpt->ui.hslTransparentFilter1->value());

	draw_filter_rect(freq + rit, bflow, bfhigh, f_color, IsDrawFilterEdges, edge_color, z_rx1_f);

	bool show_filter_info = IsVisibleInfo && IsWindow &&
			(
				(ActionObject == PRESS_LEFT_CF) ||
				(ActionObject == PRESS_LEFT_BF_LEFT) ||
				(ActionObject == PRESS_LEFT_BF_RIGHT) ||
				(LastPosObject == BAND_FILTER) ||
				(LastPosObject == LEFT_BAND) ||
				(LastPosObject == RIGHT_BAND)
				);

	bool rxtx_mark = IsFilter2 || (TxVfo == 1);
	bool is_tx = (TxVfo == 0) || rx1_filter_is_tx;
	draw_filter_info(vfoa, freq, bflow, bfhigh, rxtx_mark, is_tx, show_filter_info, vfo_ab, "A");


	if(IsFilter2)
	{
		int freq2 = GetFilter2();

		QColor filter2_col = pPanOpt->pColorFilter2->getColor();
		QColor filter2_hi(Qt::white);
		filter2_hi.setAlpha(AlphaColorCF2 * 255);

		draw_filter_center(freq2, filter2_col, filter2_hi, z_rx2_c);

		QColor rx2_edge_color(194, 252, 200, AlphaColor2);
		QColor rx2_color = pPanOpt->pColorBandFilter2->getColor();
		rx2_color.setAlpha(pPanOpt->ui.hslTransparentFilter2->value());

		double bflow, bfhigh;
		GetBandFilter(bflow, bfhigh);

		draw_filter_rect(freq2, bflow, bfhigh, rx2_color, IsDrawFilterEdges2, rx2_edge_color, z_rx2_f);

		if(Mode == CWL || Mode == CWU || Mode == DIGU || Mode == DIGL)
		{
			QColor cw2_col = pPanOpt->pColorPitch2->getColor();
			QColor cw2_hi(Qt::white);
			cw2_hi.setAlpha(AlphaColorPich2);
			int pitch = GetPitch();
			draw_filter_center(freq2 + pitch, cw2_col, cw2_hi, z_rx2_c);
		}

		bool show_filter_info = IsVisibleInfo && IsWindow &&
				(
					(ActionObject == PRESS_LEFT_CF2) ||
					(ActionObject == PRESS_LEFT_BF_LEFT) ||
					(ActionObject == PRESS_LEFT_BF_RIGHT) ||
					(LastPosObject == BAND_FILTER2) ||
					(LastPosObject == LEFT_BAND2) ||
					(LastPosObject == RIGHT_BAND2)
					);


		bool is_tx = (TxVfo == 1);
		draw_filter_info(vfob, freq2, bflow, bfhigh, true, is_tx, show_filter_info, !vfo_ab, "B");
	}
}

void Panarama::DrawSpectr()
{
	float tmpS = sScaleRuleX + dScaleRuleX;
	float tmp2 = sFilter+ dFilter-sPosZoomPan - dPosZoomPan;
	float tmpF = tmp2;
	float tmpL = -RIT_LIMIT*2.0f/SampleRate;
	if(isRitOn)
	{
		tmp2 += sRit + dRit;
		glPushMatrix();
		glTranslated(sPosZoomPan + dPosZoomPan, posRule, 0.0);
		glScaled(tmpS, 1.0 - (posRule), 1.0);
		glColor4f(1.0, 1.0, 1.0, 0.7);
		if(alphaRitLim <= 0.0f)
			band_font->draw(tmpL + 1.2*ox5 + tmpF, 1.0-oy20/(1.0 - (posRule)), -0.42, tr("RIT"));
		else
			info_font->draw(tmpL + 1.2*ox5 + tmpF, 1.0-oy20/(1.0 - (posRule)), -1.0, tr("Out of range!"));
		glColor4f(1.0, 1.0, 1.0, 0.2);
		glBegin(GL_QUADS);
			glVertex3f(tmpF-tmpL, 1.0, -0.05);
			glVertex3f(tmpF-tmpL, 0.0, -0.05);
			glVertex3f(tmpF+tmpL, 0.0, -0.05);
			glVertex3f(tmpF+tmpL, 1.0, -0.05);
		glEnd();
		glLineWidth(1.0);
		glColor4f(pPanOpt->pColorFilter1->getColor().red()/255.0, pPanOpt->pColorFilter1->getColor().green()/255.0, pPanOpt->pColorFilter1->getColor().blue()/255.0, alphaRitLim);
		glBegin(GL_LINES);
			glVertex3f(tmpF-tmpL, 1.0, -0.06);
			glVertex3f(tmpF-tmpL, 0.0, -0.06);
			glVertex3f(tmpF+tmpL, 0.0, -0.06);
			glVertex3f(tmpF+tmpL, 1.0, -0.06);
		glEnd();
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xAAAA);
		glColor4f(pPanOpt->pColorFilter1->getColor().red()/255.0, pPanOpt->pColorFilter1->getColor().green()/255.0, pPanOpt->pColorFilter1->getColor().blue()/255.0, 0.6f);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex3d(tmpF, 1.0, -0.06);
			glVertex3d(tmpF, 0.0, -0.06);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
		glPopMatrix();
	}
	Len = BeginX + 1;
	StepCount = ((int)(Len/Step));
	float Zero = sOffsetDbm + dOffsetDbm;
	float tmp1 = sPosZoomPan + dPosZoomPan - ox2_5/(sScaleRuleX + dScaleRuleX);
	if(pPanOpt->ui.cbPanType->currentIndex() == LINES)
	{
		GLdouble Begin = -1 + Step*StepCount;
		int arr_sz = 0;
		for(int j = StepCount, i = 0; Begin < EndX && j < SizeSpBuff-1; j++, i += 3, arr_sz += 2)
		{
			ArrayV[i] = Begin - (tmp1);
			ArrayV[i+1] = (pSpBuff[j] < Zero) ? Zero : pSpBuff[j];
			ArrayV[i+2] = -0.5f;

			ArrayV2[i] = ArrayV[i];
			ArrayV2[i+1] = ArrayV[i+1];
			ArrayV2[i+2] = -0.1f;

			i += 3;

			ArrayV[i] = Begin - (tmp1);
			ArrayV[i+1] = (pSpBuff[j] < Zero) ? Zero : pSpBuff[j];
			ArrayV[i+2] = -0.5f;

			ArrayV2[i] = ArrayV[i];
			ArrayV2[i+1] = Zero;
			ArrayV2[i+2] = -0.1f;

			Begin += Step;
		}

		glPushMatrix();
		glTranslated(tmp1, posRule, 0.0);
		glScaled(1.0, 1.0 - (posRule), 1.0);

		glScaled(sScaleRuleX + dScaleRuleX, 1.0/(sLenDbmY+dLenDbmY), 1.0);
		glTranslated(0.0, -Zero, 0.0);

		qglColor(ColorLine);
		glLineWidth(1);
		glVertexPointer(3, GL_FLOAT, 0, ArrayV);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_STRIP,0, arr_sz);
		glDisableClientState(GL_VERTEX_ARRAY);

		qglColor(ColorSp);
		glVertexPointer(3, GL_FLOAT, 0, ArrayV2);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_QUAD_STRIP,0, arr_sz+1);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glTranslated(tmp1, posRule, 0.0);
		glScaled(1.0, 1.0 - posRule, 1.0);
		glScaled(sScaleRuleX + dScaleRuleX, 1.0/(sLenDbmY+dLenDbmY), 1.0);
		glTranslated(0.0, -Zero, 0.0);
		GLdouble Begin = -1.0 + Step*StepCount;
		glBegin(GL_QUAD_STRIP);
			for(int j = StepCount; Begin < EndX && j < SizeSpBuff-1; j++)
			{
				qglColor(ColorTop);
				if(pSpBuff[j] < Zero)
				{
					glVertex3d(Begin - (tmp1), Zero, -0.1);
				}
				else
				{
					glVertex3d(Begin - (tmp1), pSpBuff[j], -0.1);
				}
				qglColor(ColorBot);
				glVertex3d(Begin - (tmp1), Zero, -0.1);
				Begin += Step;
			}
		glEnd();
		glPopMatrix();
	}
}

void Panarama::MeanTimer()
{
	for(int i = 0; i < BUFF_SIZE; i++)
	{
		float Tmp = 0;
		for(int j = 0; j < MeanBuff; j++)
			Tmp += pCircleBuff[j][i];
		pBuff[i] = Tmp/10;
	}
}

void Panarama::DrawDbm()
{
	glLineWidth(1.0);
	glColor4d(0.5, 0.5, 0.5, AlphaColorDbm);
	glBegin(GL_QUADS);
	glVertex3d(-1.0, 1.0, -0.55);
	glVertex3d(-1.0, posRule, -0.55);
	glVertex3d(-1.0 + ox30 + ox5, posRule, -0.55);
	glVertex3d(-1.0 + ox30 + ox5, 1.0, -0.55);
	glEnd();

	glPushMatrix();
	glTranslated(0.0, posRule, 0.0);
	glScaled(1.0, 1.0 - (posRule), 1.0);
	glScaled(1.0, 1.0/SaveLenDbmY, 1.0);
	glTranslated(0.0, -SaveOffsetDbm, 0.0);
	int x = (-200-SaveOffsetDbm)/StepDbm;
	--x *= StepDbm;
	if(pPanOpt->ui.comboBox_5->currentIndex() == 1)
	{
		glLineStipple(1,0xAAAA);
		glEnable(GL_LINE_STIPPLE);
	}
	if(pPanOpt->ui.chbGridOn->isChecked())
	{
		glBegin(GL_LINES);
		for(float i = -200 - x; i < (SaveOffsetDbm + SaveLenDbmY); i += StepDbm)
		{
			if(((int)i)%10 == 0)
				glColor4f(1.0, 1.0, 1.0, pPanOpt->ui.horizontalSlider->value()/255.0);
			else if(((int)i)%5 == 0)
				glColor4f(1.0, 1.0, 1.0, pPanOpt->ui.horizontalSlider->value()/255.0 - 0.03);
			else if(((int)i)%2 == 0)
				glColor4f(1.0, 1.0, 1.0, pPanOpt->ui.horizontalSlider->value()/255.0 - 0.06);
			else
				glColor4f(1.0, 1.0, 1.0, pPanOpt->ui.horizontalSlider->value()/255.0 - 0.06);

			glVertex3d(-1.0 + ox30 + ox5, i, -0.2);
			glVertex3d( 1.0, i, -0.2);

		}
	}
	glLineStipple(1,0xFFFF);
	glDisable(GL_LINE_STIPPLE);
	glEnd();
	int Num = -200 - x;
	QString str;
	GLfloat numH = (dbm_font->height()*3.2/height()*(1.0/(1.0 - posRule))*(sLenDbmY+dLenDbmY))*0.15f;
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for(float i = Num; i < (SaveOffsetDbm + SaveLenDbmY); i += StepDbm)
	{
		str.setNum(Num);
		if(Num <= -100)
			dbm_font->draw(-1.0 +  2*ox1, i-numH, -0.9, str);
		else if(Num <= -10)
			dbm_font->draw(-1.0 +  8*ox1, i-numH, -0.9, str);
		else if(Num < 0)
			dbm_font->draw(-1.0 + 14*ox1, i-numH, -0.9, str);
		else if(Num < 10)
			dbm_font->draw(-1.0 + 19*ox1, i-numH, -0.9, str);
		else if(Num < 100)
			dbm_font->draw(-1.0 + 13*ox1, i-numH, -0.9, str);
		else
			dbm_font->draw(-1.0 +  7*ox1, i-numH, -0.9, str);

		glLineWidth(1.2);
		glBegin(GL_LINES);
			glVertex3d(-1.0 + ox20 + ox5 + ox2_5, i, -0.9);
			glVertex3d(-1.0 + ox20 + ox10+ ox2_5, i, -0.9);
		glEnd();
		Num += StepDbm;
	}
	glPopMatrix();
}

void Panarama::DrawZoomer()
{
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2);
	qglColor(Qt::darkCyan);
	glBegin(GL_TRIANGLES);
		glVertex3f(sPosZoomPan + dPosZoomPan -ox10, posRule-1.5*oy10,  -0.7);
		glVertex3f(sPosZoomPan + dPosZoomPan +ox10, posRule-1.5*oy10,  -0.7);
		glVertex3f(sPosZoomPan + dPosZoomPan, posRule, -0.7);
	glEnd();
	qglColor(Qt::white);
	glBegin(GL_LINE_LOOP);
		glVertex3f(sPosZoomPan + dPosZoomPan -ox10, posRule-1.5*oy10,  -0.8);
		glVertex3f(sPosZoomPan + dPosZoomPan +ox10, posRule-1.5*oy10,  -0.8);
		glVertex3f(sPosZoomPan + dPosZoomPan, posRule, -0.8);
	glEnd();
	glDisable(GL_LINE_SMOOTH);
	glLineWidth(1);
}

void Panarama::DrawCursor()
{
	glLineWidth(1.0);
	double Num = 0;
	QString StrNum;
	if(ActionObject == UNPRESSED)
	{
		switch(LastPosObject)
		{
		case RULE:
			setCursor(Qt::PointingHandCursor);
			break;
		case DB_PANEL:
			setCursor(Qt::OpenHandCursor);
			if(!TimerDbmPos.isActive())
				TimerDbmPos.start(PAN_UPDATE);
			break;
		case GRID:
			setCursor(Qt::BlankCursor);
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
					Num = ((MovePosX - sPosZoomPan - dPosZoomPan)*SampleRate/2.0)/(sScaleRuleX + dScaleRuleX) - (sDDSFreq + dDDSFreq - sPosZoomPan - dPosZoomPan)*SampleRate/2.0 + SampleRate/2.0;
                    StrNum = freqToStr(Num);

					cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, 0, "VFO: "+StrNum+" Hz", Qt::white);
					Num = (MovePosY-(posRule))*(sLenDbmY+dLenDbmY)/(1.0 - (posRule)) + (sOffsetDbm+dOffsetDbm);
                    StrNum = QString("%1").arg(Num, 0, 'f', 1);
					cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY - 15*ScaleWindowY, 0, StrNum + " dBm", Qt::white);
				}
			}
			glColor4d(1.0, 1.0, 1.0, 0.3);
			glBegin(GL_LINES);
				if(Cursor == VERTICAL || Cursor == CROSS)
				{
					glVertex3f(MovePosX,  1.0, -0.7);
					glVertex3f(MovePosX, -1.0, -0.7);
				}
				if(Cursor == HORIZONTAL || Cursor == CROSS)
				{
					glVertex3f(-1.0,MovePosY, -0.7);
					glVertex3f( 1.0,MovePosY, -0.7);
				}
				glColor4d(1.0, 1.0, 1.0, 1.0);
				if(IsFilter2)
				{
					if(IsCrossF1)
						qglColor(pPanOpt->pColorFilter1->getColor());
					else
						qglColor(pPanOpt->pColorFilter2->getColor());

				}
				glVertex3f(MovePosX, MovePosY + oy10*1.5, -0.71);
				glVertex3f(MovePosX, MovePosY - oy10*1.5, -0.71);
				glVertex3f(MovePosX - ox10*1.5, MovePosY, -0.71);
				glVertex3f(MovePosX + ox10*1.5, MovePosY, -0.71);
			glEnd();
			break;
		case PITCH:
			if(!TimerPichPos.isActive())
				TimerPichPos.start(PAN_UPDATE);
		case MAIN_FILTER:
			if(!TimerFilterPos.isActive())
				TimerFilterPos.start(PAN_UPDATE);

			if(IsVisibleInfo)
			{
				if(!IsWindow)
				{
                    Num = ((sFilter + dFilter)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, "Fiter: "+StrNum+" Hz", Qt::white);
				}
			}

			setCursor(Qt::SplitHCursor);
			break;
		case PITCH2:
			if(!TimerPichPos2.isActive())
				TimerPichPos2.start(PAN_UPDATE);
		case MAIN_FILTER2:
			if(!TimerFilter2Pos.isActive())
				TimerFilter2Pos.start(PAN_UPDATE);
			if(IsVisibleInfo)
			{
				if(!IsWindow)
				{
                    Num = ((sFilter2 + dFilter2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, "Fiter: "+StrNum+" Hz", ColorFilter2);
				}
			}
			setCursor(Qt::SplitHCursor);
			break;
		case BAND_FILTER:
			if(!TimerDrawFilterEdges.isActive())
			{
				TimerDrawFilterEdges.start(PAN_UPDATE);
				IsDrawFilterEdges = true;
			}
			setCursor(Qt::SplitHCursor);
			break;
		case BAND_FILTER2:
			if(!TimerDrawFilterEdges2.isActive())
			{
				TimerDrawFilterEdges2.start(PAN_UPDATE);
				IsDrawFilterEdges2 = true;
			}
			setCursor(Qt::SplitHCursor);
			break;
		case LEFT_BAND:
        case RIGHT_BAND:
            setCursor(Qt::SizeHorCursor);
            if(!TimerDrawFilterEdges.isActive())
            {
                TimerDrawFilterEdges.start(PAN_UPDATE);
                IsDrawFilterEdges = true;
            }
			break;
		case LEFT_BAND2:
		case RIGHT_BAND2:
			setCursor(Qt::SizeHorCursor);
			if(!TimerDrawFilterEdges2.isActive())
			{
				TimerDrawFilterEdges2.start(PAN_UPDATE);
				IsDrawFilterEdges2 = true;
			}
			break;
		case WATERFALL:
			setCursor(Qt::BlankCursor);
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
					Num = ((MovePosX - sPosZoomPan - dPosZoomPan)*SampleRate/2.0)/(sScaleRuleX + dScaleRuleX) - (sDDSFreq + dDDSFreq - sPosZoomPan - dPosZoomPan)*SampleRate/2.0 + SampleRate/2.0;
                    StrNum = freqToStr(Num);
					cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, 0, "VFO: "+StrNum+" Hz", Qt::white);
				}
			}

			glColor4d(1.0, 1.0, 1.0, 0.3);
			glBegin(GL_LINES);
				if(Cursor == VERTICAL || Cursor == CROSS)
				{
					glVertex3f(MovePosX,  1.0, -0.7);
					glVertex3f(MovePosX, -1.0, -0.7);
				}
				if(Cursor == HORIZONTAL || Cursor == CROSS)
				{
					glVertex3f(-1.0,MovePosY, -0.7);
					glVertex3f( 1.0,MovePosY, -0.7);
				}
				glColor4d(1.0, 1.0, 1.0, 1.0);
				if(IsFilter2)
				{
					if(IsCrossF1)
						qglColor(pPanOpt->pColorFilter1->getColor());
					else
						qglColor(pPanOpt->pColorFilter2->getColor());

				}
				glVertex3f(MovePosX, MovePosY + oy10*1.5, -0.71);
				glVertex3f(MovePosX, MovePosY - oy10*1.5, -0.71);
				glVertex3f(MovePosX - ox10*1.5, MovePosY, -0.71);
				glVertex3f(MovePosX + ox10*1.5, MovePosY, -0.71);
				glEnd();
			break;
		case ZOOMER:
			setCursor(Qt::PointingHandCursor);
			break;
		default:
			setCursor(Qt::BlankCursor);
			break;
		}
	}
	else
	{
		switch(ActionObject)
		{
		case PRESS_LEFT_RULE:
			setCursor(Qt::ClosedHandCursor);
			break;
		case PRESS_RIGHT_RULE:
			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_LEFT_DBM:
			setCursor(Qt::ClosedHandCursor);
			break;
		case PRESS_RIGHT_DBM:
			setCursor(Qt::SplitVCursor);
			break;
		case PRESS_LEFT_GRID:
			setCursor(Qt::BlankCursor);
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
					Num = ((MovePosX - sPosZoomPan - dPosZoomPan)*SampleRate/2.0)/(sScaleRuleX + dScaleRuleX) - (sDDSFreq + dDDSFreq - sPosZoomPan - dPosZoomPan)*SampleRate/2.0 + SampleRate/2.0;
                    StrNum = freqToStr(Num);
					cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, 0, "VFO: "+StrNum+" Hz", Qt::white);
					if(MovePosY > (posRule))
					{
						Num = (MovePosY-(posRule))*(sLenDbmY+dLenDbmY)/(1.0 - (posRule)) + (sOffsetDbm+dOffsetDbm);
						StrNum = QString("%1").arg(Num, 0, 'f', 1);
						cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY - 15*ScaleWindowY, 0, StrNum + " dBm", Qt::white);
					}
				}
			}
			glColor4d(1.0, 1.0, 1.0, 0.3);
			glBegin(GL_LINES);
				if(Cursor == VERTICAL || Cursor == CROSS)
				{
					glVertex3f(MovePosX,  1.0, -0.7);
					glVertex3f(MovePosX, -1.0, -0.7);
				}
				if(Cursor == HORIZONTAL || Cursor == CROSS)
				{
					glVertex3f(-1.0,MovePosY, -0.7);
					glVertex3f( 1.0,MovePosY, -0.7);
				}
				glColor4d(1.0, 1.0, 1.0, 1.0);
				if(IsFilter2)
				{
					if(IsCrossF1)
						qglColor(pPanOpt->pColorFilter1->getColor());
					else
						qglColor(pPanOpt->pColorFilter2->getColor());

				}
				glVertex3f(MovePosX, MovePosY + oy10*1.5, -0.71);
				glVertex3f(MovePosX, MovePosY - oy10*1.5, -0.71);
				glVertex3f(MovePosX - ox10*1.5, MovePosY, -0.71);
				glVertex3f(MovePosX + ox10*1.5, MovePosY, -0.71);
			glEnd();
			break;
		case PRESS_RIGHT_GRID:
			setCursor(Qt::BlankCursor);
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
					Num = ((MovePosX - sPosZoomPan - dPosZoomPan)*SampleRate/2.0)/(sScaleRuleX + dScaleRuleX) - (sDDSFreq + dDDSFreq - sPosZoomPan - dPosZoomPan)*SampleRate/2.0 + SampleRate/2.0;
                    StrNum = freqToStr(Num);
					cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, 0, "VFO: "+StrNum+" Hz", Qt::white);
					if(MovePosY > (posRule))
					{
						Num = (MovePosY-(posRule))*(sLenDbmY+dLenDbmY)/(1.0 - (posRule)) + (sOffsetDbm+dOffsetDbm);
						StrNum = QString("%1").arg(Num, 0, 'f', 1);
						cursor_font->draw(MovePosX + 10*ScaleWindowX, MovePosY - 15*ScaleWindowY, 0, StrNum + " dBm", Qt::white);
					}
				}
			}
			glColor4d(1.0, 1.0, 1.0, 0.3);
			glBegin(GL_LINES);
				if(Cursor == VERTICAL || Cursor == CROSS)
				{
					glVertex3f(MovePosX,  1.0, -0.7);
					glVertex3f(MovePosX, -1.0, -0.7);
				}
				if(Cursor == HORIZONTAL || Cursor == CROSS)
				{
					glVertex3f(-1.0,MovePosY, -0.7);
					glVertex3f( 1.0,MovePosY, -0.7);
				}
				glColor4d(1.0, 1.0, 1.0, 1.0);
				if(IsFilter2)
				{
					if(IsCrossF1)
						qglColor(pPanOpt->pColorFilter1->getColor());
					else
						qglColor(pPanOpt->pColorFilter2->getColor());
				}
				glVertex3f(MovePosX, MovePosY + oy10*1.5, -0.71);
				glVertex3f(MovePosX, MovePosY - oy10*1.5, -0.71);
				glVertex3f(MovePosX - ox10*1.5, MovePosY, -0.71);
				glVertex3f(MovePosX + ox10*1.5, MovePosY, -0.71);
			glEnd();
			break;
		case PRESS_LEFT_CF:
		case PRESS_LEFT_CF2:
			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_RIGHT_CF:
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
                    Num = ((sFilter + dFilter)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, "Fiter: "+StrNum+" Hz", Qt::white);
				}
			}

			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_RIGHT_CF2:
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
                    Num = ((sFilter2 + dFilter2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 15*ScaleWindowY, "Fiter2: "+StrNum+" Hz", Qt::white);
				}
			}

			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_LEFT_BF:
		case PRESS_LEFT_BF2:
			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_RIGHT_BF:
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
                    Num = ((sFilter + dFilter)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 1.5*oy10, "Fiter: "+StrNum+" Hz", Qt::white);

                    Num = ((sBandPassHigh + dBandPassHigh)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY - 1.5*oy10, "High: "+StrNum+" Hz", Qt::white);

                    Num = ((sBandPassLow + dBandPassLow)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY - oy30, "Low: "+StrNum+" Hz", Qt::white);
				}
			}
			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_RIGHT_BF2:
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
                    Num = ((sFilter2 + dFilter2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 1.5*oy10, "Fiter2: "+StrNum+" Hz", Qt::white);

                    Num = ((sBandPassHigh2 + dBandPassHigh2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY - 1.5*oy10, "High: "+StrNum+" Hz", Qt::white);

                    Num = ((sBandPassLow2 + dBandPassLow2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY - oy30, "Low: "+StrNum+" Hz", Qt::white);
				}
			}
			setCursor(Qt::SplitHCursor);
			break;
		case PRESS_LEFT_BF_LEFT:
        case PRESS_LEFT_BF_RIGHT:
            setCursor(Qt::SizeHorCursor);
            break;
		case PRESS_LEFT_BF_LEFT2:
			setCursor(Qt::SizeHorCursor);
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
                    Num = ((sBandPassHigh2 + dBandPassHigh2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 1.5*oy10, "High: "+StrNum+" Hz", Qt::white);

                    Num = ((sBandPassLow2 + dBandPassLow2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY - 1.5*oy10, "Low: "+StrNum+" Hz", Qt::white);
				}
			}
			break;
		case PRESS_LEFT_BF_RIGHT2:
			setCursor(Qt::SizeHorCursor);
			if(IsVisibleInfo)
			{
				if(IsWindow)
				{
                    Num = ((sBandPassHigh2 + dBandPassHigh2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY + 1.5*oy10, "High: "+StrNum+" Hz", Qt::white);

                    Num = ((sBandPassLow2 + dBandPassLow2)*SampleRate/2.0);
                    StrNum = freqToStr(Num);
                    DrawInfo(MovePosX + 10*ScaleWindowX, MovePosY - 1.5*oy10, "Low: "+StrNum+" Hz", Qt::white);
				}
			}
			break;
		case ZOOMER_PRESS:
			setCursor(Qt::PointingHandCursor);
			break;
		default:
			break;
		}
	}
}

void Panarama::SetStepGrid()
{
	for(int i = 0; i < STEP_GRID_DDS_SIZE; i++)
	{
		if(((sScaleRuleX + dScaleRuleX)*StepGridDDSHz[i]*width()/SampleRate) >= STEP_FREQ_PIX_WIDTH)
		{
			GridStep = (double)StepGridDDSHz[i]*2.0/SampleRate;
			StepNum = StepGridDDSHz[i]/1000000.0;
			if(StepGridDDSHz[i] >= 1000)
				PrecisionNum = 3;
			else if(StepGridDDSHz[i] >= 100)
				PrecisionNum = 4;
			else if(StepGridDDSHz[i] >= 10)
				PrecisionNum = 5;
			else
				PrecisionNum = 6;

			break;
		}
	}
}

void Panarama::SetStepDbm()
{
	int font_height;
	if(dbm_font)
	{
		font_height = dbm_font->height();
	}
	else
	{
		QFontMetrics fm(this->font());
		font_height = fm.height();
	}


	float glFontH = (font_height*4.0f/height())*(1.0/(1.0 - posRule))*(sLenDbmY + dLenDbmY);
	float glMinStepDb = 1.2f*glFontH;

	for(int i = 0; i < (STEP_DB_SIZE-1); i++)
	{
		if((glMinStepDb > StepDb[i]) && (glMinStepDb < StepDb[i+1]))
		{
			stepDb = StepDb[i+1];
			break;
		}
	}
	StepDbm = stepDb;
}

void Panarama::FilterEdges()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == LEFT_BAND || Obj == RIGHT_BAND)
	{
		if(AlphaColor < 200)
		{
			AlphaColor += 25;
			isUpdate = true;
		}
	}
	else
	{
		if(IsPressFilterEdges)
		{
			;
		}
		else if(AlphaColor > 0)
		{
			AlphaColor -= 25;
			if(AlphaColor < 0)
				AlphaColor = 0;
			isUpdate = true;
		}
		else
		{
			IsDrawFilterEdges = false;
			TimerDrawFilterEdges.stop();
			isUpdate = true;
		}
	}
}

void Panarama::FilterEdges2()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == LEFT_BAND2 || Obj == RIGHT_BAND2)
	{
		if(AlphaColor2 < 200)
		{
			AlphaColor2 += 25;
			isUpdate = true;
		}
	}
	else
	{
		if(IsPressFilterEdges2)
		{
			;
		}
		else if(AlphaColor2 > 0)
		{
			AlphaColor2 -= 25;
			if(AlphaColor2 < 0)
				AlphaColor2 = 0;
			isUpdate = true;
		}
		else
		{
			IsDrawFilterEdges2 = false;
			TimerDrawFilterEdges2.stop();
			isUpdate = true;
		}
	}
}

void Panarama::DbmPos()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == DB_PANEL && !IsPressFilterEdges)
	{
		if(AlphaColorDbm < 0.7)
		{
			AlphaColorDbm += 0.05;
			isUpdate = true;
		}
	}
	else
	{
		if(AlphaColorDbm > 0.4)
			AlphaColorDbm -= 0.05;
		else
			TimerDbmPos.stop();
		isUpdate = true;
	}
}

void Panarama::FilterPos()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == MAIN_FILTER && !IsPressFilterEdges)
	{
		if(AlphaColorCF < 0.7)
		{
			AlphaColorCF += 0.09;
			isUpdate = true;
		}
	}
	else
	{
        if(AlphaColorCF > 0.009)
		{
			AlphaColorCF -= 0.09;
			isUpdate = true;
		}
		else
			TimerFilterPos.stop();
	}
}

void Panarama::Filter2Pos()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == MAIN_FILTER2 && !IsPressFilterEdges2)
	{
		if(AlphaColorCF2 < 0.8)
		{
			AlphaColorCF2 += 0.09;
			isUpdate = true;
		}
	}
	else
	{
		if(AlphaColorCF2 > 0.0)
		{
			AlphaColorCF2 -= 0.09;
			isUpdate = true;
		}
		else
			TimerFilter2Pos.stop();
	}
}

void Panarama::PichPos()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == PITCH && !IsPressFilterEdges)
	{
		if(AlphaColorPich < 200)
		{
			AlphaColorPich += 25;
			isUpdate = true;
		}
	}
	else
	{
		if(AlphaColorPich > 0)
		{
			AlphaColorPich -= 25;
			if(AlphaColorPich < 0)
				AlphaColorPich = 0;
			isUpdate = true;
		}
		else
			TimerPichPos.stop();
	}
}

void Panarama::PichPos2()
{
	OBJECTS Obj = LastPosObject;
	if(Obj == PITCH2 && !IsPressFilterEdges2)
	{
		if(AlphaColorPich2 < 200)
		{
			AlphaColorPich2 += 25;
			isUpdate = true;
		}
	}
	else
	{
		if(AlphaColorPich2 > 0)
		{
			AlphaColorPich2 -= 25;
			if(AlphaColorPich2 < 0)
				AlphaColorPich2 = 0;
			isUpdate = true;
		}
		else
			TimerPichPos2.stop();
	}
}

void Panarama::DrawInfo(GLdouble X, GLdouble Y, QString Str, QColor color)
{
	glPushMatrix();
	glTranslated(X, Y, 0.0);
	info_font->draw(0, -4*ScaleWindowY, -0.75, Str, color);
	glPopMatrix();
}

void Panarama::DrawWaterfall()
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	MakeSubTexture();
	GLdouble Zero = 0.0;
	GLdouble LenH = -IMAGE_LEN*ScaleWindowY;
	GLdouble dx = -1.0 - (sPosZoomPan + dPosZoomPan);
	GLdouble OffsetWf = (IMAGE_LEN - IndxSubTexture - ((double)CntLineSpeed)/LineSpeedMax)*ScaleWindowY;
	glColor4f(1,1,1,1);
	glTranslatef(sPosZoomPan + dPosZoomPan - ox1, posRule - (RULE_HEIGH-1)*ScaleWindowY, 0.0);
	glScaled(sScaleRuleX + dScaleRuleX, 1.15, 1.0);
	glTranslatef(0.0, OffsetWf, 0.0);
	glPushMatrix();

	// update textures
	GLubyte *pLine = (GLubyte *)pNewLineImage;

	for(int i = 0; i < WF_TEX_W_CNT; i++)
	{
		glBindTexture(GL_TEXTURE_2D, WFTextures[NumLineTextures][i]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, IndxSubTexture, IMAGE_LEN, 1, GL_RGBA, GL_UNSIGNED_BYTE, pLine);
		pLine += IMAGE_LEN * NUM_COLOR;
	}

	// draw quads
	for(int i = 0; i < WF_TEX_H_CNT; i++)
	{
		int num_line_not_negative = NumLineTextures + WF_TEX_H_CNT;
		int row = (-i + num_line_not_negative) % WF_TEX_H_CNT;

		GLdouble x = dx;
		GLdouble tex_w = 2.0/WF_TEX_W_CNT;

		for(int col = 0; col < WF_TEX_W_CNT; col++)
		{
			glBindTexture(GL_TEXTURE_2D, WFTextures[row][col]);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex3f(x, LenH, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(x, Zero, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(x + tex_w, Zero, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(x + tex_w, LenH, 0.0);
			glEnd();

			x += tex_w;
		}
		glTranslated(0.0, -IMAGE_LEN*ScaleWindowY, 0.0);
	}

	glPopMatrix();


	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	if(!IsStart)
		return;
	if((IsMoveUpdate && ((TrxCnt>TX_TO_RX_TIME) || !pPanOpt->ui.chbStopWhenTx->isChecked())  && (TRxMode==RX)) || ((pPanOpt->ui.chbStopWhenTx->isChecked() == false) && (TRxMode==TX)))
	{
		if(++CntLineSpeed >= LineSpeedMax)
		{
			CntLineSpeed = 0;

			IndxSubTexture++;
			if(IndxSubTexture >= IMAGE_LEN)
				IndxSubTexture = 0;

			CntLinesTexture++;
			if(CntLinesTexture >= IMAGE_LEN * WF_TEX_H_CNT)
				CntLinesTexture = 0;

			NumLineTextures = CntLinesTexture / IMAGE_LEN;
		}
	}
}

void Panarama::UpdateLevels()
{
	if(--last_update > 0)
		return;

	last_update = 20;

	waterfall_mode = pPanOpt->ui.cbWfMode->currentIndex();

	AUTO_LEVEL lvl;

	if(!pPanOpt->ui.chbStopWhenTx->isChecked() && TRxMode==TX)
		lvl = MULTIHAND;
	else
		lvl = (AUTO_LEVEL)pPanOpt->ui.cbAutoLevel->currentIndex();

	switch(lvl)
	{
	case WATERFALL_AUTO:
	case ALL_AUTO:
		level_max = HighDb + pPanOpt->ui.sbWfMaxOffset->value();
		level_min = LowDb + pPanOpt->ui.sbWfMinOffset->value();
		break;
	case MULTIHAND:
	default:
		level_max = SaveOffsetDbm + SaveLenDbmY;
		level_min = SaveOffsetDbm;
		break;
	}

	if((level_max - level_min) < 30)
		level_max = level_min + 30;

	level_range = level_max - level_min;
	level_multiplier = levels_count / level_range;
}

inline void Panarama::GetRGB(float Value, GLubyte *red, GLubyte *green, GLubyte *blue)
{	
	const int color_max = 255;

	if(waterfall_mode == 1)
	{
		Value -= level_min;

		if(Value < 0)
		{
			*red = 0;
			*green = 0;
			*blue = 0;
			return;
		}

		int hue256x6;

		if(Value > level_range)
			hue256x6 = levels_count;
		else
			hue256x6 = Value * level_multiplier;

		int reason = hue256x6 / 256;
		int X = hue256x6 % 256;

		switch (reason){
		case 0:
			*red = 0;
			*green = 0;
			*blue = X;
			break;
		case 1:
			*red = 0;
			*green = X;
			*blue = color_max;
			break;
		case 2:
			*red = 0;
			*green = color_max;
			*blue = color_max - X;
			break;
		case 3:
			*red = X;
			*green = color_max;
			*blue = 0;
			break;
		case 4:
			*red = color_max;
			*green = color_max - X;
			*blue = 0;
			break;
		case 5:
		default:
			*red = color_max;
			*green = 0;
			*blue = color_max - X;
			break;
		}
		return;
	}
	else
	{
		Value -= level_min;

		if(Value < 0)
		{
			*red = 0;
			*green = 0;
			*blue = 0;
			return;
		}

		QColor HSV = pPanOpt->pColorWFgrad->getColor();
		int h,s,v;
		HSV.getHsv(&h,&s,&v);

		int lvl;

		if(Value > level_range)
			lvl = levels_count;
		else
			lvl = Value * level_multiplier;

		lvl = lvl / 3;

		if(lvl < 256)
			HSV.setHsv(h, 255, lvl);
		else
		{
			int col = (lvl - 256);
			HSV.setHsv(h, col, 255);
		}
		int r, g, b;

		HSV.getRgb(&r, &g, &b);
		*red = r;
		*green = g;
		*blue = b;
		return;
	}
}

void Panarama::MakeSubTexture()
{
	for (int j = 0; j < IMAGE_LEN * 8; j++)
	{
		GetRGB(BufWf[j], &pNewLineImage[j][0], &pNewLineImage[j][1], &pNewLineImage[j][2]);
		pNewLineImage[j][3]=255;
	}
}

void Panarama::AutosetDbm()
{
	if(pPanOpt->ui.chbStopWhenTx->isChecked())
		if(TrxCnt < TX_TO_RX_TIME)
			return;

	if(!IsMoveUpdate)
		return;

	if(TRxMode == TX)
		return;

	if(!IsStart)
		return;

	MeanMin[CntMeaneBuff] = GetMin(BufWf, BUFF_SIZE);
	MeanMax[CntMeaneBuff] = GetMax(BufWf, BUFF_SIZE);
	if(++CntMeaneBuff >= NUM_MEANE_BUFF)
		CntMeaneBuff = 0;
	if(++CntBegin < NUM_MEANE_BUFF)
		CntBegin = NUM_MEANE_BUFF;

	float min;
	float max;
	if(CntBegin < NUM_MEANE_BUFF)
	{
		min = GetMean(MeanMin, CntBegin);
		max = GetMean(MeanMax, CntBegin);
	}
	else
	{
		min = GetMean(MeanMin, NUM_MEANE_BUFF);
		max = GetMean(MeanMax, NUM_MEANE_BUFF);
	}

	int hyst = pPanOpt->ui.sbHysteresis->value();
	if(min > (LowDb + hyst))
		LowDb++;
	if(min < (LowDb - hyst))
		LowDb--;
	if(max > (HighDb + hyst))
		HighDb++;
	if(max < (HighDb - hyst))
		HighDb--;

	if(pPanOpt->ui.cbAutoLevel->currentIndex() == ALL_AUTO)
	{
		if((LowDb + pPanOpt->ui.sbSpMinOffset->value()) < (int)SaveOffsetDbm)
		{
			sOffsetDbm -= 0.5;
			SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
			SetStepDbm();
			isUpdate = true;
		}
		else if((LowDb+pPanOpt->ui.sbSpMinOffset->value()) > (int)SaveOffsetDbm)
		{
			sOffsetDbm += 0.5;
			SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
			SetStepDbm();
			isUpdate = true;
		}
		if((HighDb - LowDb + pPanOpt->ui.sbSpMaxOffset->value() - pPanOpt->ui.sbSpMinOffset->value()) < (int)SaveLenDbmY)
		{
			sLenDbmY -= 0.5;
			SaveLenDbmY = sLenDbmY + dLenDbmY;
			SetStepDbm();
			isUpdate = true;
		}
		else if((HighDb - LowDb + pPanOpt->ui.sbSpMaxOffset->value() - pPanOpt->ui.sbSpMinOffset->value()) > (int)SaveLenDbmY)
		{
			sLenDbmY += 0.5;
			SaveLenDbmY = sLenDbmY + dLenDbmY;
			SetStepDbm();
			isUpdate = true;
		}
	}
	if(pPanOpt->ui.cbAutoLevel->currentIndex() == MULTIHAND)
	{
		IsChangedDbmLevel = false;
		TimerAutosetDbm.stop();
	}
}

float Panarama::GetMin(float *pBuff, int Size)
{
	float tmp = pBuff[0];
	for(int i = 1; i < Size; i++)
	{
		if(tmp > pBuff[i])
			tmp = pBuff[i];
	}
	return tmp;
}
float Panarama::GetMax(float *pBuff, int Size)
{
	float tmp = pBuff[0];
	for(int i = 1; i < Size; i++)
	{
		if(tmp < pBuff[i])
			tmp = pBuff[i];
	}
	return tmp;
}

float Panarama::GetMean(float *pBuff, int Size)
{
	float tmp = 0.0f;
	for(int i = 1; i < Size; i++)
		tmp += pBuff[i];
	tmp /= Size;
	return tmp;
}

void Panarama::SetPowerComp(int Index)
{
	TimerMeanBuff.stop();
	killTimer(TimerId);
	switch(Index)
	{
	case 0:
		fps = 60;
		TimerMeanBuff.start(60);
		TimerId = startTimer(60);
		emit changedFps(60);
		break;
	case 1:
		fps = 30;
		TimerMeanBuff.start(30);
		TimerId = startTimer(30);
		emit changedFps(30);
		break;
	case 2:
		fps = 17;
		TimerMeanBuff.start(17);
		TimerId = startTimer(17);
		emit changedFps(17);
		break;
	default:
		break;
	}
}

void Panarama::ChangedAlphaColorSp(int Val)
{
	isUpdate = true;
	int r,g,b,a;
	pPanOpt->pColorGradTop->getColor().getRgb(&r, &g, &b, &a);
	a = pPanOpt->ui.slSpGradSaturation->value();
	ColorTop.setRgb(r,g,b,a);

	pPanOpt->pColorGradBot->getColor().getRgb(&r, &g, &b, &a);
	a = pPanOpt->ui.slSpGradBrightness->value();
	ColorBot.setRgb(r,g,b,a);

	pPanOpt->pColorLineLines->getColor().getRgb(&r, &g, &b, &a);
	a = pPanOpt->ui.slTrancpSpectrum->value();
	ColorLine.setRgb(r,g,b,a);

	pPanOpt->pColorLineSpectrum->getColor().getRgb(&r, &g, &b, &a);
	a = pPanOpt->ui.slTrancpTop->value();
	ColorSp.setRgb(r,g,b,a);
}

void Panarama::SetTypeAutomatic(int Type)
{
	if(Type != MULTIHAND && IsMoveUpdate)
	{
		;
	}
	else
		TimerAutosetDbm.stop();
}

void Panarama::SetSpectrumBuffer(float *pBuffer, int Size)
{
	isUpdate = true;
	int Offset = (int)((sFilter+dFilter) * 2048.0f);
	int Offset2 = (int)((sFilter2+dFilter2) * 2048.0f);
	for(int i = 0; i < BUFF_SIZE; i++)
	{
		if(TRxMode == TX)
		{
			if(TxVfo == 0)
			{
				pCircleBuff[CircleCnt][i] = pBuffer[(i-Offset)&0x0FFF] + CalibrateLevel;
			}
			else if(TxVfo == 1 && IsFilter2)
			{
				pCircleBuff[CircleCnt][i] = pBuffer[(i-Offset2)&0x0FFF] + CalibrateLevel;
			}
			else if(TxVfo == 1 && !IsFilter2 && (sFilter2 <= 1.0 && sFilter2 >= -1.0))
			{
				pCircleBuff[CircleCnt][i] = pBuffer[(i-Offset2)&0x0FFF] + CalibrateLevel;
			}
			else if(TxVfo == 1 && !IsFilter2 && (sFilter2 > 1.0 || sFilter2 < -1.0))
			{
				pCircleBuff[CircleCnt][i] = pBuffer[(i-Offset)&0x0FFF] + CalibrateLevel;
			}
			else if(TxVfo != 0 && (sFilter2 > 1.0 || sFilter2 < -1.0))
			{
				pCircleBuff[CircleCnt][i] = pBuffer[(i-Offset)&0x0FFF] + CalibrateLevel;
			}
			else
				pCircleBuff[CircleCnt][i] = pBuffer[(i-Offset)&0x0FFF] + CalibrateLevel;
		}
		else
			pCircleBuff[CircleCnt][i] = pBuffer[i] + CalibrateLevel;

		if(TRxMode == TX)
			continue;
	}

	if(++CntUpdateBuff > CIRCLE_BUFF_SIZE)
		CntUpdateBuff = CIRCLE_BUFF_SIZE;

	if(!IsChangedFilter)
	{
		memset(pBuff, 0, BUFF_SIZE*sizeof(float));
		for(int i = 0; i < BUFF_SIZE; i++)
		{
			for(int j = 0; j < MeanBuff; j++)
				pBuff[i] += pCircleBuff[j][i];
			pBuff[i] /= MeanBuff;
		}
	}
	else
		memcpy(pBuff, pCircleBuff[CircleCnt], BUFF_SIZE*sizeof(float));

	if((++TrxCnt>=TX_TO_RX_TIME && TRxMode==RX) || !pPanOpt->ui.chbStopWhenTx->isChecked())
		memcpy(BufWf, pCircleBuff[CircleCnt], BUFF_SIZE*sizeof(float));

	if(++CircleCnt > MeanBuff)
		CircleCnt = 0;
}

void Panarama::SetDDSFreq(int Freq)
{
	isUpdate = true;
	int tmpFreq = Freq - SampleRate/2;
	if(bandNum == BAND2M && TRxMode == RX)
	{
		if(tmpFreq < BPF2_START)
		{
			int delta = BPF2_START - tmpFreq;
			sFreqFilter1 -= delta;
			dFreqFilter1 = 0;
			if(sFreqFilter1 > SampleRate/2)
				sFreqFilter1 = SampleRate/2;
			else if(sFreqFilter1 < -SampleRate/2)
				sFreqFilter1 = -SampleRate/2;
			sFilter = sFreqFilter1*2.0/SampleRate;
			dFilter = 0.0;
			SignalFilter = sFreqFilter1;
			sFreqDDS = -BPF2_START;
			dFreqDDS = 0;
			sDDSFreq = sFreqDDS*2.0/SampleRate;
			dDDSFreq = 0.0;
			SignalDds = (sFreqDDS + dFreqDDS);
		}
		else if(tmpFreq > (BPF2_END - SampleRate))
		{
			int delta = (BPF2_END - SampleRate) - tmpFreq;
			sFreqFilter1 -= delta;
			dFreqFilter1 = 0;
			if(sFreqFilter1 > SampleRate/2)
				sFreqFilter1 = SampleRate/2;
			else if(sFreqFilter1 < -SampleRate/2)
				sFreqFilter1 = -SampleRate/2;
			sFilter = sFreqFilter1*2.0/SampleRate;
			dFilter = 0.0;
			SignalFilter = sFreqFilter1;
			sFreqDDS = -(BPF2_END - SampleRate);
			dFreqDDS = 0;
			sDDSFreq = sFreqDDS*2.0/SampleRate;
			dDDSFreq = 0.0;
			SignalDds = (sFreqDDS + dFreqDDS);
		}
		else
		{
			sFreqDDS = -tmpFreq;
			dFreqDDS = 0;
			sDDSFreq = sFreqDDS*2.0/SampleRate;
			dDDSFreq = 0.0;
			SignalDds = (sFreqDDS + dFreqDDS);
		}

	}
	else if(bandNum == BAND07M && TRxMode == RX)
	{
		if(tmpFreq < BPF07_START)
		{
			int delta = BPF07_START - tmpFreq;
			sFreqFilter1 -= delta;
			dFreqFilter1 = 0;
			if(sFreqFilter1 > SampleRate/2)
				sFreqFilter1 = SampleRate/2;
			else if(sFreqFilter1 < -SampleRate/2)
				sFreqFilter1 = -SampleRate/2;
			sFilter = sFreqFilter1*2.0/SampleRate;
			dFilter = 0.0;
			SignalFilter = sFreqFilter1;
			sFreqDDS = -BPF07_START;
			dFreqDDS = 0;
			sDDSFreq = sFreqDDS*2.0/SampleRate;
			dDDSFreq = 0.0;
			SignalDds = (sFreqDDS + dFreqDDS);
		}
		else if(tmpFreq > (BPF07_END - SampleRate))
		{
			int delta = (BPF07_END - SampleRate) - tmpFreq;
			sFreqFilter1 -= delta;
			dFreqFilter1 = 0;
			if(sFreqFilter1 > SampleRate/2)
				sFreqFilter1 = SampleRate/2;
			else if(sFreqFilter1 < -SampleRate/2)
				sFreqFilter1 = -SampleRate/2;
			sFilter = sFreqFilter1*2.0/SampleRate;
			dFilter = 0.0;
			SignalFilter = sFreqFilter1;
			sFreqDDS = -(BPF07_END - SampleRate);
			dFreqDDS = 0;
			sDDSFreq = sFreqDDS*2.0/SampleRate;
			dDDSFreq = 0.0;
			SignalDds = (sFreqDDS + dFreqDDS);
		}
		else
		{
			sFreqDDS = -tmpFreq;
			dFreqDDS = 0;
			sDDSFreq = sFreqDDS*2.0/SampleRate;
			dDDSFreq = 0.0;
			SignalDds = (sFreqDDS + dFreqDDS);
		}
	}
	else
	{
		sFreqDDS = -Freq + SampleRate/2;
		dFreqDDS = 0;
		sDDSFreq = sFreqDDS*2.0/SampleRate;
		dDDSFreq = 0.0;
		SignalDds = (sFreqDDS + dFreqDDS);
	}
	updateDbmState();
}

int Panarama::GetDDSFreq()
{
	return -(sFreqDDS+dFreqDDS) + SampleRate/2;
}

void Panarama::SetLockScreen(bool state)
{
	LockScreen = state;
	IsMoveGrid = false;
	StepMoveGrid = 0;
}
void Panarama::SetLockBandFilter(bool state)
{
	LockBand = state;
	IsMoveGrid = false;
	StepMoveGrid = 0;
}

void Panarama::SetLockFilter(bool state)
{
	LockFilter = state;
	IsMoveGrid = false;
	StepMoveGrid = 0;
}

void Panarama::SetSampleRate(int Freq)
{
	int BandPassHigh = (sBandPassHigh + dBandPassHigh)*SampleRate/2.0;
	int BandPassLow = (sBandPassLow + dBandPassLow)*SampleRate/2.0;
	int Pich = sPich*SampleRate/2.0;

	SampleRate = Freq;

	sDDSFreq = sFreqDDS*2.0/SampleRate;
	dDDSFreq = 0.0;
	sFilter = sFreqFilter1*2.0/SampleRate;
	dFilter = 0.0;

	sPich = Pich*2.0/SampleRate;

	sBandPassLow = BandPassLow*2.0/SampleRate;
	dBandPassLow =  0.0;
	sBandPassHigh = BandPassHigh*2.0/SampleRate;
	dBandPassHigh = 0.0;

	LimitMinLowBand = -20000*2.0/SampleRate;
	LimitMaxHighBand = 20000*2.0/SampleRate;

	SetStepGrid();
	BeginX = -1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);
	EndX   = 1.0/(sScaleRuleX + dScaleRuleX) - (sPosZoomPan + dPosZoomPan)*(1.0/(sScaleRuleX + dScaleRuleX)) + (sPosZoomPan + dPosZoomPan);

	Len = BeginX + 1;
	StepCount = ((int)(Len/Step));

	sRit = sRitValue*2.0f/SampleRate;
	dRit = 0.0f;
	isUpdate = true;
}

int Panarama::GetSampleRate()
{
	return SampleRate;
}

void Panarama::GetBandFilter(double &Low, double &High)
{
	High = (sBandPassHigh + dBandPassHigh)*SampleRate/2.0;
	Low = (sBandPassLow + dBandPassLow)*SampleRate/2.0;
}

void Panarama::SetVisibleInfo(int state)
{
	IsVisibleInfo = (bool)state;
	isUpdate = true;
}

void Panarama::SetBandFilter(int Low, int High)
{
	if(BandLow != Low)
	{
		BandLow = Low;
		sBandPassLow = BandLow*2.0/SampleRate;
		dBandPassLow = 0.0;
		SignalBandLow = (sBandPassLow + dBandPassLow);
		BandLow2 = BandLow;
		isUpdate = true;

	}
	if(BandHigh != High)
	{
		BandHigh = High;
		sBandPassHigh = BandHigh*2.0/SampleRate;
		dBandPassHigh = 0.0;
		SignalBandHigh = (sBandPassHigh + dBandPassHigh);
		BandHigh2 = BandHigh;
		isUpdate = true;
	}
	isUpdate = true;
}

void Panarama::SetTxBandFilter(int Low, int High)
{
	BandTxLow = Low;
	BandTxHigh = High;

	isUpdate = true;
}

void Panarama::SetLowBand(int Freq)
{
	if(BandLow != Freq)
	{
		sBandPassLow = Freq*2.0/SampleRate;
		dBandPassLow = 0.0;

		SignalBandLow = (sBandPassLow + dBandPassLow);
	}
	isUpdate = true;
}

void Panarama::SetHighBand(int freq)
{
	if(BandHigh != freq)
	{
		sBandPassHigh = freq*2.0/SampleRate;
		dBandPassHigh = 0.0;

		SignalBandHigh = (sBandPassHigh + dBandPassHigh);
	}
	isUpdate = true;
}

void Panarama::SetFilter(int Freq)
{
	if((sFreqFilter1+dFreqFilter1) == Freq)
		return;

	sFreqFilter1 = Freq;
	dFreqFilter1 = 0;

	if(sFreqFilter1 > SampleRate/2)
		sFreqFilter1 = SampleRate/2;
	else if(sFreqFilter1 < -SampleRate/2)
		sFreqFilter1 = -SampleRate/2;

	sFilter = sFreqFilter1*2.0/SampleRate;
	dFilter = 0.0;
	isUpdate = true;
}

int Panarama::GetFilter()
{
	return (sFreqFilter1 + dFreqFilter1);
}

void Panarama::SetPitch(int Freq)
{
	sPich = Freq*2.0/SampleRate;
	dPich = 0.0;
	pitchVal = Freq;
}

int Panarama::GetPitch()
{
	return pitchVal;
}

void Panarama::SetSpectrRate(int Val)
{
	if(Val > NUM_MEANE_BUFF-1)
		MeanBuff = NUM_MEANE_BUFF-1;
	else
		MeanBuff = Val;
	if(MeanBuff < 1)
		MeanBuff = 1;
}

void Panarama::SetWaterfallRate(int Val)
{
	double val = Val/10.0;
	if(val < 1)
		LineSpeedMax = 1.0;
	else if(val > 10.0)
		LineSpeedMax = 10.0;
	else
		LineSpeedMax = val;

	isUpdate = true;
}

void Panarama::Start()
{
	TimerTxToRx.start(500);
	TimerPeriodGetMax.start(500);
	IsPeriodGetMax = true;
	IsStart = true;
	isUpdate = true;
	IsMoveUpdate = true;
}
void Panarama::Stop()
{
	if(pPanOpt->ui.chbStopWhenTx->isChecked())
	{
		minDbm = LowDb + pPanOpt->ui.sbWfMinOffset->value() - pPanOpt->ui.sbSpMinOffset->value();
		maxDbm = HighDb - LowDb + pPanOpt->ui.sbWfMaxOffset->value() - pPanOpt->ui.sbSpMaxOffset->value();
	}
	else
	{
		minDbm = LowDb + pPanOpt->ui.sbWfMinOffset->value() - pPanOpt->ui.sbSpMinOffset->value();
		maxDbm = HighDb - LowDb + pPanOpt->ui.sbWfMaxOffset->value() - pPanOpt->ui.sbSpMaxOffset->value();
	}
	TimerAutosetDbm.stop();
	IsMoveUpdate = false;
	TimerTxToRx.stop();
	IsMoveGrid = false;
	IsStart = false;
	StepMoveGrid = 0;

	updateDbmState();
}

void Panarama::SetLockGrid(bool state)
{
	LockGrid = state;
}
void Panarama::SlotChangedSettings()
{
	isUpdate = true;
	sOffsetDbm = pPanOpt->PanOption.OffsetDbm;
	dOffsetDbm = 0;
	sLenDbmY = pPanOpt->PanOption.LenDbm;
	dLenDbmY = 0;

	sPosZoomPan = pPanOpt->PanOption.ZoomPos;
	dPosZoomPan = 0.0;

	if(sLenDbmY < 5)
	{
		sLenDbmY = 5;
		pPanOpt->PanOption.LenDbm = 5;
	}
	SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
	SaveLenDbmY = sLenDbmY + dLenDbmY;
	PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
	for(int j = 0; j < NUM_MEANE_BUFF; j++)
	{
		MeanMin[j] = SaveOffsetDbm;
		MeanMax[j] = SaveLenDbmY;
	}

	Cursor = (CURSOR)pPanOpt->PanOption.TypeCursor;
	sRuleYPos = (double)pPanOpt->PanOption.RulePosY;
	TRxMode = RX;
	sScaleRuleX = pPanOpt->getZoom();
	if(sScaleRuleX < 1.0 && sScaleRuleX > 35.0)
		sScaleRuleX = 1.0;
	dScaleRuleX = 0.0;

	PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;

	SetStepDbm();
	OffsetNum = (1.1*ox10)/(sScaleRuleX + dScaleRuleX);
	emit ChangeZoom(sScaleRuleX + dScaleRuleX);
	SetPowerComp(pPanOpt->ui.cbPowerChange->currentIndex());
}

void Panarama::SlotChangeFilter()
{
	IsChangedFilter = false;
	TimerChangedFilter.stop();
	isUpdate = true;
}

void Panarama::SetCalibrateK(float Val)
{
	CalibrateLevel = Val;
}

void Panarama::SetTRxMode(TRXMODE Mode)
{
	TrxCnt = 0;
	IsMoveUpdate = !(bool)Mode;
	TRxMode = Mode;
	pPanOpt->PanOption.TRxMode = TRxMode;
	if(TRxMode == RX)
		Start();
	else
	{
		MemTmp = IndxSubTexture;
		if(pPanOpt->ui.chbStopWhenTx->isChecked())
		{
			IsStart = false;
			IsMoveUpdate = false;
		}
		TimerAutosetDbm.stop();
		TimerTxToRx.stop();
		IsMoveGrid = false;
		StepMoveGrid = 0;
		updateDbmState();
	}
	updateDbmState();
}

TRXMODE Panarama::GetTrxMode()
{
	return TRxMode;
}

void Panarama::SetSetupFilterMaxLevel()
{
	int nMax = 0;
	float tmp = pBuff[0];
	for(int i = 1; i < BUFF_SIZE; i++)
	{
		if(tmp < pBuff[i])
		{
			nMax = i;
			tmp = pBuff[i];
		}
	}
	nMax -= 2048;
	SetFilter((nMax/2048.0)*SampleRate/2.0);
	emit ChangeFilterFreq((nMax/2048.0)*SampleRate/2.0);
}

void Panarama::SlotPeriodGetMax()
{
	IsPeriodGetMax = false;
	TimerPeriodGetMax.stop();
}

float Panarama::GetfreqForMaxLevel()
{
	int nMax = 0;
	float tmp = pBuff[0];
	for(int i = 1; i < BUFF_SIZE; i++)
	{
		if(tmp < pBuff[i])
		{
			nMax = i;
			tmp = pBuff[i];
		}
	}
	nMax -= 2048;
	return (float)((nMax/2048.0)*SampleRate/2.0) + (-(sDDSFreq + dDDSFreq)*SampleRate*0.5 + SampleRate*0.5);
}

void Panarama::SpectrumEnable(bool State)
{
	IsEnableSp = State;
	isUpdate = true;
}

void Panarama::SlotTxToRx()
{
	isUpdate = true;
	if(TRxMode == TX)
		return;

	TimerTxToRx.stop();
	IsMoveUpdate = true;
	IsStart = true;
	if(pPanOpt->ui.cbAutoLevel->currentIndex() != MULTIHAND && IsMoveUpdate)
		TimerAutosetDbm.start(PAN_UPDATE);
}

void Panarama::SlotMousePos()
{
	posPress.setX(posMove.x());
	isUpdate = true;
}

void Panarama::SetScaleGrid(float Value)
{
	sScaleRuleX = Value;
	dScaleRuleX = 0.0;

	if(sScaleRuleX < 1.0)
		sScaleRuleX = 1.0;
	else if(sScaleRuleX > 35.0)
		sScaleRuleX = 35.0;

	OffsetNum = (1.1*ox10)/(sScaleRuleX + dScaleRuleX);
	SetStepGrid();
	isUpdate = true;
}

float Panarama::GetScaleGrid()
{
	return (sScaleRuleX + dScaleRuleX);
}

void Panarama::Filter2On(bool state)
{
	IsFilter2 = state;
	IsCrossF1 = true;
	if(IsFilter2)
		emit ChangeFilterFreq2((sFilter2 + dFilter2)*SampleRate/2.0);
	isUpdate = true;
}

bool Panarama::IsFilter2On()
{
	return IsFilter2;
}

void Panarama::SetFilter2(int Freq)
{
	sFilter2 = Freq*2.0/SampleRate - sPich;
	dFilter2 = 0.0;

	if(sFilter2 <= -1.0)
	{
		emit IsOnFilter2(false);
	}
	if(sFilter2 >= 1.0)
	{
		emit IsOnFilter2(false);
	}
	if(SignalFilter2 != (sFilter2 + dFilter2))
		emit ChangeFilterFreq2(Freq);
	SignalFilter2 = (sFilter2 + dFilter2);
	isUpdate = true;
}

int Panarama::GetFilter2()
{
	return (sFilter2 + dFilter2)*SampleRate/2;
}

void Panarama::GetBandFilter2(double &Low, double &High)
{
	High = (sBandPassHigh2 + dBandPassHigh2)*SampleRate/2.0;
	Low = (sBandPassLow2 + dBandPassLow2)*SampleRate/2.0;
}

void Panarama::SetBandFilter2(int Low, int High)
{
	if(BandLow2 != Low)
	{
		sBandPassLow2 = Low*2.0/SampleRate;
		dBandPassLow2 = 0.0;
		SignalBandLow2 = (sBandPassLow2 + dBandPassLow2);
		BandLow2 = Low;
	}
	if(BandHigh2 != High)
	{
		sBandPassHigh2 = High*2.0/SampleRate;
		dBandPassHigh2 = 0.0;
		SignalBandHigh2 = (sBandPassHigh2 + dBandPassHigh2);
		BandHigh2 = High;
	}
	BandLow = BandLow2;
	BandHigh = BandHigh2;
}

void Panarama::SetTxVfo(int val)
{
	if(val > 1)
		TxVfo = 1;
	else if(val < 1)
		TxVfo = 0;
	else
		TxVfo = val;
	isUpdate = true;
}

bool Panarama::IsFilter2OnPanoram()
{
	if((TxVfo != 0) && ((sFilter2+dFilter2) < 1.0 && (sFilter2+dFilter2) > -1.0))
		return true;
	else
		return false;
}

void Panarama::SetStepFilter(int index)
{
	numStepGridFilter = index;
}

void Panarama::SetStepDDS(int index)
{
	numStepGridDDS = index;
}

void Panarama::SetMode(SDRMODE mode)
{
	Mode = mode;
	updateDbmState();
}

void Panarama::MedianFilter(float *pIn, int size)
{
	static int k = 0;
	static float MedianData[7];
	//
	memcpy(MedianTimeBuff[k], pIn, size*sizeof(float));
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < 7; j++)
			MedianData[j] = MedianTimeBuff[j][i];
		qsort(MedianData, 7, sizeof(float), pancompare);
		pIn[i] = MedianData[3];

	}
	if(++k > 6)
		k = 0;
}

void Panarama::GetPeak(int *freq, float *dbm)
{
	int nMax = 0;
	float tmp = pBuff[0];
	for(int i = BUFF_SIZE/2 + 20; i < BUFF_SIZE; i++)
	{
		if(tmp < pBuff[i])
		{
			nMax = i;
			tmp = pBuff[i];
		}
	}
	nMax -= 2048;
	nMax = (float)((nMax/2048.0f)*SampleRate/2.0f) + (-(sDDSFreq + dDDSFreq)*SampleRate*0.5f + SampleRate*0.5f);

	*freq = nMax;
	*dbm = tmp;
}

void Panarama::setRit(bool state)
{
	isRitOn = state;
	ritLimLow = sFreqFilter1 - RIT_LIMIT;
	if(ritLimLow < -SampleRate/2)
		ritLimLow = -SampleRate/2;
	ritLimHigh = sFreqFilter1 + RIT_LIMIT;
	if(ritLimHigh > SampleRate/2)
		ritLimHigh = SampleRate/2;
	glRitLimLow = ritLimLow *2.0f/SampleRate;
	glRitLimHigh= ritLimHigh*2.0f/SampleRate;
	isUpdate = true;
	emit ChangeRitValue(signalRitValue);
	emit ChangeFilterFreq(SignalFilter);
}

bool Panarama::isRit()
{
	return isRitOn;
}

void Panarama::setRitValue(int value)
{
	if(value == (sRitValue+dRitValue))
		return;

	if(value > RIT_LIMIT)
		ritValue = RIT_LIMIT;
	else if(value < -RIT_LIMIT)
		ritValue = -RIT_LIMIT;
	else
		ritValue = value;
	sRitValue = ritValue;
	dRitValue = 0;
	sRit = sRitValue*2.0f/SampleRate;
	dRit = 0.0f;
	isUpdate = true;
}

int Panarama::getRitValue()
{
	return sRitValue + dRitValue;
}

void Panarama::SlotChangedParametersFreq()
{
	if(SignalDds != (sFreqDDS + dFreqDDS))
	{
		emit ChangeCentrFreq(-sFreqDDS-dFreqDDS+ SampleRate/2);
		SignalDds = sFreqDDS + dFreqDDS;

		updateDbmState();
	}
	if(offsetDbm != (sOffsetDbm + dOffsetDbm))
	{
		offsetDbm = sOffsetDbm + dOffsetDbm;
		emit ChangedDbm(offsetDbm, scaleDbm);
	}
	else if(scaleDbm != (sLenDbmY + dLenDbmY))
	{
		scaleDbm = sLenDbmY + dLenDbmY;
		emit ChangedDbm(offsetDbm, scaleDbm);
	}
	if(SignalFilter != (sFreqFilter1 + dFreqFilter1))
	{
		emit ChangeFilterFreq(sFreqFilter1 + dFreqFilter1);
		SignalFilter = sFreqFilter1 + dFreqFilter1;

		updateDbmState();
	}
	if(SignalBandLow != (sBandPassLow + dBandPassLow))
	{
		emit ChangeFilterLowFreq((sBandPassLow + dBandPassLow)*SampleRate/2.0);
		SignalBandLow = (sBandPassLow + dBandPassLow);
		BandLow = (sBandPassLow + dBandPassLow)*SampleRate/2.0;
	}
	if(SignalBandHigh != (sBandPassHigh + dBandPassHigh))
	{
		emit ChangeFilterHighFreq((sBandPassHigh + dBandPassHigh)*SampleRate/2.0);
		SignalBandHigh = sBandPassHigh + dBandPassHigh;
		BandHigh = (sBandPassHigh + dBandPassHigh)*SampleRate/2.0;
	}
	if(SignalFilter2 != (sFilter2 + dFilter2))
	{
		if(IsFilter2)
		{
			emit ChangeFilterFreq2((sFilter2 + dFilter2)*SampleRate/2.0 + pitchVal);
			SignalFilter2 = (sFilter2 + dFilter2);
		}
	}
	if(SignalBandLow2 != (sBandPassLow2 + dBandPassLow2))
	{
		emit ChangeFilterLowFreq2((sBandPassLow2 + dBandPassLow2)*SampleRate/2.0);
		SignalBandLow2 = (sBandPassLow2 + dBandPassLow2);
		BandLow2 = (sBandPassLow2 + dBandPassLow2)*SampleRate/2.0;
	}
	if(SignalBandHigh2 != (sBandPassHigh2 + dBandPassHigh2))
	{
		emit ChangeFilterHighFreq2((sBandPassHigh2 + dBandPassHigh2)*SampleRate/2.0);
		SignalBandHigh2 = sBandPassHigh2 + dBandPassHigh2;
		BandHigh2 = (sBandPassHigh2 + dBandPassHigh2)*SampleRate/2.0;
	}
	if(signalRitValue != (sRitValue + dRitValue))
	{
		emit ChangeRitValue(sRitValue + dRitValue);
		signalRitValue = sRitValue + dRitValue;
		emit ChangeFilterFreq(SignalFilter);
	}
}

void Panarama::SlotRitColor()
{
	if(isOutOfRit)
	{
		alphaRitLim = 1.0;
		isOutOfRit = false;
		isUpdate = true;
	}
	else if(alphaRitLim > 0.0)
	{
		alphaRitLim -= 0.01;
		isUpdate = true;
	}
}

int Panarama::getBandNum()
{
	int F = -sFreqDDS - dFreqDDS + SampleRate/2 + sFreqFilter1 + dFreqFilter1;
	if(F >= BPF160_START && F <= BPF160_END)
		return (0);
	else if(F >= BPF80_START && F <= BPF80_END)
		return (1);
	else if(F >= BPF60_START && F <= BPF60_END)
		return (2);
	else if(F >= BPF40_START && F <= BPF40_END)
		return (3);
	else if(F >= BPF30_START && F <= BPF30_END)
		return (4);
	else if(F >= BPF20_START && F <= BPF20_END)
		return (5);
	else if(F >= BPF17_START && F <= BPF17_END)
		return (6);
	else if(F >= BPF15_START && F <= BPF15_END)
		return (7);
	else if(F >= BPF12_START && F <= BPF12_END)
		return (8);
	else if(F >= BPF10_START && F <= BPF10_END)
		return (9);
	else if(F >= BPF6_START && F <= BPF6_END)
		return (10);
	else if(F >= BPF2_START && F <= BPF2_END)
		return (11);
	else if(F >= BPF07_START && F <= BPF07_END)
		return (12);
	else
		return (13);
}

void Panarama::updateDbmState()
{
	int tmpBand = getBandNum();
	if((currentBand != tmpBand) || ((TRxMode == RX) && (TRxModeOld == TX)))
	{
		if(TRxModeOld == TX)
		{
			DbmTxState.offset = sOffsetDbm + dOffsetDbm;
			DbmTxState.scale = sLenDbmY + dLenDbmY;
			TRxModeOld = TRxMode;
		}
		else
		{
			DbmRxState[currentBand].offset = sOffsetDbm + dOffsetDbm;
			DbmRxState[currentBand].scale = sLenDbmY + dLenDbmY;
		}
		currentBand = tmpBand;
		sOffsetDbm = DbmRxState[currentBand].offset;
		dOffsetDbm = 0.0;
		sLenDbmY = DbmRxState[currentBand].scale;
		dLenDbmY = 0.0;
		SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
		SaveLenDbmY = sLenDbmY + dLenDbmY;
		PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
		for(int j = 0; j < NUM_MEANE_BUFF; j++)
		{
			MeanMin[j] = SaveOffsetDbm;
			MeanMax[j] = SaveLenDbmY;
		}
		PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
		SetStepDbm();
		OffsetNum = (1.1*ox10)/(sScaleRuleX + dScaleRuleX);
		IsMoveUpdate = true;
		isUpdate = true;
	}
	else if(((TRxMode == TX) && (TRxModeOld == RX)))
	{
		TRxModeOld = TRxMode;
		if(currentBand == 13)
				currentBand = tmpBand;
		DbmRxState[currentBand].offset = sOffsetDbm + dOffsetDbm;
		DbmRxState[currentBand].scale = sLenDbmY + dLenDbmY;
		sOffsetDbm = DbmTxState.offset;
		dOffsetDbm = 0.0;
		sLenDbmY = DbmTxState.scale;
		dLenDbmY = 0.0;

		SaveOffsetDbm = sOffsetDbm + dOffsetDbm;
		SaveLenDbmY = sLenDbmY + dLenDbmY;
		PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
		for(int j = 0; j < NUM_MEANE_BUFF; j++)
		{
			MeanMin[j] = SaveOffsetDbm;
			MeanMax[j] = SaveLenDbmY;
		}
		PosYNum = oy2_5/(1.0-posRule)*SaveLenDbmY;
		SetStepDbm();
		OffsetNum = (1.1*ox10)/(sScaleRuleX + dScaleRuleX);
		IsMoveUpdate = true;
		isUpdate = true;
	}
}

QString Panarama::freqToStr(double freq)
{
    bool addminus = freq < 0;
    if (addminus)
        freq *= -1;
    QString str = QString::number(qint64(freq));
    int pass = (str.length() - 1) / 3;
    if (pass > 3)
        pass = 3;
    int start_len = str.length();
    for(int i = 1; i <= pass; i++)
    {
       str.insert(start_len - i * 3, " ");
    }

    if (addminus)
        str = "-" + str;

    return str;
}

void Panarama::readSettings(QSettings *pSettings)
{
	float tmp = 0;
	pSettings->beginGroup("PanSettings");
		tmp = pSettings->value("TxBandOffset", -140).toFloat();
		if(tmp < -200 || tmp > 50)
		{
			qWarning() << "Panorama: readSettings(): TxBandOffset = " << tmp;
			DbmTxState.offset = -140.0f;
		}
		DbmTxState.offset = tmp;

		tmp = pSettings->value("TxBandScale", 120).toFloat();
		if(tmp < 10 || tmp > 200)
		{
			qWarning() << "Panorama: readSettings(): TxBandScale = " << tmp;
			DbmTxState.scale = 120;
		}
		DbmTxState.scale = tmp;

		for(int i = 0; i < NUM_BANDS+1; i++)
		{
			tmp = pSettings->value("RxBandOffset" + QString::number(i), -140).toFloat();
			if(tmp < -200 || tmp > 50)
			{
				qWarning() << "Panorama: readSettings(): TxBandOffset" + QString::number(i) + " = " << tmp;
				DbmRxState[i].offset = -140.0f;
			}
			DbmRxState[i].offset = tmp;

			tmp = pSettings->value("RxBandScale"  + QString::number(i), 120).toFloat();
			if(tmp < 10 || tmp > 200)
			{
				qWarning() << "Panorama: readSettings(): TxBandScale" + QString::number(i) + " = " << tmp;
				DbmRxState[i].scale = 120;
			}
			DbmRxState[i].scale = tmp;
		}
		currentBand = getBandNum();
		sOffsetDbm = DbmRxState[currentBand].offset;
		dOffsetDbm = 0.0;
		sLenDbmY = DbmRxState[currentBand].scale;
		dLenDbmY = 0.0;
		sPosRule = pSettings->value("posRule", 250).toInt();
		dPosRule = 0;

		if(sPosRule > (height()-RULE_HEIGH))
			posRule = 1.0 - (height() - RULE_HEIGH)*2.0/height();

		posRule = 1.0 - sPosRule*2.0/height();

		lenRule = RULE_HEIGH*2.0f/height();
	pSettings->endGroup();

	SetStepDbm();
}

void Panarama::writeSettings(QSettings *pSettings)
{
	qDebug() << "Panarama: writeSettings()";
	pSettings->beginGroup("PanSettings");
        pSettings->setValue("TxBandOffset", ((double)DbmTxState.offset));
        pSettings->setValue("TxBandScale" , ((double)DbmTxState.scale) );
		for(int i = 0; i < NUM_BANDS+1; i++)
		{
            pSettings->setValue("RxBandOffset" + QString::number(i), ((double)DbmRxState[i].offset));
            pSettings->setValue("RxBandScale"  + QString::number(i), ((double)DbmRxState[i].scale) );
		}
		pSettings->setValue("posRule", sPosRule);
	pSettings->endGroup();
}

