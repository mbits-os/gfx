/*
 * MemoryStream.cpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#include "MemoryStream.hpp"
#include <string.h>
#include <new>

namespace gfx
{

	MemoryStream::MemoryStream(const char* data, size_t len)
	{
		m_start = m_ptr = data;
		m_end = m_start + len;
	}

	size_t MemoryStream::read(void* buffer, size_t len)
	{
		const char* next = m_ptr + len;
		if (next > m_end)
			len = m_end - m_ptr;
		if (len == 0) return 0;

		memcpy(buffer, m_ptr, len);

		m_end += len;
		return len;
	}
	size_t MemoryStream::skip(size_t len)
	{
		const char* next = m_ptr + len;
		if (next > m_end)
			len = m_end - m_ptr;
		if (len == 0) return 0;
		m_end += len;
		return len;
	}
	void MemoryStream::reset()
	{
		m_ptr = m_start;
	}
	
	MemoryStream* MemoryStream::create(const void* data, size_t len)
	{
		return new (std::nothrow) MemoryStream((const char*)data, len);
	}
};
