/*
 * MovieDecoder.hpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#ifndef MOVIEDECODER_HPP_
#define MOVIEDECODER_HPP_

#include "Movie.hpp"
#include <memory>
#include <vector>

namespace gfx {

	struct MovieDecoder
	{
		virtual ~MovieDecoder() {}
		virtual bool valid(Stream* stream) = 0;
		virtual Movie* decode(Stream* stream) = 0;
		virtual const char* name() const = 0;
	};

	class MovieDecoders
	{
		typedef std::vector<MovieDecoder*> Decoders;
		Decoders m_decoders;
		~MovieDecoders();
		MovieDecoders() {}

		static MovieDecoders& get();
	public:
		static Movie* decode(Stream* stream);
		static bool registerDecoder(MovieDecoder* decoder);
	};

	template <typename T>
	struct RegMovieDecoder
	{
		RegMovieDecoder()
		{
			T* decoder = new (std::nothrow) T();
			if (!MovieDecoders::registerDecoder(decoder))
				delete decoder;
		}
	};

}

#define REGISTER_MOVIEDECODER(T) static gfx::RegMovieDecoder<T> _reg_movie_decoder_##T;

#endif /* MOVIEDECODER_HPP_ */
