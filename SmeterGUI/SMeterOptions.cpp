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

#include "SMeterOptions.h"

SMeterOptions::SMeterOptions(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

    pSmLinesLow = new ColorWgt(ui.cdSmLinesLow);
    pSmLinesHigh = new ColorWgt(ui.cdSmLinesHigh);
    pSmScaleNum = new ColorWgt(ui.cdSmScaleNum);
    pSmDbmNum = new ColorWgt(ui.cdSmDbmNum);
    pSmBackTopColor = new ColorWgt(ui.cdSmBackTopColor);
    pSmBackBottColor = new ColorWgt(ui.cdSmBackBottColor);
    pSmCursorColor = new ColorWgt(ui.cdSmCursorColor);
    pSmCursorBorderColor = new ColorWgt(ui.cdSmCursorBorderColor);
    pSmSQLCursorColor = new ColorWgt(ui.cdSmSQLCursorColor);
    pSmSQLBorderColor = new ColorWgt(ui.cdSmSQLBorderColor);
    pSmLimitColor = new ColorWgt(ui.cdSmLimitColor);

    pSmLinesLow->resize(50, 25);
    pSmLinesHigh->resize(50, 25);
    pSmScaleNum->resize(50, 25);
    pSmDbmNum->resize(50, 25);
    pSmBackTopColor->resize(50, 25);
    pSmBackBottColor->resize(50, 25);
    pSmCursorColor->resize(50, 25);
    pSmCursorBorderColor->resize(50, 25);
    pSmSQLCursorColor->resize(50, 25);
    pSmSQLBorderColor->resize(50, 25);
    pSmLimitColor->resize(50, 25);

    connect(pSmLinesLow, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLinesLow(QColor)));
    connect(pSmLinesHigh, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLinesHigh(QColor)));
    connect(pSmScaleNum, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmScaleNum(QColor)));
    connect(pSmDbmNum, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmDbmNum(QColor)));
    connect(pSmBackTopColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmBackTopColor(QColor)));
    connect(pSmBackBottColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmBackBottColor(QColor)));
    connect(pSmCursorColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmCursorColor(QColor)));
    connect(pSmCursorBorderColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmCursorBorderColor(QColor)));
    connect(pSmSQLCursorColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmSQLCursorColor(QColor)));
    connect(pSmSQLBorderColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmSQLBorderColor(QColor)));
    connect(pSmLimitColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLimitColor(QColor)));

    connect(ui.cbSmStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(setStyle(int)));

	pSmLinesLow->setColor(QColor(14, 255, 126));
    pSmLinesHigh->setColor(QColor(255, 0, 0));
    pSmScaleNum->setColor(Qt::white);
    pSmDbmNum->setColor(Qt::yellow);
    pSmBackTopColor->setColor(QColor(38, 45, 41));
    pSmBackBottColor->setColor(QColor(43, 91, 59));
    pSmCursorColor->setColor(Qt::white);
    pSmCursorBorderColor->setColor(Qt::black);
    pSmSQLCursorColor->setColor(Qt::yellow);
    pSmSQLBorderColor->setColor(Qt::black);
    pSmLimitColor->setColor(QColor(255, 255, 255, 150));

    currentParam.ColorSmLinesLow = QColor(14, 255, 126);
    currentParam.ColorSmLinesHigh = QColor(255, 0, 0);
    currentParam.ColorSmScaleNum = Qt::white;
    currentParam.ColorSmDbmNum = Qt::yellow;
    currentParam.ColorSmBackTopColor = QColor(38, 45, 41);
    currentParam.ColorSmBackBottColor = QColor(43, 91, 59);
    currentParam.ColorSmCursorColor = Qt::white;
    currentParam.ColorSmCursorBorderColor = Qt::black;
    currentParam.ColorSmSQLCursorColor = Qt::yellow;
    currentParam.ColorSmSQLBorderColor = Qt::black;
    currentParam.ColorSmLimitColor = QColor(255, 255, 255, 150);

    userParam = currentParam;
    currentParam.Style = USER;
}

