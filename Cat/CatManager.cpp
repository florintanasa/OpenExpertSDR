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

#include "CatManager.h"
#include "ui_expertsdr_va2_1.h"
#include "expertsdr_va2_1.h"
#include "../expertsdr_va2_1.h"

CatManager::CatManager(ExpertSDR_vA2_1 *pE, Options *pO, QWidget *parent) : QWidget(parent)
{
	pOpt = pO;
	pSdr = pE;
	TimerId = 0;
    pOpt->ui.cbCatPortName->clear();

       QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
       if (ports.size()!=0)
       {
           for (int i = 0; i < ports.size(); i++)
           {
                  pOpt->ui.cbCatPortName->addItem(ports.at(i).portName.toLocal8Bit().constData(),0);
           }
       }


    #ifdef Q_OS_LINUX
        pCom = new QextSerialPort("/dev/ttyS0", QextSerialPort::EventDriven);
    #else
        pCom = new QextSerialPort("COM1", QextSerialPort::EventDriven);
    #endif /*Q_OS_LINUX*/

	connect(pOpt->ui.chbCatEnable, SIGNAL(clicked(bool)), this, SLOT(Open(bool)));
	connect(pCom, SIGNAL(readyRead()), this, SLOT(comReceive()));
	connect(pCom, SIGNAL(dsrChanged(bool)), this, SLOT(OnKeyDsr(bool)));
	connect(pCom, SIGNAL(ctsChanged(bool)), this, SLOT(OnKeyCts(bool)));
}

CatManager::~CatManager()
{
	if(pCom->isOpen())
		pCom->close();
    delete pCom;
}

void CatManager::closeEvent(QCloseEvent *event)
{
	if(pCom->isOpen())
		pCom->close();
}

void CatManager::timerEvent(QTimerEvent *event)
{

}

void CatManager::comReceive()
{
	int Len;
	if((Len = pCom->bytesAvailable()) > 0)
	{
		QByteArray data = pCom->read(Len);

		if(Len <= 2)
			return;

		SearchCmdList(&data, &Param);

		for(int i = 0; i < Param.count(); i++)
		{
			PerformCmd(Param.at(i));
		}
		Param.clear();
	}
}

void CatManager::Open(bool status)
{
	if(status)
	{
    	pCom->setBaudRate((BaudRateType)pOpt->ui.cbBaudRate->currentText().toInt());
    	pCom->setDataBits((DataBitsType)(pOpt->ui.cbCatData->currentIndex()+5));
    	pCom->setStopBits((StopBitsType)(pOpt->ui.cbCatStopBits->currentIndex()));
    	pCom->setParity((ParityType)pOpt->ui.cbCatParity->currentIndex());
    	pCom->setFlowControl(FLOW_OFF);
    	pCom->setTimeout(1);

    	if(pOpt->ui.cbCatPortName->currentIndex() < 8)
    	{
    		pCom->setPortName(pOpt->ui.cbCatPortName->currentText());
    		pCom->open(QIODevice::ReadWrite);
    	}
    	else
    	{
    		pCom->setPortName("\\\\.\\" + pOpt->ui.cbCatPortName->currentText());
    		pCom->open(QIODevice::ReadWrite);
    	}

    	if(!pCom->isOpen())
    	{
    		pOpt->ui.chbCatEnable->setChecked(false);
    		pOpt->ui.cbCatStopBits->setEnabled(true);
    		pOpt->ui.cbCatParity->setEnabled(true);
    		pOpt->ui.cbCatPortName->setEnabled(true);
    		pOpt->ui.cbBaudRate->setEnabled(true);
    		pOpt->ui.cbCatData->setEnabled(true);
			QMessageBox msgBox;
			msgBox.setText(tr("Couldn't open CAT Com port!"));
			msgBox.exec();
    		return;
    	}
    	pOpt->ui.chbCatEnable->setChecked(true);
    	pOpt->ui.cbCatStopBits->setEnabled(false);
    	pOpt->ui.cbCatParity->setEnabled(false);
    	pOpt->ui.cbCatPortName->setEnabled(false);
    	pOpt->ui.cbBaudRate->setEnabled(false);
    	pOpt->ui.cbCatData->setEnabled(false);
	}
	else
	{
		Close();
	}
}

