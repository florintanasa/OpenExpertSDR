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

//#include "wdggraph.h"
#include "WdgGraph.h"

WdgGraph::WdgGraph(Options *p,QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	pGl = new Panarama(p->ui.wdgPanOpt,ui.WdgGraph);
}

void WdgGraph::resizeEvent(QResizeEvent *event)
{
	ui.WdgGraph->resize(event->size().width()- ui.WdgTm->width(), event->size().height());
	pGl->resize(ui.WdgGraph->size());
}
