/*
 * JavaBitmap.cpp
 *
 *  Created on: 11-02-2013
 *      Author: Marcin
 */

#include "JavaBitmap.hpp"

namespace gfx
{
	bool Bitmap::resize(unsigned short width, unsigned short height)
	{
		if (width == m_width && height == m_height)
			return true;
		JNIEnv* env = jni::Env();
	
		agfx::Bitmap::Config config = agfx::Bitmap::Config::ARGB_8888();
		jni::Exception::check(env);
	
		agfx::Bitmap bm = agfx::Bitmap::createBitmap(width, height, config);
		jni::Exception::check(env);
		
		m_width = m_height = 0;
		jobject copy = NULL;
	
		if ((jobject)bm)
			copy = env->NewGlobalRef((jobject)bm);
		jni::Exception::check(env);
	
		if ((jobject)m_backing)
			env->DeleteGlobalRef((jobject)m_backing);
		jni::Exception::check(env);
	
		m_backing = copy;
		if (m_backing)
		{
			m_width = m_backing.getWidth();
			jni::Exception::check(env);
			m_height = m_backing.getHeight();
			jni::Exception::check(env);
		}
		
		return m_backing._this() != NULL;
	}
} //gfx


