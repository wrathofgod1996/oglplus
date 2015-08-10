/**
 *  @file oglplus/texgen/sibling_input.ipp
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2015 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <oglplus/config/basic.hpp>
#include <cassert>

namespace oglplus {
namespace texgen {

OGLPLUS_LIB_FUNC
bool
SiblingInputSlot::
DoesAcceptValueType(SlotDataType type)
{
	return Base_::AcceptsValueType(type);
}

OGLPLUS_LIB_FUNC
void
SiblingInputSlot::
AddSibling(SiblingInputSlot& sibling)
{
	assert(&sibling != this);
	_siblings.insert(&sibling);
}

OGLPLUS_LIB_FUNC
bool
SiblingInputSlot::
AcceptsValueType(SlotDataType type)
{
	bool ok;
	for(SiblingInputSlot* sibling: _siblings)
	{
		ok = sibling->Fallback().CanProvideValueType(type);
		ok|= DataTypeConvertible(sibling->ValueType(), type);

		if(!ok) return false;
	}
	return true;
}

OGLPLUS_LIB_FUNC
bool
SiblingInputSlot::
Connect(OutputSlot& output)
{
	SlotDataType type = output.ValueType();

	if(DoConnect(output))
	{
		bool ok;
		for(SiblingInputSlot* sibling: _siblings)
		{
			if(sibling->IsConnected())
			{
				ok = sibling->DoesAcceptValueType(type);
				ok|= DataTypeConvertible(type, sibling->ValueType());
				if(!ok)
				{
					sibling->Disconnect();
				}
				else continue;
			}
			sibling->Fallback().RequireValueType(type);
		}
		Parent().Validate(*this);
		Parent().Update();
		return true;
	}
	return false;
}

} // namespace texgen
} // namespace oglplus

