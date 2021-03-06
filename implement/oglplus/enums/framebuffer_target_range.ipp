//  File implement/oglplus/enums/framebuffer_target_range.ipp
//
//  Automatically generated file, DO NOT modify manually.
//  Edit the source 'source/enums/oglplus/framebuffer_target.txt'
//  or the 'source/enums/make_enum.py' script instead.
//
//  Copyright 2010-2017 Matus Chochlik.
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
namespace enums {
OGLPLUS_LIB_FUNC aux::CastIterRange<
	const GLenum*,
	FramebufferTarget
> ValueRange_(FramebufferTarget*)
#if (!OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)) && \
	!defined(OGLPLUS_IMPL_EVR_FRAMEBUFFERTARGET)
#define OGLPLUS_IMPL_EVR_FRAMEBUFFERTARGET
{
static const GLenum _values[] = {
#if defined GL_DRAW_FRAMEBUFFER
GL_DRAW_FRAMEBUFFER,
#endif
#if defined GL_READ_FRAMEBUFFER
GL_READ_FRAMEBUFFER,
#endif
0
};
return aux::CastIterRange<
	const GLenum*,
	FramebufferTarget
>(_values, _values+sizeof(_values)/sizeof(_values[0])-1);
}
#else
;
#endif
} // namespace enums

