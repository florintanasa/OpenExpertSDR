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

#include "Ea1dev.h"
#include <string.h>

Ea1dev::Ea1dev()
{
	KeyStatus = 0;
	DevCount = 0;
	Opened = false;
	OutReport[0] = 0;
	InReport[0] = 0;
	Leds = 0;
}

QStringList Ea1dev::devList()
{
	QStringList Dev;
	QString DevName;
	HANDLE DevHandle;
	WCHAR  wbuf[128];
	int    i;
	BOOL   ok;

	GUID                             HidGuid;
	HDEVINFO                         DevInfo;
	SP_DEVICE_INTERFACE_DATA         DevData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA DevDetail;
	PHIDP_PREPARSED_DATA	         PreparsedData;
	HIDP_CAPS	                     Capabilities;
	ULONG                            Length;
	int                              Index;

	DevCount = 0;
	DevName = '\0';
	DevName.toWCharArray(wbuf);

	for(i = 1; i < 128; i++)
		wbuf[i] = wbuf[0];

	HidD_GetHidGuid(&HidGuid);
	DevInfo = SetupDiGetClassDevs(&HidGuid,	NULL, NULL,	(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	DevData.cbSize = sizeof(DevData);
	DevDetail = NULL;
	Index = -1;
	do{
		Index++;
		ok = SetupDiEnumDeviceInterfaces(DevInfo, 0, &HidGuid, Index, &DevData);
		if (!ok) break;
		ok = SetupDiGetDeviceInterfaceDetail(DevInfo, &DevData, NULL, 0, &Length, NULL);
		if (DevDetail) free(DevDetail);
		DevDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(Length);
		DevDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ok = SetupDiGetDeviceInterfaceDetail(DevInfo, &DevData, DevDetail, Length, NULL, NULL);
		if(!ok)
			continue;
		DevHandle = CreateFile(DevDetail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);
		if(DevHandle == INVALID_HANDLE_VALUE)
			continue;
		ok = HidD_GetPreparsedData(DevHandle, &PreparsedData);
		if(!ok)
			continue;
		HidP_GetCaps(PreparsedData, &Capabilities);
		HidD_FreePreparsedData(PreparsedData);
		if((Capabilities.UsagePage == 0xFF00) && (Capabilities.Usage == 0x0001))
		{
		  DevDetailData[DevCount++] = DevDetail;
		  DevDetail = NULL;
		}
		CloseHandle (DevHandle);
	}while(DevCount < DEV_NUM);

	if (DevDetail) free(DevDetail);
	SetupDiDestroyDeviceInfoList (DevInfo);
	for(int num = 0; num < DevCount; num++)
	{
		if (DevDetailData[num] == NULL)
			break;
		DevHandle = CreateFile(DevDetailData[num]->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);
		if (DevHandle == INVALID_HANDLE_VALUE)
		  break;
		if (HidD_GetProductString(DevHandle, wbuf, sizeof(wbuf)) != TRUE) break;
		Dev << QString::fromWCharArray(wbuf, 13);
		CloseHandle (DevHandle);
	}
	return Dev;
}

bool Ea1dev::open(int DevNum)
{
	if(Opened)
		close();
	DevHandle = INVALID_HANDLE_VALUE;
	if(DevDetailData[DevNum] == NULL) return (false);
	DevHandle = CreateFile(DevDetailData[DevNum]->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if(DevHandle == INVALID_HANDLE_VALUE) return (false);
	Opened = true;
	start(QThread::HighestPriority);
	return true;
}

bool Ea1dev::isOpen()
{
	return Opened;
}

void Ea1dev::close()
{
	if(Opened)
	{
		terminate();
		if(DevHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(DevHandle);
			DevHandle = INVALID_HANDLE_VALUE;
			Opened = false;
		}
	}
}

int Ea1dev::readBuf(BYTE *pBuf, DWORD Len)
{
	DWORD Cnt;
	int Result;
	QString str("");
	WCHAR  wbuf[128];
	HANDLE hEventObject;
	str.toWCharArray(wbuf);
	hEventObject = CreateEvent((LPSECURITY_ATTRIBUTES)NULL, FALSE, TRUE, wbuf);
	memset(&hidReadOverlapped, 0, sizeof(OVERLAPPED));
	hidReadOverlapped.hEvent = hEventObject;
	Result = ReadFile(DevHandle, pBuf, Len, &Cnt, &hidReadOverlapped);
	DWORD d1 = GetLastError();

	if(Result)
	{
		CloseHandle(hEventObject);
		return Cnt;
	}
	if(Result == 0 && ERROR_IO_PENDING != d1 )
	{
		CloseHandle(hEventObject);
		return 0;
	}

	Result = WaitForSingleObject (hEventObject,  3000);
	switch (Result)
	{
		case WAIT_OBJECT_0:
		  CloseHandle(hEventObject);
		  return hidReadOverlapped.InternalHigh;
		break;

		case WAIT_TIMEOUT:
		  CloseHandle(hEventObject);
		return 0;

		default:
		  CloseHandle(hEventObject);
		  return 0;
		break;
	}
	return 0;
}

int Ea1dev::writeBuf(BYTE *pBuf, DWORD Len)
{
	DWORD Cnt;
	QString str("");
	WCHAR  wbuf[128];
	str.toWCharArray(wbuf);
	memset(&hidWriteOverlapped, 0, sizeof(OVERLAPPED));
	if(WriteFile(DevHandle, pBuf, Len, &Cnt, &hidWriteOverlapped) != TRUE)
		return 0;
	return Cnt;
}

bool Ea1dev::setLedRit(bool Status)
{
	(Status) ? Leds |= LED_RIT_MASK : Leds &= ~LED_RIT_MASK;

	OutReport[0] = 0;
	OutReport[1] = Leds;
	if(writeBuf(OutReport, 2) == 0)
		return false;
	return true;
}

bool Ea1dev::setLedXit(bool Status)
{
	(Status) ? Leds |= LED_XIT_MASK : Leds &= ~LED_XIT_MASK;

	OutReport[0] = 0;
	OutReport[1] = Leds;
	if(writeBuf(OutReport, 2) == 0)
		return false;
	return true;
}

bool Ea1dev::setLedPwr(bool Status)
{
	(Status) ? Leds |= LED_PWR_MASK : Leds &= ~LED_PWR_MASK;

	OutReport[0] = 0;
	OutReport[1] = Leds;
	if(writeBuf(OutReport, 2) == 0)
		return false;
	return true;
}

bool Ea1dev::setLedEnc(bool Status)
{
	(Status) ? Leds |= LED_ENC_MASK : Leds &= ~LED_ENC_MASK;

	OutReport[0] = 0;
	OutReport[1] = Leds;
	if(writeBuf(OutReport, 2) == 0)
		return false;
	return true;
}

bool Ea1dev::setLedKey(bool Status)
{
	(Status) ? Leds |= LED_KEY_MASK : Leds &= ~LED_KEY_MASK;

	OutReport[0] = 0;
	OutReport[1] = Leds;
	if(writeBuf(OutReport, 2) == 0)
		return false;
	return true;
}

void Ea1dev::run()
{
	DWORD Keys;

	while(1)
	{
		InReport[0] = 0;
		if(readBuf(InReport, 18) == 0)
			continue;
		Keys = (InReport[4] << 16) | (InReport[3]<<8) | (InReport[2]);
		if(KeyStatus != Keys)
		{
			if((Keys & KEY1_MASK) != (KeyStatus & KEY1_MASK))
				emit Key1((bool)(Keys & KEY1_MASK));
			if((Keys & KEY2_MASK) != (KeyStatus & KEY2_MASK))
				emit Key2((bool)(Keys & KEY2_MASK));
			if((Keys & KEY3_MASK) != (KeyStatus & KEY3_MASK))
				emit Key3((bool)(Keys & KEY3_MASK));
			if((Keys & KEY4_MASK) != (KeyStatus & KEY4_MASK))
				emit Key4((bool)(Keys & KEY4_MASK));
			if((Keys & KEY5_MASK) != (KeyStatus & KEY5_MASK))
				emit Key5((bool)(Keys & KEY5_MASK));
			if((Keys & KEY6_MASK) != (KeyStatus & KEY6_MASK))
				emit Key6((bool)(Keys & KEY6_MASK));
			if((Keys & KEY7_MASK) != (KeyStatus & KEY7_MASK))
				emit Key7((bool)(Keys & KEY7_MASK));
			if((Keys & KEY8_MASK) != (KeyStatus & KEY8_MASK))
				emit Key8((bool)(Keys & KEY8_MASK));
			if((Keys & KEY9_MASK) != (KeyStatus & KEY9_MASK))
				emit Key9((bool)(Keys & KEY9_MASK));
			if((Keys & KEY10_MASK) != (KeyStatus & KEY10_MASK))
				emit Key10((bool)(Keys & KEY10_MASK));
			if((Keys & KEY11_MASK) != (KeyStatus & KEY11_MASK))
				emit Key11((bool)(Keys & KEY11_MASK));
			if((Keys & KEY12_MASK) != (KeyStatus & KEY12_MASK))
				emit Key12((bool)(Keys & KEY12_MASK));
			if((Keys & KEY13_MASK) != (KeyStatus & KEY13_MASK))
				emit Key13((bool)(Keys & KEY13_MASK));
			if((Keys & KEY14_MASK) != (KeyStatus & KEY14_MASK))
				emit Key14((bool)(Keys & KEY14_MASK));
			if((Keys & KEY_E0_MASK) != (KeyStatus & KEY_E0_MASK))
				emit KeyEnc0((bool)(Keys & KEY_E0_MASK));
			if((Keys & KEY_E1_MASK) != (KeyStatus & KEY_E1_MASK))
				emit KeyEnc1((bool)(Keys & KEY_E1_MASK));
			if((Keys & KEY_E2_MASK) != (KeyStatus & KEY_E2_MASK))
				emit KeyEnc2((bool)(Keys & KEY_E2_MASK));
			if((Keys & KEY_E3_MASK) != (KeyStatus & KEY_E3_MASK))
				emit KeyEnc3((bool)(Keys & KEY_E3_MASK));
			if((Keys & KEY_PTT_MASK) != (KeyStatus & KEY_PTT_MASK))
				emit KeyPtt((bool)(Keys & KEY_PTT_MASK));
			if((Keys & KEY_DASH_MASK) != (KeyStatus & KEY_DASH_MASK))
				emit KeyDash((bool)(Keys & KEY_DASH_MASK));
			if((Keys & KEY_DOT_MASK) != (KeyStatus & KEY_DOT_MASK))
				emit KeyDot((bool)(Keys & KEY_DOT_MASK));
			KeyStatus = Keys;
		}
		if(InReport[14]!=0)
			emit Enc0((int)((char)InReport[14]));
		if(InReport[15]!=0)
			emit Enc1((int)((char)InReport[15]));
		if(InReport[16]!=0)
			emit Enc2((int)((char)InReport[16]));
		if(InReport[17]!=0)
			emit Enc3((int)((char)InReport[17]));
	}
}
