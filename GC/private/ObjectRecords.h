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
		size_t m_id1;
		size_t m_id2;
		const ManagedObject* m_object1_ = nullptr;
		const ManagedObject* m_object2_ = nullptr;
	};

	class ObjectRecord
	{
	public:
		const ManagedObject* m_object = nullptr;
		int m_age = 0;
		GCObjectState m_state = GCObjectState::Unchecked;

		int m_refs = 0;
		std::list<size_t> m_links;

	};

	class ObjectRecordManager
	{
	private:
	public:
		static ObjectRecordManager& GetManager();

		std::map<size_t, ObjectRecord> m_records;
		ObjectRecord& GetRecord(const ManagedObject* object);

		void Tick(std::list<const ManagedObject*>& managedObjectsToDelete);
		void UpdateObjectsState(std::queue<GCOperation>& operations, std::list<size_t>& toCheck);
		void UpdateVitality(std::list<size_t>& objects);

		void SetGCActivatedListener(const GCActivatedListener& listener);
	};

	void PushOp(const GCOperation& op);
}
