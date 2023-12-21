#pragma once

namespace gc
{
	class ManagedObject
	{
	};

	void IncrementRefs(const ManagedObject* object);
	void DecrementRefs(const ManagedObject* object);

	void AddLink(const ManagedObject* from, const ManagedObject* to);
	void RemoveLink(const ManagedObject* from, const ManagedObject* to);
}