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

#ifndef SMETERSDR_H
#define SMETERSDR_H

#include <QtGui>
#include <QWidget>
#include "SMeterOptions.h"

#define OFFSET 5
#define DBM_BUF_SIZE 255

class QAction;
class QActionGroup;
class QMenu;

class SMeterSDR : public QWidget
{
    Q_OBJECT

    typedef enum
    {
    	SMETER = 0,
    	ADCL,
    	ADCR,
    	MIC_METER,
    	PWR_METER,
    	SWR_METER
    }SMETER_TYPE;

    typedef enum
    {
    	RX_SMETER = 0,
    	RX_ADCL,
    	RX_ADCR
    }METER_RX_TYPE;

    typedef enum
    {
    	TX_MIC_METER = 0,
    	TX_PWR_METER,
    	TX_SWR_METER
    }METER_TX_TYPE;

    SMeterOptions *pSmOpt;

    QPainter *pPainter;
    QFont *pFontScale;
    QFont *pFontMainNum;
    QFontMetrics *pFontM;
    QPen *pPenLines;
    QPixmap *pImgBackground;
    QPixmap *pImgCursor;
    QPixmap *pImgCursorShadow;
    QPixmap *pImgSql;
    QLinearGradient *pLineGradient;

    float StepdBm;
    float StepS;
    float StepLevel;

    float dBmVal;
    float dBmValMin;
    float dBmValMax;
    float dBmValDraw;
    float *pBuffDbm;
    int dBmLimitsCnt;
    QTimer timerGenVal;
    QTimer timerChangeDbmVal;

    float AdcrVal;
    float AdcrValMin;
    float AdcrValMax;
    float AdcrValDraw;
    float *pBuffAdcr;
    int AdcrLimitsCnt;

    float AdclVal;
    float AdclValMin;
    float AdclValMax;
    float AdclValDraw;
    float *pBuffAdcl;
    int AdclLimitsCnt;

    float MicVal;
    float MicValMin;
    float MicValMax;
    float MicValDraw;
    float *pBuffMic;
    int MicLimitsCnt;

    float PowerVal;
    float PowerValMin;
    float PowerValMax;
    float PowerValDraw;
    float *pBuffPower;
    int PowerLimitsCnt;

    float SWRVal;
    float SWRValMin;
    float SWRValMax;
    float SWRValDraw;
    float *pBuffSWR;
    int SWRLimitsCnt;

    bool SqlOn;
    float SqlVal;
    float sSql, dSql;
    SM_PARAM ColorParam;

    QMenu *pMenuRx;
    QMenu *pMenuTx;
    QAction *pRx;
    QAction *pTx;

    QActionGroup *pRxActGroup;
    QAction *pRxSmeter;
    QAction *pRxAdclMeter;
    QAction *pRxAdcrMeter;

    QActionGroup *pTxActGroup;
    QAction *pTxMicMeter;
    QAction *pTxPwrMeter;
    QAction *pTxSwrMeter;

    bool Tx;
    METER_RX_TYPE RxType;
    METER_TX_TYPE TxType;
    SMETER_TYPE SmType;

    bool isVHF;

public:
    SMeterSDR(QWidget *pOpt, QWidget *parent = 0);
    ~SMeterSDR();

    void SetSMeterVal(double Value);
    void SetAdcLMeterVal(double Value);
    void SetAdcRMeterVal(double Value);
    void SetMicMeterVal(double Value);
    void SetPowerMeterVal(double Value);
    void SetSwrMeterVal(double Value);

    void SetTx(bool state);
    void SetTxType(int type);
    void SetRxType(int type);
    int GetTxType();
    int GetRxType();
    void SetVHF(bool type);

    void saveSettings(QSettings *pS);
    void restoreSettings(QSettings *pS);

    QString dBmToText();
private:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    void drawDbm();
    void drawAdcL();
    void drawAdcR();
    void drawMic();
    void drawPower();
    void drawSWR();

    float getPosFromDbm(float dBm);
    float getdBmMin();
    float getDbmMax();

    float getPosFromAdcl(float val);
    float getAdclMin();
    float getAdclMax();

    float getPosFromAdcr(float val);
    float getAdcrMin();
    float getAdcrMax();

    float getPosFromMic(float val);
    float getMicMin();
    float getMicMax();

    float getPosFromPower(float val);
    float getPowerMin();
    float getPowerMax();

    float getPosFromSWR(float val);
    float getSWRMin();
    float getSWRMax();

private slots:
	void setValRnd();
	void setChangeDbmVal();
	void setChangeDbmLimits();

	void setColorParam(SM_PARAM param);

	void GetRxType(QAction * action);
	void GetTxType(QAction * action);
};

#endif // SMETERSDR_H
