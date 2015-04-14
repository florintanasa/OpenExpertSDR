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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif

#include <QWidget>
#include <QDesktopServices>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QFileInfo>
#include "ui_Options.h"
#include <QSysInfo>

#include "Defines.h"
#include "Cat/QextSerialPort/qextserialport.h"
#include "Cat/QextSerialPort/qextserialenumerator.h"

class Options : public QWidget
{
		Q_OBJECT

		QStringList arguments;
		QFileInfo FileInfo;

		QProcess *pProg0;
		QProcess *pProg1;
		QProcess *pProg2;
		QProcess *pProg3;
		QProcess *pProg4;
		QByteArray GlitchBuf;

		QString DirProg0;
		QString DirProg1;
		QString DirProg2;
		QString DirProg3;
		QString DirProg4;

		QString pathDefaultWaveIQ;
		QString pathDefaultWaveIQDefault;

        //QSerialPort	*pPttPort;
        //QSerialPort	*pKeyPort;
        //QSerialPort	*pAddKeyPort;
        QextSerialPort	*pPttPort;
        QextSerialPort	*pKeyPort;
        QextSerialPort	*pAddKeyPort;

	public:
		Options(QWidget *parent = 0);
		~Options();

		Ui::OptionsClass ui;

		QString PathProg0;
		QString PathProg1;
		QString PathProg2;
		QString PathProg3;
		QString PathProg4;

		void StartProgs();
		void StopProgs();

		void SetProg0(bool IsStarted, QString Path);
		void SetProg1(bool IsStarted, QString Path);
		void SetProg2(bool IsStarted, QString Path);
		void SetProg3(bool IsStarted, QString Path);
		void SetProg4(bool IsStarted, QString Path);

		void GetPowerCorrection(BAND_MODE Band, float *pValDb);
		void SetPowerCorrection(BAND_MODE Band, float ValDb);

		int GetWindowType();

		float getSampleRate();
		void setSampleRate(uint value);

		void setWaveFilesDirLocation(QString path);
		QString getWaveFilesDirLocation();
		QString getWaveFilesDirLocationDefault();

    private:
        void SetupPluginList();

	private slots:
		void ChangePage(QListWidgetItem *current, QListWidgetItem *previous);

		void OnOK();
		void OnCancel();

		void OnTxGainChange(int Val);
		void OnTxPhaseChange(int Val);
        void OnTxGainChangeCw(int Val);
        void OnTxPhaseChangeCw(int Val);
        void OnTxGainChangeAmFm(int Val);
        void OnTxPhaseChangeAmFm(int Val);
		void OnProg0();
		void OnProg1();
		void OnProg2();
		void OnProg3();
		void OnProg4();
		void OnEnableExControl(int State);
		void LaunchProgs();
		void PowerCorrect160(int Val);
		void PowerCorrect80(int Val);
		void PowerCorrect60(int Val);
		void PowerCorrect40(int Val);
		void PowerCorrect30(int Val);
		void PowerCorrect20(int Val);
		void PowerCorrect17(int Val);
		void PowerCorrect15(int Val);
		void PowerCorrect12(int Val);
		void PowerCorrect10(int Val);
		void PowerCorrect6(int Val);
        void PowerCorrect2(int Val);
        void PowerCorrect07(int Val);
		void OnPttDsr(bool stat);
		void OnPttCts(bool stat);
		void OnKeyDsr(bool stat);
		void OnKeyCts(bool stat);
		void OnAddKeyDsr(bool stat);
		void OnAddKeyCts(bool stat);
		void soundChanged(int val);
		void soundVacChanged(int val);
		void soundDrvChanged(int val);
		void soundVacDrvChanged(int val);
		void openWaveDir();
		void viewLocationLogFile();

	public slots:
		void OnGlitchCompChanged(int Val);
		void pttOpen(bool stat);
		void keyOpen(bool stat);
		void addKeyOpen(bool stat);
        void OnSdrType(int Type);
        void SetWindowType(int type);
        void onEnableXvtrx(bool stat);
        void onSdrTypeChanged(int index);

	signals:
		void SoundCardOptChanged();
		void driverChanged(int);
		void driverVacChanged(int);
		void RxMagRej(double);
		void RxPhaseRej(double);
		void TxMagRej(double);
		void TxPhaseRej(double);
        void TxMagRejCw(double);
        void TxPhaseRejCw(double);
        void TxMagRejAmFm(double);
        void TxPhaseRejAmFm(double);
		void PowerCorrectChanged160(int Val);
		void PowerCorrectChanged80(int Val);
		void PowerCorrectChanged60(int Val);
		void PowerCorrectChanged40(int Val);
		void PowerCorrectChanged30(int Val);
		void PowerCorrectChanged20(int Val);
		void PowerCorrectChanged17(int Val);
		void PowerCorrectChanged15(int Val);
		void PowerCorrectChanged12(int Val);
		void PowerCorrectChanged10(int Val);
		void PowerCorrectChanged6(int Val);
        void PowerCorrectChanged2(int Val);
        void PowerCorrectChanged07(int Val);
		void GlitchCompChanged(QByteArray);
        void PttChanged(bool);
        void KeyChanged(bool);
        void DashChanged(bool);
        void DotChanged(bool);
        void XvtrxEnable(bool);
		void wavePathChanged(QString path);
        void SdrPluginChanged(QString);

};

#endif // OPTIONS_H
