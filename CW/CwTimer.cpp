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

#include "CwTimer.h"

#if USE_WINMM_TIMER

void CALLBACK TimerProc(UINT uTimerID, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CwTimer *p = (CwTimer*)dwUser;
	p->emitTimeout();
}

CwTimer::CwTimer(QWidget *parent) : QWidget(parent)
{
    TimerID = 0;
    isStarted = false;
}

CwTimer::~CwTimer()
{
    if(TimerID) timeKillEvent(TimerID);
    stopTimer();
}

void CwTimer::startTimer(quint32 ms)
{
	TimerID = timeSetEvent(ms, 1, TimerProc, (DWORD_PTR)this, TIME_PERIODIC);
	isStarted = true;
}

bool CwTimer::isStart()
{
	return isStarted;
}

void CwTimer::stopTimer()
{
	if(TimerID) timeKillEvent(TimerID);
    TimerID = 0;
    isStarted = false;
}

void CwTimer::emitTimeout()
{
	emit timeout();
}
#else

CwTimer::CwTimer(QWidget *parent) : QThread(parent)
{
	isStarted = false;
}

CwTimer::~CwTimer()
{
	if(isRunning())
	{
        exit();
		wait();
	}
}

void CwTimer::startTimer(quint32 ms)
{
	start(QThread::HighestPriority);
	isStarted = true;
}

bool CwTimer::isStart()
{
	return isStarted;
}

void CwTimer::stopTimer()
{
    exit();
	wait();
	isStarted = false;
}

void CwTimer::run()
{
	QTimer t;
	connect(&t, SIGNAL(timeout()), this, SIGNAL(timeout()), Qt::DirectConnection);
	t.start(2);
	exec();
}
#endif
