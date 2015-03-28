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

#include "PanoramOpt.h"

PanoramOpt::PanoramOpt(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	ui.tabWidget1->setCurrentIndex(0);
	ui.stackedWidget->setCurrentIndex(0);
	ui.stackedWidget_2->setCurrentIndex(0);
	ui.stackedWidget_3->setCurrentIndex(0);
	ui.stackedWidget_4->setCurrentIndex(0);
	ui.stackedWidget_5->setCurrentIndex(0);
	PanOption.OffsetDbm = 0.0;
	PanOption.LenDbm = 50;
	PanOption.TypeCursor = CROSS;
	IsEnableChangedStyle = true;
	IsEnableWaitStyle = true;
	PanOption.RulePosY = 0.0f;
	PanOption.TRxMode = false;
	PanOption.GridOn = true;
	PanOption.StyleType = 0;
    PanOption.GridMoving = false;
    PanOption.GridSlowDown = false;
    PanOption.LevelSlowdown = 9800;
	PanOption.PanMode = ui.cbPanMode->currentIndex();
	PanOption.PanType = ui.cbPanType->currentIndex();
	PanOption.RigthClickChangeFilter = ui.chbChangeDDS->isChecked();
	PanOption.SwapLeftRigthButton = ui.chbSwapLeftRight->isChecked();
	pColorGradTop = new ColorWgt(ui.WdgGradTopColor);
	pColorGradBot = new ColorWgt(ui.WdgGradBotColor);
	pColorGradBack = new ColorWgt(ui.WdgGradBgrnColor);
	pColorSolidBack = new ColorWgt(ui.WdgGradBgrnColor_2);
	pColorLineLines = new ColorWgt(ui.WdgLineColor);
	pColorLineSpectrum = new ColorWgt(ui.WdgSpectrumColor);
	pColorLineBack = new ColorWgt(ui.WdgBackColor);
	pColorSolidLineBack = new ColorWgt(ui.WdgBackColor_2);
	pColorWFgrad = new ColorWgt(ui.WdgWfGradColor);
    pColorFilter1 = new ColorWgt(ui.WdgFilter1Color);
    pColorFilter2 = new ColorWgt(ui.WdgFilter2Color);
    pColorBandFilter1 = new ColorWgt(ui.WdgBand1Color);
    pColorBandFilter2 = new ColorWgt(ui.WdgBand2Color);
    pColorPitch1 = new ColorWgt(ui.WdgPitchColor1);
    pColorPitch2 = new ColorWgt(ui.WdgPitchColor2);
	pColorGradTop->resize(49, 24);
	pColorGradBot->resize(49, 24);
	pColorGradBack->resize(49, 24);
	pColorSolidBack->resize(49, 24);
	pColorLineLines->resize(49, 24);
	pColorLineSpectrum->resize(49, 24);
	pColorLineBack->resize(49, 24);
	pColorSolidLineBack->resize(49, 24);
	pColorWFgrad->resize(49, 24);
    pColorFilter1->resize(49, 24);
    pColorFilter2->resize(49, 24);
    pColorBandFilter1->resize(49, 24);
    pColorBandFilter2->resize(49, 24);
    pColorPitch1->resize(49, 24);
    pColorPitch2->resize(49, 24);

	pColorGradTop->setColor(QColor(0,255,162,255));
	pColorGradBot->setColor(QColor(32,107,73,255));
	pColorGradBack->setColor(QColor(88,0,0,255));
	pColorSolidBack->setColor(QColor(88,0,0,255));
	pColorLineLines->setColor(QColor(0,255,162,255));
	pColorLineSpectrum->setColor(QColor(32,107,73,255));
	pColorLineBack->setColor(QColor(88,0,0,255));
	pColorSolidLineBack->setColor(QColor(88,0,0,255));
	pColorWFgrad->setColor(QColor(0,213,255,255));
    pColorFilter1->setColor(Qt::red);
    pColorFilter2->setColor(QColor(31,182,220,180));
    pColorBandFilter1->setColor(QColor(120,120,120,180));
    pColorBandFilter2->setColor(QColor(100, 100, 120, 180));
    pColorPitch1->setColor(Qt::green);
    pColorPitch2->setColor(Qt::yellow);

	PanOption.ColorFilter1 = pColorFilter1->getColor();
	PanOption.ColorFilter2 = pColorFilter2->getColor();
	PanOption.ColorBand1 = pColorBandFilter1->getColor();
	PanOption.ColorBand2 = pColorBandFilter2->getColor();
	PanOption.ColorPitch1 = pColorPitch1->getColor();
	PanOption.ColorPitch2 = pColorPitch2->getColor();
	PanOption.TransparentFilter1 = ui.hslTransparentFilter1->value();
	PanOption.TransparentFilter2 = ui.hslTransparentFilter2->value();

	TrancpFilter = 150;
	ui.cbPanType->setCurrentIndex(0);
	ui.cbWfMode->setCurrentIndex(1);
	pColorGradTop->setColor(QColor(8, 255, 123));
	pColorGradBot->setColor(QColor(0, 62, 22));
	pColorGradBack->setColor(QColor(0, 0, 0, 255));
	ui.comboBox_2->setCurrentIndex(2);
	ui.comboBox_3->setCurrentIndex(0);
	ui.slSpGradSaturation->setValue(255);
	ui.slSpGradBrightness->setValue(255);

	PanType = 0;
	WfMode = 1;
	ColorGradTop = QColor(8, 255, 123);
	ColorGradBot = QColor(0, 62, 22);
	ColorGradBack = QColor(40, 0, 0, 255);
	ColorWFgrad = QColor(0, 41, 135);
    BackgroundType = 2;
    ImgBackgroundType = 0;
    TrancpGradTop = 255;
    TrancpGradBot = 255;
    ColorLineLines = QColor(197, 240, 255);
    ColorLineSpectrum = QColor(0, 41, 135);
    ColorLineBack = QColor(40, 0, 0, 255);
    TrancpSpectrumLine = 255;
    TrancpSpectrum = 180;

	PanOption.LineColor = pColorLineLines->getColor();
	PanOption.SpectrumColor = pColorLineSpectrum->getColor();
	PanOption.LineBackColor = pColorLineBack->getColor();
	PanOption.GradTopColor = pColorGradTop->getColor();
	PanOption.GradBotColor = pColorGradBot->getColor();
	PanOption.GradBgrnColor = pColorGradBack->getColor();
	PanOption.WfGradColor = pColorWFgrad->getColor();
	PanOption.ColorFilter1 = pColorFilter1->getColor();
	PanOption.ColorFilter1 = pColorFilter1->getColor();
	PanOption.ColorFilter2 = pColorFilter2->getColor();
	PanOption.ColorBand1 = pColorBandFilter1->getColor();
	PanOption.ColorBand2 = pColorBandFilter2->getColor();
	PanOption.ColorPitch1 = pColorPitch1->getColor();
	PanOption.ColorPitch2 = pColorPitch2->getColor();
	PanOption.LineBackColor2 = pColorSolidLineBack->getColor();
	PanOption.GradBgrnColor2 = pColorSolidBack->getColor();
	PanOption.TrancpSpectrumLine = ui.slTrancpSpectrum->value();
	PanOption.TrancpSpectrum = ui.slTrancpTop->value();
	PanOption.TrancpGradTop = ui.slSpGradSaturation->value();
	PanOption.TrancpGradBot = ui.slSpGradBrightness->value();
	PanOption.WfMode = ui.cbWfMode->currentIndex();
	PanOption.TrncpLineGrid = ui.horizontalSlider->value();
	PanOption.BackgroundType = ui.comboBox_2->currentIndex();
	PanOption.ImgBackgroundType = ui.comboBox_3->currentIndex();
	PanOption.StatePowerChange = ui.cbPowerChange->currentIndex();
	PanOption.WfMaxOffset = ui.sbWfMaxOffset->value();
	PanOption.WfMinOffset = ui.sbWfMinOffset->value();
	PanOption.SpMaxOffset = ui.sbSpMaxOffset->value();
	PanOption.SpMinOffset = ui.sbSpMinOffset->value();
	PanOption.Hysteresis = ui.sbHysteresis->value();

    PanOption.CustomColorGradTop = ColorGradTop;
    PanOption.CustomColorGradBot = ColorGradBot;
    PanOption.CustomColorGradBack = ColorGradBack;
    PanOption.CustomColorLineLines = ColorLineLines;
    PanOption.CustomColorLineSpectrum = ColorLineSpectrum;
    PanOption.CustomColorLineBack = ColorLineBack;
    PanOption.CustomColorWFgrad = ColorWFgrad;
    PanOption.CustomPanType = PanType;
    PanOption.CustomTrancpSpectrumLine = TrancpSpectrumLine;
    PanOption.CustomTrancpSpectrum = TrancpSpectrum;
    PanOption.CustomWfMode = WfMode;
    PanOption.CustomBackgroundType = BackgroundType;
    PanOption.CustomImgBackgroundType = ImgBackgroundType;
    PanOption.CustomTrancpGradTop = TrancpGradTop;
    PanOption.CustomTrancpGradBot = TrancpGradBot;
	Zoom = 1.0;
	ZoomPos = 0.0;

	connect(ui.cbDefaultSettingColor, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeStyle(int)));
	connect(ui.cbPanType, SIGNAL(currentIndexChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.cbWfMode, SIGNAL(currentIndexChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.slTrancpSpectrum, SIGNAL(valueChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.slTrancpTop, SIGNAL(valueChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.slSpGradSaturation, SIGNAL(valueChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.slSpGradBrightness, SIGNAL(valueChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.hslTransparentFilter1, SIGNAL(valueChanged(int)), this, SLOT(StyleChange(int)));
	connect(ui.hslTransparentFilter2, SIGNAL(valueChanged(int)), this, SLOT(StyleChange(int)));
	connect(pColorGradTop, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorGradBot, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorGradBack, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorWFgrad, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorLineLines, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorLineSpectrum, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorLineBack, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorFilter1, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorFilter2, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorBandFilter1, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorBandFilter2, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorPitch1, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(pColorPitch2, SIGNAL(ColorChanged(QColor)), this, SLOT(StyleChange(QColor)));
	connect(&TimerWait, SIGNAL(timeout()), this, SLOT(EnableChangedStyle()));
	connect(&TimerWaitStyle, SIGNAL(timeout()), this, SLOT(EnableChangedWaitStyle()));
}

PanoramOpt::~PanoramOpt()
{
	delete pColorGradTop;
	delete pColorGradBot;
	delete pColorGradBack;
	delete pColorLineLines;
	delete pColorLineSpectrum;
	delete pColorLineBack;
	delete pColorWFgrad;
}

void PanoramOpt::readSettings(QSettings *pSettings)
{
	pSettings->beginGroup("Panorama");
		ui.cbPanMode->setCurrentIndex(pSettings->value("PanMode", 0).toInt());
        ui.cbPanType->setCurrentIndex(pSettings->value("PanType", 1).toInt());
		ui.chbChangeDDS->setChecked(pSettings->value("ChangeDDS", true).toBool());
        ui.chbWheelChangeFilter->setChecked(pSettings->value("WheelChangeFilter", false).toBool());
		ui.chbSwapLeftRight->setChecked(pSettings->value("SwapLeftRight", true).toBool());
        pColorLineLines->setColor(pSettings->value("ColorLineLines", QColor(0,255,0,255)).value<QColor>());
        pColorLineSpectrum->setColor(pSettings->value("ColorLineSpectrum", QColor(0,200,40,255)).value<QColor>());
        pColorLineBack->setColor(pSettings->value("ColorLineBack", QColor(0,50,30,255)).value<QColor>());
		pColorGradTop->setColor(pSettings->value("ColorGradTop", QColor(8, 255, 123)).value<QColor>());
		pColorGradBot->setColor(pSettings->value("ColorGradBot", QColor(0, 62, 22)).value<QColor>());
        pColorGradBack->setColor(pSettings->value("ColorGradBack", QColor(8, 43, 33, 255)).value<QColor>());
		pColorWFgrad->setColor(pSettings->value("ColorWFgrad", QColor(0,213,255,255)).value<QColor>());
        pColorFilter1->setColor(pSettings->value("ColorFilter1",QColor(Qt::red)).value<QColor>());
		pColorFilter2->setColor(pSettings->value("ColorFilter2", QColor(31,182,220,180)).value<QColor>());
		pColorBandFilter1->setColor(pSettings->value("ColorBandFilter1", QColor(120,120,120,180)).value<QColor>());
		pColorBandFilter2->setColor(pSettings->value("ColorBandFilter2", QColor(100, 100, 120, 180)).value<QColor>());
        pColorPitch1->setColor(pSettings->value("ColorPitch1", QColor(Qt::green)).value<QColor>());
        pColorPitch2->setColor(pSettings->value("ColorPitch2", QColor(Qt::yellow)).value<QColor>());
		ui.hslTransparentFilter1->setValue(pSettings->value("TransparentFilter1", 128).toInt());
		ui.hslTransparentFilter2->setValue(pSettings->value("TransparentFilter2", 128).toInt());
		ui.slTrancpSpectrum->setValue(pSettings->value("TrancpSpectrum", 255).toInt());
        ui.slTrancpTop->setValue(pSettings->value("TrancpTop", 75).toInt());
		ui.slSpGradSaturation->setValue(pSettings->value("SpGradSaturation", 255).toInt());
		ui.slSpGradBrightness->setValue(pSettings->value("SpGradBrightness", 255).toInt());
		ui.cbWfMode->setCurrentIndex(pSettings->value("WfMode", 1).toInt());
        ui.comboBox_2->setCurrentIndex(pSettings->value("BackgroundType", 1).toInt());
		ui.comboBox_3->setCurrentIndex(pSettings->value("ImgBackgroundType", 0).toInt());
		ui.cbPowerChange->setCurrentIndex(pSettings->value("StatePowerChange", 1).toInt());
		ui.horizontalSlider->setValue(pSettings->value("TrncpLineGrid", 30).toInt());
		ui.cbAutoLevel->setCurrentIndex(pSettings->value("AutoSettingType", 0).toInt());
		ui.chbAverange->setChecked(pSettings->value("AvaregingWhenChangingDDS", true).toBool());
		ui.sbWfMaxOffset->setValue(pSettings->value("WfMaxOffset", 10).toInt());
		ui.sbWfMinOffset->setValue(pSettings->value("WfMinOffset", 25).toInt());
		ui.sbSpMaxOffset->setValue(pSettings->value("SpMaxOffset", 0).toInt());
		ui.sbSpMinOffset->setValue(pSettings->value("SpMinOffset", 0).toInt());
		ui.sbHysteresis->setValue(pSettings->value("Hysteresis", 5).toInt());
		ui.chbGridOn->setChecked(pSettings->value("GridOn", true).toBool());
		ColorGradTop = pSettings->value("ColorGradTop", QColor(8, 255, 123)).value<QColor>();
		ColorGradBot = pSettings->value("ColorGradBot", QColor(0, 62, 22)).value<QColor>();
        ColorGradBack = pSettings->value("ColorGradBack", QColor(8, 43, 33, 255)).value<QColor>();
        ColorLineLines = pSettings->value("ColorLineLines", QColor(0,255,0,255)).value<QColor>();
        ColorLineSpectrum = pSettings->value("ColorLineSpectrum", QColor(0,200,40,255)).value<QColor>();
        ColorLineBack = pSettings->value("ColorLineBack", QColor(0,50,30,255)).value<QColor>();
		ColorWFgrad = pSettings->value("ColorWFgrad", QColor(0, 41, 135)).value<QColor>();
        PanType = pSettings->value("CustomPanType", 1).toInt();
		TrancpSpectrumLine = pSettings->value("CustomTrancpSpectrumLine", 255).toInt();
        TrancpSpectrum = pSettings->value("CustomTrancpSpectrum", 75).toInt();
		WfMode = pSettings->value("CustomWfMode", 1).toInt();
		BackgroundType = pSettings->value("CustomBackgroundType", 2).toInt();
		ImgBackgroundType = pSettings->value("CustomImgBackgroundType", 0).toInt();
		TrancpGradTop = pSettings->value("CustomTrancpGradTop", 255).toInt();
		TrancpGradBot = pSettings->value("CustomTrancpGradBot", 255).toInt();
        pColorSolidBack->setColor(pSettings->value("GradBgrnColor2", QColor(88,0,0,255)).value<QColor>());
		pColorSolidLineBack->setColor(pSettings->value("LineBackColor2", QColor(20,20,20,255)).value<QColor>());
		ui.cbDefaultSettingColor->setCurrentIndex(pSettings->value("StyleType", 0).toInt());
		ui.chbGridMoving->setChecked(pSettings->value("GridMoving", false).toBool());
		ui.chbGridSlowDown->setChecked(pSettings->value("GridSlowDown", false).toBool());
		ui.slLevelSlowdown->setValue(pSettings->value("LevelSlowdown", 9800).toInt());
		Zoom = pSettings->value("Zoom", 1).toDouble();
		ZoomPos = pSettings->value("ZoomPos", 0).toDouble();
		ui.cbStepFilter->setCurrentIndex(pSettings->value("StepFilter", 0).toInt());
		PanOption.OffsetDbm = pSettings->value("OffsetDbm", 0).toFloat();
		PanOption.LenDbm = pSettings->value("LenDbm", 50).toFloat();
        PanOption.RulePosY = pSettings->value("RulePos", 0).toFloat();
        PanOption.TypeCursor = pSettings->value("CursorType", 1).toInt();
		PanOption.ZoomPos = ZoomPos;
		ui.chbStopWhenTx->setChecked(pSettings->value("StopWhenTx", true).toBool());
	pSettings->endGroup();

	emit ChangeSettings();
	emit SignalChangeStyle(0);
}

void PanoramOpt::writeSettings(QSettings *pSettings)
{
	pSettings->beginGroup("Panorama");
		pSettings->setValue("PanMode", ui.cbPanMode->currentIndex());
		pSettings->setValue("PanType", ui.cbPanType->currentIndex());
		pSettings->setValue("ChangeDDS", ui.chbChangeDDS->isChecked());
        pSettings->setValue("WheelChangeFilter", ui.chbWheelChangeFilter->isChecked());
		pSettings->setValue("SwapLeftRight", ui.chbSwapLeftRight->isChecked());
		pSettings->setValue("ColorLineLines", pColorLineLines->getColor());
		pSettings->setValue("ColorLineSpectrum", pColorLineSpectrum->getColor());
		pSettings->setValue("ColorLineBack", pColorLineBack->getColor());
		pSettings->setValue("ColorGradTop", pColorGradTop->getColor());
		pSettings->setValue("ColorGradBot", pColorGradBot->getColor());
		pSettings->setValue("ColorGradBack", pColorGradBack->getColor());
		pSettings->setValue("ColorWFgrad", pColorWFgrad->getColor());
		pSettings->setValue("ColorFilter1", pColorFilter1->getColor());
		pSettings->setValue("ColorFilter2", pColorFilter2->getColor());
		pSettings->setValue("ColorBandFilter1", pColorBandFilter1->getColor());
		pSettings->setValue("ColorBandFilter2", pColorBandFilter2->getColor());
		pSettings->setValue("ColorPitch1", pColorPitch1->getColor());
		pSettings->setValue("ColorPitch2", pColorPitch2->getColor());
		pSettings->setValue("TransparentFilter1", ui.hslTransparentFilter1->value());
		pSettings->setValue("TransparentFilter2", ui.hslTransparentFilter2->value());
		pSettings->setValue("TrancpSpectrum", ui.slTrancpSpectrum->value());
		pSettings->setValue("TrancpTop", ui.slTrancpTop->value());
		pSettings->setValue("SpGradSaturation", ui.slSpGradSaturation->value());
		pSettings->setValue("SpGradBrightness", ui.slSpGradBrightness->value());
		pSettings->setValue("WfMode", ui.cbWfMode->currentIndex());
		pSettings->setValue("BackgroundType", ui.comboBox_2->currentIndex());
		pSettings->setValue("ImgBackgroundType", ui.comboBox_3->currentIndex());
		pSettings->setValue("StatePowerChange", ui.cbPowerChange->currentIndex());
		pSettings->setValue("TrncpLineGrid", ui.horizontalSlider->value());
		pSettings->setValue("AutoSettingType", ui.cbAutoLevel->currentIndex());
		pSettings->setValue("AvaregingWhenChangingDDS", ui.chbAverange->isChecked());
		pSettings->setValue("WfMaxOffset", ui.sbWfMaxOffset->value());
		pSettings->setValue("WfMinOffset", ui.sbWfMinOffset->value());
		pSettings->setValue("SpMaxOffset", ui.sbSpMaxOffset->value());
		pSettings->setValue("SpMinOffset", ui.sbSpMinOffset->value());
		pSettings->setValue("Hysteresis", ui.sbHysteresis->value());
		pSettings->setValue("GridOn", ui.chbGridOn->isChecked());
		pSettings->setValue("ColorGradTop", ColorGradTop);
		pSettings->setValue("ColorGradBot", ColorGradBot);
		pSettings->setValue("ColorGradBack", ColorGradBack);
		pSettings->setValue("ColorLineLines", ColorLineLines);
		pSettings->setValue("ColorLineSpectrum", ColorLineSpectrum);
		pSettings->setValue("ColorLineBack", ColorLineBack);
		pSettings->setValue("ColorWFgrad", ColorWFgrad);
		pSettings->setValue("CustomPanType", PanType);
		pSettings->setValue("CustomTrancpSpectrumLine", TrancpSpectrumLine);
		pSettings->setValue("CustomTrancpSpectrum", TrancpSpectrum);
		pSettings->setValue("CustomWfMode", WfMode);
		pSettings->setValue("CustomBackgroundType", BackgroundType);
		pSettings->setValue("CustomImgBackgroundType", ImgBackgroundType);
		pSettings->setValue("CustomTrancpGradTop", TrancpGradTop);
		pSettings->setValue("CustomTrancpGradBot", TrancpGradBot);
		pSettings->setValue("GradBgrnColor2", pColorSolidBack->getColor());
		pSettings->setValue("LineBackColor2", pColorSolidLineBack->getColor());
		pSettings->setValue("StyleType", ui.cbDefaultSettingColor->currentIndex());
		pSettings->setValue("GridMoving", ui.chbGridMoving->isChecked());
		pSettings->setValue("GridSlowDown", ui.chbGridSlowDown->isChecked());
		pSettings->setValue("LevelSlowdown", ui.slLevelSlowdown->value());
		pSettings->setValue("Zoom", Zoom);
		pSettings->setValue("ZoomPos", ZoomPos);
		pSettings->setValue("StepFilter", ui.cbStepFilter->currentIndex());
		pSettings->setValue("OffsetDbm", PanOption.OffsetDbm);
		pSettings->setValue("LenDbm", PanOption.LenDbm);
		pSettings->setValue("RulePos", PanOption.RulePosY);
		pSettings->setValue("CursorType", PanOption.TypeCursor);
		pSettings->setValue("StopWhenTx", ui.chbStopWhenTx->isChecked());
	pSettings->endGroup();
}

double PanoramOpt::getZoom()
{
	return Zoom;
}

void PanoramOpt::ChangeStyle(int Type)
{
	if(!IsEnableWaitStyle)
		return;

	IsEnableChangedStyle = false;
	TimerWait.start(200);
	switch(Type)
	{
	case 0:
		ui.cbPanType->setCurrentIndex(1);
		pColorLineLines->setColor(QColor(131, 255, 103, 255));
		pColorLineSpectrum->setColor(QColor(32, 107, 73, 70));
		pColorLineBack->setColor(QColor(40, 0, 0, 255));
		ui.slTrancpSpectrum->setValue(255);
		ui.slTrancpTop->setValue(50);
		ui.cbWfMode->setCurrentIndex(1);
		ui.comboBox_2->setCurrentIndex(2);
		ui.comboBox_3->setCurrentIndex(1);
		pColorGradTop->setColor(QColor(0,255,162,255));
		pColorGradBot->setColor(QColor(32,107,73,255));
		pColorGradBack->setColor(QColor(88,0,0,255));
		pColorSolidBack->setColor(QColor(88,0,0,255));
		pColorLineLines->setColor(QColor(0,255,162,255));
		pColorLineSpectrum->setColor(QColor(32,107,73,255));
		pColorLineBack->setColor(QColor(88,0,0,255));
		pColorSolidLineBack->setColor(QColor(88,0,0,255));
	    pColorFilter1->setColor(Qt::red);
	    pColorFilter2->setColor(QColor(31,182,220,180));
	    pColorBandFilter1->setColor(QColor(120,120,120,180));
	    pColorBandFilter2->setColor(QColor(100, 100, 120, 180));
	    pColorPitch1->setColor(Qt::green);
	    pColorPitch2->setColor(Qt::yellow);
		ui.hslTransparentFilter1->setValue(100);
		ui.hslTransparentFilter2->setValue(100);
	    PanOption.GradTopColor = pColorGradTop->getColor();
	    PanOption.GradBotColor = pColorGradBot->getColor();
	    PanOption.GradBgrnColor = pColorGradBack->getColor();
	    PanOption.LineColor = pColorLineLines->getColor();
	    PanOption.SpectrumColor = pColorLineSpectrum->getColor();
	    PanOption.LineBackColor = pColorLineBack->getColor();
	    PanOption.WfGradColor = pColorWFgrad->getColor();
	    PanOption.PanType = ui.cbPanType->currentIndex();
	    PanOption.TrancpSpectrumLine = ui.slTrancpSpectrum->value();
	    PanOption.TrancpSpectrum = ui.slTrancpTop->value();
	    PanOption.WfMode = ui.cbWfMode->currentIndex();
	    PanOption.BackgroundType = ui.comboBox_2->currentIndex();
	    PanOption.ImgBackgroundType = ui.comboBox_3->currentIndex();
	    PanOption.TrancpGradTop = ui.slSpGradSaturation->value();
	    PanOption.TrancpGradBot = ui.slSpGradBrightness->value();
		break;
	case 1:
		ui.cbPanType->setCurrentIndex(0);
		ui.cbWfMode->setCurrentIndex(1);
		pColorGradTop->setColor(QColor(8, 255, 123));
		pColorGradBot->setColor(QColor(0, 62, 22));
		pColorGradBack->setColor(QColor(40, 0, 0, 255));
		ui.comboBox_2->setCurrentIndex(2);
		ui.comboBox_3->setCurrentIndex(0);
		ui.slSpGradSaturation->setValue(255);
		ui.slSpGradBrightness->setValue(255);
		pColorGradTop->setColor(QColor(0,255,162,255));
		pColorGradBot->setColor(QColor(32,107,73,255));
		pColorGradBack->setColor(QColor(88,0,0,255));
		pColorSolidBack->setColor(QColor(88,0,0,255));
		pColorLineLines->setColor(QColor(0,255,162,255));
		pColorLineSpectrum->setColor(QColor(32,107,73,255));
		pColorLineBack->setColor(QColor(88,0,0,255));
		pColorSolidLineBack->setColor(QColor(88,0,0,255));
	    pColorFilter1->setColor(Qt::red);
	    pColorFilter2->setColor(QColor(31,182,220,180));
	    pColorBandFilter1->setColor(QColor(120,120,120,180));
	    pColorBandFilter2->setColor(QColor(100, 100, 120, 180));
	    pColorPitch1->setColor(Qt::green);
	    pColorPitch2->setColor(Qt::yellow);
		ui.hslTransparentFilter1->setValue(100);
		ui.hslTransparentFilter2->setValue(100);
	    PanOption.GradTopColor = pColorGradTop->getColor();
	    PanOption.GradBotColor = pColorGradBot->getColor();
	    PanOption.GradBgrnColor = pColorGradBack->getColor();
	    PanOption.LineColor = pColorLineLines->getColor();
	    PanOption.SpectrumColor = pColorLineSpectrum->getColor();
	    PanOption.LineBackColor = pColorLineBack->getColor();
	    PanOption.WfGradColor = pColorWFgrad->getColor();
	    PanOption.PanType = ui.cbPanType->currentIndex();
	    PanOption.TrancpSpectrumLine = ui.slTrancpSpectrum->value();
	    PanOption.TrancpSpectrum = ui.slTrancpTop->value();
	    PanOption.WfMode = ui.cbWfMode->currentIndex();
	    PanOption.BackgroundType = ui.comboBox_2->currentIndex();
	    PanOption.ImgBackgroundType = ui.comboBox_3->currentIndex();
	    PanOption.TrancpGradTop = ui.slSpGradSaturation->value();
	    PanOption.TrancpGradBot = ui.slSpGradBrightness->value();
		break;
	case 2:
		ui.cbPanType->setCurrentIndex(0);
		ui.cbWfMode->setCurrentIndex(0);
		pColorGradTop->setColor(QColor(0, 255, 162));
		pColorGradBot->setColor(QColor(16, 54, 36));
		pColorGradBack->setColor(QColor(40, 0, 0, 255));
		ui.comboBox_2->setCurrentIndex(2);
		ui.comboBox_3->setCurrentIndex(2);
		ui.slSpGradSaturation->setValue(255);
		ui.slSpGradBrightness->setValue(255);
		pColorWFgrad->setColor(QColor(0, 88, 56));
		pColorGradTop->setColor(QColor(0,255,162,255));
		pColorGradBot->setColor(QColor(32,107,73,255));
		pColorGradBack->setColor(QColor(88,0,0,255));
		pColorSolidBack->setColor(QColor(88,0,0,255));
		pColorLineLines->setColor(QColor(0,255,162,255));
		pColorLineSpectrum->setColor(QColor(32,107,73,255));
		pColorLineBack->setColor(QColor(88,0,0,255));
		pColorSolidLineBack->setColor(QColor(88,0,0,255));
	    pColorFilter1->setColor(Qt::red);
	    pColorFilter2->setColor(QColor(31,182,220,180));
	    pColorBandFilter1->setColor(QColor(120,120,120,180));
	    pColorBandFilter2->setColor(QColor(100, 100, 120, 180));
	    pColorPitch1->setColor(Qt::green);
	    pColorPitch2->setColor(Qt::yellow);
		ui.hslTransparentFilter1->setValue(100);
		ui.hslTransparentFilter2->setValue(100);
	    PanOption.GradTopColor = pColorGradTop->getColor();
	    PanOption.GradBotColor = pColorGradBot->getColor();
	    PanOption.GradBgrnColor = pColorGradBack->getColor();
	    PanOption.LineColor = pColorLineLines->getColor();
	    PanOption.SpectrumColor = pColorLineSpectrum->getColor();
	    PanOption.LineBackColor = pColorLineBack->getColor();
	    PanOption.WfGradColor = pColorWFgrad->getColor();
	    PanOption.PanType = ui.cbPanType->currentIndex();
	    PanOption.TrancpSpectrumLine = ui.slTrancpSpectrum->value();
	    PanOption.TrancpSpectrum = ui.slTrancpTop->value();
	    PanOption.WfMode = ui.cbWfMode->currentIndex();
	    PanOption.BackgroundType = ui.comboBox_2->currentIndex();
	    PanOption.ImgBackgroundType = ui.comboBox_3->currentIndex();
	    PanOption.TrancpGradTop = ui.slSpGradSaturation->value();
	    PanOption.TrancpGradBot = ui.slSpGradBrightness->value();
		break;
	case 3:
		ui.cbPanType->setCurrentIndex(1);
		pColorLineLines->setColor(QColor(197, 240, 255));
		pColorLineSpectrum->setColor(QColor(0, 41, 135));
		pColorLineBack->setColor(QColor(40, 0, 0, 255));
		ui.slTrancpSpectrum->setValue(255);
		ui.slTrancpTop->setValue(180);
		ui.cbWfMode->setCurrentIndex(0);
		ui.comboBox_2->setCurrentIndex(2);
		ui.comboBox_3->setCurrentIndex(3);
		pColorWFgrad->setColor(QColor(0, 41, 135));
		pColorGradTop->setColor(QColor(0,255,162,255));
		pColorGradBot->setColor(QColor(32,107,73,255));
		pColorGradBack->setColor(QColor(88,0,0,255));
		pColorSolidBack->setColor(QColor(88,0,0,255));
		pColorLineLines->setColor(QColor(0,255,162,255));
		pColorLineSpectrum->setColor(QColor(32,107,73,255));
		pColorLineBack->setColor(QColor(88,0,0,255));
		pColorSolidLineBack->setColor(QColor(88,0,0,255));
	    pColorFilter1->setColor(Qt::red);
	    pColorFilter2->setColor(QColor(31,182,220,180));
	    pColorBandFilter1->setColor(QColor(120,120,120,180));
	    pColorBandFilter2->setColor(QColor(100, 100, 120, 180));
	    pColorPitch1->setColor(Qt::green);
	    pColorPitch2->setColor(Qt::yellow);
		ui.hslTransparentFilter1->setValue(100);
		ui.hslTransparentFilter2->setValue(100);
	    PanOption.GradTopColor = pColorGradTop->getColor();
	    PanOption.GradBotColor = pColorGradBot->getColor();
	    PanOption.GradBgrnColor = pColorGradBack->getColor();
	    PanOption.LineColor = pColorLineLines->getColor();
	    PanOption.SpectrumColor = pColorLineSpectrum->getColor();
	    PanOption.LineBackColor = pColorLineBack->getColor();
	    PanOption.WfGradColor = pColorWFgrad->getColor();
	    PanOption.PanType = ui.cbPanType->currentIndex();
	    PanOption.TrancpSpectrumLine = ui.slTrancpSpectrum->value();
	    PanOption.TrancpSpectrum = ui.slTrancpTop->value();
	    PanOption.WfMode = ui.cbWfMode->currentIndex();
	    PanOption.BackgroundType = ui.comboBox_2->currentIndex();
	    PanOption.ImgBackgroundType = ui.comboBox_3->currentIndex();
	    PanOption.TrancpGradTop = ui.slSpGradSaturation->value();
	    PanOption.TrancpGradBot = ui.slSpGradBrightness->value();
		break;
	case 4:
		PanOption.PanType = PanType;
		PanOption.LineColor = ColorLineLines;
		PanOption.SpectrumColor = ColorLineSpectrum;
		PanOption.LineBackColor = ColorLineBack;
		PanOption.TrancpSpectrumLine = TrancpSpectrumLine;
		PanOption.TrancpSpectrum = TrancpSpectrum;
		PanOption.WfMode = WfMode;
		PanOption.BackgroundType = BackgroundType;
		PanOption.ImgBackgroundType = ImgBackgroundType;
		PanOption.WfGradColor = ColorWFgrad;
		PanOption.GradTopColor = ColorGradTop;
		PanOption.GradBotColor = ColorGradBot;
		PanOption.GradBgrnColor = ColorGradBack;
		ui.cbPanType->setCurrentIndex(PanOption.PanType);
		pColorLineLines->setColor(PanOption.LineColor);
		pColorLineSpectrum->setColor(PanOption.SpectrumColor);
		pColorLineBack->setColor(PanOption.LineBackColor);
		pColorGradTop->setColor(PanOption.GradTopColor);
		pColorGradBot->setColor(PanOption.GradBotColor);
		pColorGradBack->setColor(PanOption.GradBgrnColor);
		pColorWFgrad->setColor(PanOption.WfGradColor);
		ui.slTrancpSpectrum->setValue(PanOption.TrancpSpectrumLine);
		ui.slTrancpTop->setValue(PanOption.TrancpSpectrum);
		ui.slSpGradSaturation->setValue(PanOption.TrancpGradTop);
		ui.slSpGradBrightness->setValue(PanOption.TrancpGradBot);
		ui.cbWfMode->setCurrentIndex(PanOption.WfMode);
		ui.comboBox_2->setCurrentIndex(PanOption.BackgroundType);
		ui.comboBox_3->setCurrentIndex(PanOption.ImgBackgroundType);
		pColorFilter1->setColor(PanOption.ColorFilter1);
		pColorFilter2->setColor(PanOption.ColorFilter2);
		pColorBandFilter1->setColor(PanOption.ColorBand1);
		pColorBandFilter2->setColor(PanOption.ColorBand2);
		pColorPitch1->setColor(PanOption.ColorPitch1);
		pColorPitch2->setColor(PanOption.ColorPitch2);
		ui.hslTransparentFilter1->setValue(PanOption.TransparentFilter1);
		ui.hslTransparentFilter2->setValue(PanOption.TransparentFilter2);
		break;
	default:
		break;
	}
	emit SignalChangeStyle(0);
}


void PanoramOpt::StyleChange(QColor i)
{
	StyleChange(0);
}

void PanoramOpt::StyleChange(int i)
{
	if(!IsEnableChangedStyle)
		return;

	PanOption.TransparentFilter1 = ui.hslTransparentFilter1->value();
	PanOption.TransparentFilter2 = ui.hslTransparentFilter2->value();
	IsEnableWaitStyle = false;
	TimerWaitStyle.start(200);
    ColorGradTop = pColorGradTop->getColor();
    ColorGradBot = pColorGradBot->getColor();
    ColorGradBack = pColorGradBack->getColor();
    ColorLineLines = pColorLineLines->getColor();
    ColorLineSpectrum = pColorLineSpectrum->getColor();
    ColorLineBack = pColorLineBack->getColor();
    ColorWFgrad = pColorWFgrad->getColor();
    PanType = ui.cbPanType->currentIndex();
    TrancpSpectrumLine = ui.slTrancpSpectrum->value();
    TrancpSpectrum = ui.slTrancpTop->value();
    WfMode = ui.cbWfMode->currentIndex();
    BackgroundType = ui.comboBox_2->currentIndex();
    ImgBackgroundType = ui.comboBox_3->currentIndex();
    TrancpGradTop = ui.slSpGradSaturation->value();
    TrancpGradBot = ui.slSpGradBrightness->value();
	PanOption.PanType = PanType;
	PanOption.LineColor = ColorLineLines;
	PanOption.SpectrumColor = ColorLineSpectrum;
	PanOption.LineBackColor = ColorLineBack;
	PanOption.TrancpSpectrumLine = TrancpSpectrumLine;
	PanOption.TrancpSpectrum = TrancpSpectrum;
	PanOption.WfMode = WfMode;
	PanOption.BackgroundType = BackgroundType;
	PanOption.ImgBackgroundType = ImgBackgroundType;
	PanOption.WfGradColor = ColorWFgrad;
	PanOption.GradTopColor = ColorGradTop;
	PanOption.GradBotColor = ColorGradBot;
	PanOption.GradBgrnColor = ColorGradBack;
	PanOption.ColorFilter1 = pColorFilter1->getColor();
	PanOption.ColorFilter2 = pColorFilter2->getColor();
	PanOption.ColorBand1 = pColorBandFilter1->getColor();
	PanOption.ColorBand2 = pColorBandFilter2->getColor();
	PanOption.ColorPitch1 = pColorPitch1->getColor();
	PanOption.ColorPitch2 = pColorPitch2->getColor();
    ui.cbDefaultSettingColor->setCurrentIndex(4);
    PanOption.StyleType = ui.cbDefaultSettingColor->currentIndex();
    emit SignalChangeStyle(0);
}

void PanoramOpt::EnableChangedStyle()
{
	IsEnableChangedStyle = true;
	TimerWait.stop();
}

void PanoramOpt::EnableChangedWaitStyle()
{
	IsEnableWaitStyle = true;
	TimerWaitStyle.stop();
}

void PanoramOpt::setZoom(double val)
{
	Zoom = val;
}

void PanoramOpt::setZoomPos(double val)
{
	ZoomPos = val;
}

double PanoramOpt::getZoomPos()
{
	return ZoomPos;
}

