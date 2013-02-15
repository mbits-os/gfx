/*
 * MemoryStream.hpp
 *
 *  Created on: 12-02-2013
 *      Author: Marcin
 */

#ifndef MEMORYSTREAM_HPP_
#define MEMORYSTREAM_HPP_

#include "Stream.hpp"

namespace gfx {

	class MemoryStream: public Stream
	{
		const char* m_start;
		const char* m_end;
		const char* m_ptr;
	public:
		MemoryStream(const char* start, size_t len);
		~MemoryStream() {}

		size_t read(void*, size_t);
		size_t skip(size_t length);
		void reset();

		static MemoryStream* create(const void* data, size_t len);
	};

} //gfx

#endif /* MEMORYTREAM_HPP_ */
