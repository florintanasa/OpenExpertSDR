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

#include "ColorWgt.h"
#include <QColorDialog>

ColorWgt::ColorWgt(QWidget *parent)
	: QWidget(parent)
{
	painter = new QPainter;
}

ColorWgt::~ColorWgt()
{
	delete painter;
}

void ColorWgt::mouseReleaseEvent(QMouseEvent *event)
{
	QColor clr = Color;
	Color = QColorDialog::getColor(Color);
	if (!Color.isValid())
		Color = clr;
	else
	{
		emit ColorChanged(Color);
		update();
	}
}

void ColorWgt::paintEvent(QPaintEvent *event)
{
    painter->begin(this);
        painter->setPen(Qt::black);
        painter->setBrush(QBrush(Color, Qt::SolidPattern));
        painter->drawRect(0, 0, width()-1, height()-1);
    painter->end();
}

void ColorWgt::resizeEvent(QResizeEvent *event)
{
	update();
}

void ColorWgt::setColor(QColor color)
{
	if(Color == color)
		return;

	Color = color;
	emit ColorChanged(Color);
	update();
}

QColor ColorWgt::getColor()
{
	return Color;
}



















