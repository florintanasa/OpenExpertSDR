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

#ifndef SMETEROPTIONS_H
#define SMETEROPTIONS_H

#include <QWidget>
#include "ColorWgt.h"
#include "ui_SMeterOptions.h"


typedef enum
{
	LIGHT = 0,
	GREEN,
	DARK,
	USER
}SM_STYLE;

typedef enum
{
	SM_COLOR_ALL,
	SM_COLOR_LINE_LOW,
	SM_COLOR_LINE_HIGH,
	SM_COLOR_SCALE_NUM,
	SM_COLOR_DBM_NUM,
	SM_COLOR_BACK_TOP,
	SM_COLOR_BACK_BOTTOM,
	SM_COLOR_CURSOR,
	SM_COLOR_CURSOR_BORDER,
	SM_COLOR_SQL_CURSOR,
	SM_COLOR_SQL_CURSOR_BORDER,
	SM_COLOR_LIMIT
}SM_CMD;

typedef struct
{
	SM_CMD Cmd;
    QColor ColorSmLinesLow;
    QColor ColorSmLinesHigh;
    QColor ColorSmScaleNum;
    QColor ColorSmDbmNum;
    QColor ColorSmBackTopColor;
    QColor ColorSmBackBottColor;
    QColor ColorSmCursorColor;
    QColor ColorSmCursorBorderColor;
    QColor ColorSmSQLCursorColor;
    QColor ColorSmSQLBorderColor;
    QColor ColorSmLimitColor;
    int Style;
}SM_PARAM;

class SMeterOptions : public QWidget
{
    Q_OBJECT

    ColorWgt *pSmLinesLow;
    ColorWgt *pSmLinesHigh;
    ColorWgt *pSmScaleNum;
    ColorWgt *pSmDbmNum;
    ColorWgt *pSmBackTopColor;
    ColorWgt *pSmBackBottColor;
    ColorWgt *pSmCursorColor;
    ColorWgt *pSmCursorBorderColor;
    ColorWgt *pSmSQLCursorColor;
    ColorWgt *pSmSQLBorderColor;
    ColorWgt *pSmLimitColor;

    SM_PARAM currentParam;
    SM_PARAM userParam;

public:
    SMeterOptions(QWidget *parent = 0);
    ~SMeterOptions();

	void setParam(SM_PARAM param);
	void setSettingsParam(SM_PARAM param);
	SM_PARAM getParam();
	SM_PARAM getSettingsParam();

private:
    Ui::SMeterOptionsClass ui;

private slots:
	void setColorSmLinesLow(QColor color);
    void setColorSmLinesHigh(QColor color);
    void setColorSmScaleNum(QColor color);
    void setColorSmDbmNum(QColor color);
    void setColorSmBackTopColor(QColor color);
    void setColorSmBackBottColor(QColor color);
    void setColorSmCursorColor(QColor color);
    void setColorSmCursorBorderColor(QColor color);
    void setColorSmSQLCursorColor(QColor color);
    void setColorSmSQLBorderColor(QColor color);
    void setColorSmLimitColor(QColor color);

    void setStyle(int val);

signals:
	void changedParam(SM_PARAM param);
};

#endif // SMETEROPTIONS_H
