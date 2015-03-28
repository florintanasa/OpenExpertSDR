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

#include "Didgit.h"

Didgit::Didgit(QWidget *parent)
	: QPushButton(parent)
{
	p = parent;
	i = 0;
    setMouseTracking(true);
    setFixedSize(parent->width(), height());

    Font.setFamily("Century Gothic");
    Font.setPointSize(24);
	setText("0");

    StrEnable = "QPushButton  { color: #C5C5C5; border-radius: 0px; min-width: 30px; } QPushButton::hover { color: #FFFFFF; border: 0px;} ";
    StrDisable = "QPushButton { color: #787878; border-radius: 0px; min-width: 30px; } QPushButton::hover { color: #787878; border: 0px;} ";
}

Didgit::~Didgit()
{

}
void Didgit::SetEnableNum(bool Value)
{
	if(Value)
		setStyleSheet(StrEnable);
	else
		setStyleSheet(StrDisable);
}

void Didgit::SetFontType(QString Name, int Size)
{
	Font.setFamily(Name);
	Font.setPointSize(Size);
	setFont(Font);
}
void Didgit::wheelEvent(QWheelEvent *event)
{
	int k = 0;
	k = event->delta();
	if(k > 0)
	{
		if(++i > 9)
			i = 0;
		emitIncr();
	}
	if(k < 0)
	{
		if(--i < 0)
			i = 9;
		emitDecr();
	}
	event->accept();
	emitChange();
	QString Str;
	Str.setNum(i);
	setText(Str);
	emitNum();
}

void Didgit::Set(int Num)
{
	if(Num > 9)
		i = 9;
	else if(Num < 0)
		i = 0;
	else
		i = Num;
    QString Str;
    Str.setNum(i);
    setText(Str);
}
int Didgit::Get()
{
	return i;
}

void Didgit::resizeEvent(QResizeEvent *event)
{
	setFixedSize(p->width(), p->height());
}

void Didgit::emitNum()
{
	emit Num(i);
}
void Didgit::emitLow()
{
	emit Low();
}
void Didgit::emitHigh()
{
	emit High();
}

void Didgit::emitChange()
{
	emit Change();
}

void Didgit::emitIncr()
{
	emit Incr();
}

void Didgit::emitDecr()
{
	emit Decr();
}

