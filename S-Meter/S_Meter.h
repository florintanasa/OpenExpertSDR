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

#ifndef S_METER_H
#define S_METER_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#include <QtWidgets/QWidget>
#else
#include <QtGui>
#include <QtGui/QWidget>
#endif

#include "ui_S_Meter.h"
#include "Draw.h"
#include "Number.h"
#include "Number2.h"

class S_Meter : public QWidget
{
    Q_OBJECT

    QPoint dragPosition, oldPos;
    Draw *pDraw;

    QMenu *pMenuBand;
    QMenu *pMenuMod;
    QActionGroup *pMgrBand;
    QActionGroup *pMgrMod;
    int IndexBand;
    int IndexMod;
    int Frequency;
    float EDbm;
    int VolumeVal;
    bool EMute;
    bool ESql;
    int EBand;
    int SdrMod;

    int MinFreq;
    int MaxFreq;

    int Pitch;

    Number2 *pNum0;
    Number2 *pNum1;
    Number2 *pNum2;
    Number *pNum3;
    Number *pNum4;
    Number *pNum5;
    Number *pNum6;
    Number *pNum7;
    Number *pNum8;

public:
    S_Meter(QWidget *parent = 0);

    Ui::S_MeterClass ui;

    void setSettings(QVariant settings);
    QVariant getSettings();

    void setLockBand(bool state);
    void setLockMode(bool state);

private:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);

    void InitNum();

public slots:
	void SetFreq(int val);
	void SetDbm(float dB);
	void SetVolume(int val);
	void SetMute(bool val);
	void SetSQL(bool val);
	void SetBand(int val);
	void SetMod(int val);
	void SetPitch(int val);

    void SetLockFreq(bool state);

private slots:
	void ChangeMod(QAction* action);
	void ChangeBand(QAction* action);
	void ChangedVolume(int val);

	void ChangedTranceparent();

	//
	void ChangetFreq0(int Num);
	void ChangetFreq1(int Num);
	void ChangetFreq2(int Num);
	void ChangetFreq3(int Num);
	void ChangetFreq4(int Num);
	void ChangetFreq5(int Num);
	void ChangetFreq6(int Num);
	void ChangetFreq7(int Num);
	void ChangetFreq8(int Num);

signals:
	void FreqChange(int val);
	void VolumeChange(int val);
	void ModChange(int val);
	void BandChange(int val);
	void Close();
};

#endif // S_METER_H
