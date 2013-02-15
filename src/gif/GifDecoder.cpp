/*
 * GifDecoder.cpp
 *
 *  Created on: 10-02-2013
 *      Author: Marcin
 */

#include <string.h>
#include <ctype.h>

#include "gif/Gif.hpp"
#include "gif/GifDecoder.hpp"

#include "Stream.hpp"

#include "CharacterStream.hpp"

namespace gfx { namespace gif {
	template <typename T>
	class auto_memory
	{
		T* m_data;
	public:
		auto_memory(): m_data(NULL) {}
		auto_memory(size_t count): m_data(new T[count]) {}
		~auto_memory() { reset(); }
		
		void reset(T* data = NULL)
		{
			if (data == m_data) return;
			delete [] m_data;
			m_data = data;
		}
		
		T* get() { return m_data; }
	};

	enum {
		IMAGE_DECRIPTION = 0x2C,
		EXTENSION_INTRODUCER = 0x21,
		GIF_TRAILER = 0x3B,
		GRAPHIC_CONTROL = 0xF9
	};

	struct GraphicControl
	{
		bool m_hasGCE;
		bool m_hasTransparent;
		unsigned int m_delay; 
		dispose::DISPOSE m_disposal;
		int m_transparentIndex;
		GraphicControl()
		: m_hasGCE(false)
		, m_hasTransparent(false)
		, m_delay(0)
		, m_disposal(dispose::NOT_SPECIFIED)
		, m_transparentIndex(0)
		{
		}
	};

	#if 0
	void GIFDecoder::dump(unsigned char* data, size_t size)
	{
		return;
		if (printEnable != m_stream)
			return;

		unsigned char _alph[] = "0123456789ABCDEF";
		unsigned char _template[] = "00 00 00 00 00 00 00 00|00 00 00 00 00 00 00 00|................";
		unsigned char _line[sizeof(_template)];
		ALOGI("%u bytes:", size);

		memcpy(_line, _template, sizeof(_template));
		for (size_t i = 0; i < size; ++i)
		{
			size_t pos = i % 16;
			_line[pos * 3] = _alph[(data[i] & 0xF0) >> 4];
			_line[pos * 3 + 1] = _alph[data[i] & 0xF];
			if (isprint(data[i]))
				_line[16*3 + pos] = data[i];
			if (pos == 15)
			{
				ALOGI("%s", _line);
				memcpy(_line, _template, sizeof(_template));
			}
		}
		if (size % 16 != 0)
		{
			for (size_t i = size % 16; i < 16; ++i)
			{
				_line[i*3] = ' ';
				_line[i*3+1] = ' ';
				_line[16*3 + i] = ' ';
			}
			ALOGI("%s", _line);
		}
	}
	#endif

	bool GIFDecoder::readPalette(int colorMapSize, GIFPalette& out)
	{
		size_t size = colorMapSize * 3;
		auto_memory<char> palette(size);
		if (!palette.get()) return false;
		if (m_stream->read(palette.get(), size) != size)
			return false;

		if (!out.reserve(colorMapSize))
			return false;
		
		for (int i = 0; i < colorMapSize; ++i)
		{
			char* color = palette.get() + i * 3;
			out.setColor(i, color[0], color[1], color[2]);
		}

		return true;
	}

	bool GIFDecoder::readHeader(GIF& gif)
	{
		unsigned char sig[7] = "";
		unsigned char header[7];
		unsigned short * sizes = (unsigned short *)header;

		if (m_stream->read(sig, 6) != 6) return false;
		if (m_stream->read(header) != sizeof(header)) return false;
		
		bool hasGlobalColorMap = header[4] & 0x80;
		int colorResolution    =(header[4] & 0x70) >> 4 + 1;
		int colorMapSize =1 << ((header[4] & 0x07) + 1);
		
		gif.setSize(sizes[0], sizes[1]);
		gif.setTransparent(header[5]);
		
		if (hasGlobalColorMap)
			if (!readPalette(colorMapSize, gif.palette()))
				return false;

		return true;
	}

