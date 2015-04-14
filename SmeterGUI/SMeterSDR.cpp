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

#include "SMeterSDR.h"
#include <QMenu>

inline double dBm_to_Watts(double dBm)
{
	return qPow(10.0, (dBm - 30.0) / 10.0);
}

inline double dBm_to_RMSVolts(double dBm, double impedance)
{
	return qSqrt(dBm_to_Watts(dBm) * impedance);
}

SMeterSDR::SMeterSDR(QWidget *pOpt, QWidget *parent) : QWidget(parent)
{
	setMouseTracking(true);

	pSmOpt = new SMeterOptions(pOpt);

    ColorParam = pSmOpt->getParam();

	Tx = false;

    RxType = RX_SMETER;
    TxType = TX_MIC_METER;

    SmType = SMETER;

    isVHF = false;

    pRx = new QAction(tr("Rx"), this);
    pRx->setStatusTip(tr("Rx"));
    pRx->setCheckable(false);

    pTx = new QAction(tr("Tx"), this);
    pTx->setStatusTip(tr("Tx"));
    pTx->setCheckable(false);

	pRxSmeter = new QAction(tr("&S-meter"), this);
	pRxSmeter->setStatusTip(tr("View S-meter"));
	pRxSmeter->setCheckable(true);
	pRxSmeter->setChecked(true);

	pRxAdclMeter = new QAction(tr("&ADC L"), this);
	pRxAdclMeter->setStatusTip(tr("View ADC L"));
	pRxAdclMeter->setCheckable(true);

	pRxAdcrMeter = new QAction(tr("&ADC R"), this);
	pRxAdcrMeter->setStatusTip(tr("View ADC R"));
	pRxAdcrMeter->setCheckable(true);

	pTxMicMeter = new QAction(tr("&Mic-meter"), this);
	pTxMicMeter->setStatusTip(tr("View Mic-meter"));
	pTxMicMeter->setCheckable(true);
	pTxMicMeter->setChecked(true);

	pTxPwrMeter = new QAction(tr("&Power-meter"), this);
	pTxPwrMeter->setStatusTip(tr("View Power-meter"));
	pTxPwrMeter->setCheckable(true);

	pTxSwrMeter = new QAction(tr("&SWR-meter"), this);
	pTxSwrMeter->setStatusTip(tr("View SWR-meter"));
	pTxSwrMeter->setCheckable(true);

	pRxActGroup = new QActionGroup(this);
	pRxActGroup->addAction(pRxSmeter);
	pRxActGroup->addAction(pRxAdclMeter);
	pRxActGroup->addAction(pRxAdcrMeter);

	pTxActGroup = new QActionGroup(this);
	pTxActGroup->addAction(pTxMicMeter);
	pTxActGroup->addAction(pTxPwrMeter);
	pTxActGroup->addAction(pTxSwrMeter);

	pMenuRx = new QMenu;
	pMenuRx->addAction(pRxSmeter);
	pMenuRx->addAction(pRxAdclMeter);
	pMenuRx->addAction(pRxAdcrMeter);

	pMenuTx = new QMenu;
	pMenuTx->addAction(pTxMicMeter);
	pMenuTx->addAction(pTxPwrMeter);
	pMenuTx->addAction(pTxSwrMeter);

	pRx->setMenu(pMenuRx);
	pTx->setMenu(pMenuTx);

	pImgBackground = new QPixmap(":images/background.png");
	pImgCursor = new QPixmap(":images/cursor.png");
	pImgCursorShadow = new QPixmap(":images/cursorImg.png");
	pImgSql = new QPixmap(":images/Sql.png");
	pPenLines = new QPen;
	pPenLines->setStyle(Qt::SolidLine);
	pPenLines->setWidth(1);
	pFontScale = new QFont(font());
	pFontScale->setPointSize(7);
	pFontMainNum = new QFont(font());
	pFontMainNum->setPointSize(10);

	pFontM = new QFontMetrics((*pFontScale));
	pPainter = new QPainter(this);

	pLineGradient = new QLinearGradient;

	setMinimumSize(200, 36);
	setFixedHeight(36);
	resize(400, 36);

	dBmValDraw = -140;
	dBmVal = -140;
	dBmLimitsCnt = 0;

	dBmValMin = -140;
	dBmValMax = -140;

    AdclVal = -100;
    AdclValMin = -100;
    AdclValMax = -100;
    AdclValDraw = -100;
    AdclLimitsCnt = 0;

    AdcrVal = -100;
    AdcrValMin = -100;
    AdcrValMax = -100;
    AdcrValDraw = -100;
    AdcrLimitsCnt = 0;

    MicVal = -30;
    MicValMin = -30;
    MicValMax = -20;
    MicValDraw = -30;
    MicLimitsCnt = 0;

    PowerVal = 0;
    PowerValMin = 0;
    PowerValMax = 0;
    PowerValDraw = 0;
    PowerLimitsCnt = 0;

    SWRVal = 1;
    SWRValMin = 1;
    SWRValMax = 1;
    SWRValDraw = 1;
    SWRLimitsCnt = 1;

	SqlOn = false;
	SqlVal = -100;
	sSql = getPosFromDbm(SqlVal);
	dSql = 0.0f;

	pBuffDbm = new float[DBM_BUF_SIZE];
	pBuffAdcl = new float[DBM_BUF_SIZE];
	pBuffAdcr = new float[DBM_BUF_SIZE];
	pBuffMic = new float[DBM_BUF_SIZE];
	pBuffPower = new float[DBM_BUF_SIZE];
	pBuffSWR = new float[DBM_BUF_SIZE];
	for(int i = 0; i < DBM_BUF_SIZE; i++)
	{
		pBuffDbm[i] = -140;
		pBuffAdcl[i] = -100;
		pBuffAdcr[i] = -100;
		pBuffMic[i] = -30;
		pBuffPower[i] = 0;
		pBuffSWR[i] = 1.0;
	}

	startTimer(25);
	timerChangeDbmVal.start(5);
	connect(&timerChangeDbmVal, SIGNAL(timeout()), this, SLOT(setChangeDbmVal()));
	connect(&timerChangeDbmVal, SIGNAL(timeout()), this, SLOT(setChangeDbmLimits()));
	connect(pSmOpt, SIGNAL(changedParam(SM_PARAM)), this, SLOT(setColorParam(SM_PARAM)));
	connect(pRxActGroup, SIGNAL(triggered(QAction *)), this, SLOT(GetRxType(QAction *)));
	connect(pTxActGroup, SIGNAL(triggered(QAction *)), this, SLOT(GetTxType(QAction *)));

	pSmOpt->show();
}

