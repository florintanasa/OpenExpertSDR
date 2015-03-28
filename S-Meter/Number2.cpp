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

#include "Number2.h"

Number2::Number2(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	Num = 0;
}

Number2::~Number2()
{

}

void Number2::wheelEvent(QWheelEvent *event)
{
	int k = 0;
	k = event->delta();
	if(k > 0)
	{
		if(++Num > 9)
			Num = 0;
		emit ChangedNum(1);
	}
	if(k < 0)
	{
		if(--Num < 0)
			Num = 9;
		emit ChangedNum(-1);
	}
	event->accept();
	QString str;
	str.setNum(Num);
	ui.pbNum->setText(str);
}

void Number2::SetNumber(int val)
{
	Num = val;
	if(Num < 0)
		Num = 0;
	else if(Num > 9)
		Num = 9;

	QString str;
	str.setNum(Num);
	ui.pbNum->setText(str);
}
