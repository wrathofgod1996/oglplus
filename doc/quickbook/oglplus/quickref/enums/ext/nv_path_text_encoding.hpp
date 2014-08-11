//  File doc/quickbook/oglplus/quickref/enums/ext/nv_path_text_encoding.hpp
//
//  Automatically generated file, DO NOT modify manually.
//  Edit the source 'source/enums/oglplus/ext/nv_path_text_encoding.txt'
//  or the 'source/enums/make_enum.py' script instead.
//
//  Copyright 2010-2014 Matus Chochlik.
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//[oglplus_enums_ext_nv_path_text_encoding
namespace oglplus {

enum class PathNVTextEncoding : GLenum
{
	UTF8  = GL_UTF8_NV,
	UTF16 = GL_UTF16_NV
};

#if !__OGLPLUS_NO_ENUM_VALUE_RANGES
template <>
__Range<PathNVTextEncoding> __EnumValueRange<PathNVTextEncoding>(void);
#endif

#if !__OGLPLUS_NO_ENUM_VALUE_NAMES
__StrCRef __EnumValueName(PathNVTextEncoding);
#endif

} // namespace oglplus
//]