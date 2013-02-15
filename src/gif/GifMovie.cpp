/*
 * GifMovie.cpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#include "gif/GifMovie.hpp"

namespace gfx { namespace gif {
	bool GIFMovie::internalGetInfo(Info& info)
	{
		if (!m_gif)
			return false;

		info.m_width = m_gif->width();
		info.m_height = m_gif->height();
		info.m_opaque = false;
		info.m_duration = 0;

		GIF::const_iterator _cur = m_gif->begin();
		GIF::const_iterator _end = m_gif->end();
		while (_cur != _end)
		{
			const GIFFrame& frm = *_cur++;
			info.m_duration += frm.delay();
		}
		
		return true;
	}
	bool GIFMovie::internalSetTime(int ms)
	{
		if (!m_gif)
	        return false;

		long long dur = 0;
		GIF::const_iterator _cur = m_gif->begin();
		GIF::const_iterator _end = m_gif->end();
		long long index = 0;
		while (_cur != _end)
		{
			const GIFFrame& frm = *_cur++;
			dur += frm.delay();

			if (dur >= ms)
			{
				m_currentIndex = index;
				return m_lastIndex != m_currentIndex;
			}
			++index;
		}

		m_currentIndex = m_gif->size() - 1;
	    return m_lastIndex != m_currentIndex;
	}

	#define IS_OPAQUE(c) (((c) & 0xFF000000) == 0xFF000000)
	bool GIFMovie::applyFrame(BitmapLock& bitmap, const GIFFrame& frame, GIFPalette palette)
	{
		int transparent = frame.transparent();
		if (transparent < 0)
			transparent = m_gif->transparent();
		if (transparent >= 0 && (unsigned short)transparent < palette.size())
			palette.setTransparent((unsigned char)transparent);
		
		unsigned short offX = frame.offX();
		unsigned short offY = frame.offY();
		unsigned short width = frame.width();
		unsigned short height = frame.height();

		if (bitmap.width() - offX < width)
			width = bitmap.width() - offX;
		if (bitmap.height() - offY < height)
			height = bitmap.height() - offY;
		
		if (width == 0 || height == 0)
			return true;

		const GIFPixmap& src = frame.pixmap();

		for (unsigned short y = 0; y < height; y++)
			for (unsigned short x = 0; x < width; x++)
			{
				GIFPalette::COLORREF c = palette.getColor(src(x, y));
				if (IS_OPAQUE(c))
					bitmap(x + offX, y + offY) = c;
			}

		return true;
	}

	bool GIFMovie::applyFrame(BitmapLock& bitmap, const GIFFrame& frame)
	{
		const GIFPalette& global = m_gif->palette();
		const GIFPalette& local = frame.palette();
		
		const GIFPalette* effective = local.size() == 0 ? &global : &local;

		return applyFrame(bitmap, frame, *effective);
	}
	bool GIFMovie::internalBitmap(Bitmap& bitmap)
	{
		if (!m_gif)
			return false;
		
		if (m_gif->empty())
			return false;

		unsigned short width = m_gif->width();
		unsigned short height = m_gif->height();
		if (width == 0 || height == 0)
			return false;

		if (m_lastIndex > -1 && m_currentIndex == m_lastIndex)
			return true;

		long long smallest = m_lastIndex + 1;
		if (m_lastIndex < 0 || bitmap.width() != width || bitmap.height() != height)
		{
			smallest = 0;
			if (!bitmap.resize(width, height))
				return false;
		}
		
		if (smallest > m_currentIndex)
			smallest = 0;
		
		while (smallest > 0 && m_gif->at(smallest).dispose() == dispose::SKIP)
			--smallest;

		BitmapLock bmp(bitmap);
		if (!bmp.locked()) return false;

		const GIFFrame& current = m_gif->at(m_currentIndex);
		if (m_currentIndex == 0 || current.dispose() == dispose::RESTART)
		{
			bmp.reset();
			if (!applyFrame(bmp, current))
				return false;
			m_lastIndex = m_currentIndex;
			return true;
		}

		long long start = m_currentIndex - 1;
		bool restart = false;
		while (start > smallest)
		{
			if (m_gif->at(start).dispose() == dispose::RESTART)
			{
				restart = true;
				break;
			}
			--start;
		}

		if (restart)
			bmp.reset();

		for (; start < m_currentIndex; ++start)
		{
			const GIFFrame& frame = m_gif->at(start);
			if (frame.dispose() == dispose::SKIP) continue;
			if (!applyFrame(bmp, frame)) return false;
		}

		if (!applyFrame(bmp, current))
			return false;

		m_lastIndex = m_currentIndex;
		return true;
	}
}}

