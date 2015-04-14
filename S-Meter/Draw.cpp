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

#include "Draw.h"
#include <cstring>

Draw::Draw(QWidget *parent)
    : QWidget(parent)
{
	Pix.load(":images/Strelka.png");
	setFixedSize(300, 120);

	Dbm = 0;
	Dbm1 = 0;

//	qMemSet(Buff, -100, sizeof(float)*BUFF_SP);
    std::memset(Buff, -100, sizeof(float)*BUFF_SP);
	Cnt = 0;
    startTimer(20);
}

void Draw::paintEvent(QPaintEvent *event)
{
	Painter.begin(this);
		Painter.setRenderHint(QPainter::SmoothPixmapTransform);
		Painter.translate(width()/2 + 2, Pix.height() + 82);
		Painter.rotate(180.0);

		if(Dbm1 < -44)
			Dbm1 = -44;
		else if(Dbm1 > 44)
			Dbm1 = 44;
		Painter.rotate(Dbm1);
		Painter.drawPixmap(0, 0, Pix.width(), 180, Pix);
	Painter.end();
}

void Draw::timerEvent(QTimerEvent *event)
{
	float tmp = Buff[0];
	for(int i =1; i < BUFF_SP; i++)
	{
		if(tmp < Buff[i])
                    tmp = Buff[i];
	}
        if(Dbm1 < tmp-1)
            Dbm1  += 1.5;
        else if(Dbm1 > tmp+1)
            Dbm1 -= 1;

	update();
}

void Draw::setDB(float dB)
{
	if(dB < -121)
	Dbm = -121;
        else if(dB > -13)
            Dbm = -13;
	else
            Dbm = dB;

	if(Dbm <= -73)
	{
		Dbm += 79;
		Dbm *= (44.0/42.0);
	}
	else
	{
		float L = 46 - ((-73.0+79.0)*46.0)/52.0;
		Dbm = L/60.0 * (Dbm + 79);
	}

	Buff[Cnt] = Dbm;
	if(++Cnt >= BUFF_SP)
		Cnt = 0;
}




