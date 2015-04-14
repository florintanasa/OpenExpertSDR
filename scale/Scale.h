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

#ifndef SCALE_H
#define SCALE_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

#include "ui_Scale.h"
#include "ScaleNum.h"

#define SCALE_MAX_VALUE	 1300000000

class Scale : public QWidget
{
    Q_OBJECT

	public:
		Scale(QWidget *parent = 0);
		~Scale();

		void setValue(int value);
		int  value();
		void setMaxValue(int val);
		int  maxValue();
		void setMinValue(int val);
		int  minValue();


	private:
		Ui::ScaleClass ui;

		ScaleNum *pDg0;
		ScaleNum *pDg1;
		ScaleNum *pDg2;
		ScaleNum *pDg3;
		ScaleNum *pDg4;
		ScaleNum *pDg5;
		ScaleNum *pDg6;
		ScaleNum *pDg7;
		ScaleNum *pDg8;
		ScaleNum *pDg9;

		long long Value;
		QString Freq;

		QString styleButOn;
		QString styleButOff;

		int maxVal;
		int minVal;

		void setStyle(int dig, QString style);

	private slots:
		void onChanged(int, int);

	signals:
		void valueChanged(int);
};

#endif // SCALE_H
