/*
 * CharacterStream.hpp
 *
 *  Created on: 11-02-2013
 *      Author: Marcin
 */

#ifndef CHARACTERSTREAM_HPP_
#define CHARACTERSTREAM_HPP_

namespace gfx { namespace gif {
	namespace interlaced {
		static struct {
			int start;
			int jump;
		} stages[] = {
			{ 0, 8 },
			{ 4, 8 },
			{ 2, 4 },
			{ 1, 2 }
		};

		class CharacterStream: public gfx::gif::CharacterStream
		{
			unsigned char* m_buffer;
			size_t m_linePtr;
			size_t m_line;
			const size_t m_width;
			const size_t m_height;
			size_t m_stage;
		public:
			CharacterStream(unsigned char* buffer, size_t width, size_t height)
			: m_buffer(buffer)
			, m_linePtr(0)
			, m_line(0)
			, m_width(width)
			, m_height(height)
			, m_stage(0)
			{
			}
			bool output(unsigned char c)
			{
				if (m_linePtr == m_width)
				{
					m_line += stages[m_stage].jump;
					if (m_line >= m_height)
					{
						m_stage ++;
						if (m_stage >= sizeof(stages)/sizeof(stages[0]))
							return false;
						m_line = stages[m_stage].start;
					}
					m_linePtr = 0;
				}
				m_buffer[m_line * m_width + m_linePtr++] = c;
				return true;
			}
		};
	}

	namespace direct {

		class CharacterStream: public gfx::gif::CharacterStream
		{
			unsigned char* m_buffer;
			size_t m_size;
			size_t m_ptr;
		public:
			CharacterStream(unsigned char* buffer, size_t size)
			: m_buffer(buffer)
			, m_size(size)
			, m_ptr(0)
			{
			}

			bool output(unsigned char c)
			{
				if (m_size <= m_ptr) return false;
				m_buffer[m_ptr++] = c;
				return true;
			};
		};
	
	}

}}

#endif /* CHARACTERSTREAM_HPP_ */
