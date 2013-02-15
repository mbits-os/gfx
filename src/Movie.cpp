/*
 * Movie.cpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#include "Movie.hpp"
#include <string.h>

namespace gfx {
	Movie::Movie()
	: m_currentTime(-1)
	, m_needBitmap(true)
	{
		m_info.m_duration = -1;
		m_info.m_width = -1;
		m_info.m_height = -1;
		m_info.m_opaque = false;
	}
	bool Movie::setTime(long long ms)
	{
		long long dur = duration();
		if (ms > dur) ms = dur;

		bool changed = false;
		if (ms != m_currentTime)
		{
			changed = internalSetTime(ms);
			m_needBitmap |= changed;
			m_currentTime = ms;
		}
		return changed;
	}
	void Movie::getInfo()
	{
		if (m_info.m_duration == -1)
			if (!internalGetInfo(m_info))
				memset(&m_info, 0, sizeof(m_info));
	}

	Bitmap& Movie::bitmap()
	{
		if (m_currentTime == -1)
			setTime(0);

		if (m_needBitmap)
		{
			m_needBitmap = false;
			if (!internalBitmap(m_bitmap))
			{
				BitmapLock bmp(m_bitmap);
				if (bmp.locked())
					bmp.reset();
			}
		}

		return m_bitmap;
	}
}