SMeterSDR::~SMeterSDR()
{
	delete pImgBackground;
	delete pImgCursorShadow;
	delete pImgCursor;
	delete pImgSql;
	delete pPenLines;
	delete pFontScale;
	delete pFontM;
	delete pPainter;
	delete[] pBuffDbm;
	delete[] pBuffAdcl;
	delete[] pBuffAdcr;
	delete[] pBuffMic;
	delete[] pBuffPower;
	delete[] pBuffSWR;
	delete pSmOpt;
}

void SMeterSDR::paintEvent(QPaintEvent *event)
{
	switch(SmType)
	{
	case ADCL:
		drawAdcL();
		break;
	case ADCR:
		drawAdcR();
		break;
	case MIC_METER:
		drawMic();
		break;
	case PWR_METER:
		drawPower();
		break;
	case SWR_METER:
		drawSWR();
		break;
	default:
		drawDbm();
		break;
	}
}

void SMeterSDR::resizeEvent(QResizeEvent *event)
{
	float Len = width() - 2*OFFSET;
	StepdBm = Len / 8.0;
	StepLevel = Len / 4.0;
	StepS = StepdBm/(20.0/6.0);
	sSql = getPosFromDbm(SqlVal);
	dSql = 0.0f;
}

void SMeterSDR::timerEvent(QTimerEvent *event)
{
	update();
}

void SMeterSDR::mouseMoveEvent(QMouseEvent *event)
{
	float x = event->x();

	if(SqlOn)
	{
		if((x >= (sSql+dSql -3)) && (x < ((sSql+dSql+2))))
			setCursor(Qt::SplitHCursor);
		else
			setCursor(Qt::ArrowCursor);
	}
}

void SMeterSDR::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
	menu.addAction(pRx);
	menu.addAction(pTx);
    menu.exec(event->globalPos());
}

void SMeterSDR::setColorParam(SM_PARAM param)
{
	ColorParam = param;
}

float SMeterSDR::getPosFromDbm(float dBm)
{
	float Len = width() - 2*OFFSET;
	return (OFFSET + Len*((160.0f + dBm -10)/160.0f));
}

float SMeterSDR::getPosFromAdcl(float val)
{
	float Len = width() - 2*OFFSET;
	return (OFFSET + Len*((120.0f + val)/120.0f));
}

float SMeterSDR::getPosFromAdcr(float val)
{
	float Len = width() - 2*OFFSET;
	return (OFFSET + Len*((120.0f + val)/120.0f));
}

float SMeterSDR::getAdclMin()
{
	float val = pBuffAdcl[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val > pBuffAdcl[i])
			val = pBuffAdcl[i];
	}
	return val;
}
float SMeterSDR::getAdcrMin()
{
	float val = pBuffAdcr[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val > pBuffAdcr[i])
			val = pBuffAdcr[i];
	}
	return val;
}
float SMeterSDR::getAdclMax()
{
	float val = pBuffAdcl[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val < pBuffAdcl[i])
			val = pBuffAdcl[i];
	}
	return val;
}
float SMeterSDR::getAdcrMax()
{
	float val = pBuffAdcr[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val < pBuffAdcr[i])
			val = pBuffAdcr[i];
	}
	return val;
}

float SMeterSDR::getPosFromMic(float val)
{
	float Len = (width() - 2*OFFSET);
	if(val < 0)
		return (OFFSET + Len*((40.0f + val - 10)/40.0f));
	else
	{
		if(val > 3)
			val = 3;

		val = val*10.0/3.0;

		return (OFFSET + Len*((40.0f + val-10)/40.0f));
	}
}

float SMeterSDR::getMicMin()
{
	float val = pBuffMic[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val > pBuffMic[i])
			val = pBuffMic[i];
	}
	return val;
}

float SMeterSDR::getMicMax()
{
	float val = pBuffMic[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val < pBuffMic[i])
			val = pBuffMic[i];
	}
	return val;
}

float SMeterSDR::getPosFromPower(float val)
{
	float Len = width() - 2*OFFSET;
	return (OFFSET + Len*(val/50.0f));
}

float SMeterSDR::getPowerMin()
{
	float val = pBuffPower[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val > pBuffPower[i])
			val = pBuffPower[i];
	}
	return val;
}

float SMeterSDR::getPowerMax()
{
	float val = pBuffPower[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val < pBuffPower[i])
			val = pBuffPower[i];
	}
	return val;
}

float SMeterSDR::getPosFromSWR(float val)
{
	float Len = (width() - 2*OFFSET)/2;
	if(val <= 3)
	{
		return (OFFSET + Len*((-1 + val)/2.0f));
	}
	else
	{
		if(val > 11)
			val = 11;

		val -= 3;

		return (OFFSET + Len*(val/8.0f) + Len);
	}
}

float SMeterSDR::getSWRMin()
{
	float val = pBuffSWR[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val > pBuffSWR[i])
			val = pBuffSWR[i];
	}
	return val;
}

float SMeterSDR::getSWRMax()
{
	float val = pBuffSWR[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val < pBuffSWR[i])
			val = pBuffSWR[i];
	}
	return val;
}

void SMeterSDR::setValRnd()
{
	SWRVal = 3.5;
}

void SMeterSDR::setChangeDbmVal()
{
	float step = 0.7;
	if(dBmValDraw < dBmVal)
	{
			if(dBmValDraw+3*step < dBmVal)
				dBmValDraw += 3*step;
			else
				dBmValDraw = dBmVal;
	}
	else if(dBmValDraw > dBmVal)
	{
			if(dBmValDraw-0.5*step > dBmVal)
				dBmValDraw -= 0.5*step;
			else
				dBmValDraw = dBmVal;
	}
	dBmLimitsCnt++;
	dBmLimitsCnt &= DBM_BUF_SIZE;
	pBuffDbm[dBmLimitsCnt] = dBmValDraw;
	step = 0.3;
	if(AdclValDraw < AdclVal)
	{
		if(AdclValDraw+3*step < AdclVal)
			AdclValDraw += 3*step;
		else
			AdclValDraw = AdclVal;
	}
	else if(AdclValDraw > AdclVal)
	{
		if(AdclValDraw-step > AdclVal)
			AdclValDraw -= step;
		else
			AdclValDraw = AdclVal;
	}
	AdclLimitsCnt++;
	AdclLimitsCnt &= DBM_BUF_SIZE;
	pBuffAdcl[AdclLimitsCnt] = AdclValDraw;

	step = 0.3;
	if(AdcrValDraw < AdcrVal)
	{
		if(AdcrValDraw+3*step < AdcrVal)
			AdcrValDraw += 3*step;
		else
			AdcrValDraw = AdcrVal;
	}
	else if(AdcrValDraw > AdcrVal)
	{
		if(AdcrValDraw-step > AdcrVal)
			AdcrValDraw -= step;
		else
			AdcrValDraw = AdcrVal;
	}
	AdcrLimitsCnt++;
	AdcrLimitsCnt &= DBM_BUF_SIZE;
	pBuffAdcr[AdcrLimitsCnt] = AdcrValDraw;

	step = 0.1;
	if(MicValDraw < MicVal)
	{
		if(MicValDraw+3*step < MicVal)
			MicValDraw += 3*step;
		else
			MicValDraw = MicVal;
	}
	else if(MicValDraw > MicVal)
	{
		if(MicValDraw-step > MicVal)
			MicValDraw -= step;
		else
			MicValDraw = MicVal;
	}
	MicLimitsCnt++;
	MicLimitsCnt &= DBM_BUF_SIZE;
	pBuffMic[MicLimitsCnt] = MicValDraw;
}

