/*
 * GifDecoder.hpp
 *
 *  Created on: 10-02-2013
 *      Author: Marcin
 */

#include "gif/LzwDecoder.hpp"

namespace gfx {
	struct Stream;
}

namespace gfx { namespace gif {

	struct GIFPalette;
	struct GIFFrame;
	struct GraphicControl;

	class GIFDecoder
	{
		Stream* m_stream;
		LzwDecoder m_decoder;
		//void dump(unsigned char* data, size_t size);
		bool readHeader(GIF& gif);
		bool readPalette(int colorMapSize, GIFPalette& out);
		bool readBlocks(GIF& gif);
		bool readFrame(const GraphicControl&, GIFFrame&);
		bool readFrameHeader(const GraphicControl&, GIFFrame&);
		bool decode(GIFFrame& frame);
		bool readGraphicControl(GraphicControl&);
		bool skipDataBlocks();
	public:
		GIFDecoder(Stream* stream): m_stream(stream) {}
		bool decode(GIF& gif);
	};

}}
