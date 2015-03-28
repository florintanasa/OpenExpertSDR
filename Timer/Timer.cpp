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
/*
#include "Timer.h"

Timer::Timer()
{
	init();
	calcLatency();
	start();
}

void Timer::init()
{
	available = QueryPerformanceFrequency(&frequency);
	startCount.QuadPart = 0;
	count.QuadPart = 0;
}

void Timer::calcLatency()
{
	minMicroSecondsLatency = 0;
	normMicroSecondsLatency = 0;

	if (!available)
		return;

	double minimumAccumulation = 0;
	int minimumAccumulations = 0;
	double normalAccumulation = 0;
	int normalAccumulations = 0;

	for (int i = 10; i > 0; i--)
	{
		LARGE_INTEGER tick1;
		LARGE_INTEGER tick2;
		QueryPerformanceCounter(&tick1);
		QueryPerformanceCounter(&tick2);
		double begin = tick1.QuadPart * (1000000.0 / frequency.QuadPart);
		double end = tick2.QuadPart * (1000000.0 / frequency.QuadPart);
		minimumAccumulation += end - begin;
		minimumAccumulations++;

		start();
		normalAccumulation += getMicroSeconds();
		normalAccumulations++;
	}

	if (minimumAccumulations > 0)
		minMicroSecondsLatency = minimumAccumulation / minimumAccumulations;

	if (normalAccumulations > 0)
		normMicroSecondsLatency = normalAccumulation / normalAccumulations;
}

void Timer::start()
{
	if (!available)
		return;

	QueryPerformanceCounter(&startCount);
}

double Timer::getMicroSeconds()
{
	if (!available)
		return 0;

	QueryPerformanceCounter(&count);
	double microSecondsStart = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
	double microSecondsEnd = count.QuadPart * (1000000.0 / frequency.QuadPart);

	double us = (microSecondsEnd - microSecondsStart) - normMicroSecondsLatency;
	if (us < 0)
		us = 0;

	return (us);
}

double Timer::getMilliSeconds()
{
	return (getMicroSeconds() * 0.001);
}

double Timer::getSeconds()
{
	return (getMilliSeconds() * 0.000001);
}
*/
//////////////////////////////////////////////////////////////////////////////
// Timer.cpp
// =========
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#include "Timer.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// constructor
///////////////////////////////////////////////////////////////////////////////
Timer::Timer()
{
#ifdef WIN32
    QueryPerformanceFrequency(&frequency);
    startCount.QuadPart = 0;
    endCount.QuadPart = 0;
#else
    startCount.tv_sec = startCount.tv_usec = 0;
    endCount.tv_sec = endCount.tv_usec = 0;
#endif

    stopped = 0;
    startTimeInMicroSec = 0;
    endTimeInMicroSec = 0;
}



///////////////////////////////////////////////////////////////////////////////
// distructor
///////////////////////////////////////////////////////////////////////////////
Timer::~Timer()
{
}



///////////////////////////////////////////////////////////////////////////////
// start timer.
// startCount will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Timer::start()
{
    stopped = 0; // reset stop flag
#ifdef WIN32
    QueryPerformanceCounter(&startCount);
#else
    gettimeofday(&startCount, NULL);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// stop the timer.
// endCount will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Timer::stop()
{
    stopped = 1; // set timer stopped flag

#ifdef WIN32
    QueryPerformanceCounter(&endCount);
#else
    gettimeofday(&endCount, NULL);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// compute elapsed time in micro-second resolution.
// other getElapsedTime will call this first, then convert to correspond resolution.
///////////////////////////////////////////////////////////////////////////////
double Timer::getMicroSeconds()
{
#ifdef WIN32
    if(!stopped)
        QueryPerformanceCounter(&endCount);

    startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
    endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    if(!stopped)
        gettimeofday(&endCount, NULL);

    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

    return endTimeInMicroSec - startTimeInMicroSec;
}



///////////////////////////////////////////////////////////////////////////////
// divide MicroSeconds by 1000
///////////////////////////////////////////////////////////////////////////////
double Timer::getMilliSeconds()
{
    return this->getMicroSeconds() * 0.001;
}



///////////////////////////////////////////////////////////////////////////////
// divide MicroSeconds by 1000000
///////////////////////////////////////////////////////////////////////////////
double Timer::getSeconds()
{
    return this->getMicroSeconds() * 0.000001;
}



///////////////////////////////////////////////////////////////////////////////
// same as getSeconds()
///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTime()
{
    return this->getSeconds();
}
