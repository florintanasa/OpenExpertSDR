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

#ifndef SCALENUM_H_
#define SCALENUM_H_

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif


class ScaleNum : public QPushButton
{
	Q_OBJECT

	private:
		bool Ena;
		int Num;
		int ID;
		QPushButton *p;
		void wheelEvent(QWheelEvent *event);

	public:
		ScaleNum(int id, QPushButton *parent = 0);
		void setNum(int Val);
		int  num();

	public slots:
		void changeOvf(int val);

	signals:
		void changed(int id, int sign);
		void overflow(int ovf);

};

#endif /* SCALENUM_H_ */
