//  File doc/quickbook/oglplus/quickref/enums/ext/nv_path_fill_cover_mode_class.hpp
//
//  Automatically generated file, DO NOT modify manually.
//  Edit the source 'source/enums/oglplus/ext/nv_path_fill_cover_mode.txt'
//  or the 'source/enums/make_enum.py' script instead.
//
//  Copyright 2010-2017 Matus Chochlik.
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//[oglplus_enums_ext_nv_path_fill_cover_mode_class
#if !__OGLPLUS_NO_ENUM_VALUE_CLASSES
namespace enums {

template <typename Base, template<__PathNVFillCoverMode> class Transform>
class __EnumToClass<Base, __PathNVFillCoverMode, Transform> /*<
Specialization of __EnumToClass for the __PathNVFillCoverMode enumeration.
>*/
 : public Base
{
public:
	EnumToClass(void);
	EnumToClass(Base&& base);

	Transform<PathNVFillCoverMode::ConvexHull>
		ConvexHull;
	Transform<PathNVFillCoverMode::BoundingBox>
		BoundingBox;
	Transform<PathNVFillCoverMode::BoundingBoxOfBoundingBoxes>
		BoundingBoxOfBoundingBoxes;
	Transform<PathNVFillCoverMode::FillCover>
		FillCover;
};

} // namespace enums
#endif
//]