void SMeterSDR::setChangeDbmLimits()
{
	float step = 0.3;

	if(dBmValMin < getdBmMin())
	{
		if(dBmValMin+step < getdBmMin())
			dBmValMin += step;
		else
			dBmValMin = getdBmMin();
	}
	else if(dBmValMin > getdBmMin())
	{
			dBmValMin = getdBmMin();
	}

	if(dBmValMax < getDbmMax())
	{
			dBmValMax = getDbmMax();
	}
	else if(dBmValMax >= getDbmMax())
	{
		if(dBmValMax-step > getDbmMax())
			dBmValMax -= step;
		else
			dBmValMax = getDbmMax();
	}

	if(AdclValMin < getAdclMin())
	{
		if(AdclValMin+step < getAdclMin())
			AdclValMin += step;
		else
			AdclValMin = getAdclMin();
	}
	else if(AdclValMin > getAdclMin())
	{
			AdclValMin = getAdclMin();
	}

	if(AdclValMax < getAdclMax())
	{
			AdclValMax = getAdclMax();
	}
	else if(AdclValMax >= getAdclMax())
	{
		if(AdclValMax-step > getAdclMax())
			AdclValMax -= step;
		else
			AdclValMax = getAdclMax();
	}

	if(AdcrValMin < getAdcrMin())
	{
		if(AdcrValMin+step < getAdcrMin())
			AdcrValMin += step;
		else
			AdcrValMin = getAdcrMin();
	}
	else if(AdcrValMin > getAdcrMin())
	{
			AdcrValMin = getAdcrMin();
	}

	if(AdcrValMax < getAdcrMax())
	{
			AdcrValMax = getAdcrMax();
	}
	else if(AdcrValMax >= getAdcrMax())
	{
		if(AdcrValMax-step > getAdcrMax())
			AdcrValMax -= step;
		else
			AdcrValMax = getAdcrMax();
	}

	if(MicValMin < getMicMin())
	{
		if(MicValMin+step < getMicMin())
			MicValMin += step;
		else
			MicValMin = getMicMin();
	}
	else if(MicValMin > getMicMin())
	{
			MicValMin = getMicMin();
	}

	if(MicValMax < getMicMax())
	{
			MicValMax = getMicMax();
	}
	else if(MicValMax >= getMicMax())
	{
		if(MicValMax-step > getMicMax())
			MicValMax -= step;
		else
			MicValMax = getMicMax();
	}
}

float SMeterSDR::getdBmMin()
{
	float val = pBuffDbm[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val > pBuffDbm[i])
			val = pBuffDbm[i];
	}
	return val;
}

float SMeterSDR::getDbmMax()
{
	float val = pBuffDbm[0];
	for(int i = 1; i < DBM_BUF_SIZE; i++)
	{
		if(val < pBuffDbm[i])
			val = pBuffDbm[i];
	}
	return val;
}

QString SMeterSDR::dBmToText()
{
    QString strV;
    if(dBmValDraw <= -127+3)
        strV = "S0";
    else if(dBmValDraw <= -121+3)
        strV = "S1";
    else if(dBmValDraw <= -115+3)
        strV = "S2";
    else if(dBmValDraw <= -109+3)
        strV = "S3";
    else if(dBmValDraw <= -103+3)
        strV = "S4";
    else if(dBmValDraw <= -97+3)
        strV = "S5";
    else if(dBmValDraw <= -91+3)
        strV = "S6";
    else if(dBmValDraw <= -85+3)
        strV = "S7";
    else if(dBmValDraw <= -79+3)
        strV = "S8";
    else if(dBmValDraw <= -73+3)
        strV = "S9";
    else if(dBmValDraw <= -73+5+2.5)
        strV = "S9 +5";
    else if(dBmValDraw <= -73+10+2.5)
        strV = "S9 +10";
    else if(dBmValDraw <= -73+15+2.5)
        strV = "S9 +15";
    else if(dBmValDraw <= -73+20+2.5)
        strV = "S9 +20";
    else if(dBmValDraw <= -73+25+2.5)
        strV = "S9 +25";
    else if(dBmValDraw <= -73+30+2.5)
        strV = "S9 +30";
    else if(dBmValDraw <= -73+35+2.5)
        strV = "S9 +35";
    else if(dBmValDraw <= -73+40+2.5)
        strV = "S9 +40";
    else if(dBmValDraw <= -73+45+2.5)
        strV = "S9 +45";
    else if(dBmValDraw <= -73+50+2.5)
        strV = "S9 +50";
    else if(dBmValDraw <= -73+55+2.5)
        strV = "S9 +55";
    else if(dBmValDraw <= -73+60+2.5)
        strV = "S9 +60";
    else if(dBmValDraw <= -73+65+2.5)
        strV = "S9 +65";
    else if(dBmValDraw <= -73+70+2.5)
        strV = "S9 +70";
    else if(dBmValDraw <= -73+75+2.5)
        strV = "S9 +75";
    else
        strV = "S9 +80";

    return strV;
}

