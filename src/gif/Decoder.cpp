/*
 * Decoder.cpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#include <stdio.h>
#include "MovieDecoder.hpp"
#include "Stream.hpp"
#include "gif/Gif.hpp"
#include "gif/GifMovie.hpp"

#define SIG_SIZE 6
#define SIG_GIF87a "GIF87a"
#define SIG_GIF89a "GIF89a"

namespace gfx { namespace gif {

	class GifDecoder: public MovieDecoder
	{
		bool valid(Stream* stream)
		{
			char sig[SIG_SIZE+1] = "";
			int read = stream->read(sig, SIG_SIZE);
			if (read != SIG_SIZE)
				return false;
			if (!strncmp(sig, SIG_GIF87a, SIG_SIZE) || !strncmp(sig, SIG_GIF89a, SIG_SIZE))
				return true;
			if (!strncmp(sig, "GIF", 3))
			{
				return true;
			}
			return false;
		}
		Movie* decode(Stream* stream)
		{
			GIF* gif = GIF::decode(stream);
			if (gif == NULL)
				return NULL;

			Movie* movie = new (std::nothrow) GIFMovie(gif);
			if (!movie)
				delete gif;

			return movie;
		}
		const char* name() const { return "GIF"; }
	};

	REGISTER_MOVIEDECODER(GifDecoder);
}}