void CatManager::Close()
{
	pCom->close();

	pOpt->ui.chbCatEnable->setChecked(false);
	pOpt->ui.cbCatStopBits->setEnabled(true);
	pOpt->ui.cbCatParity->setEnabled(true);
	pOpt->ui.cbCatPortName->setEnabled(true);
	pOpt->ui.cbBaudRate->setEnabled(true);
	pOpt->ui.cbCatData->setEnabled(true);
}

bool CatManager::IsOpen()
{
	return pCom->isOpen();
}

void CatManager::SearchCmdList(QByteArray *pArray, QStringList *pList)
{
	QString str((*pArray));
    for(int i = 0; str.section(';', i, i) != 0; i++)
    	(*pList)<<str.section(';', i, i);
}

void CatManager::PerformCmd(QString str)
{
	short cmd = GetNameCmd(str);
	QString tmpstr1, tmpstr2, tmpFreq, tmpStep, tmpMod;
	int Freq, Tmp;
	QString SendStr;
	QByteArray Data;
	switch(cmd)
	{
	case 0x4741:
		if(str.size() == 3)
		{
			if(str.right(1).toInt() == 0)
			{
                Tmp = (pSdr->ui->slVol->value()*255)/100;
                tmpstr1.setNum((pSdr->ui->slVol->value()*255)/100);
				if(Tmp < 10)
					SendStr = "AG000" + tmpstr1 + ";";
				else if(Tmp < 100)
					SendStr = "AG00" + tmpstr1 + ";";
				else
					SendStr = "AG0" + tmpstr1 + ";";

                Data = SendStr.toLatin1();
				pCom->write(Data);
			}
		}
        else if(str.size() == 6)
		{
        	emit changedVolume(str.right(3).toInt());
            pSdr->ui->slVol->setValue((str.right(3).toInt()*100)/255);
            pSdr->OnVolume((str.right(3).toInt()*100)/255);



		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4941:
		SentErrorCode(ERROR_CMD);
		break;
	case 0x4442:
		if(str.size() == 2)
		{
			emit changedBand(-1);
			Tmp = pSdr->pBandBut->checkedId();
			if(--Tmp < 0)
				Tmp = 13;
			pSdr->OnChangeBand(Tmp);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5542:
		if(str.size() == 2)
		{
			emit changedBand(1);
			Tmp = pSdr->pBandBut->checkedId();
			if(++Tmp > 13)
				Tmp = 0;
			pSdr->OnChangeBand(Tmp);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4E44:
		if(str.size() == 2)
		{
			emit changedIncrDecrVFO_A(-1);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4146:
	case 0x5246:
	case 0x5446:
		if(str.size() == 2)
		{
			Freq = pSdr->FreqNum + pSdr->pGraph->pGl->GetPitch();
			tmpstr1.setNum(Freq);

			if(Freq < 10)
				tmpFreq = "0000000000" + tmpstr1;
			else if(Freq < 100)
				tmpFreq = "000000000" + tmpstr1;
			else if(Freq < 1000)
				tmpFreq = "00000000" + tmpstr1;
			else if(Freq < 10000)
				tmpFreq = "0000000" + tmpstr1;
			else if(Freq < 100000)
				tmpFreq = "000000" + tmpstr1;
			else if(Freq < 1000000)
				tmpFreq = "00000" + tmpstr1;
			else if(Freq < 10000000)
				tmpFreq = "0000" + tmpstr1;
			else if(Freq < 100000000)
				tmpFreq = "000" + tmpstr1;
			else if(Freq < 1000000000)
				tmpFreq = "00" + tmpstr1;

			if(cmd == 0x4146)
			{
				SendStr = "FA" + tmpFreq + ";";
			}
			else if(cmd == 0x5246)
			{
				SendStr = "FR" + tmpFreq + ";";
			}
			else if(cmd == 0x5446)
			{
                if(pSdr->ui->pbSplit->isChecked())
				{
					SendStr = "FT" + tmpFreq + ";";
				}
				else
				{
					Freq = pSdr->pVfoB->value() + pSdr->pGraph->pGl->GetPitch();
					tmpstr1.setNum(Freq);
					SendStr = "FT" + tmpFreq + ";";
				}
			}
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 13)
		{
			emit changedVFO_A(str.right(11).toInt() - pSdr->pGraph->pGl->GetPitch());
			pSdr->OnChangeMainFreqWithWdgMem(str.right(11).toInt() - pSdr->pGraph->pGl->GetPitch(), pSdr->MainMod());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4246:
		if(str.size() == 2)
		{
			Freq = pSdr->pVfoB->value() + pSdr->pGraph->pGl->GetPitch();//FreqNum;
			tmpstr1.setNum(Freq);

			if(Freq < 10)
				tmpFreq = "0000000000" + tmpstr1;
			else if(Freq < 100)
				tmpFreq = "000000000" + tmpstr1;
			else if(Freq < 1000)
				tmpFreq = "00000000" + tmpstr1;
			else if(Freq < 10000)
				tmpFreq = "0000000" + tmpstr1;
			else if(Freq < 100000)
				tmpFreq = "000000" + tmpstr1;
			else if(Freq < 1000000)
				tmpFreq = "00000" + tmpstr1;
			else if(Freq < 10000000)
				tmpFreq = "0000" + tmpstr1;
			else if(Freq < 100000000)
				tmpFreq = "000" + tmpstr1;
			else if(Freq < 1000000000)
				tmpFreq = "00" + tmpstr1;

			SendStr = "FB" + tmpFreq + ";";

            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 13)
		{
			emit changedVFO_B(str.right(11).toInt() - pSdr->pGraph->pGl->GetPitch());
			pSdr->pVfoB->setValue(str.right(11).toInt() - pSdr->pGraph->pGl->GetPitch());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
		}
		break;
	case 0x5746:
		SentErrorCode(ERROR_CMD);
		break;
	case 0x5447:
		if(str.size() == 2)
		{
			Tmp = pSdr->MainAGC();
			tmpstr1.setNum(Tmp);
			SendStr = "GT00" + tmpstr1 + ";";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 5)
		{
			Tmp = str.right(3).toInt();
			if(Tmp >= 0 && Tmp < 6)
			{
				emit changedAGC(str.right(3).toInt());
				pSdr->OnAgc(Tmp);
			}
			else
			{
				SentErrorCode(ERROR_PARAM);
			}
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4449:
		if(str.size() == 2)
		{
			SendStr = "ID019;";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4649:
		if(str.size() != 2)
		{
			SentErrorCode(ERROR_CMD);
			return;
		}
		tmpMod = ModToCat[pSdr->MainMod()];
		if(tmpMod == "0")
		{
			SentErrorCode(ERROR_PARAM);
			return;
		}

		Freq = pSdr->FreqNum + pSdr->pGraph->pGl->GetPitch();
		tmpstr1.setNum(Freq);

		if(Freq < 10)
			tmpFreq = "0000000000" + tmpstr1;
		else if(Freq < 100)
			tmpFreq = "000000000" + tmpstr1;
		else if(Freq < 1000)
			tmpFreq = "00000000" + tmpstr1;
		else if(Freq < 10000)
			tmpFreq = "0000000" + tmpstr1;
		else if(Freq < 100000)
			tmpFreq = "000000" + tmpstr1;
		else if(Freq < 1000000)
			tmpFreq = "00000" + tmpstr1;
		else if(Freq < 10000000)
			tmpFreq = "0000" + tmpstr1;
		else if(Freq < 100000000)
			tmpFreq = "000" + tmpstr1;
		else if(Freq < 1000000000)
			tmpFreq = "00" + tmpstr1;

		tmpStep = StepToCat[pSdr->MainStep()];

		tmpstr1 = "IF" + tmpFreq + tmpStep + "000000" + "0" + "0" + "0" + "00";

        tmpstr2.setNum((int)(pSdr->ui->pbMox->isChecked()));

		tmpstr1 += tmpstr2 + tmpMod + "0" + "0" + "0" + "0" + "00" + "0;";

                Data = tmpstr1.toLatin1();
		pCom->write(Data);

		break;
	case 0x534B:
		if(str.size() == 2)
		{
			SendStr = "KS000;";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 5)
		{
			emit changetCWLSpeed(str.right(3).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x594B:
		SentErrorCode(ERROR_CMD);
		break;
	case 0x444D:
		if(str.size() == 2)
		{
			tmpMod = ModToCat[pSdr->MainMod()];
			if(tmpMod == "0")
			{
				SentErrorCode(ERROR_PARAM);
				return;
			}

			SendStr = "MD" + tmpMod + ";";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			emit changedMode(str.right(1).toInt());
			Tmp = CatToMod[str.right(1).toInt()];
			if(Tmp >= 0)
				pSdr->OnChangeMode(Tmp);
			else
				SentErrorCode(ERROR_PARAM);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x474D:
		if(str.size() == 2)
		{
            Tmp = pSdr->ui->slMic->value();
			if(Tmp < 10)
			{
				tmpstr1.setNum(Tmp);
				SendStr = "MG00" + tmpstr1 + ";";
			}
			else if(Tmp < 100)
			{
				tmpstr1.setNum(Tmp);
				SendStr = "MG0" + tmpstr1 + ";";
			}
			else
			{
				tmpstr1.setNum(Tmp);
				SendStr = "MG" + tmpstr1 + ";";
			}
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 5)
		{
			emit changedGainMicrophone(str.right(3).toInt());
			Tmp = str.right(3).toInt();
			pSdr->OnMic(Tmp);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4F4D:
		SentErrorCode(ERROR_CMD);
		break;
	case 0x424E:
		if(str.size() == 2)
		{
            if(pSdr->ui->pbNb->isChecked())
				SendStr = "NB0;";
			else
				SendStr = "NB1;";

            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			if(str.right(1).toInt() != 0)
				pSdr->OnNb(false);
			else
				pSdr->OnNb(true);

			emit changedNB((bool)str.right(1).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x544E:
		if(str.size() == 2)
		{
            if(pSdr->ui->pbAnf->isChecked())
				SendStr = "NT0;";
			else
				SendStr = "NT1;";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			if(str.right(1).toInt() != 0)
				pSdr->OnAnf(false);
			else
				pSdr->OnAnf(true);

			emit changedANF((bool)str.right(1).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4350:
		if(str.size() == 2)
		{
            Tmp = pSdr->ui->slDrive->value();
			tmpstr1.setNum(Tmp);
			if(Tmp < 10)
				SendStr = "PC00" + tmpstr1 + ";";
			else if(Tmp < 100)
				SendStr = "PC0" + tmpstr1 + ";";
			else
				SendStr = "PC" + tmpstr1 + ";";

			SendStr = "PC" + tmpstr1 + ";";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 5)
		{
			Tmp = str.right(3).toInt();
			if(Tmp >= 0 && Tmp <= 100)
			{
				pSdr->OnDrive(Tmp);
				emit changedPWR(str.right(3).toInt());
			}
			else
				SentErrorCode(ERROR_PARAM);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5250:
		if(str.size() == 2)
		{
            if(pSdr->ui->pbComp->isChecked())
				SendStr = "PR0;";
			else
				SendStr = "PR1;";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			if(str.right(1).toInt() != 0)
				pSdr->OnComp(false);
			else
				pSdr->OnComp(true);

			emit changedCOMP((bool)str.right(1).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5350:
		if(str.size() == 2)
		{
            if(pSdr->ui->pbStart->isChecked())
				SendStr = "PS1;";
			else
				SendStr = "PS0;";

            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			if(str.right(1).toInt() != 0)
				pSdr->OnStart(true);
			else
				pSdr->OnStart(false);
			emit changedOnOff((bool)str.right(1).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4352:
		if(str.size() == 2)
		{

		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5452:
		if(str.size() == 2)
		{
			SendStr = "RT0;";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			emit changedOnOff((bool)str.right(1).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5852:
		if(str.size() == 2)
		{
			emit rxMode();
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x4853:
		SentErrorCode(ERROR_CMD);
		break;
	case 0x4C53:
		SentErrorCode(ERROR_CMD);
		break;
	case 0x4D53:
		if(str.size() == 3)
		{
			Tmp = pSdr->MainSmeter();
			Tmp += 73;
			Tmp /= 4.0;
			Tmp += 15;
			if(Tmp < 0)
				Tmp = 0;
			else if(Tmp > 30)
				Tmp = 30;

			tmpstr1.setNum(Tmp);
			if(Tmp < 10)
				SendStr = "SM0000" + tmpstr1 + ";";
			else
				SendStr = "SM000" + tmpstr1 + ";";

            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5153:
		if(str.size() == 3)
		{
            Tmp = pSdr->ui->slSql->value();
			Tmp *= -255.0/160.0;
			tmpstr1.setNum(Tmp);
			if(Tmp < 10)
				SendStr = "SQ000" + tmpstr1 + ";";
			else if(Tmp < 100)
				SendStr = "SQ00" + tmpstr1 + ";";
			else
				SendStr = "SQ0" + tmpstr1 + ";";

            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 6)
		{
			Tmp = str.right(3).toInt();
			Tmp *= -160.0/255.0;
			pSdr->OnSql(Tmp);
			emit changedSQLval(Tmp);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5854:
		if((str.size() == 2) || (str.size() == 3))
		{
			emit txMode();
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5055:
		if(str.size() == 2)
		{
			emit changedIncrDecrVFO_A(1);
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;
	case 0x5458:
		if(str.size() == 2)
		{
			SendStr = "XT0;";
            Data = SendStr.toLatin1();
			pCom->write(Data);
		}
		else if(str.size() == 3)
		{
			emit changedXIT((bool)str.right(1).toInt());
		}
		else
		{
			SentErrorCode(ERROR_PARAM);
			break;
		}
		break;

	default:
		SentErrorCode(ERROR_CMD);
		break;
	}
}

short CatManager::GetNameCmd(QString str)
{
	QChar *pData = str.data();
	short Cmd = pData[0].toLatin1() | pData[1].toLatin1()<<8;
	return Cmd;
}

void CatManager::SentErrorCode(ERROR_CAT code)
{
	QString SendStr;
	QByteArray Data;
	if(code == ERROR_CMD)
	{
		SendStr = "?;";
        Data = SendStr.toLatin1();
		pCom->write(Data);
	}
	else
	{
		SendStr = "O;";
        Data = SendStr.toLatin1();
		pCom->write(Data);
	}
}

void CatManager::OnKeyDsr(bool stat)
{
    if(pOpt->ui.chbCatEnable->isChecked())
    {
    	if(pOpt->ui.cbKeyPttLine->currentIndex() == 1)
    		emit PttChanged(stat);
    	if(pOpt->ui.cbKeyKeyLine->currentIndex() == 1)
    		emit KeyChanged(stat);
    }
	qDebug() << "Cat DSR changed..";
}

void CatManager::OnKeyCts(bool stat)
{
    if(pOpt->ui.chbCatEnable->isChecked())
    {
    	qDebug() << "Cat Emit CTS.." << stat;
    	if(pOpt->ui.cbCatPttLine->currentIndex() == 2)
    		emit PttChanged(stat);
    	if(pOpt->ui.cbCatKeyLine->currentIndex() == 2)
    		emit KeyChanged(stat);
    }
    qDebug() << "Cat CTS changed.." << stat;
}

