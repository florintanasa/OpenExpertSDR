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

#include "Scale.h"

Scale::Scale(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

    pDg0 = new ScaleNum(9, ui.pbD0);
    pDg1 = new ScaleNum(8, ui.pbD1);
    pDg2 = new ScaleNum(7, ui.pbD2);
    pDg3 = new ScaleNum(6, ui.pbD3);
    pDg4 = new ScaleNum(5, ui.pbD4);
    pDg5 = new ScaleNum(4, ui.pbD5);
    pDg6 = new ScaleNum(3, ui.pbD6);
    pDg7 = new ScaleNum(2, ui.pbD7);
    pDg8 = new ScaleNum(1, ui.pbD8);
    pDg9 = new ScaleNum(0, ui.pbD9);

	connect(pDg0, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg1, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg2, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg3, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg4, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg5, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg6, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg7, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg8, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));
	connect(pDg9, SIGNAL(changed(int, int)), this, SLOT(onChanged(int, int)));

	Freq = "0000000000";

    styleButOn =    "QPushButton { color: #C5C5C5;  min-width: 10px; background-color: transparent;} \n"
                    "QPushButton::hover {color: White; border: 0px; }";

    styleButOff =   "QPushButton { color: #2D2D2D;  min-width: 10px; background-color: transparent;} \n"
                    "QPushButton::hover {color: #2D2D2D; border: 0px; }";


	ui.pbDot->setStyleSheet(styleButOn);

	setStyle(0, styleButOn);
	setStyle(1, styleButOn);
	setStyle(2, styleButOn);
	setStyle(3, styleButOn);
	setStyle(4, styleButOn);
	setStyle(5, styleButOn);
	setStyle(6, styleButOn);
	setStyle(7, styleButOff);
	setStyle(8, styleButOff);
	setStyle(9, styleButOff);

	maxVal = 65000000;
	minVal = 0;
    Value = 0;
    setValue(60000000);
}

Scale::~Scale()
{
	delete pDg0;
	delete pDg1;
	delete pDg2;
	delete pDg3;
	delete pDg4;
	delete pDg5;
	delete pDg6;
	delete pDg7;
	delete pDg8;
	delete pDg9;
}

void Scale::setValue(int val)
{
	if((val < minVal))	val = minVal;
	if((val > maxVal))	val = maxVal;
    if(val == Value) return;

	Value = val;
	QString str;
	str.setNum(Value);
	Freq.clear();
	Freq.fill('0', 10 - str.size());
	Freq += str;
	ui.pbD0->setText(QString(Freq.data()[9]));
	ui.pbD1->setText(QString(Freq.data()[8]));
	ui.pbD2->setText(QString(Freq.data()[7]));
	ui.pbD3->setText(QString(Freq.data()[6]));
	ui.pbD4->setText(QString(Freq.data()[5]));
	ui.pbD5->setText(QString(Freq.data()[4]));
	ui.pbD6->setText(QString(Freq.data()[3]));
	ui.pbD7->setText(QString(Freq.data()[2]));
	ui.pbD8->setText(QString(Freq.data()[1]));
	ui.pbD9->setText(QString(Freq.data()[0]));
	if(ui.pbD9->text().toInt() == 0)
	{
		setStyle(9, styleButOff);
		if(ui.pbD8->text().toInt() == 0)
		{
			setStyle(8, styleButOff);
			if(ui.pbD7->text().toInt() == 0)
			{
				setStyle(7, styleButOff);
			}
			else
			{
				setStyle(7, styleButOn);
				setStyle(6, styleButOn);
				setStyle(5, styleButOn);
				setStyle(4, styleButOn);
				setStyle(3, styleButOn);
			}
		}
		else
		{
			setStyle(8, styleButOn);
			setStyle(7, styleButOn);
			setStyle(6, styleButOn);
			setStyle(5, styleButOn);
			setStyle(4, styleButOn);
			setStyle(3, styleButOn);
		}
	}
	else
	{
		setStyle(9, styleButOn);
		setStyle(8, styleButOn);
		setStyle(7, styleButOn);
		setStyle(6, styleButOn);
		setStyle(5, styleButOn);
		setStyle(4, styleButOn);
		setStyle(3, styleButOn);
	}
	emit valueChanged(Freq.toInt());
}

int Scale::value()
{
	return Value;
}

