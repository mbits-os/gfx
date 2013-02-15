/*
 * JavaBitmap.hpp
 *
 *  Created on: 11-02-2013
 *      Author: Marcin
 */

#ifndef JAVABITMAP_HPP_
#define JAVABITMAP_HPP_

#include "android/graphics/Bitmap.hpp"
#include <string.h>

namespace gfx {

	namespace agfx = android::graphics;
	class Bitmap
	{
		agfx::Bitmap m_backing;
		unsigned short m_width, m_height;
	public:
		Bitmap(): m_width(0), m_height(0)
		{
		}
		Bitmap(const Bitmap& o)
		{
			m_width = o.m_width;
			m_height = o.m_height;
			if (o.m_backing._this())
				m_backing = jni::Env()->NewGlobalRef(o.m_backing._this());
		}
		~Bitmap()
		{
			if ((jobject)m_backing)
				jni::Env()->DeleteGlobalRef((jobject)m_backing);
		}
		Bitmap& operator=(const Bitmap& o)
		{
			jobject copy = NULL;

			JNIEnv* env = jni::Env();
			if (o.m_backing._this())
				copy = env->NewGlobalRef(o.m_backing._this());
			if ((jobject)m_backing)
				env->DeleteGlobalRef((jobject)m_backing);

			m_width = o.m_width;
			m_height = o.m_height;
			m_backing = copy;

			return *this;
		}
		agfx::Bitmap& backing() { return m_backing; }
		unsigned short width() const { return m_width; }
		unsigned short height() const { return m_height; }

		bool resize(unsigned short width, unsigned short height);
	};

	class BitmapLock
	{
		Bitmap& m_bmp;
		agfx::BitmapLockInfo m_bli;
	public:
		BitmapLock(Bitmap& bmp): m_bmp(bmp)
		{
			m_bli = m_bmp.backing().lock();
		}
		~BitmapLock()
		{
			if (locked())
				m_bmp.backing().unlock();
		}
		bool locked() const { return m_bli.m_pixels; }
		unsigned short width() const { return m_bli.m_info.width; }
		unsigned short height() const { return m_bli.m_info.height; }
		uint32_t& operator()(size_t x, size_t y)
		{
			uint8_t* pixel = m_bli.m_pixels + m_bli.m_info.stride * y + 4 * x;
			return *(uint32_t*)pixel; 
		} 
		void reset()
		{
			for (uint32_t i = 0; i < m_bli.m_info.height; ++i)
				memset(m_bli.m_pixels + i * m_bli.m_info.stride, 0, sizeof(uint32_t)*m_bli.m_info.width);
		}
	};

} //gfx


#endif /* JAVABITMAP_HPP_ */
