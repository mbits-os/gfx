/*
 * GifMovie.hpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#ifndef GIFMOVIE_HPP_
#define GIFMOVIE_HPP_

#include "Movie.hpp"
#include "gif/Gif.hpp"

namespace gfx { namespace gif {

	class GIFMovie: public Movie
	{
		GIF* m_gif;
		long long m_currentIndex;
		long long m_lastIndex;
	
		bool applyFrame(BitmapLock& bitmap, const GIFFrame& frame);
		bool applyFrame(BitmapLock& bitmap, const GIFFrame& frame, GIFPalette palette);
	public:
		GIFMovie(GIF* gif): m_gif(gif), m_currentIndex(0), m_lastIndex(-1) {}
		~GIFMovie() { delete m_gif; }

		bool internalGetInfo(Info& info);
		bool internalSetTime(int ms);
		bool internalBitmap(Bitmap& bitmap);
	};

}}

#endif /* GIFMOVIE_HPP_ */