void Scale::setMaxValue(int val)
{
	QString Str;

	maxVal = val;
	if(Value <= maxVal) return;
	if(Value > maxVal) Value = maxVal;
	Str.setNum(Value);
	Freq.fill('0', 10 - Str.size());
	Freq += Str;
	ui.pbD0->setText(QString(Freq.data()[9]));
	ui.pbD1->setText(QString(Freq.data()[8]));
	ui.pbD2->setText(QString(Freq.data()[7]));
	ui.pbD3->setText(QString(Freq.data()[6]));
	ui.pbD4->setText(QString(Freq.data()[5]));
	ui.pbD5->setText(QString(Freq.data()[4]));
	ui.pbD6->setText(QString(Freq.data()[3]));
	ui.pbD7->setText(QString(Freq.data()[2]));
	ui.pbD8->setText(QString(Freq.data()[1]));
	ui.pbD9->setText(QString(Freq.data()[0]));

	if(ui.pbD9->text().toInt() == 0)
	{
		setStyle(9, styleButOff);
		if(ui.pbD8->text().toInt() == 0)
		{
			setStyle(8, styleButOff);
			if(ui.pbD7->text().toInt() == 0)
			{
				setStyle(7, styleButOff);
			}
			else
			{
				setStyle(7, styleButOn);
				setStyle(6, styleButOn);
				setStyle(5, styleButOn);
				setStyle(4, styleButOn);
				setStyle(3, styleButOn);
			}
		}
		else
		{
			setStyle(8, styleButOn);
			setStyle(7, styleButOn);
			setStyle(6, styleButOn);
			setStyle(5, styleButOn);
			setStyle(4, styleButOn);
			setStyle(3, styleButOn);
		}
	}
	else
	{
		setStyle(9, styleButOn);
		setStyle(8, styleButOn);
		setStyle(7, styleButOn);
		setStyle(6, styleButOn);
		setStyle(5, styleButOn);
		setStyle(4, styleButOn);
		setStyle(3, styleButOn);
	}
	emit valueChanged(Freq.toInt());
}

int  Scale::maxValue()
{
	return maxVal;
}

void Scale::setMinValue(int val)
{
	QString Str;

	minVal = val;
	if(Value >= minVal) return;
    if(Value < minVal)
        Value = minVal;
	Str.setNum(Value);
	Freq.fill('0', 10 - Str.size());
	Freq += Str;
	ui.pbD0->setText(QString(Freq.data()[9]));
	ui.pbD1->setText(QString(Freq.data()[8]));
	ui.pbD2->setText(QString(Freq.data()[7]));
	ui.pbD3->setText(QString(Freq.data()[6]));
	ui.pbD4->setText(QString(Freq.data()[5]));
	ui.pbD5->setText(QString(Freq.data()[4]));
	ui.pbD6->setText(QString(Freq.data()[3]));
	ui.pbD7->setText(QString(Freq.data()[2]));
	ui.pbD8->setText(QString(Freq.data()[1]));
	ui.pbD9->setText(QString(Freq.data()[0]));

	if(ui.pbD9->text().toInt() == 0)
	{
		setStyle(9, styleButOff);
		if(ui.pbD8->text().toInt() == 0)
		{
			setStyle(8, styleButOff);
			if(ui.pbD7->text().toInt() == 0)
			{
				setStyle(7, styleButOff);
			}
			else
			{
				setStyle(7, styleButOn);
				setStyle(6, styleButOn);
				setStyle(5, styleButOn);
				setStyle(4, styleButOn);
				setStyle(3, styleButOn);
			}
		}
		else
		{
			setStyle(8, styleButOn);
			setStyle(7, styleButOn);
			setStyle(6, styleButOn);
			setStyle(5, styleButOn);
			setStyle(4, styleButOn);
			setStyle(3, styleButOn);
		}
	}
	else
	{
		setStyle(9, styleButOn);
		setStyle(8, styleButOn);
		setStyle(7, styleButOn);
		setStyle(6, styleButOn);
		setStyle(5, styleButOn);
		setStyle(4, styleButOn);
		setStyle(3, styleButOn);
	}
	emit valueChanged(Freq.toInt());
}

int  Scale::minValue()
{
	return minVal;
}

void Scale::setStyle(int dig, QString style)
{
	switch(dig)
	{
        case 0:  ui.pbD0->setStyleSheet(style); break;
        case 1:  ui.pbD1->setStyleSheet(style); break;
        case 2:  ui.pbD2->setStyleSheet(style); break;
        case 3:  ui.pbD3->setStyleSheet(style); break;
        case 4:  ui.pbD4->setStyleSheet(style); break;
        case 5:  ui.pbD5->setStyleSheet(style); break;
        case 6:  ui.pbD6->setStyleSheet(style); break;
        case 7:  ui.pbD7->setStyleSheet(style); break;
        case 8:  ui.pbD8->setStyleSheet(style); break;
		default: ui.pbD9->setStyleSheet(style); break;
	}
}

