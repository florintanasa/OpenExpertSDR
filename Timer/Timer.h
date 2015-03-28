
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
#ifndef TIMER_H_
#define TIMER_H_

#include <QCoreApplication>
//#include <windows.h>
//#include <time.h>


class Timer
{
	bool	available;
	double	minMicroSecondsLatency;
	double	normMicroSecondsLatency;
typedef unsigned long long LARGE_INTEGER;
    LARGE_INTEGER frequency;
    LARGE_INTEGER startCount;
    LARGE_INTEGER count;
   // u_int64_t frequency;
   // u_int64_t startCount;
    //u_int64_t count;


public:
	Timer();

	void start();
	double getMicroSeconds();
	double getMilliSeconds();
	double getSeconds();

private:
	void init();
	void calcLatency();
};

#endif /* TIMER_H_ */


//////////////////////////////////////////////////////////////////////////////
// Timer.h
// =======
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

#ifndef TIMER_H_DEF
#define TIMER_H_DEF

#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif


class Timer
{
public:
    Timer();                                    // default constructor
    ~Timer();                                   // default destructor

    void   start();                             // start timer
    void   stop();                              // stop the timer
    double getElapsedTime();                    // get elapsed time in second
    //double getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
    double getSeconds();
    //double getElapsedTimeInMilliSec();          // get elapsed time in milli-second
    double getMilliSeconds();
    //double getElapsedTimeInMicroSec();          // get elapsed time in micro-second
    double getMicroSeconds();

protected:


private:
    bool	available;
    double startTimeInMicroSec;                 // starting time in micro-second
    double endTimeInMicroSec;                   // ending time in micro-second
    int    stopped;                             // stop flag
#ifdef WIN32
    LARGE_INTEGER frequency;                    // ticks per second
    LARGE_INTEGER startCount;                   //
    LARGE_INTEGER endCount;                     //
#else
    timeval startCount;                         //
    timeval endCount;                           //
#endif
};

#endif // TIMER_H_DEF