SMeterOptions::~SMeterOptions()
{
    delete pSmLinesLow;
    delete pSmLinesHigh;
    delete pSmScaleNum;
    delete pSmDbmNum;
    delete pSmBackTopColor;
    delete pSmBackBottColor;
    delete pSmCursorColor;
    delete pSmCursorBorderColor;
    delete pSmSQLCursorColor;
    delete pSmSQLBorderColor;
    delete pSmLimitColor;
}

void SMeterOptions::setParam(SM_PARAM param)
{
	currentParam = param;
	ui.cbSmStyle->setCurrentIndex(currentParam.Style);
}

void SMeterOptions::setSettingsParam(SM_PARAM param)
{
	userParam = param;
	userParam.Style = USER;
	currentParam = userParam;
    pSmLinesLow->setColor(userParam.ColorSmLinesLow);
    pSmLinesHigh->setColor(userParam.ColorSmLinesHigh);
    pSmScaleNum->setColor(userParam.ColorSmScaleNum);
    pSmDbmNum->setColor(userParam.ColorSmDbmNum);
    pSmBackTopColor->setColor(userParam.ColorSmBackTopColor);
    pSmBackBottColor->setColor(userParam.ColorSmBackBottColor);
    pSmCursorColor->setColor(userParam.ColorSmCursorColor);
    pSmCursorBorderColor->setColor(userParam.ColorSmCursorBorderColor);
    pSmSQLCursorColor->setColor(userParam.ColorSmSQLCursorColor);
    pSmSQLBorderColor->setColor(userParam.ColorSmSQLBorderColor);
    pSmLimitColor->setColor(userParam.ColorSmLimitColor);
    ui.cbSmStyle->setCurrentIndex(param.Style);
}

SM_PARAM SMeterOptions::getParam()
{
	return currentParam;
}

SM_PARAM SMeterOptions::getSettingsParam()
{
	SM_PARAM tmp = userParam;
	tmp.Style = ui.cbSmStyle->currentIndex();
	return tmp;
}

