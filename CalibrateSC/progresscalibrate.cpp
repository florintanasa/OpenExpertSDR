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

#include "progresscalibrate.h"

ProgressCalibrate::ProgressCalibrate(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Drawer | Qt::WindowStaysOnTopHint);

	Count = 0;

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(close()));
}

void ProgressCalibrate::closeEvent(QCloseEvent *event)
{
	killTimer(TimerID);
	emit CloseSignal();
	Count = 0;
}
void ProgressCalibrate::timerEvent(QTimerEvent *event)
{
	if(++Count > 100)
		Count = 0;

	ui.progressBar->setValue(Count);
}

void ProgressCalibrate::showEvent(QShowEvent *event)
{
	TimerID = startTimer(50);
}
