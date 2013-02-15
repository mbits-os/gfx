/*
 * Gif.cpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#include <memory>

#include "gif/Gif.hpp"
#include "gif/GifDecoder.hpp"

namespace gfx { namespace gif {

	GIF* GIF::decode(Stream* stream)
	{
		std::auto_ptr<GIF> out(new GIF());
		if (!out.get())
			return NULL;

		GIFDecoder decoder(stream);
		if (!decoder.decode(*out.get()))
			return NULL;

		return out.release();
	}

}}
