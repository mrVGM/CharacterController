#pragma once

#include <list>

namespace gc
{
	class ManagedObject
	{
	private:
		static size_t m_index;
		size_t m_id = 0;

	public:
		ManagedObject();
		virtual ~ManagedObject();

		size_t GetId() const;
	};

	void IncrementRefs(const ManagedObject* object);
	void DecrementRefs(const ManagedObject* object);

	void AddLink(const ManagedObject* from, const ManagedObject* to);
	void RemoveLink(const ManagedObject* from, const ManagedObject* to);

	void GCTick(std::list<const ManagedObject*>& managedObjectsToDelete);
}