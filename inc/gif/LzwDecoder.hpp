/*
 * LzwDecoder.hpp
 *
 *  Created on: 10-02-2013
 *      Author: Marcin
 */

#ifndef LZWDECODER_HPP_
#define LZWDECODER_HPP_

#include <stddef.h>

namespace gfx {
	struct Stream;
}

namespace gfx { namespace gif {

	namespace lzw
	{
		enum
		{
			LZW_MAX_BITS = 12,
			LZW_MAX = (1 << LZW_MAX_BITS) - 1,
			LZW_EMPTY
		};
	}

	class CodeStream;

	struct CharacterStream
	{
		virtual ~CharacterStream() {}
		virtual bool output(unsigned char c) = 0;
	};

	class LzwDecoder
	{
		int m_bpp;
		int m_clearCode;
		int m_eofCode;
		int m_invalidCode;
		int m_runningCode;
		int m_currentCode;
		int m_lastCode;
		int m_currentPrefix;
		int m_prefix[lzw::LZW_MAX + 1];
		int m_suffix[lzw::LZW_MAX + 1];
		int m_stack[lzw::LZW_MAX];
		int m_stackPtr;
		
		void reset(int BitsPerPixel)
		{
			m_bpp = BitsPerPixel;
			m_clearCode = 1 << m_bpp;
			m_eofCode = m_clearCode + 1;
			m_runningCode = m_eofCode + 1;
			m_invalidCode = 1 << (m_bpp + 1);
			m_currentCode = lzw::LZW_EMPTY;
			m_lastCode = lzw::LZW_EMPTY;
			m_currentPrefix = lzw::LZW_EMPTY;
			m_stackPtr = 0;
			for (size_t i = 0; i < lzw::LZW_MAX; ++i)
				m_prefix[i] = lzw::LZW_EMPTY;
		}

		bool decode(Stream* stream, CharacterStream& output);
		int getPrefixChar(int code);

		void clear(CodeStream& in);
		bool storeChar(CharacterStream&);
		bool storeString(CharacterStream&);
		void storeCode();
		void widen(CodeStream&);
	public:
		LzwDecoder()
		{
			m_bpp = 0;
			m_clearCode = 1 << m_bpp;
			m_eofCode = m_clearCode + 1;
			m_runningCode = m_eofCode + 1;
			m_invalidCode = 1 << (m_bpp + 1);
			m_currentCode = lzw::LZW_EMPTY;
			m_lastCode = lzw::LZW_EMPTY;
			m_currentPrefix = lzw::LZW_EMPTY;
			m_stackPtr = 0;
		}
		
		bool decode(Stream* stream, int BitsPerPixel, CharacterStream& output)
		{
			reset(BitsPerPixel);
			return decode(stream, output);
		}
	};

}}


#endif /* LZWDECODER_HPP_ */
