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

#ifndef EA1DEV_H_
#define EA1DEV_H_

#include <stdlib.h>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "qt_windows.h"

extern "C"
{
	#include <ddk/hidsdi.h>
	#include <setupapi.h>
	#include <dbt.h>
}

#define DEV_NUM  10

#define	CMD_START_DDS		0x01
#define	CMD_STOP_DDS		0x02
#define	CMD_SET_MODE		0x03
#define	CMD_SET_DDS_FREQ	0x04
#define	CMD_GET_LINEAR_BUF	0x05
#define	CMD_GET_LOG_BUF		0x06

#define	LED_RIT_MASK		0x01
#define	LED_XIT_MASK		0x02
#define	LED_PWR_MASK		0x04
#define	LED_ENC_MASK		0x08
#define	LED_KEY_MASK		0x10

#define KEY0_MASK			0x00001
#define KEY1_MASK			0x00002
#define KEY2_MASK			0x00004
#define KEY3_MASK			0x00008
#define KEY4_MASK			0x00010
#define KEY5_MASK			0x00020
#define KEY6_MASK			0x00040
#define KEY7_MASK			0x00080
#define KEY8_MASK			0x00100
#define KEY9_MASK			0x00200
#define KEY10_MASK			0x00400
#define KEY11_MASK			0x00800
#define KEY12_MASK			0x01000
#define KEY13_MASK			0x02000
#define KEY14_MASK			0x04000
#define KEY_E0_MASK			0x08000
#define KEY_E1_MASK			0x10000
#define KEY_E2_MASK			0x20000
#define KEY_E3_MASK			0x40000
#define KEY_PTT_MASK		0x80000
#define KEY_DASH_MASK		0x100000
#define KEY_DOT_MASK		0x200000


class Ea1dev: public QThread
{
	Q_OBJECT

	private:
		PSP_DEVICE_INTERFACE_DETAIL_DATA   DevDetailData[DEV_NUM];
		HANDLE                             DevHandle;
		int                                DevCount;
		bool Opened;
		BYTE OutReport[65];
		BYTE InReport[65];
		int TimerID;

		DWORD Leds;
		DWORD KeyStatus;


	public:
		Ea1dev();

		QStringList devList();
		bool open(int DevNum);
		bool isOpen();
		void close();

		bool setLedRit(bool Status);
		bool setLedXit(bool Status);
		bool setLedPwr(bool Status);
		bool setLedEnc(bool Status);
		bool setLedKey(bool Status);

		void run();

	signals:
		void Key0(bool Status);
		void Key1(bool Status);
		void Key2(bool Status);
		void Key3(bool Status);
		void Key4(bool Status);
		void Key5(bool Status);
		void Key6(bool Status);
		void Key7(bool Status);
		void Key8(bool Status);
		void Key9(bool Status);
		void Key10(bool Status);
		void Key11(bool Status);
		void Key12(bool Status);
		void Key13(bool Status);
		void Key14(bool Status);
		void KeyEnc0(bool Status);
		void KeyEnc1(bool Status);
		void KeyEnc2(bool Status);
        void KeyEnc3(bool Status);
		void Enc0(int Val);
		void Enc1(int Val);
		void Enc2(int Val);
        void Enc3(int Val);
		void KeyPtt(bool Status);
		void KeyDash(bool Status);
		void KeyDot(bool Status);


	private:
		OVERLAPPED hidWriteOverlapped;
		OVERLAPPED hidReadOverlapped;
		int readBuf(BYTE *pBuf, DWORD Len);
		int writeBuf(BYTE *pBuf, DWORD Len);
		WORD DataBuffer0[4096];
		WORD DataBuffer1[4096];
};

#endif /* EA1DEV_H_ */
