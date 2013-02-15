/*
 * Bitmap.hpp
 *
 *  Created on: 10-02-2013
 *      Author: Marcin
 */

#ifndef BITMAP_HPP_
#define BITMAP_HPP_

#include <string.h>

namespace vgl {

struct Pixmap
{
	typedef unsigned long COLORREF;
private:
	typedef COLORREF* Pixels;
	Pixels m_pixels;
	size_t m_size;
	size_t m_scanline;
public:
	Pixmap(): m_pixels(0), m_size(0), m_scanline(0) {}
	Pixmap(const Pixmap& oth): m_pixels(0), m_size(0), m_scanline(0)
	{
		if (!reserve(oth.m_scanline, oth.m_size / oth.m_scanline)) return;
		memcpy(m_pixels, oth.m_pixels, m_size);
	}
	Pixmap& operator =(const Pixmap& oth)
	{
		if (this == &oth) return *this;
		if (!reserve(oth.width(), oth.height())) return *this;
		memcpy(m_pixels, oth.m_pixels, sizeof(COLORREF) * m_size);
		return *this;
	}
	~Pixmap()
	{
		delete [] m_pixels;
	}
	size_t size() const { return m_size; }
	size_t width() const { return m_scanline; }
	size_t height() const { return m_size / m_scanline; }
	Pixels data() { return m_pixels; }
	const Pixels data() const { return m_pixels; }

	bool reserve(size_t width, size_t height)
	{
		size_t size = width * height;
		if (size == 0)
		{
			delete [] m_pixels;
			m_pixels = 0;
			m_size = 0;
			return true;
		}
		COLORREF* colors = new COLORREF[size];
		if (!colors) return false;
		delete [] m_pixels;
		m_pixels = colors;
		m_size = size;
		m_scanline = width;
		memset(m_pixels, 0, sizeof(COLORREF) * size);
		return true;
	}
	
	COLORREF& operator()(size_t x, size_t y)
	{
		size_t pos = y * m_scanline + x;
		if (pos >= m_size)
		{
			static COLORREF b;
			b = 0;
			return b;
		}
		return m_pixels[pos];
	}
	
	COLORREF operator()(size_t x, size_t y) const
	{
		size_t pos = y * m_scanline + x;
		if (pos >= m_size)
			return 0;
		return m_pixels[pos];
	}

	void reset()
	{
		memset(m_pixels, 0, sizeof(COLORREF) * m_size);
	}
};

class Bitmap
{
	class data
	{
		unsigned long count;
		Pixmap m_pixmap;
		unsigned short m_width;
		unsigned short m_height;
	public:
		data(unsigned short width, unsigned short height)
		: count(1)
		, m_width(width)
		, m_height(height)
		{
		}
		data(const data& oth)
		: count(1)
		, m_pixmap(oth.m_pixmap)
		, m_width(oth.m_width)
		, m_height(oth.m_height)
		{
		}
		void addref() {
			++count;
		}
		void release()
		{
			if (!--count) delete this;
		}
		bool single() const { return count < 2; }

		Pixmap& pixmap() { return m_pixmap; }
		const Pixmap& pixmap() const { return m_pixmap; }
		unsigned short width() const { return m_width; }
		unsigned short height() const { return m_height; }
	};
	data* ptr;

	void make_single()
	{
		if (ptr && !ptr->single())
		{
			data* nptr = new data(*ptr);
			ptr->release();
			ptr = nptr;
		}
	}

public:
	Bitmap()
	: ptr(0)
	{
	}
	Bitmap(const Bitmap& o): ptr(o.ptr)
	{
		if (ptr) ptr->addref();
	}
	~Bitmap()
	{
		if (ptr) ptr->release();
		ptr = 0;
	}
	Bitmap& operator=(const Bitmap& o)
	{
		if (o.ptr) o.ptr->addref();
		if (ptr) ptr->release();
		ptr = o.ptr;
		return *this;
	}
	unsigned short width() const { if (!ptr) return 0; return ptr->width(); }
	unsigned short height() const { if (!ptr) return 0; return ptr->height(); }
	size_t dataSize() const { return pixmap().size(); }
	Pixmap::COLORREF* dataPtr() { return pixmap().data(); }
	const Pixmap::COLORREF* dataPtr() const { return pixmap().data(); }
	void reset()
	{
		pixmap().reset();
	}
	bool resize(unsigned short width, unsigned short height)
	{
		make_single();
		if (!ptr) ptr = new data(width, height);
		if (!ptr) return false;
		return ptr->pixmap().reserve(width, height);
	}
	Pixmap::COLORREF& operator()(size_t x, size_t y) { return pixmap()(x, y); }
private:
	Pixmap& pixmap()
	{
		make_single();
		if (!ptr)
		{
			static Pixmap map;
			return map;
		}
		return ptr->pixmap();
	}
	const Pixmap& pixmap() const
	{
		if (!ptr)
		{
			static Pixmap map;
			return map;
		}
		return ptr->pixmap();
	}
};

class BitmapLock
{
	Bitmap& m_bmp;
public:
	BitmapLock(Bitmap& bmp): m_bmp(bmp) {}
	bool locked() const { return m_bmp.dataPtr() != NULL; }
	unsigned short width() const { return m_bmp.width(); }
	unsigned short height() const { return m_bmp.height(); }
	unsigned long& operator()(size_t x, size_t y) { return m_bmp(x, y); }
	void reset() { m_bmp.reset(); }
};

}


#endif /* BITMAP_HPP_ */
