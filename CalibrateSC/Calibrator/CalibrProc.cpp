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

#include "CalibrProc.h"

CalibrProc::CalibrProc(QWidget *parent) : QThread(parent)
{
	readyFreqDmb = false;
}

CalibrProc::~CalibrProc()
{
	if(isRunning())
	{
		terminate();
		wait();
	}
}

void CalibrProc::run()
{
    mut.lock();
    CALIBRATION_TYPE type = CalibrationType;
    mut.unlock();

    int i = 0, j = 0;
    switch(type)
	{
		case FREQ_HF:
            emit progress(0);
            emit push();
            emit ifFreq(12000);
            emit freq(freqVal);
            emit mode(AM);
            emit preamp(0);
            emit progress(5);
            emit setWinType(0);
            emit calibrationFreqCoeff(1.0f);
            readyFreqDmb = false;
            msleep(1000);
            emit getFreqDbm();
            mut.lock();
            while(!readyFreqDmb)
            {
            	mut.unlock();
            	msleep(50);
            	mut.lock();
            }
            mut.unlock();
            vecFreq.clear();
            vecDbm.clear();

            emit progress(10);
            for(i = -30; i < 30; i++)
            {
                emit freq(freqVal + i);
                  msleep(30);
                readyFreqDmb = false;
                emit getFreqDbm();
                mut.lock();
                while(!readyFreqDmb)
                {
                	mut.unlock();
                	msleep(10);
                	mut.lock();
                }
                mut.unlock();
                vecFreq << Freq;
                vecDbm  << Dbm;
                //
                emit progress(40+i);
            }
            Freq = vecFreq.at(0);
            Dbm = vecDbm.at(0);
            emit progress(95);
            for(i = 1; i < vecDbm.count(); i++)
            {
            	if(Dbm < vecDbm.at(i))
            	{
            		Dbm = vecDbm.at(i);
            		Freq = vecFreq.at(i);
            	}
            }
            emit calibrationFreqCoeff((float)Freq/ (float)freqVal);
            emit pop();
            emit progress(100);
        break;

        case SMETER_HF:
            emit progress(0);
            emit push();
            emit ifFreq(12000);
            emit freq(freqVal);
            emit mode((int)AM);
            emit preamp(0);
            emit setWinType(11);
            emit progress(5);
            for(j = 0; j < 5; j++)
            {
            	emit preamp(j);
            	msleep(1000);
                vecDbm.clear();
				for(i = 0; i < 10; i++)
				{
					readyFreqDmb = false;
					emit getDbm();
					mut.lock();
					while(!readyFreqDmb)
					{
						mut.unlock();
						msleep(10);
						mut.lock();
					}
                    mut.unlock();
					vecDbm << Dbm;
					msleep(25);
				}
				emit progress(20*(j+1));
				Dbm = 0;
                for(i = 0; i < vecDbm.count(); i++)
	            	Dbm += vecDbm.at(i);
                qDebug() << vecDbm;
	            Dbm /= vecDbm.count();
	            qDebug() << "Dbm: " << Dbm;
	            Dbm += LEVEL_SMETER;
				emit calibrationDbmCoeff(j , smeterVal - Dbm);
            }
            emit pop();
            emit progress(100);
            break;

		case POWER_HF:
			break;
		case FREQ_VHF:
			break;
		case SMETER_VHF:
			break;
		case FREQ_UHF:
			break;
		case SMETER_UHF:
			break;
		default:
			break;
	}
	emit end();
}

void CalibrProc::setFreqValue(double val)
{
	mut.lock();
    freqVal = val;
    mut.unlock();
}

void CalibrProc::setSmeterValue(double val)
{
	mut.lock();
	smeterVal = val;
	mut.unlock();
}

void CalibrProc::begin(CALIBRATION_TYPE cal)
{
	QMutexLocker locker(&mut);
	CalibrationType = cal;
	start(QThread::LowPriority);
}

bool CalibrProc::isProcess()
{
	return isRunning();
}

void CalibrProc::setFreqDbm(int freq, float dbm)
{
    Freq = freq;
    Dbm = dbm;
    mut.lock();
    readyFreqDmb = true;
    mut.unlock();
}

void CalibrProc::setDbm(float val)
{
    mut.lock();
	Dbm = val;    
    readyFreqDmb = true;
    mut.unlock();
}
