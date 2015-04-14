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

#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "ui_Calibrator.h"
#include "CalibrProc.h"

class Calibrator : public QDialog
{
    Q_OBJECT

    	CalibrProc *pCalibrProc;
        bool xvtrxEnable;

	public:
		Calibrator(QWidget *parent = 0);
		~Calibrator();
        void setParam(int freq, int fltFreq, int mode, int filtId, int preamp);
        void setXvtrx(bool en);

	private slots:
        void onBack();
		void onSkip();
		void onNext();
		void onCalFreq(double freq);
		void onCalLevel(double freq);
        void onEnd();

	private:
		Ui::CalibratorClass ui;
		void showEvent(QShowEvent* e);
		void closeEvent(QCloseEvent *event);

        int freqVal;
        int fltFreqVal;
        int modeVal;
        int filtIdVal;
        int preampVal;

	signals:
		void showCalibration();
        void push();
        void pop();
        void getFreqDbm();
        void calibrationFreqCoeff(float val);
        void getDbm();
        void setDbm(float val);
        void calibrationDbmCoeff(int num, float val);
        void setWinType(int type);

		void freq(int);
		void ifFreq(int);
		void mode(int);
		void filtId(int);
		void preamp(int);
		void emitFreqDbm(int freq, float dbm);

};

#endif // CALIBRATOR_H