void SMeterSDR::drawDbm()
{
	pPainter->begin(this);
		pPainter->setRenderHint(QPainter::TextAntialiasing);
		int w = width();
		int h = height();

		pLineGradient->setColorAt(0.0, ColorParam.ColorSmBackTopColor);
		pLineGradient->setColorAt(1.0, ColorParam.ColorSmBackBottColor);
		pLineGradient->setStart(0, 0);
		pLineGradient->setFinalStop(0, h);
		pPainter->setBrush((*pLineGradient));
		pPainter->setPen(Qt::black);
		pPainter->drawRect(0,0, w-1, h-1);

		if(SqlOn)
		{
			pPenLines->setColor(ColorParam.ColorSmSQLBorderColor);
			pPainter->setPen((*pPenLines));
			pPainter->setBrush(ColorParam.ColorSmSQLCursorColor);
			pPainter->drawRect(sSql + dSql -3, 2, 3, h-4);
		}

		pPenLines->setColor(ColorParam.ColorSmLinesLow);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(Qt::NoBrush);
		pPainter->drawLine(OFFSET, 17, w - OFFSET, 17);
		pPainter->drawLine(OFFSET, 19, w - OFFSET, 19);
		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(OFFSET, 18, w - OFFSET, 18);
		int dBmNum = -120;
		if(isVHF)
			dBmNum = -140;
		QString strNum;
		pPainter->setFont((*pFontScale));
		for(float begin = OFFSET; begin <= (w - OFFSET); begin += StepdBm)
		{
			if(begin <= (w - OFFSET - StepdBm*0.5f))
			{
				pPenLines->setColor(ColorParam.ColorSmLinesLow);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 17, begin, 13);
				pPainter->drawLine(begin + StepdBm*0.25f, 17, begin + StepdBm*0.25f, 15);
				pPainter->drawLine(begin + StepdBm*0.75f, 17, begin + StepdBm*0.75f, 15);
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin + StepdBm*0.5f, 17, begin + StepdBm*0.5f, 13);
				if((begin + StepdBm*0.5f) >= (OFFSET + StepdBm))
				{
					strNum.setNum(dBmNum);
					QFontMetrics fm((*pFontMainNum));
					int TextW = pFontM->width(strNum);
					if((begin + StepdBm*0.5f +1 - TextW/2) > 1+fm.width("-000.0dBm"))
					{
						pPenLines->setColor(Qt::black);
						pPenLines->setWidth(1);
						pPainter->setPen((*pPenLines));
						pPainter->drawText(begin + StepdBm*0.5f - TextW/2, 10, strNum);
						pPenLines->setColor(ColorParam.ColorSmScaleNum);
						pPainter->setPen((*pPenLines));
						pPainter->drawText(begin + StepdBm*0.5f - TextW/2 -1, 9, strNum);
					}
					dBmNum += 20;
				}
			}
			else
			{
				pPenLines->setColor(ColorParam.ColorSmLinesHigh);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(w - OFFSET - StepdBm*0.25f, 17, w - OFFSET - StepdBm*0.25f, 15);
				pPainter->drawLine(w - OFFSET, 17, w - OFFSET, 13);
				pPainter->drawLine(w - OFFSET - StepdBm*0.5f + 1, 17, w - OFFSET, 17);
				break;
			}
		}
		dBmNum = 1;
		for(float begin = getPosFromDbm(-127); begin < getPosFromDbm(-72); begin += 2*StepS)
		{
			pPenLines->setColor(ColorParam.ColorSmLinesLow);
			pPenLines->setWidth(1);
			pPainter->setPen((*pPenLines));
			pPainter->drawLine(begin, 19, begin, 23);
			pPenLines->setWidth(2);
			pPainter->setPen((*pPenLines));
			pPainter->drawLine(begin+StepS, 20, begin+StepS, 23);
			strNum = "S" + QString::number(dBmNum);
			int TextW = pFontM->width(strNum);
			pPenLines->setColor(Qt::black);
			pPenLines->setWidth(1);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin + StepS - TextW/2, 34, strNum);
			pPenLines->setColor(ColorParam.ColorSmScaleNum);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin + StepS - TextW/2 -1, 33, strNum);
			dBmNum += 2;
		}
		pPenLines->setColor(ColorParam.ColorSmLinesHigh);
		pPenLines->setWidth(1);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(getPosFromDbm(-73)+1, 19, w - OFFSET, 19);
		dBmNum = 20;
		for(float begin = getPosFromDbm(-73)+StepdBm; begin <= (w - OFFSET); begin += StepdBm)
		{
			pPenLines->setColor(ColorParam.ColorSmLinesHigh);
			pPenLines->setWidth(1);
			pPainter->setPen((*pPenLines));
			pPainter->drawLine(begin - 0.5f*StepdBm, 19, begin - 0.5f*StepdBm, 23);
			pPenLines->setWidth(2);
			pPainter->setPen((*pPenLines));
			pPainter->drawLine(begin, 20, begin, 23);
			strNum = "+" + QString::number(dBmNum);
			int TextW = pFontM->width(strNum);
			pPenLines->setColor(Qt::black);
			pPenLines->setWidth(1);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2 -1, 34, strNum);
			pPenLines->setColor(ColorParam.ColorSmScaleNum);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2 -2, 33, strNum);
			dBmNum += 20;
		}

		pPainter->setFont((*pFontMainNum));
		if(isVHF)
			strNum = QString::number(dBmValDraw -20, 'f', 1) + "dBm";
		else
			strNum = QString::number(dBmValDraw, 'f', 1) + "dBm";
		pPenLines->setColor(Qt::black);
		pPenLines->setWidth(1);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 12, strNum);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 11, strNum);

        QString strV = dBmToText();

		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 35, strV);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 34, strV);
		pPainter->setFont((*pFontScale));
		pPainter->setPen(Qt::NoPen);
		int r, g, b, a;
		ColorParam.ColorSmLimitColor.getRgb(&r, &g, &b, &a);
		pPainter->setBrush(QBrush(QColor(r,g,b,150), Qt::SolidPattern));
		pPainter->drawRect(getPosFromDbm(dBmValMin), 15, getPosFromDbm(dBmValMax)-getPosFromDbm(dBmValMin), 7);
		float pos = getPosFromDbm(dBmValDraw);
		pPenLines->setColor(ColorParam.ColorSmCursorBorderColor);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(ColorParam.ColorSmCursorColor);
		pPainter->drawRect(pos, 5, 3, 27);
		pPainter->drawPixmap(pos+4, 5, 3, 27, (*pImgCursorShadow));
	pPainter->end();
}

void SMeterSDR::SetSMeterVal(double Value)
{
    if(isVHF)
        Value += 20;

	if(Value < -150)
		dBmVal = -150;
	else if(Value > 10)
		dBmVal = 10;
	else
		dBmVal = Value;
}
void SMeterSDR::SetAdcLMeterVal(double Value)
{
	if(Value < -120)
		AdclVal = -120;
	else if(Value > 0)
		AdclVal = 0;
	else
		AdclVal = Value;
}