void SMeterOptions::setColorSmLinesLow(QColor color)
{
	currentParam.ColorSmLinesLow = color;
	currentParam.Cmd = SM_COLOR_LINE_LOW;
	currentParam.Style = USER;
	userParam = currentParam;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmLinesHigh(QColor color)
{
	currentParam.ColorSmLinesHigh = color;
	currentParam.Cmd = SM_COLOR_LINE_HIGH;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmScaleNum(QColor color)
{
	currentParam.ColorSmScaleNum = color;
	currentParam.Cmd = SM_COLOR_SCALE_NUM;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmDbmNum(QColor color)
{
	currentParam.ColorSmDbmNum = color;
	currentParam.Cmd = SM_COLOR_DBM_NUM;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmBackTopColor(QColor color)
{
	currentParam.ColorSmBackTopColor = color;
	currentParam.Cmd = SM_COLOR_BACK_TOP;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmBackBottColor(QColor color)
{
	currentParam.ColorSmBackBottColor = color;
	currentParam.Cmd = SM_COLOR_BACK_BOTTOM;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmCursorColor(QColor color)
{
	currentParam.ColorSmCursorColor = color;
	currentParam.Cmd = SM_COLOR_CURSOR;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmCursorBorderColor(QColor color)
{
	currentParam.ColorSmCursorBorderColor = color;
	currentParam.Cmd = SM_COLOR_CURSOR_BORDER;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmSQLCursorColor(QColor color)
{
	currentParam.ColorSmSQLCursorColor = color;
	currentParam.Cmd = SM_COLOR_SQL_CURSOR;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmSQLBorderColor(QColor color)
{
	currentParam.ColorSmSQLBorderColor = color;
	currentParam.Cmd = SM_COLOR_SQL_CURSOR_BORDER;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setColorSmLimitColor(QColor color)
{
	currentParam.ColorSmLimitColor = color;
	currentParam.Cmd = SM_COLOR_LIMIT;
	userParam = currentParam;
	currentParam.Style = USER;
	userParam.Style = USER;
	ui.cbSmStyle->setCurrentIndex((int)USER);
	emit changedParam(currentParam);
}

void SMeterOptions::setStyle(int val)
{
    disconnect(pSmLinesLow, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLinesLow(QColor)));
    disconnect(pSmLinesHigh, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLinesHigh(QColor)));
    disconnect(pSmScaleNum, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmScaleNum(QColor)));
    disconnect(pSmDbmNum, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmDbmNum(QColor)));
    disconnect(pSmBackTopColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmBackTopColor(QColor)));
    disconnect(pSmBackBottColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmBackBottColor(QColor)));
    disconnect(pSmCursorColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmCursorColor(QColor)));
    disconnect(pSmCursorBorderColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmCursorBorderColor(QColor)));
    disconnect(pSmSQLCursorColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmSQLCursorColor(QColor)));
    disconnect(pSmSQLBorderColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmSQLBorderColor(QColor)));
    disconnect(pSmLimitColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLimitColor(QColor)));
	switch((SM_STYLE)val)
	{
	case LIGHT:
	    pSmLinesLow->setColor(QColor(72, 217, 245));
	    pSmLinesHigh->setColor(QColor(252, 255, 20));
	    pSmScaleNum->setColor(Qt::white);
	    pSmDbmNum->setColor(Qt::yellow);
	    pSmBackTopColor->setColor(QColor(80, 80, 80));
	    pSmBackBottColor->setColor(QColor(127, 127, 127));
	    pSmCursorColor->setColor(Qt::white);
	    pSmCursorBorderColor->setColor(QColor(5, 5, 255));
	    pSmSQLCursorColor->setColor(Qt::white);
	    pSmSQLBorderColor->setColor(Qt::red);
	    pSmLimitColor->setColor(QColor(255, 255, 255, 150));

	    currentParam.ColorSmLinesLow = QColor(72, 217, 245);
	    currentParam.ColorSmLinesHigh = QColor(252, 255, 20);
	    currentParam.ColorSmScaleNum = Qt::white;
	    currentParam.ColorSmDbmNum = Qt::yellow;
	    currentParam.ColorSmBackTopColor = QColor(80, 80, 80);
	    currentParam.ColorSmBackBottColor = QColor(127, 127, 127);
	    currentParam.ColorSmCursorColor = Qt::white;
	    currentParam.ColorSmCursorBorderColor = QColor(5, 5, 255);
	    currentParam.ColorSmSQLCursorColor = Qt::white;
	    currentParam.ColorSmSQLBorderColor = Qt::red;
	    currentParam.ColorSmLimitColor = QColor(255, 255, 255, 150);
	    currentParam.Style = (int)LIGHT;
		break;
	case GREEN:
	    pSmLinesLow->setColor(QColor(14, 255, 126));
	    pSmLinesHigh->setColor(QColor(255, 0, 0));
	    pSmScaleNum->setColor(Qt::white);
	    pSmDbmNum->setColor(Qt::yellow);
	    pSmBackTopColor->setColor(QColor(38, 45, 41));
	    pSmBackBottColor->setColor(QColor(43, 91, 59));
	    pSmCursorColor->setColor(Qt::white);
	    pSmCursorBorderColor->setColor(Qt::black);
	    pSmSQLCursorColor->setColor(Qt::yellow);
	    pSmSQLBorderColor->setColor(Qt::black);
	    pSmLimitColor->setColor(QColor(255, 255, 255, 150));

	    currentParam.ColorSmLinesLow = QColor(14, 255, 126);
	    currentParam.ColorSmLinesHigh = QColor(255, 0, 0);
	    currentParam.ColorSmScaleNum = Qt::white;
	    currentParam.ColorSmDbmNum = Qt::yellow;
	    currentParam.ColorSmBackTopColor = QColor(38, 45, 41);
	    currentParam.ColorSmBackBottColor = QColor(43, 91, 59);
	    currentParam.ColorSmCursorColor = Qt::white;
	    currentParam.ColorSmCursorBorderColor = Qt::black;
	    currentParam.ColorSmSQLCursorColor = Qt::yellow;
	    currentParam.ColorSmSQLBorderColor = Qt::black;
	    currentParam.ColorSmLimitColor = QColor(255, 255, 255, 150);
	    currentParam.Style = (int)GREEN;
		break;
	case DARK:
	    pSmLinesLow->setColor(QColor(32, 203, 58));
	    pSmLinesHigh->setColor(QColor(211, 211, 211));
	    pSmScaleNum->setColor(Qt::white);
	    pSmDbmNum->setColor(Qt::yellow);
	    pSmBackTopColor->setColor(QColor(74, 74, 74));
	    pSmBackBottColor->setColor(QColor(47, 47, 47));
	    pSmCursorColor->setColor(Qt::white);
	    pSmCursorBorderColor->setColor(Qt::red);
	    pSmSQLCursorColor->setColor(Qt::red);
	    pSmSQLBorderColor->setColor(Qt::yellow);
	    pSmLimitColor->setColor(QColor(255, 255, 255, 150));

	    currentParam.ColorSmLinesLow = QColor(32, 203, 58);
	    currentParam.ColorSmLinesHigh = QColor(211, 211, 211);
	    currentParam.ColorSmScaleNum = Qt::white;
	    currentParam.ColorSmDbmNum = Qt::yellow;
	    currentParam.ColorSmBackTopColor = QColor(74, 74, 74);
	    currentParam.ColorSmBackBottColor = QColor(47, 47, 47);
	    currentParam.ColorSmCursorColor = Qt::white;
	    currentParam.ColorSmCursorBorderColor = Qt::red;
	    currentParam.ColorSmSQLCursorColor = Qt::red;
	    currentParam.ColorSmSQLBorderColor = Qt::yellow;
	    currentParam.ColorSmLimitColor = QColor(255, 255, 255, 150);
	    currentParam.Style = (int)DARK;
		break;
	default:
	    pSmLinesLow->setColor(userParam.ColorSmLinesLow);
	    pSmLinesHigh->setColor(userParam.ColorSmLinesHigh);
	    pSmScaleNum->setColor(userParam.ColorSmScaleNum);
	    pSmDbmNum->setColor(userParam.ColorSmDbmNum);
	    pSmBackTopColor->setColor(userParam.ColorSmBackTopColor);
	    pSmBackBottColor->setColor(userParam.ColorSmBackBottColor);
	    pSmCursorColor->setColor(userParam.ColorSmCursorColor);
	    pSmCursorBorderColor->setColor(userParam.ColorSmCursorBorderColor);
	    pSmSQLCursorColor->setColor(userParam.ColorSmSQLCursorColor);
	    pSmSQLBorderColor->setColor(userParam.ColorSmSQLBorderColor);
	    pSmLimitColor->setColor(userParam.ColorSmLimitColor);
	    currentParam = userParam;
		break;
	}
	emit changedParam(currentParam);
    connect(pSmLinesLow, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLinesLow(QColor)));
    connect(pSmLinesHigh, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLinesHigh(QColor)));
    connect(pSmScaleNum, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmScaleNum(QColor)));
    connect(pSmDbmNum, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmDbmNum(QColor)));
    connect(pSmBackTopColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmBackTopColor(QColor)));
    connect(pSmBackBottColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmBackBottColor(QColor)));
    connect(pSmCursorColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmCursorColor(QColor)));
    connect(pSmCursorBorderColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmCursorBorderColor(QColor)));
    connect(pSmSQLCursorColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmSQLCursorColor(QColor)));
    connect(pSmSQLBorderColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmSQLBorderColor(QColor)));
    connect(pSmLimitColor, SIGNAL(ColorChanged(QColor)), this, SLOT(setColorSmLimitColor(QColor)));
}
