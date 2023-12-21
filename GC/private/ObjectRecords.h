#pragma once

#include "GC.h"

#include <list>
#include <map>

#include <queue>

namespace gc
{
	enum GCOp
	{
		IncrementRefsOp,
		DecrementRefsOp,
		AddLinkOp,
		RemoveLinkOp
	};

	enum GCObjectState
	{
		Unchecked,
		Dead,
		Alive,
	};

	struct GCOperation
	{
		GCOp m_op;
		const ManagedObject* m_object1 = nullptr;
		const ManagedObject* m_object2 = nullptr;
	};

	class ObjectRecord
	{
	public:
		const ManagedObject* m_object = nullptr;
		int m_age = 0;
		GCObjectState m_state = GCObjectState::Unchecked;

		int m_refs = 0;
		std::list<ObjectRecord*> m_links;

	};

	class ObjectRecordManager
	{
	private:
		std::map<const ManagedObject*, ObjectRecord> m_records;
		ObjectRecord& GetRecord(const ManagedObject* object);
	public:
		static ObjectRecordManager& GetManager();

		void Tick();
		void UpdateObjectsState(std::queue<GCOperation>& operations, std::list<const ManagedObject*>& toCheck);
		void UpdateVitality(std::list<const ManagedObject*> objects);
	};

	void PushOp(const GCOperation& op);
}