void SMeterSDR::SetAdcRMeterVal(double Value)
{
	if(Value < -120)
		AdcrVal = -120;
	else if(Value > 0)
		AdcrVal = 0;
	else
		AdcrVal = Value;
}

void SMeterSDR::SetMicMeterVal(double Value)
{
	if(Value < -30)
		MicVal = -30;
	else if(Value > 3)
		MicVal = 3;
	else
		MicVal = Value;
}
void SMeterSDR::SetPowerMeterVal(double Value)
{
	if(Value < 0)
		PowerVal = 0;
	else if(Value > 50)
		PowerVal = 50;
	else
		PowerVal = Value;
}
void SMeterSDR::SetSwrMeterVal(double Value)
{
	if(Value < 1)
		SWRVal = 1;
	else if(Value > 11)
		SWRVal = 11;
	else
        SWRVal = Value;
}

void SMeterSDR::SetTx(bool state)
{
	Tx = state;
	if(Tx)
	{
		switch(TxType)
		{
		case TX_PWR_METER:
			SmType = PWR_METER;
			break;
		case TX_SWR_METER:
			SmType = SWR_METER;
			break;
		case TX_MIC_METER:
		default:
			SmType = MIC_METER;
			break;
		}
	}
	else
	{
		switch(RxType)
		{
		case RX_ADCL:
			SmType = ADCL;
			break;
		case RX_ADCR:
			SmType = ADCR;
			break;
		case RX_SMETER:
		default:
			SmType = SMETER;
			break;
		}
	}
}
void SMeterSDR::SetTxType(int type)
{
	if(!Tx)
		return;

	TxType = (METER_TX_TYPE)type;
	switch(TxType)
	{
	case TX_PWR_METER:
		pTxPwrMeter->setChecked(true);
		SmType = PWR_METER;
		break;
	case TX_SWR_METER:
		pTxSwrMeter->setChecked(true);
		SmType = SWR_METER;
		break;
	case TX_MIC_METER:
	default:
		pTxMicMeter->setChecked(true);
		SmType = MIC_METER;
		break;
	}
}

int SMeterSDR::GetTxType()
{
	return (int)TxType;
}

int SMeterSDR::GetRxType()
{
	return (int)RxType;
}

void SMeterSDR::SetVHF(bool type)
{
	isVHF = type;
}

void SMeterSDR::SetRxType(int type)
{
	if(Tx)
		return;

	RxType = (METER_RX_TYPE)type;

	switch(RxType)
	{
	case RX_ADCL:
		pRxAdclMeter->setChecked(true);
		SmType = ADCL;
		break;
	case RX_ADCR:
		pRxAdcrMeter->setChecked(true);
		SmType = ADCR;
		break;
	case RX_SMETER:
	default:
		pRxSmeter->setChecked(true);
		SmType = SMETER;
		break;
	}
}
void SMeterSDR::GetTxType(QAction * action)
{
	int num = pTxActGroup->actions().indexOf(action);

	if(num == 0)
	{
		if(Tx)
			SmType = MIC_METER;
		TxType = TX_MIC_METER;
	}
	else if(num == 1)
	{
		if(Tx)
			SmType = PWR_METER;
		TxType = TX_PWR_METER;
	}
	else if(num == 2)
	{
		if(Tx)
			SmType = SWR_METER;
		TxType = TX_SWR_METER;
	}
}
void SMeterSDR::GetRxType(QAction * action)
{
	int num = pRxActGroup->actions().indexOf(action);
	if((SMETER_TYPE)num == SMETER)
	{
		if(!Tx)
			SmType = SMETER;
		RxType = RX_SMETER;
	}
	else if((SMETER_TYPE)num == ADCL)
	{
		if(!Tx)
			SmType = ADCL;
		RxType = RX_ADCL;
	}
	else if((SMETER_TYPE)num == ADCR)
	{
		if(!Tx)
			SmType = ADCR;
		RxType = RX_ADCR;
	}
}

void SMeterSDR::drawAdcL()
{
	pPainter->begin(this);
		pPainter->setRenderHint(QPainter::TextAntialiasing);
		int w = width();
		int h = height();
		float Len = w - 2*OFFSET;
		float StepL = Len/6;

		pLineGradient->setColorAt(0.0, ColorParam.ColorSmBackTopColor);
		pLineGradient->setColorAt(1.0, ColorParam.ColorSmBackBottColor);
		pLineGradient->setStart(0, 0);
		pLineGradient->setFinalStop(0, h);
		pPainter->setBrush((*pLineGradient));
		pPainter->setPen(Qt::black);
		pPainter->drawRect(0,0, w-1, h-1);
		pPenLines->setColor(ColorParam.ColorSmLinesLow);
		pPenLines->setWidth(2);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(Qt::NoBrush);
		pPainter->drawLine(OFFSET, 30, w - OFFSET - StepL, 30);
		pPenLines->setColor(ColorParam.ColorSmLinesHigh);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(w - OFFSET - StepL, 30, w - OFFSET, 30);

		int LevelNum = -100;
		QString strNum;
		pPainter->setFont((*pFontScale));
		QFontMetrics fm((*pFontScale));
		for(float begin = OFFSET; begin <= (w - 0.9*OFFSET); begin += StepL)
		{
			if(begin <= (w - OFFSET - 0.9*StepL))
			{
				pPenLines->setColor(ColorParam.ColorSmLinesLow);
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);

				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);
			}
			else
			{
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPenLines->setColor(ColorParam.ColorSmLinesHigh);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);

				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);
			}
			strNum.setNum(LevelNum);
			int TextW = pFontM->width(strNum);
			pPenLines->setColor(Qt::black);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2.0 + StepL, 24, strNum);
			pPenLines->setColor(ColorParam.ColorSmScaleNum);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2.0 -1 + StepL, 23, strNum);
			LevelNum += 20;
		}
		pPainter->setPen(Qt::NoPen);
		int r, g, b, a;
		ColorParam.ColorSmLimitColor.getRgb(&r, &g, &b, &a);
		pPainter->setBrush(QBrush(QColor(r,g,b,150), Qt::SolidPattern));
		pPainter->drawRect(getPosFromAdcl(AdclValMin), 28, getPosFromAdcl(AdclValMax)-getPosFromAdcl(AdclValMin), 5);

		float pos = getPosFromAdcl(AdclValDraw);
		pPenLines->setWidth(1);
		pPenLines->setColor(ColorParam.ColorSmCursorBorderColor);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(ColorParam.ColorSmCursorColor);
		pPainter->drawRect(pos-1, 10, 3, 23);
		pPainter->drawPixmap(pos+3, 10, 3, 23, (*pImgCursorShadow));

		pPainter->setFont((*pFontMainNum));
		strNum = "ADC L: " + QString::number(AdclValDraw, 'f', 1);
		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 12, strNum);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 11, strNum);
	pPainter->end();
}

