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

#ifndef CALIBRPROC_H_
#define CALIBRPROC_H_

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QThread>
#include "../../Defines.h"

class CalibrProc : public QThread
{
	Q_OBJECT

    public:
		typedef enum
		{
			FREQ_HF = 1,
			SMETER_HF,
			POWER_HF,
			FREQ_VHF,
			SMETER_VHF,
			FREQ_UHF,
            SMETER_UHF
		}CALIBRATION_TYPE;

    private:
        CALIBRATION_TYPE CalibrationType;
        bool readyFreqDmb;
        int Freq;
        float Dbm;
        QVector<int> vecFreq;
        QVector<float> vecDbm;

    public:
         CalibrProc(QWidget *parent = 0);
		 ~CalibrProc();

		 void begin(CALIBRATION_TYPE cal);
		 bool isProcess();

	private:
		 void run();

		 QMutex mut;
		 int freqVal;
		 double smeterVal;

    public slots:
         void setFreqValue(double val);
         void setSmeterValue(double val);
         void setFreqDbm(int freq, float dbm);
         void setDbm(float val);

	signals:
         void push();
         void pop();
         void progress(int);
         void getFreqDbm();
         void calibrationFreqCoeff(float val);
         void getDbm();
         void calibrationDbmCoeff(int num, float val);
         void setWinType(int type);

         void freq(int);
         void ifFreq(int);
         void mode(int);
         void filtId(int);
         void preamp(int);
		 void end();

};

#endif /* CALIBRPROC_H_ */