	bool GIFDecoder::readBlocks(GIF& gif)
	{
		GraphicControl control;
		bool _continue = true;

		while(_continue)
		{
			unsigned char blockType = 0;
			if (m_stream->read(blockType) != 1) return NULL;
			switch (blockType)
			{
			case IMAGE_DECRIPTION:
			{
				GIFFrame frame;
				if (!readFrame(control, frame)) return NULL;
				gif.push_back(frame);
				break;
			}
			case EXTENSION_INTRODUCER:
				if (m_stream->read(blockType) != 1) return NULL;
				switch(blockType)
				{
				case GRAPHIC_CONTROL:
					if (!readGraphicControl(control)) return NULL;
					break;
				default:
					if (!skipDataBlocks()) return NULL;
					break;
				}
				break;
			case GIF_TRAILER:
				_continue = false;
				break;
			default:
				return false;
			}
		}

		return true;
	}

	bool GIFDecoder::readFrame(const GraphicControl& control, GIFFrame& frame)
	{
		bool ret = true;
		bool retRFH, retD, retSDB;
		if (ret) ret = readFrameHeader(control, frame); retRFH = ret;
		if (ret) ret = decode(frame); retD = ret;
		if (ret) ret = skipDataBlocks(); retSDB = ret;

		return ret;
	}

	bool GIFDecoder::readFrameHeader(const GraphicControl& control, GIFFrame& frame)
	{
		unsigned char header[9] = "";
		unsigned short * pos = (unsigned short *)header;
		auto_memory<char> palette;

		if (m_stream->read(header) != sizeof(header)) return false;

		bool hasLocalColorMap       = header[8] & 0x80;
		bool isInterlaced           = header[8] & 0x40;
		unsigned short colorMapSize =1 << ((header[8] & 0x07) + 1);

		frame = GIFFrame(pos[0], pos[1], pos[2], pos[3], control.m_delay, control.m_disposal, control.m_hasTransparent ? control.m_transparentIndex : -1, isInterlaced);
		if (!frame.pixmap().reserve(pos[2], pos[3]))
			return false;

		if (hasLocalColorMap)
			if (!readPalette(colorMapSize, frame.palette()))
				return false;

		return true;
	}

	bool GIFDecoder::decode(GIFFrame& frame)
	{
		unsigned char codeSize = 0;
		if (m_stream->read(codeSize) != 1) return false;

		if (frame.interlaced())
		{
			interlaced::CharacterStream stream(frame.pixmap().data(), frame.pixmap().width(), frame.pixmap().height());
			return m_decoder.decode(m_stream, codeSize, stream);
		}
		else
		{
			direct::CharacterStream stream(frame.pixmap().data(), frame.pixmap().size());
			return m_decoder.decode(m_stream, codeSize, stream);
		}
	}

	#if 0
	static inline const char* disposal2Name(dispose::DISPOSE disp)
	{
		switch(disp)
		{
		case dispose::DONT_DISPOSE: return "continue";
		case dispose::RESTART: return "restart";
		case dispose::SKIP: return "skip";
		default: return "not specified";
		}
	}
	#endif

	bool GIFDecoder::readGraphicControl(GraphicControl& ctrl)
	{
		unsigned char size = 0, size2 = 0;
		unsigned char buffer[256];
		if (m_stream->read(size) != 1) return false;
		if (size < 4) return false;
		if (m_stream->read(buffer, size) != size) return false;
		if (m_stream->read(size2) != 1) return false;
		if (size2 != 0) return false;

		ctrl.m_hasGCE = true;
		ctrl.m_delay            = (((unsigned short)buffer[2] << 8) | buffer[1]) * 10; 
		int disposal            =(buffer[0] & 0x1C) >> 2;
		ctrl.m_hasTransparent   = buffer[0] & 0x01;
		ctrl.m_transparentIndex = buffer[3];
		ctrl.m_disposal         = disposal >= dispose::DISPOSE_MAX ? dispose::NOT_SPECIFIED : (dispose::DISPOSE)disposal;
		
		if (ctrl.m_delay < 20)
			ctrl.m_delay = 100; //default for 0ms and 10ms

		return true;
	}

	bool GIFDecoder::skipDataBlocks()
	{
		unsigned char size = 0;
		while (true)
		{
			if (m_stream->read(size) != 1) return false;
			if (size == 0) break;
			if (m_stream->skip(size) != size) return false;
		}
		return true;
	}

	bool GIFDecoder::decode(GIF& gif)
	{
		if (!readHeader(gif)) return false;
		if (!readBlocks(gif)) return false;

		return true;
	}
}}
