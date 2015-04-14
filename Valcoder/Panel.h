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

#ifndef PANEL_H
#define PANEL_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

#include "ui_Panel.h"
#include "HID/Ea1dev.h"

class Panel : public QWidget
{
    Q_OBJECT

	public:

		Panel(QWidget *parent = 0);
		~Panel();
		void SetIndexButton(int funcNum, QPushButton *pPb);
        void SetIndexDial(int Enc, int funcNum, QPushButton *pPb);
        void writeSettings(QSettings *pSett);
        void readSettings(QSettings *pSett);
        void onWinEvent();
        void onClose();

	signals:

		void band(int);
		void mode(int);
		void mute();
		void power();
		void bin();
		void nr();
		void anf();
		void nb();
		void nb2();
		void lock();
		void memory(int);
		void pa();
		void station(int);

		void filter(int);
		void agc(int);
		void preamp(int);
		void sqlOnOff();
		void step(int);
		void paState();
		void mainFreq(int);
		void mon();
		void timeUp();
		void timePlus();
		void timeMinus();
		void zoom(int);

		void volume(int);
		void micGain(int);
		void squelch(int);

		void filterHigh(int);
		void filterLow(int);
		void filterWidth(int);
		void filterShift(int);
		void drive(int);
		void ifVal(int);
		void rfGain(int);

        void KeyDash(bool);
        void KeyDot(bool);
        void KeyPtt(bool);


	public slots:

		void ChangeFunc1(QAction *action);
		void ChangeFunc2(QAction *action);
		void ChangeFunc3(QAction *action);
		void ChangeFunc4(QAction *action);
		void ChangeFunc5(QAction *action);
		void ChangeFunc6(QAction *action);
		void ChangeFunc7(QAction *action);
		void ChangeFunc8(QAction *action);
		void ChangeFunc9(QAction *action);
		void ChangeFunc10(QAction *action);
		void ChangeFunc11(QAction *action);
		void ChangeFunc12(QAction *action);
		void ChangeFunc13(QAction *action);
		void ChangeFunc14(QAction *action);
		void ChangeDial0(QAction *action);
		void ChangeDial1(QAction *action);
		void ChangeDial2(QAction *action);
		void ChangeDial3(QAction *action);

		void ChangeMoveDial1(QAction *action);
		void ChangeMoveDial2(QAction *action);
		void ChangeMoveDial3(QAction *action);

        void onOpen(bool stat);
        void onMox(bool tx);

        void onEncLed(bool on);
        void onKeyLed(bool on);

	private:
		Ui::PanelClass ui;
		Ea1dev *pPan;

		// меню кнопок
	    QMenu *pMenuFunc[14];
	    QActionGroup *pActionFunc[14];

	    QMenu *pMenuDial[4];
	    QActionGroup *pActionDial[4];

	    QMenu *pMenuDialPress[4];
	    QActionGroup *pActionDialFunc[4];

	    int Func0;
	    int Func1;
	    int Func2;
	    int Func3;
	    int Func4;
	    int Func5;
	    int Func6;
	    int Func7;
	    int Func8;
	    int Func9;
	    int Func10;
	    int Func11;
	    int Func12;
	    int Func13;
	    int Func14;
	    int EncFuncVal0;
	    int EncFuncVal1;
	    int EncFuncVal2;
	    int EncFuncVal3;
	    int FuncDial0;
	    int FuncDial1;
	    int FuncDial2;
	    int FuncDial3;
	    int Encoder0Val;
	    int Encoder1Val;
	    int Encoder2Val;
	    int Encoder3Val;

	    void createMenu();
	    void createDialMenu();
	    void AddActionButton(QMenu *p);
	    void AddActionDial(QMenu *p);

	private slots:
		void SendFunc0(bool state);
		void SendFunc1(bool state);
		void SendFunc2(bool state);
		void SendFunc3(bool state);
		void SendFunc4(bool state);
		void SendFunc5(bool state);
		void SendFunc6(bool state);
		void SendFunc7(bool state);
		void SendFunc8(bool state);
		void SendFunc9(bool state);
		void SendFunc10(bool state);
		void SendFunc11(bool state);
		void SendFunc12(bool state);
		void SendFunc13(bool state);
		void SendFunc14(bool state);
		void SendFunc15(bool state);
		void SendFunc16(bool state);
		void SendFunc17(bool state);
		void SendFunc18(bool state);
		void SendFunc19(bool state);
		void SendFunc20(bool state);
		void SendFunc21(bool state);
		void SendFunc22(bool state);
		void SendFunc23(bool state);
		void SendFunc24(bool state);
		void SendFunc25(bool state);
		void SendFunc26(bool state);
		void SendFunc27(bool state);
	    void SendFunc28(bool state);
	    void SendFunc29(bool state);
	    void SendFunc30(bool state);
	    void SendFunc31(bool state);
	    void SendFunc32(bool state);

        void SendFuncDial0(int val);
        void SendFuncDial1(int val);
        void SendFuncDial2(int val);
        void SendFuncDial3(int val);
        void SendFuncDial4(int val);
        void SendFuncDial5(int val);
        void SendFuncDial6(int val);
        void SendFuncDial7(int val);
        void SendFuncDial8(int val);
        void SendFuncDial9(int val);

        void ChangeFreq(int val);
};

#endif // PANEL_H
