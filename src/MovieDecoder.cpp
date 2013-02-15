/*
 * MovieDecoder.cpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#include "MovieDecoder.hpp"
#include "Movie.hpp"
#include "Stream.hpp"

namespace gfx
{
	MovieDecoders::~MovieDecoders()
	{
		Decoders::iterator
			_cur = m_decoders.begin(), _end = m_decoders.end();

		for (; _cur != _end; ++_cur)
			delete *_cur;

		m_decoders.clear();
	}

	MovieDecoders& MovieDecoders::get()
	{
		static MovieDecoders decoders;
		return decoders;
	}

	Movie* MovieDecoders::decode(Stream* stream)
	{
		Decoders::iterator
			_cur = get().m_decoders.begin(), _end = get().m_decoders.end();

		for (; _cur != _end; ++_cur)
		{
			MovieDecoder* decoder = *_cur;
			if (decoder->valid(stream))
			{
				stream->reset();
				return decoder->decode(stream);
			}
		}
		
		return NULL;
	}

	bool MovieDecoders::registerDecoder(MovieDecoder* decoder)
	{
		if (!decoder)
			return false;

		int size = get().m_decoders.size();
		get().m_decoders.push_back(decoder);
		return size < get().m_decoders.size();
	}

	Movie* Movie::decodeStream(Stream* stream)
	{
		return MovieDecoders::decode(stream);
	}
}