void SMeterSDR::drawAdcR()
{
	pPainter->begin(this);
		pPainter->setRenderHint(QPainter::TextAntialiasing);
		int w = width();
		int h = height();
		float Len = w - 2*OFFSET;
		float StepL = Len/6;
		pLineGradient->setColorAt(0.0, ColorParam.ColorSmBackTopColor);
		pLineGradient->setColorAt(1.0, ColorParam.ColorSmBackBottColor);
		pLineGradient->setStart(0, 0);
		pLineGradient->setFinalStop(0, h);
		pPainter->setBrush((*pLineGradient));
		pPainter->setPen(Qt::black);
		pPainter->drawRect(0,0, w-1, h-1);

		pPenLines->setColor(ColorParam.ColorSmLinesLow);
		pPenLines->setWidth(2);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(Qt::NoBrush);
		pPainter->drawLine(OFFSET, 30, w - OFFSET - StepL, 30);
		pPenLines->setColor(ColorParam.ColorSmLinesHigh);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(w - OFFSET - StepL, 30, w - OFFSET, 30);
		int LevelNum = -100;
		QString strNum;
		pPainter->setFont((*pFontScale));
		QFontMetrics fm((*pFontScale));
		for(float begin = OFFSET; begin <= (w - 0.9*OFFSET); begin += StepL)
		{
			if(begin <= (w - OFFSET - 0.9*StepL))
			{
				pPenLines->setColor(ColorParam.ColorSmLinesLow);
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);

				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);
			}
			else
			{
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPenLines->setColor(ColorParam.ColorSmLinesHigh);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);

				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);
			}
			strNum.setNum(LevelNum);
			int TextW = pFontM->width(strNum);
			pPenLines->setColor(Qt::black);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2.0 + StepL, 24, strNum);
			pPenLines->setColor(ColorParam.ColorSmScaleNum);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2.0 -1 + StepL, 23, strNum);
			LevelNum += 20;
		}
		pPainter->setPen(Qt::NoPen);
		int r, g, b, a;
		ColorParam.ColorSmLimitColor.getRgb(&r, &g, &b, &a);
		pPainter->setBrush(QBrush(QColor(r,g,b,150), Qt::SolidPattern));
		pPainter->drawRect(getPosFromAdcr(AdcrValMin), 28, getPosFromAdcr(AdcrValMax)-getPosFromAdcr(AdcrValMin), 5);
		float pos = getPosFromAdcr(AdcrValDraw);
		pPenLines->setWidth(1);
		pPenLines->setColor(ColorParam.ColorSmCursorBorderColor);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(ColorParam.ColorSmCursorColor);
		pPainter->drawRect(pos-1, 10, 3, 23);
		pPainter->drawPixmap(pos+3, 10, 3, 23, (*pImgCursorShadow));
		pPainter->setFont((*pFontMainNum));
		strNum = "ADC R: " + QString::number(AdcrValDraw, 'f', 1);
		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 12, strNum);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 11, strNum);
	pPainter->end();
}

void SMeterSDR::drawMic()
{
	pPainter->begin(this);
		pPainter->setRenderHint(QPainter::TextAntialiasing);
		int w = width();
		int h = height();
		float Len = w - 2*OFFSET;
		float StepL = Len/4;
		pLineGradient->setColorAt(0.0, ColorParam.ColorSmBackTopColor);
		pLineGradient->setColorAt(1.0, ColorParam.ColorSmBackBottColor);
		pLineGradient->setStart(0, 0);
		pLineGradient->setFinalStop(0, h);
		pPainter->setBrush((*pLineGradient));
		pPainter->setPen(Qt::black);
		pPainter->drawRect(0,0, w-1, h-1);
		pPenLines->setColor(ColorParam.ColorSmLinesLow);
		pPenLines->setWidth(2);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(Qt::NoBrush);
		pPainter->drawLine(OFFSET, 30, w - OFFSET - StepL, 30);
		pPenLines->setColor(ColorParam.ColorSmLinesHigh);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(w - OFFSET - StepL, 30, w - OFFSET, 30);
		int LevelNum = -20;
		QString strNum;
		pPainter->setFont((*pFontScale));
		QFontMetrics fm((*pFontScale));
		for(float begin = OFFSET; begin <= (w - 0.9*OFFSET); begin += StepL)
		{
			if(begin <= (w - OFFSET - 0.9*StepL))
			{
				pPenLines->setColor(ColorParam.ColorSmLinesLow);
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);
				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);

				if(begin == OFFSET)
					continue;

				strNum.setNum(LevelNum);
				int TextW = pFontM->width(strNum);
				pPenLines->setColor(Qt::black);
				pPainter->setPen((*pPenLines));
				pPainter->drawText(begin - TextW/2.0, 24, strNum);
				pPenLines->setColor(ColorParam.ColorSmScaleNum);
				pPainter->setPen((*pPenLines));
				pPainter->drawText(begin - TextW/2.0 -1, 23, strNum);
				LevelNum += 10;
			}
			else
			{
				pPenLines->setColor(ColorParam.ColorSmLinesHigh);
				pPainter->setPen((*pPenLines));
				float tmp = w - OFFSET - StepL;
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(tmp + (1.0f/3.0f)*StepL, 30, tmp + (1.0f/3.0f)*StepL, 25);
				pPainter->drawLine(tmp + (2.0f/3.0f)*StepL, 30, tmp + (2.0f/3.0f)*StepL, 25);
				pPainter->drawLine(tmp + StepL, 30, tmp + StepL, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(tmp + (1.0f/3.0f)*StepL - (1.0f/6.0f)*StepL, 30, tmp + (1.0f/3.0f)*StepL - (1.0f/6.0f)*StepL, 27);
				pPainter->drawLine(tmp + (2.0f/3.0f)*StepL - (1.0f/6.0f)*StepL, 30, tmp + (2.0f/3.0f)*StepL - (1.0f/6.0f)*StepL, 27);
				pPainter->drawLine(tmp + StepL - (1.0f/6.0f)*StepL, 30, tmp + StepL - (1.0f/6.0f)*StepL, 27);
				pPenLines->setColor(Qt::black);
				pPainter->setPen((*pPenLines));
				strNum.setNum(1);
				int TextW = pFontM->width(strNum);
				pPainter->drawText(tmp + (1.0f/3.0f)*StepL - TextW/2.0+1, 24, strNum);
				strNum.setNum(2);
				TextW = pFontM->width(strNum);
				pPainter->drawText(tmp + (2.0f/3.0f)*StepL - TextW/2.0+1, 24, strNum);
				strNum.setNum(3);
				TextW = pFontM->width(strNum);
				pPainter->drawText(tmp + StepL - TextW/2.0+1, 24, strNum);

				pPenLines->setColor(ColorParam.ColorSmScaleNum);
				pPainter->setPen((*pPenLines));
				strNum.setNum(1);
				TextW = pFontM->width(strNum);
				pPainter->drawText(tmp + (1.0f/3.0f)*StepL - TextW/2.0, 23, strNum);
				strNum.setNum(2);
				TextW = pFontM->width(strNum);
				pPainter->drawText(tmp + (2.0f/3.0f)*StepL - TextW/2.0, 23, strNum);
				strNum.setNum(3);
				TextW = pFontM->width(strNum);
				pPainter->drawText(tmp + StepL - TextW/2.0, 23, strNum);

				break;
			}
		}
		pPainter->setPen(Qt::NoPen);
		int r, g, b, a;
		ColorParam.ColorSmLimitColor.getRgb(&r, &g, &b, &a);
		pPainter->setBrush(QBrush(QColor(r,g,b,150), Qt::SolidPattern));
		pPainter->drawRect(getPosFromMic(MicValMin), 28, getPosFromMic(MicValMax)-getPosFromMic(MicValMin), 5);
		float pos = getPosFromMic(MicValDraw);
		pPenLines->setWidth(1);
		pPenLines->setColor(ColorParam.ColorSmCursorBorderColor);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(ColorParam.ColorSmCursorColor);
		pPainter->drawRect(pos-1, 10, 3, 23);
		pPainter->drawPixmap(pos+3, 10, 3, 23, (*pImgCursorShadow));
		pPainter->setFont((*pFontMainNum));
		strNum = "Mic: " + QString::number(MicValDraw, 'f', 1) + "dB";
		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 12, strNum);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 11, strNum);
	pPainter->end();
}

