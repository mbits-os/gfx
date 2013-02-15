/*
 * Gif.hpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#ifndef GIF_HPP_
#define GIF_HPP_

#include <vector>

namespace gfx {
	class Stream;
}

namespace gfx { namespace gif {

	namespace dispose {
		enum DISPOSE
		{
			NOT_SPECIFIED,
			DONT_DISPOSE,
			RESTART,
			SKIP,
			DISPOSE_MAX
		};
	}

	struct GIFPalette
	{
		typedef uint32_t COLORREF;
	private:
		typedef COLORREF* Colors;
		Colors m_colors;
		unsigned short m_size;
	public:
		GIFPalette(): m_colors(NULL), m_size(0) {}
		GIFPalette(const GIFPalette& oth): m_colors(NULL), m_size(0)
		{
			if (!reserve(oth.m_size)) return;
			memcpy(m_colors, oth.m_colors, sizeof(COLORREF) * m_size);
		}
		GIFPalette& operator =(const GIFPalette& oth)
		{
			if (this == &oth) return *this;
			if (!reserve(oth.m_size)) return *this;
			memcpy(m_colors, oth.m_colors, sizeof(COLORREF) * m_size);
			return *this;
		}
		~GIFPalette()
		{
			delete [] m_colors;
		}
		bool reserve(unsigned short size)
		{
			if (size > 0x100) size = 0x100;
			if (size == 0)
			{
				delete [] m_colors;
				m_colors = NULL;
				m_size = 0;
				return true;
			}
			COLORREF* colors = new COLORREF[size];
			if (!colors) return false;
			delete [] m_colors;
			m_colors = colors;
			m_size = size;
			memset(m_colors, 0, sizeof(COLORREF) * size);
			return true;
		}
		void setTransparent(unsigned char index)
		{
			if (index >= m_size) return;
			m_colors[index] = 0x00000000;
		}
		void setColor(unsigned char index, unsigned char R, unsigned char G, unsigned char B)
		{
			if (index >= m_size) return;
			m_colors[index] = ((COLORREF)B << 16) | ((COLORREF)G << 8) | ((COLORREF)R) | 0xFF000000;
		}
		COLORREF getColor(unsigned char index)
		{
			if (index >= m_size) return 0x0;
			return m_colors[index];
		}
		unsigned short size() const { return m_size; }
	};

	struct GIFPixmap
	{
		typedef unsigned char BYTE;
	private:
		typedef BYTE* Pixels;
		Pixels m_pixels;
		size_t m_size;
		size_t m_height;
		size_t m_width;
	public:
		GIFPixmap(): m_pixels(NULL), m_size(0), m_width(0), m_height(0) {}
		GIFPixmap(const GIFPixmap& oth): m_pixels(NULL), m_size(0), m_width(0), m_height(0)
		{
			if (!reserve(oth.m_width, oth.m_height)) return;
			memcpy(m_pixels, oth.m_pixels, m_size);
		}
		GIFPixmap& operator =(const GIFPixmap& oth)
		{
			if (this == &oth) return *this;
			if (!reserve(oth.width(), oth.height())) return *this;
			memcpy(m_pixels, oth.m_pixels, sizeof(BYTE) * m_size);
			return *this;
		}
		~GIFPixmap()
		{
			delete [] m_pixels;
		}
		size_t size() const { return m_size; }
		size_t width() const { return m_width; }
		size_t height() const { return m_height; }
		Pixels data() { return m_pixels; }

		bool reserve(size_t width, size_t height)
		{
			size_t size = width * height;
			if (size == 0)
			{
				delete [] m_pixels;
				m_pixels = NULL;
				m_size = 0;
				return true;
			}
			BYTE* colors = new BYTE[size];
			if (!colors) return false;
			delete [] m_pixels;
			m_pixels = colors;
			m_size = size;
			m_width = width;
			m_height = height;
			memset(m_pixels, 0, sizeof(BYTE) * size);
			return true;
		}
		
		BYTE& operator()(size_t x, size_t y)
		{
			size_t pos = y * m_width + x;
			if (pos >= m_size)
			{
				static BYTE b;
				b = 0;
				return b;
			}
			return m_pixels[pos];
		}
		
		BYTE operator()(size_t x, size_t y) const
		{
			size_t pos = y * m_width + x;
			if (pos >= m_size)
				return 0;
			return m_pixels[pos];
		}
	};

	class GIFFrame
	{
		class data
		{
			unsigned long count;
			GIFPalette m_palette;
			GIFPixmap m_pixmap;
			unsigned short m_offX;
			unsigned short m_offY;
			unsigned short m_width;
			unsigned short m_height;
			unsigned int m_delay;
			dispose::DISPOSE m_dispose;
			int m_transparent;
			bool m_interlaced;
		public:
			data(unsigned short offX, unsigned short offY, unsigned short width, unsigned short height, unsigned int delay, dispose::DISPOSE dispose, int transparent, bool interlaced)
			: count(1)
			, m_offX(offX)
			, m_offY(offY)
			, m_width(width)
			, m_height(height)
			, m_delay(delay)
			, m_dispose(dispose)
			, m_transparent(transparent)
			, m_interlaced(interlaced)
			{
			}
			data(const data& oth)
			: count(1)
			, m_palette(oth.m_palette)
			, m_pixmap(oth.m_pixmap)
			, m_offX(oth.m_offX)
			, m_offY(oth.m_offY)
			, m_width(oth.m_width)
			, m_height(oth.m_height)
			, m_delay(oth.m_delay)
			, m_dispose(oth.m_dispose)
			, m_transparent(oth.m_transparent)
			, m_interlaced(oth.m_interlaced)
			{
			}
			void addref() {
				++count;
			}
			void release()
			{
				if (!--count) delete this;
			}
			bool single() const { return count < 2; }

			GIFPalette& palette() { return m_palette; }
			const GIFPalette& palette() const { return m_palette; }
			GIFPixmap& pixmap() { return m_pixmap; }
			const GIFPixmap& pixmap() const { return m_pixmap; }
			unsigned short offX() const { return m_offX; }
			unsigned short offY() const { return m_offY; }
			unsigned short width() const { return m_width; }
			unsigned short height() const { return m_height; }
			unsigned int delay() const { return m_delay; }
			dispose::DISPOSE dispose() const { return m_dispose; }
			int transparent() const { return m_transparent; }
			bool interlaced() const { return m_interlaced; }
		};
		data* ptr;

		void make_single()
		{
			if (ptr && !ptr->single())
			{
				data* nptr = new data(*ptr);
				ptr->release();
				ptr = nptr;
			}
		}

	public:
		GIFFrame()
		: ptr(NULL)
		{
		}
		GIFFrame(unsigned short offX, unsigned short offY, unsigned short width, unsigned short height, unsigned int delay, dispose::DISPOSE dispose, int transparent, bool interlaced)
		: ptr(new data(offX, offY, width, height, delay, dispose, transparent, interlaced))
		{
		}
		GIFFrame(const GIFFrame& o): ptr(o.ptr)
		{
			if (ptr) ptr->addref();
		}
		~GIFFrame()
		{
			if (ptr) ptr->release();
			ptr = NULL;
		}
		GIFFrame& operator=(const GIFFrame& o)
		{
			if (o.ptr) o.ptr->addref();
			if (ptr) ptr->release();
			ptr = o.ptr;
			return *this;
		}
		GIFPalette& palette()
		{
			make_single();
			if (!ptr)
			{
				static GIFPalette pal;
				return pal;
			}
			return ptr->palette();
		}
		const GIFPalette& palette() const
		{
			if (!ptr)
			{
				static GIFPalette pal;
				return pal;
			}
			return ptr->palette();
		}
		GIFPixmap& pixmap()
		{
			make_single();
			if (!ptr)
			{
				static GIFPixmap map;
				return map;
			}
			return ptr->pixmap();
		}
		const GIFPixmap& pixmap() const
		{
			if (!ptr)
			{
				static GIFPixmap map;
				return map;
			}
			return ptr->pixmap();
		}
		unsigned short offX() const { if (!ptr) return 0; return ptr->offX(); }
		unsigned short offY() const { if (!ptr) return 0; return ptr->offY(); }
		unsigned short width() const { if (!ptr) return 0; return ptr->width(); }
		unsigned short height() const { if (!ptr) return 0; return ptr->height(); }
		unsigned int delay() const { if (!ptr) return 0; return ptr->delay(); }
		dispose::DISPOSE dispose() const { if (!ptr) return dispose::NOT_SPECIFIED; return ptr->dispose(); }
		int transparent() const { if (!ptr) return -1; return ptr->transparent(); }
		bool interlaced() const { if (!ptr) return false; return ptr->interlaced(); }
	};

	struct GIF
	{
		typedef std::vector<GIFFrame> GIFFrames;
		typedef GIFFrames::iterator iterator;
		typedef GIFFrames::const_iterator const_iterator;
		typedef GIFFrames::reverse_iterator reverse_iterator;
		typedef GIFFrames::const_reverse_iterator const_reverse_iterator;
		typedef GIFFrames::size_type size_type;
	private:
		GIFFrames m_frames;
		GIFPalette m_palette;
		unsigned short m_width;
		unsigned short m_height;
		int m_transparent;
	public:
		GIF()
		: m_width(0)
		, m_height(0)
		, m_transparent(-1)
		{
		}
		iterator push_back(const GIFFrame& frame)
		{
			return m_frames.insert(m_frames.end(), frame);
		}
		void setSize(unsigned short width, unsigned short height)
		{
			m_width = width;
			m_height = height;
		}
		void setTransparent(int transparent) { m_transparent = transparent; }

		iterator begin() { return m_frames.begin(); }
		iterator end() { return m_frames.end(); }
		const_iterator begin() const { return m_frames.begin(); }
		const_iterator end() const { return m_frames.end(); }
		reverse_iterator rbegin() { return m_frames.rbegin(); }
		reverse_iterator rend() { return m_frames.rend(); }
		const_reverse_iterator rbegin() const { return m_frames.rbegin(); }
		const_reverse_iterator rend() const { return m_frames.rend(); }
		size_type size() const { return m_frames.size(); }
		bool empty() const { return m_frames.empty(); }
		GIFFrame& at(size_type index) { return m_frames.at(index); };
		const GIFFrame& at(size_type index) const { return m_frames.at(index); };

		GIFPalette& palette() { return m_palette; }
		const GIFPalette& palette() const { return m_palette; }
		unsigned short width() const { return m_width; }
		unsigned short height() const { return m_height; }
		int transparent() const { return m_transparent; }

		static GIF* decode(Stream*);
	};

}}


#endif /* GIF_HPP_ */
