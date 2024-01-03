#pragma once

#include <list>

#include <sstream>

namespace gc
{
	struct GCLogger
	{
		static std::stringstream m_log;
	};

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

	class GCActivatedListener
	{
	public:
		virtual void OnActivated() const = 0;
	};

	void IncrementRefs(size_t id);
	void DecrementRefs(size_t id);

	void AddLink(size_t from, size_t to);
	void RemoveLink(size_t from, size_t to);

	void GCTick(std::list<const ManagedObject*>& managedObjectsToDelete);

	void SetGCActivatedListener(const GCActivatedListener& listener);
}