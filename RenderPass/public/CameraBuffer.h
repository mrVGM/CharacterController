#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include "d3dx12.h"

#include "DXBuffer.h"

#include <wrl.h>

namespace rendering::render_pass
{
	class CameraBufferTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(CameraBufferTypeDef)

	public:
		CameraBufferTypeDef();
		virtual ~CameraBufferTypeDef();

		void Construct(Value& container) const override;
	};
}