void SMeterSDR::drawPower()
{
	pPainter->begin(this);
		pPainter->setRenderHint(QPainter::TextAntialiasing);
		int w = width();
		int h = height();
		float Len = w - 2*OFFSET;
		float StepL = Len/5;
		pLineGradient->setColorAt(0.0, ColorParam.ColorSmBackTopColor);
		pLineGradient->setColorAt(1.0, ColorParam.ColorSmBackBottColor);
		pLineGradient->setStart(0, 0);
		pLineGradient->setFinalStop(0, h);
		pPainter->setBrush((*pLineGradient));
		pPainter->setPen(Qt::black);
		pPainter->drawRect(0,0, w-1, h-1);
		pPenLines->setColor(ColorParam.ColorSmLinesLow);
		pPenLines->setWidth(2);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(Qt::NoBrush);
		pPainter->drawLine(OFFSET, 30, w - OFFSET - StepL, 30);
		pPenLines->setColor(ColorParam.ColorSmLinesHigh);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(w - OFFSET - StepL, 30, w - OFFSET, 30);
		int LevelNum = 10;
		QString strNum;
		pPainter->setFont((*pFontScale));
		QFontMetrics fm((*pFontScale));
		for(float begin = OFFSET; begin <= (w - 0.9*OFFSET); begin += StepL)
		{
			if(begin <= (w - OFFSET - 0.9*StepL))
			{
				pPenLines->setColor(ColorParam.ColorSmLinesLow);
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);
				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);
			}
			else
			{
				pPenLines->setColor(ColorParam.ColorSmLinesHigh);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 26);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);
				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);
			}
			strNum.setNum(LevelNum);
			int TextW = pFontM->width(strNum);
			pPenLines->setColor(Qt::black);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2.0 + StepL, 24, strNum);
			pPenLines->setColor(ColorParam.ColorSmScaleNum);
			pPainter->setPen((*pPenLines));
			pPainter->drawText(begin - TextW/2.0 -1 + StepL, 23, strNum);
			LevelNum += 10;
		}
		float pos = getPosFromPower(PowerVal);
		pPenLines->setWidth(1);
		pPenLines->setColor(ColorParam.ColorSmCursorBorderColor);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(ColorParam.ColorSmCursorColor);
		pPainter->drawRect(pos-1, 10, 3, 23);
		pPainter->drawPixmap(pos+3, 10, 3, 23, (*pImgCursorShadow));
		pPainter->setFont((*pFontMainNum));
		strNum = "Power: " + QString::number(PowerVal, 'f', 1) + "W";
		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 12, strNum);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 11, strNum);
	pPainter->end();
}

