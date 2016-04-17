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

#ifndef PA19_H_
#define PA19_H_

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "portaudio.h"
#include "../../Defines.h"

typedef void StreamCallback(const void *, void *, unsigned long, void *);

class pa19
{

	public:

		pa19(void *UserData);
		~pa19();

		StreamCallback *pCallbackFunc;
		void *pUserData;

		void open();
		void close();

		int start(StreamCallback *Func);
		void stop();
		bool isStart();
		void setParam(PLUGIN_OPTIONS *Param);

		QString getVersion();
		QStringList driverName();
		QStringList outDevName(int Host);
		QStringList inDevName(int Host);

	private:
		bool IsOpened, isStarted;
        int outOffsetIndex, inOffsetIndex, DrvIndex;
		double sampleRate;
		int bufferSize;
		PaStreamParameters inParam, outParam;
		PaStream	*pStream1;
};

#endif /* PA19_H_ */