void Scale::onChanged(int ID, int sign)
{
	QString Freq, Str;

	if((ID > 9) || (ID < 0))
	{
		qDebug() << "Scale::onChanged() error ...";
		return;
	}

	if(sign > 0)
		switch(ID)
		{
			case 0:  ((Value + 1000000000) > maxVal) ? Value = Value : Value += 1000000000;  break;
			case 1:	 ((Value + 100000000) > maxVal) ? Value = Value : Value += 100000000;  break;
			case 2:  ((Value + 10000000) > maxVal) ? Value = Value : Value += 10000000;  break;
			case 3:  ((Value + 1000000) > maxVal) ? Value = Value : Value += 1000000;  break;
			case 4:  ((Value + 100000) > maxVal) ? Value = Value : Value += 100000;  break;
			case 5:  ((Value + 10000) > maxVal) ? Value = Value : Value += 10000;  break;
			case 6:  ((Value + 1000) > maxVal) ? Value = Value : Value += 1000;  break;
			case 7:  ((Value + 100) > maxVal) ? Value = Value : Value += 100;  break;
			case 8:  ((Value + 10) > maxVal) ? Value = Value : Value += 10;  break;
			default: ((Value + 1) > maxVal) ? Value = Value : Value += 1;  break;
		}
	else if(sign < 0)
		switch(ID)
		{
			case 0:  ((Value - 1000000000) < minVal) ? Value = Value : Value -= 1000000000;  break;
			case 1:	 ((Value - 100000000) < minVal) ? Value = Value : Value -= 100000000;  break;
			case 2:  ((Value - 10000000) < minVal) ? Value = Value : Value -= 10000000;  break;
			case 3:  ((Value - 1000000) < minVal) ? Value = Value : Value -= 1000000;  break;
			case 4:  ((Value - 100000) < minVal) ? Value = Value : Value -= 100000;  break;
			case 5:  ((Value - 10000) < minVal) ? Value = Value : Value -= 10000;  break;
			case 6:  ((Value - 1000) < minVal) ? Value = Value : Value -= 1000;  break;
			case 7:  ((Value - 100) < minVal) ? Value = Value : Value -= 100;  break;
			case 8:  ((Value - 10) < minVal) ? Value = Value : Value -= 10;  break;
			default: ((Value - 1) < minVal) ? Value = Value : Value -= 1;  break;
		}
	else
		return;
	qDebug() << Value;

	Str.setNum(Value);
	Freq.fill('0', 10 - Str.size());
	Freq += Str;
	ui.pbD0->setText(QString(Freq.data()[9]));
	ui.pbD1->setText(QString(Freq.data()[8]));
	ui.pbD2->setText(QString(Freq.data()[7]));
	ui.pbD3->setText(QString(Freq.data()[6]));
	ui.pbD4->setText(QString(Freq.data()[5]));
	ui.pbD5->setText(QString(Freq.data()[4]));
	ui.pbD6->setText(QString(Freq.data()[3]));
	ui.pbD7->setText(QString(Freq.data()[2]));
	ui.pbD8->setText(QString(Freq.data()[1]));
	ui.pbD9->setText(QString(Freq.data()[0]));

	if(ui.pbD9->text().toInt() == 0)
	{
		setStyle(9, styleButOff);
		if(ui.pbD8->text().toInt() == 0)
		{
			setStyle(8, styleButOff);
			if(ui.pbD7->text().toInt() == 0)
			{
				setStyle(7, styleButOff);
			}
			else
			{
				setStyle(7, styleButOn);
				setStyle(6, styleButOn);
				setStyle(5, styleButOn);
				setStyle(4, styleButOn);
				setStyle(3, styleButOn);
			}
		}
		else
		{
			setStyle(8, styleButOn);
			setStyle(7, styleButOn);
			setStyle(6, styleButOn);
			setStyle(5, styleButOn);
			setStyle(4, styleButOn);
			setStyle(3, styleButOn);
		}
	}
	else
	{
		setStyle(9, styleButOn);
		setStyle(8, styleButOn);
		setStyle(7, styleButOn);
		setStyle(6, styleButOn);
		setStyle(5, styleButOn);
		setStyle(4, styleButOn);
		setStyle(3, styleButOn);
	}
	emit valueChanged(Freq.toInt());
}
