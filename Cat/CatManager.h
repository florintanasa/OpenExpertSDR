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

#ifndef CATMANAGER_H
#define CATMANAGER_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

#include "Cat/QextSerialPort/qextserialport.h"
#include "../expertsdr_va2_1.h"
#include "expertsdr_va2_1.h"
#include "../Options.h"

typedef enum
{
	ERROR_CMD,
	ERROR_PARAM
}ERROR_CAT;

#define MAX_RX_BUF_SIZE		1024
#define MAX_TX_BUF_SIZE		1024

const QString StepToCat[] = { "0000", "0001", "0001", "1000", "1000", "1001", "1001", "1010", "0011", "1011", "0100", "0100", "0101", "0101", "0101", "0110", "0110", "0111", "0111"};
const QString ModToCat[] = {"1", "2", "0", "7", "3", "4", "5", "9", "0", "6", "0", "0"};
const int CatToMod[] = {-1, 0, 1, 4, 5, 6, 9, 3, -1, 7};

class ExpertSDR_vA2_1;

class CatManager : public QWidget
{
    Q_OBJECT

    int TimerId;
    char RxData[MAX_RX_BUF_SIZE];
    QStringList Param;
    QextSerialPort *pCom;

    Options *pOpt;
    ExpertSDR_vA2_1 *pSdr;

public:
    CatManager(ExpertSDR_vA2_1 *pE, Options *pO, QWidget *parent = 0);
    ~CatManager();

    bool IsOpen();

private:

    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

    void SearchCmdList(QByteArray *pArray, QStringList *pList);
    void PerformCmd(QString str);
    short GetNameCmd(QString str);

    void SentErrorCode(ERROR_CAT code);

public slots:
	void Open(bool status);
	void Close();
	void comReceive();
	void OnKeyDsr(bool stat);
	void OnKeyCts(bool stat);

signals:
	void changedVolume(int val);
	void changedBand(int val);
	void changetStep(int val);
	void changedVFO_A(int freq);
	void changedVFO_B(int freq);
	void changedAGC(int val);
	void changetCWLSpeed(int val);
	void changedMode(int val);
	void changedGainMicrophone(int val);
	void changedNB(bool state);
	void changedANF(bool state);
	void changedPWR(int val);
	void changedCOMP(bool state);
	void changedOnOff(bool state);
	void changedFilterHighFreq(int val);
	void changedFilterLowFreq(int val);
	void changedSQLval(int val);
	void changedSQL(bool state);
	void changedIncrDecrVFO_A(int val);
	void changedXIT(bool state);
	void changedRIT(bool state);
	void changedRX0();
	void txMode();
	void rxMode();
    void PttChanged(bool);
    void KeyChanged(bool);
};

#endif // CATMANAGER_H
