#include "GC.h"

#include "ObjectRecords.h"

std::stringstream gc::GCLogger::m_log;

size_t gc::ManagedObject::m_index = 1;

gc::ManagedObject::ManagedObject()
{
	m_id = m_index++;

	ObjectRecordManager& manager = ObjectRecordManager::GetManager();
	manager.GetRecord(this);
}

gc::ManagedObject::~ManagedObject()
{
}

size_t gc::ManagedObject::GetId() const
{
	return m_id;
}

void gc::IncrementRefs(size_t id)
{
	GCOperation op{ GCOp::IncrementRefsOp, id };
	PushOp(op);
}

void gc::DecrementRefs(size_t id)
{
	GCOperation op{ GCOp::DecrementRefsOp, id };
	PushOp(op);
}

void gc::AddLink(size_t from, size_t to)
{
	GCOperation op{ GCOp::AddLinkOp, from, to };
	PushOp(op);
}

void gc::RemoveLink(size_t from, size_t to)
{
	GCOperation op{ GCOp::RemoveLinkOp, from, to };
	PushOp(op);
}


void gc::GCTick(std::list<const ManagedObject*>& managedObjectsToDelete)
{
	gc::GCLogger::m_log << "START TICK" << std::endl;

	ObjectRecordManager& manager = ObjectRecordManager::GetManager();
	manager.Tick(managedObjectsToDelete);

	gc::GCLogger::m_log << "END TICK" << std::endl;
}

void gc::SetGCActivatedListener(const GCActivatedListener& listener)
{
	ObjectRecordManager& manager = ObjectRecordManager::GetManager();
	manager.SetGCActivatedListener(listener);
}