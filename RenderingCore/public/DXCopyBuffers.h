#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "DXBuffer.h"

#include "Jobs.h"

#include "MultiLoader.h"

#include <d3d12.h>
#include <wrl.h>

namespace rendering
{
	class DXCopyBuffersTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DXCopyBuffersTypeDef)

	public:
		TypeProperty m_copyJobSystem;
		TypeProperty m_copyFence;

		DXCopyBuffersTypeDef();
		virtual ~DXCopyBuffersTypeDef();

		void Construct(Value& container) const override;
	};

	class DXCopyBuffers : public ObjectValue, public jobs::LoadingClass
	{
		Value m_device;
		Value m_commandQueue;
		Value m_copyJobSytem;
		Value m_copyFence;

		UINT64 m_copyCounter = 1;

	protected:
		void LoadData(jobs::Job done) override;

	public:
		Value m_copyJobSystemDef;
		Value m_copyFenceDef;

		DXCopyBuffers(const ReferenceTypeDef& typeDef);
		virtual ~DXCopyBuffers();

		void Execute(
			DXBuffer& dst,
			const DXBuffer& src,
			jobs::Job done
		);

		void Execute(
			ID3D12CommandList* const* lists,
			UINT64 numLists,
			jobs::Job done
		);
	};
}