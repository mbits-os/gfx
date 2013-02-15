/*
 * Stream.hpp
 *
 *  Created on: 09-02-2013
 *      Author: Marcin
 */

#ifndef STREAM_HPP_
#define STREAM_HPP_

#include <stddef.h>

/*struct _JNIEnv;
class _jobject;
class _jbyteArray;

typedef _JNIEnv      JNIEnv;
typedef _jobject*    jobject;
typedef _jbyteArray* jbyteArray;*/

namespace gfx
{
	struct Stream
	{
		virtual ~Stream() {}

		virtual size_t read(void*, size_t) = 0;
		template <class T, size_t len> size_t read(T (&buffer)[len]) { return read(buffer, sizeof(buffer)); }
		template <class T> size_t read(T& buffer) { return read(&buffer, sizeof(buffer)); }

		virtual size_t skip(size_t length) = 0;
		virtual void reset() = 0;
		//static Stream* createStream(JNIEnv* env, jobject stream, jbyteArray storage);
		//static Stream* createStream(const void* data, int len);
	};
}

#endif /* STREAM_HPP_ */
