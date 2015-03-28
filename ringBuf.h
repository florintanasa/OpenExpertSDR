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

#ifndef RINGBUF_H_
#define RINGBUF_H_

#include <QtWidgets>

class ringBuf
{
	private:
		int Size;
		unsigned int Mask;
		float *pBuffer;
		QSemaphore usedSpace;
		QSemaphore freeSpace;
		QMutex	updMut;
		int wPtr;
		int rPtr;

	public:
		ringBuf(int size);
		~ringBuf();
		void setSize(int size);
		void write(float *pBuf, int size);
		void read(float *pBuf, int size);
		void clear();
		int  writeCount();
		int  readCount();
};

#endif /* RINGBUF_H_ */