void SMeterSDR::drawSWR()
{
	pPainter->begin(this);
		pPainter->setRenderHint(QPainter::TextAntialiasing);
		int w = width();
		int h = height();
		float Len = w - 2*OFFSET;
		float StepL = Len/4;
		pLineGradient->setColorAt(0.0, ColorParam.ColorSmBackTopColor);
		pLineGradient->setColorAt(1.0, ColorParam.ColorSmBackBottColor);
		pLineGradient->setStart(0, 0);
		pLineGradient->setFinalStop(0, h);
		pPainter->setBrush((*pLineGradient));
		pPainter->setPen(Qt::black);
		pPainter->drawRect(0,0, w-1, h-1);
		pPenLines->setColor(ColorParam.ColorSmLinesLow);
		pPenLines->setWidth(2);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(Qt::NoBrush);
		pPainter->drawLine(OFFSET, 30, w - OFFSET - Len/2.0, 30);
		pPenLines->setColor(ColorParam.ColorSmLinesHigh);
		pPainter->setPen((*pPenLines));
		pPainter->drawLine(w - OFFSET - Len/2.0, 30, w - OFFSET, 30);
		int LevelNum = 2;
		QString strNum;
		pPainter->setFont((*pFontScale));
		QFontMetrics fm((*pFontScale));
		for(float begin = OFFSET; begin <= (w - 0.9*OFFSET); begin += StepL)
		{
			if(begin <= (w - OFFSET - Len/2.1))
			{
				pPenLines->setColor(ColorParam.ColorSmLinesLow);
				pPenLines->setWidth(2);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin, 30, begin, 25);
				pPenLines->setWidth(1);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(begin-StepL*0.5, 30, begin-StepL*0.5, 25);
				pPainter->drawLine(begin-StepL*0.25, 30, begin-StepL*0.25, 27);
				pPainter->drawLine(begin-StepL*0.75, 30, begin-StepL*0.75, 27);

				if(begin == OFFSET)
					continue;

				strNum.setNum(LevelNum);
				int TextW = pFontM->width(strNum);
				pPenLines->setColor(Qt::black);
				pPainter->setPen((*pPenLines));
				pPainter->drawText(begin - TextW/2.0+1, 24, strNum);
				pPenLines->setColor(ColorParam.ColorSmScaleNum);
				pPainter->setPen((*pPenLines));
				pPainter->drawText(begin - TextW/2.0, 23, strNum);
				LevelNum += 1;
			}
			else
			{
				pPenLines->setColor(ColorParam.ColorSmLinesHigh);
				pPainter->setPen((*pPenLines));
				pPainter->drawLine(getPosFromSWR(5), 30, getPosFromSWR(5), 26);
				pPainter->drawLine(getPosFromSWR(10), 30, getPosFromSWR(10), 26);
				pPenLines->setColor(Qt::black);
				pPainter->setPen((*pPenLines));
				strNum.setNum(5);
				int TextW = pFontM->width(strNum);
				pPainter->drawText(getPosFromSWR(5) - TextW/2.0+1, 24, strNum);
				strNum.setNum(10);
				TextW = pFontM->width(strNum);
				pPainter->drawText(getPosFromSWR(10) - TextW/2.0+1, 24, strNum);
				pPenLines->setColor(ColorParam.ColorSmScaleNum);
				pPainter->setPen((*pPenLines));
				strNum.setNum(5);
				TextW = pFontM->width(strNum);
				pPainter->drawText(getPosFromSWR(5) - TextW/2.0, 23, strNum);
				strNum.setNum(10);
				TextW = pFontM->width(strNum);
				pPainter->drawText(getPosFromSWR(10) - TextW/2.0, 23, strNum);
				break;
			}
		}
		float pos = getPosFromSWR(SWRVal);
		pPenLines->setWidth(1);
		pPenLines->setColor(ColorParam.ColorSmCursorBorderColor);
		pPainter->setPen((*pPenLines));
		pPainter->setBrush(ColorParam.ColorSmCursorColor);
		pPainter->drawRect(pos-1, 10, 3, 23);
		pPainter->drawPixmap(pos+3, 10, 3, 23, (*pImgCursorShadow));
		pPainter->setFont((*pFontMainNum));
		strNum = "SWR: " + QString::number(SWRVal, 'f', 1);
		pPenLines->setColor(Qt::black);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(3, 12, strNum);
		pPenLines->setColor(ColorParam.ColorSmDbmNum);
		pPainter->setPen((*pPenLines));
		pPainter->drawText(2, 11, strNum);
	pPainter->end();
}

void SMeterSDR::saveSettings(QSettings *pSettings)
{
	SM_PARAM tmp = pSmOpt->getSettingsParam();
	pSettings->beginGroup("SmeterGui");
		pSettings->setValue("ColorSmBackBottColor", tmp.ColorSmBackBottColor);
		pSettings->setValue("ColorSmBackTopColor", tmp.ColorSmBackTopColor);
		pSettings->setValue("ColorSmCursorBorderColor", tmp.ColorSmCursorBorderColor);
		pSettings->setValue("ColorSmCursorColor", tmp.ColorSmCursorColor);
		pSettings->setValue("ColorSmDbmNum",  tmp.ColorSmDbmNum);
		pSettings->setValue("ColorSmLimitColor",  tmp.ColorSmLimitColor);
		pSettings->setValue("ColorSmLinesHigh", tmp.ColorSmLinesHigh);
		pSettings->setValue("ColorSmLinesLow", tmp.ColorSmLinesLow);
		pSettings->setValue("ColorSmSQLBorderColor", tmp.ColorSmSQLBorderColor);
		pSettings->setValue("ColorSmSQLCursorColor", tmp.ColorSmSQLCursorColor);
		pSettings->setValue("ColorSmScaleNum", tmp.ColorSmScaleNum);
		pSettings->setValue("ColorSmStyle", tmp.Style);
		pSettings->setValue("RxType", GetRxType());
		pSettings->setValue("TxType", GetTxType());
	pSettings->endGroup();
}
void SMeterSDR::restoreSettings(QSettings *pSettings)
{
	SM_PARAM tmp;
	pSettings->beginGroup("SmeterGui");
        tmp.ColorSmBackBottColor = pSettings->value("ColorSmBackBottColor", QColor(0, 0, 0)).value<QColor>();
        tmp.ColorSmBackTopColor = pSettings->value("ColorSmBackTopColor", QColor(0, 0, 0)).value<QColor>();
        tmp.ColorSmCursorBorderColor = pSettings->value("ColorSmCursorBorderColor", QColor(Qt::black)).value<QColor>();
        tmp.ColorSmCursorColor = pSettings->value("ColorSmCursorColor", QColor(Qt::white)).value<QColor>();
        tmp.ColorSmDbmNum = pSettings->value("ColorSmDbmNum", QColor(Qt::yellow)).value<QColor>();
		tmp.ColorSmLimitColor = pSettings->value("ColorSmLimitColor", QColor(255, 255, 255, 150)).value<QColor>();
        tmp.ColorSmLinesHigh = pSettings->value("ColorSmLinesHigh", QColor(255, 0, 0)).value<QColor>();
        tmp.ColorSmLinesLow = pSettings->value("ColorSmLinesLow", QColor(Qt::green)).value<QColor>();
        tmp.ColorSmSQLBorderColor = pSettings->value("ColorSmSQLBorderColor", QColor(Qt::black)).value<QColor>();
        tmp.ColorSmSQLCursorColor = pSettings->value("ColorSmSQLCursorColor", QColor(Qt::yellow)).value<QColor>();
        tmp.ColorSmScaleNum = pSettings->value("ColorSmScaleNum", QColor(Qt::white)).value<QColor>();
        tmp.Style = pSettings->value("ColorSmStyle", (int)USER).toInt();
        SetRxType(pSettings->value("RxType", 0).toInt());
        SetTxType(pSettings->value("TxType", 0).toInt());

        TxType = (METER_TX_TYPE)(pSettings->value("TxType", 0).toInt());
        switch(TxType)
        {
        case TX_PWR_METER:
            pTxPwrMeter->setChecked(true);
            break;
        case TX_SWR_METER:
            pTxSwrMeter->setChecked(true);
            break;
        case TX_MIC_METER:
        default:
            pTxMicMeter->setChecked(true);
            break;
        }

    pSettings->endGroup();
    pSmOpt->setSettingsParam(tmp);
}
