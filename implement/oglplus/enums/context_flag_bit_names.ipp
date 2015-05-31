//  File implement/oglplus/enums/context_flag_bit_names.ipp
//
//  Automatically generated file, DO NOT modify manually.
//  Edit the source 'source/enums/oglplus/context_flag_bit.txt'
//  or the 'source/enums/make_enum.py' script instead.
//
//  Copyright 2010-2015 Matus Chochlik.
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
namespace enums {
OGLPLUS_LIB_FUNC CStrRef ValueName_(
	ContextFlagBit*,
	GLbitfield value
) noexcept
#if (!OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)) && \
	!defined(OGLPLUS_IMPL_EVN_CONTEXTFLAGBIT)
#define OGLPLUS_IMPL_EVN_CONTEXTFLAGBIT
{
switch(value)
{
#if defined GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT
	case GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT: return CStrRef("CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT");
#endif
#if defined GL_CONTEXT_FLAG_DEBUG_BIT
	case GL_CONTEXT_FLAG_DEBUG_BIT: return CStrRef("CONTEXT_FLAG_DEBUG_BIT");
#endif
#if defined GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB
	case GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB: return CStrRef("CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB");
#endif
	default:;
}
OGLPLUS_FAKE_USE(value);
return CStrRef();
}
#else
;
#endif
} // namespace enums

