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

#ifndef NUMBER2_H
#define NUMBER2_H

#include <QtWidgets>
#include <QtWidgets/QWidget>
#include "ui_Number2.h"

class Number2 : public QWidget
{
    Q_OBJECT

    int Num;

public:
    Number2(QWidget *parent = 0);
    ~Number2();

    void SetNumber(int val);

private:
    Ui::Number2Class ui;
    void wheelEvent(QWheelEvent *event);

signals:
	void ChangedNum(int val);
};

#endif // NUMBER2_H
