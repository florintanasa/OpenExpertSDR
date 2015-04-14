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

#ifndef CWMACRO_H
#define CWMACRO_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "ui_CwMacro.h"
#include "CwCore.h"
#include "../Defines.h"

class CwMacro : public QWidget
{
	Q_OBJECT

	public:
		CwMacro(QWidget *parent = 0);
		~CwMacro();
		void writeSetting(QSettings *pSett);
		void readSettings(QSettings *pSett);
		CwCore *pCwCore;
		void start();
		void stop();
		void setMode(SDRMODE mode);

	private:
		Ui::CwMacroClass ui;
		SDRMODE sdrMode;
		bool isStart;
		int rxMode;
		bool isSsbKey;

	private slots:
		void onM1();
		void onM2();
		void onM3();
		void onM4();
		void onM5();
		void onM6();
		void onM7();
		void onM8();
		void onM9();

		void onDropDel(int val);
		void onPttDel(int val);

		void onTx(bool val);

	signals:
		void trxChanged(bool);
		void changeMode(int);
};

#endif // CWMACRO_H
