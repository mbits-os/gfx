/*
 * LzwDecoder.cpp
 *
 *  Created on: 10-02-2013
 *      Author: Marcin
 */

#include "gif/LzwDecoder.hpp"
#include "Stream.hpp"

#include <string.h>

namespace gfx { namespace gif {

	class CodeStream
	{
		friend class LzwDecoder;
		Stream* m_stream;
		unsigned char m_size;
		unsigned char m_ptr;
		unsigned char m_buffer[255];
		unsigned int m_runningBits;
		unsigned int m_currentShift;
		unsigned long m_dwShift;
		bool readBuffer();
		bool nextByte(unsigned char& next);
	public:
		CodeStream(Stream* stream, unsigned int runningBits): m_stream(stream), m_size(0), m_ptr(0), m_runningBits(runningBits), m_currentShift(0), m_dwShift(0) {}
		bool nextCode(int& code);
	};

	bool CodeStream::readBuffer()
	{
		if (m_stream->read(m_size) != 1) return false;
		if (m_size == 0) return false;
		if (m_stream->read(m_buffer, m_size) != m_size) return false;
		return true;
	}

	bool CodeStream::nextByte(unsigned char& next)
	{
		if (m_ptr == m_size)
		{
			if (!readBuffer())
				return false;
			m_ptr = 0;
		}
		next = m_buffer[m_ptr++];
		return true;
	}
	bool CodeStream::nextCode(int& code)
	{
	    static unsigned short CodeMasks[] = {
	        0x0000, 0x0001, 0x0003, 0x0007,
	        0x000f, 0x001f, 0x003f, 0x007f,
	        0x00ff, 0x01ff, 0x03ff, 0x07ff,
	        0x0fff
	    };

	    if (m_runningBits > lzw::LZW_MAX_BITS)
	    	return false;
	    
	    while (m_currentShift < m_runningBits)
	    {
	    	unsigned char byte;
	    	if (!nextByte(byte)) return false;
	    	m_dwShift |= ((unsigned long) byte) << m_currentShift;
	    	m_currentShift += 8;
	    }
	    
	    code = m_dwShift & CodeMasks[m_runningBits];

	    m_dwShift >>= m_runningBits;
	    m_currentShift -= m_runningBits;

	    return true;
	};

	int LzwDecoder::getPrefixChar(int code)
	{
	    int i = 0;

	    while (code > m_clearCode && i++ <= lzw::LZW_MAX)
	    {
	        if (code > lzw::LZW_MAX)
	            return lzw::LZW_EMPTY;
	        code = m_prefix[code];
	    }
	    return code;
	}

	bool LzwDecoder::decode(Stream* stream, CharacterStream& out)
	{
		CodeStream in(stream, m_bpp + 1);
		
		m_currentCode = lzw::LZW_EMPTY;
		m_lastCode = lzw::LZW_EMPTY;
		m_currentPrefix = lzw::LZW_EMPTY;
		while(true)
		{
			if (!in.nextCode(m_currentCode)) return false;
			widen(in);

			if (m_currentCode == m_eofCode) return true;

			if (m_currentCode == m_clearCode) { clear(in); continue; }

			if (!(m_currentCode < m_clearCode ? storeChar(out) : storeString(out)))
				return false;

			storeCode();
		}

		return false;
	}

	void LzwDecoder::clear(CodeStream& in)
	{
	    m_runningCode = m_eofCode + 1;
	    m_lastCode = lzw::LZW_EMPTY;
	    in.m_runningBits = m_bpp + 1;
	    m_invalidCode = 1 << in.m_runningBits;
		for (size_t i = 0; i < lzw::LZW_MAX; ++i)
			m_prefix[i] = lzw::LZW_EMPTY;
	}
	bool LzwDecoder::storeChar(CharacterStream& out)
	{
		return out.output((unsigned char)m_currentCode);
	}

	bool LzwDecoder::storeString(CharacterStream& out)
	{
		if (m_prefix[m_currentCode] == lzw::LZW_EMPTY)
		{
			if (m_currentCode != m_runningCode - 2) return false;
			m_currentPrefix = m_lastCode;
			m_suffix[m_runningCode - 2] = m_stack[m_stackPtr++] =
					getPrefixChar(m_lastCode);

		}
		else
			m_currentPrefix = m_currentCode;
		
		int j = 0;
		while (j++ <= lzw::LZW_MAX && m_currentPrefix > m_clearCode && m_currentPrefix <= lzw::LZW_MAX) {
			m_stack[m_stackPtr++] = m_suffix[m_currentPrefix];
			m_currentPrefix = m_prefix[m_currentPrefix];
		}
		
		if (j >= lzw::LZW_MAX || m_currentPrefix > lzw::LZW_MAX)
			return false;

		m_stack[m_stackPtr++] = m_currentPrefix;

	    while (m_stackPtr != 0)
			if (!out.output((unsigned char)m_stack[--m_stackPtr]))
				return false;

	    return true;
	}

	void LzwDecoder::storeCode()
	{
		if (m_lastCode != lzw::LZW_EMPTY)
		{
			m_prefix[m_runningCode - 2] = m_lastCode;
		
			if (m_currentCode == m_runningCode - 2)
				m_suffix[m_runningCode - 2] = getPrefixChar(m_lastCode);
			else
				m_suffix[m_runningCode - 2] = getPrefixChar(m_currentCode);
		}
		m_lastCode = m_currentCode;
	}

	void LzwDecoder::widen(CodeStream& in)
	{
	    if (m_runningCode < lzw::LZW_MAX + 2 &&
	    		++m_runningCode > m_invalidCode &&
	    		in.m_runningBits < lzw::LZW_MAX_BITS)
	    {
			in.m_runningBits++;
			m_invalidCode <<= 1;
	    }
	}
}}
