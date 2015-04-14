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

#ifndef WAVSAMPLE_H
#define WAVSAMPLE_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "ui_wavSample.h"

class wavSample : public QWidget
{
    Q_OBJECT

	public:
		wavSample(QWidget *parent = 0);
		~wavSample();
		void setActive(bool active);
		bool isActive();
		void setFileName(QString fileName);
		QString fileName();
		void setNumber(int num);
		void setDateTime(QDateTime dt);
		void setFrequency(quint32 freq);
		quint32 frequency();
		int number();
		void setPlayTime(QTime time);
		QTime playTime();

	private:
		Ui::wavSampleClass ui;
		bool isAct;
		QString activeStyle;
		QTime timeOfPlay;
		QFile file;
};

#endif // WAVSAMPLE_H
