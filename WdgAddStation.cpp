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

#include "WdgAddStation.h"

WdgAddStation::WdgAddStation(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pbSave, SIGNAL(clicked()), this, SLOT(OnSave()));

	Qt::WindowFlags flags = 0;

	flags = Qt::Tool;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	flags |= Qt::WindowStaysOnTopHint;

	setWindowFlags(flags);
}

void WdgAddStation::OnSave()
{
	QString Str = ui.leComment->text();
	if(Str.capacity()<1)
		return;
	if(ui.sbFreq->value() > 100000 && ui.sbFreq->value() < 440000000)
	{
		emit Station(ui.leComment->text(), ui.sbFreq->value(), ui.comboBox->currentIndex());
		close();
	}
}

void WdgAddStation::showEvent(QShowEvent *event)
{
	ui.comboBox->setCurrentIndex(0);
	ui.leComment->clear();
}
