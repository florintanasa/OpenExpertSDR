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

#include "ScaleNum.h"

ScaleNum::ScaleNum(int id, QPushButton *parent) : QPushButton(parent)
{
    QString str;
	Ena = false;
    Num = 0;
	ID = id;
    p = parent;
    this->setMaximumSize(p->size());
    this->setMinimumSize(p->size());
    str.setNum(Num);
    p->setText(str);
}

void ScaleNum::setNum(int Val)
{
	QString str;
	if((Val > 9) || (Val < 0))	return;
	Val = Num;
	str.setNum(Num);
	p->setText(str);
}

int ScaleNum::num()
{
	return p->text().toInt();
}

void ScaleNum::changeOvf(int val)
{
    QString str;
	if(val > 0)
	{        
        if(++Num > 9)
		{
			Num = 0;
			emit overflow(1);
		}
        str.setNum(Num);
        p->setText(str);
	} else if(val < 0)
	{
		if(--Num < 0)
		{
			Num = 9;
			emit overflow(-1);
		}
        str.setNum(Num);
        p->setText(str);
	}
    emit changed(ID, 0);
}

void ScaleNum::wheelEvent(QWheelEvent *event)
{
	int k = 0;
	k = event->delta();
	if(k > 0)
	{
		if(++Num > 9)
		{
			Num = 0;
			emit overflow(1);
		}
	} else if(k < 0)
	{
		if(--Num < 0)
		{
			Num = 9;
			emit overflow(-1);
		}
	}
	else
		return;

	event->accept();
	emit changed(ID, (k > 0) ? 1 : -1);
}
