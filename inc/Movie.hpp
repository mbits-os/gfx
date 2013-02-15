#ifndef __MOVIE_HPP__
#define __MOVIE_HPP__

#include <stddef.h>

#define USE_JAVA_BACKED_BITMAP 1

#if USE_JAVA_BACKED_BITMAP
#include "JavaBitmap.hpp"
#else
#include "Bitmap.hpp"
#endif

namespace gfx {

	struct Stream;

	struct Movie
	{
		Movie();
		virtual ~Movie() {}

		int width() { getInfo(); return m_info.m_width; }
		int height() { getInfo(); return m_info.m_height; }
		bool isOpaque() { getInfo(); return m_info.m_opaque; }
		long long duration() { getInfo(); return m_info.m_duration; }
		bool setTime(long long ms);
		bool needsBitmap() const { return m_needBitmap; }
		Bitmap& bitmap();
		static Movie* decodeStream(Stream*);
	protected:
		struct Info
		{
			long long m_duration;
			int m_width;
			int m_height;
			bool m_opaque;
		};
	
		virtual bool internalGetInfo(Info& info) = 0;
		virtual bool internalSetTime(int ms) = 0;
		virtual bool internalBitmap(Bitmap& bm) = 0;
	private:
		Info m_info;
		long long m_currentTime;
		bool m_needBitmap;
		Bitmap m_bitmap;
		void getInfo();
	};
}
#endif
