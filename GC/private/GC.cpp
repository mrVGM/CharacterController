#include "GC.h"

#include "ObjectRecords.h"

void gc::IncrementRefs(const ManagedObject* object)
{
	GCOperation op{ GCOp::IncrementRefsOp, object };
	PushOp(op);
}

void gc::DecrementRefs(const ManagedObject* object)
{
	GCOperation op{ GCOp::DecrementRefsOp, object };
	PushOp(op);
}

void gc::AddLink(const ManagedObject* from, const ManagedObject* to)
{
	GCOperation op{ GCOp::AddLinkOp, from, to };
	PushOp(op);
}

void gc::RemoveLink(const ManagedObject* from, const ManagedObject* to)
{
	GCOperation op{ GCOp::RemoveLinkOp, from, to };
	PushOp(op);
}


void gc::GCTick(std::list<const ManagedObject*>& managedObjectsToDelete)
{
	ObjectRecordManager& manager = ObjectRecordManager::GetManager();
	manager.Tick(managedObjectsToDelete);
}