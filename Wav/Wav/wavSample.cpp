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

#include "wavSample.h"

wavSample::wavSample(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	isAct = false;
	activeStyle = 	"QWidget{background-color: transparent; color: rgb(220, 0, 0);}";
}

void wavSample::setActive(bool active)
{
	isAct = active;
	if(isAct)
	{
        ui.dteInfo->setStyleSheet("QWidget{background-color: transparent; color: rgb(147, 82, 0);}");
        ui.labFreq->setStyleSheet("QWidget{background-color: transparent; color: rgb(147, 82, 0);}");
	}
	else
	{
        ui.dteInfo->setStyleSheet("QWidget{background-color: transparent; color: rgb(128, 128, 128);}");
        ui.labFreq->setStyleSheet("QWidget{background-color: transparent; color: rgb(128, 128, 128);}");
	}
}

bool wavSample::isActive()
{
	return isAct;
}

void wavSample::setFileName(QString fileName)
{
	file.setFileName(fileName);
	QFileInfo fInfo(file);
	ui.labFileName->setText(fInfo.baseName());
}

QString wavSample::fileName()
{
	return file.fileName();
}

void wavSample::setNumber(int num)
{
	QString str;
	str.setNum(num);
	str += ".  ";
	ui.labNum->setText(str);
}

void wavSample::setDateTime(QDateTime dt)
{
	ui.dteInfo->setDateTime(dt);
}

void wavSample::setFrequency(quint32 freq)
{
	QString str;
	str.setNum(freq/1000000.0f, 'f', 6);
	str = "f: " + str + " MHz";
	ui.labFreq->setText(str);
}

int wavSample::number()
{
	return ui.labNum->text().toInt();
}

void wavSample::setPlayTime(QTime time)
{
	timeOfPlay = time;
	ui.labTime->setText(time.toString("hh:mm"));
}

QTime wavSample::playTime()
{
	return timeOfPlay;
}
