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

#ifndef CWTIMER_H_
#define CWTIMER_H_



#include <QtWidgets>

#ifdef Q_OS_WIN
#include "qt_windows.h"
#define USE_WINMM_TIMER	1
#if USE_WINMM_TIMER
    #include "mmsystem.h"
#endif
#endif /*Q_OS_WIN*/

#if USE_WINMM_TIMER
class CwTimer  : public QWidget
#else
class CwTimer  : public QThread
#endif
{
	Q_OBJECT

	public:
		CwTimer(QWidget *parent = 0);
		~CwTimer();
		void startTimer(quint32 ms);
		bool isStart();
		void stopTimer();
		void emitTimeout();

	private:

		bool isStarted;

#if USE_WINMM_TIMER
        LARGE_INTEGER m_liPerfFreq;
        LARGE_INTEGER m_liPerfStart;

		int TimerID;
#else
		void run();
#endif

	signals:
		void timeout();
};

#endif /* CWTIMER_H_ */
