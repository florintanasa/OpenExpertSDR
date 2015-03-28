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

#ifndef PANORAMOPT_H
#define PANORAMOPT_H

#include <QtGui>
#include <QWidget>
#include "ui_PanoramOpt.h"
#include "SmeterGUI/ColorWgt.h"
#include "PanoramOpt.h"
#include "PanaramDefines.h"

class PanoramOpt : public QWidget
{
    Q_OBJECT

    QTimer TimerWait;
    bool IsEnableChangedStyle;
    QTimer TimerWaitStyle;
    bool IsEnableWaitStyle;
    double Zoom;
    double ZoomPos;

private slots:
    void ChangeStyle(int Type);
    void StyleChange(int i);
    void StyleChange(QColor i);
    void EnableChangedStyle();
    void EnableChangedWaitStyle();

public slots:
	void setZoom(double val);
	void setZoomPos(double val);

public:
    PanoramOpt(QWidget *parent = 0);
    ~PanoramOpt();

    void readSettings(QSettings *pSettings);
    void writeSettings(QSettings *pSettings);
    double getZoom();
    double getZoomPos();

    ColorWgt *pColorGradTop;
    ColorWgt *pColorGradBot;
    ColorWgt *pColorGradBack;
    ColorWgt *pColorSolidBack;
    ColorWgt *pColorLineLines;
    ColorWgt *pColorLineSpectrum;
    ColorWgt *pColorLineBack;
    ColorWgt *pColorSolidLineBack;
    ColorWgt *pColorWFgrad;
    ColorWgt *pColorFilter1;
    ColorWgt *pColorFilter2;
    ColorWgt *pColorBandFilter1;
    ColorWgt *pColorBandFilter2;
    ColorWgt *pColorPitch1;
    ColorWgt *pColorPitch2;
    Ui::PanoramOptClass ui;
    QColor ColorGradTop;
    QColor ColorGradBot;
    QColor ColorGradBack;
    QColor ColorLineLines;
    QColor ColorLineSpectrum;
    QColor ColorLineBack;
    QColor ColorWFgrad;
    QColor ColorFilter;
    int PanType;
    int TrancpSpectrumLine;
    int TrancpSpectrum;
    int WfMode;
    int BackgroundType;
    int ImgBackgroundType;
    int TrancpGradTop;
    int TrancpGradBot;
    int TrancpFilter;
    PAN_OPTION PanOption;
    QVariant Setting;

signals:
	void ChangeSettings();
	void SignalChangeStyle(int i);
};

#endif // PANORAMOPT_H
