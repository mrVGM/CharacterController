#include "ObjectRecords.h"

#include <mutex>
#include <stack>

namespace
{
	std::queue<gc::GCOperation>* m_primaryQueue = nullptr;
	std::mutex m_queueMutex;
	
	std::queue<gc::GCOperation> m_queue1;
	std::queue<gc::GCOperation> m_queue2;

	std::queue<gc::GCOperation>& GetPrimaryQueue()
	{
		if (!m_primaryQueue)
		{
			m_primaryQueue = &m_queue1;
		}

		return *m_primaryQueue;
	}

	std::queue<gc::GCOperation>& GetSecondaryQueue()
	{
		std::queue<gc::GCOperation>& primary = GetPrimaryQueue();
		return &primary == &m_queue1 ? m_queue2 : m_queue1;
	}

	void SwapQueues()
	{
		std::queue<gc::GCOperation>& secondary = GetSecondaryQueue();
		m_primaryQueue = &secondary;
	}

	gc::ObjectRecordManager m_manager;
}

gc::ObjectRecord& gc::ObjectRecordManager::GetRecord(const ManagedObject* object)
{
	ObjectRecord* rec = nullptr;
	auto it = m_records.find(object);

	if (it == m_records.end())
	{
		m_records[object] = ObjectRecord();
		ObjectRecord& tmp = m_records[object];
		tmp.m_object = object;
		rec = &tmp;
	}
	else
	{
		rec = &it->second;
	}

	return *rec;
}

gc::ObjectRecordManager& gc::ObjectRecordManager::GetManager()
{
	return m_manager;
}

void gc::ObjectRecordManager::Tick()
{
	m_queueMutex.lock();
	
	SwapQueues();

	m_queueMutex.unlock();

	std::list<const ManagedObject*> toCheck;
	UpdateObjectsState(GetSecondaryQueue(), toCheck);

	for (auto it = m_records.begin(); it != m_records.end(); ++it)
	{
		ObjectRecord& cur = it->second;
		if (cur.m_state == GCObjectState::Alive)
		{
			cur.m_state = GCObjectState::Unchecked;
		}
	}

	UpdateVitality(toCheck);
}

void gc::ObjectRecordManager::UpdateObjectsState(std::queue<GCOperation>& operations, std::list<const ManagedObject*>& toCheck)
{
	while (!operations.empty())
	{
		GCOperation cur = operations.front();
		operations.pop();

		switch (cur.m_op)
		{
		case IncrementRefsOp:
			++GetRecord(cur.m_object1).m_refs;
			break;
		case AddLinkOp:
			GetRecord(cur.m_object1).m_links.push_back(&GetRecord(cur.m_object2));
			break;
		case DecrementRefsOp:
			++GetRecord(cur.m_object1).m_refs;
			toCheck.push_back(cur.m_object1);
			break;
		case RemoveLinkOp:
		{
			ObjectRecord& rec1 = GetRecord(cur.m_object1);
			ObjectRecord& rec2 = GetRecord(cur.m_object1);
			for (auto it = rec1.m_links.begin(); it != rec1.m_links.end(); ++it)
			{
				if (*it == &rec2)
				{
					rec1.m_links.erase(it);
					break;
				}
			}
			toCheck.push_back(cur.m_object1);
			break;
		}
		}
	}
}


namespace
{
	struct VitalityCheckTask
	{
		std::stack<VitalityCheckTask*>& m_taskStack;

		gc::ObjectRecord* m_record;
		std::list<VitalityCheckTask*> m_subTasks;
		bool m_done = false;
		bool m_createdSubtasks = false;

		VitalityCheckTask(std::stack<VitalityCheckTask*>& taskStack, gc::ObjectRecord* record) :
			m_taskStack(taskStack),
			m_record(record)
		{
		}

		void Dispose()
		{
			for (auto it = m_subTasks.begin(); it != m_subTasks.end(); ++it)
			{
				delete *it;
			}
		}

		void UpdateTask()
		{
			using namespace gc;

			if (m_record->m_state != GCObjectState::Unchecked)
			{
				m_done = true;
				return;
			}

			if (m_record->m_refs > 0)
			{
				m_record->m_state = GCObjectState::Alive;
				m_done = true;
				return;
			}

			if (!m_createdSubtasks)
			{
				for (auto it = m_record->m_links.begin(); it != m_record->m_links.end(); ++it)
				{
					ObjectRecord* cur = *it;
					VitalityCheckTask* subTask = new VitalityCheckTask(m_taskStack, cur);
					m_subTasks.push_back(subTask);
					m_taskStack.push(subTask);
				}

				m_createdSubtasks = true;
				return;
			}

			for (auto it = m_record->m_links.begin(); it != m_record->m_links.end(); ++it)
			{
				ObjectRecord* cur = *it;
				if (cur->m_state == GCObjectState::Alive)
				{
					m_record->m_state = GCObjectState::Alive;
					m_done = true;
					return;
				}
			}
			
			m_record->m_state = GCObjectState::Dead;
			m_done = true;
		}
	};
}

void gc::ObjectRecordManager::UpdateVitality(std::list<const ManagedObject*> objects)
{
	std::stack<VitalityCheckTask*> workStack;
	std::list<VitalityCheckTask*> initialTasks;

	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		ObjectRecord& cur = GetRecord(*it);
		VitalityCheckTask* task = new VitalityCheckTask(workStack, &cur);
		initialTasks.push_back(task);
		workStack.push(task);
	}

	while (!workStack.empty())
	{
		VitalityCheckTask* cur = workStack.top();
		cur->UpdateTask();

		if (cur->m_done)
		{
			workStack.pop();
			cur->Dispose();
			continue;
		}

		cur->UpdateTask();
	}
}

void gc::PushOp(const GCOperation& op)
{
	m_queueMutex.lock();

	std::queue<GCOperation>& primaryQueue = GetPrimaryQueue();
	primaryQueue.push(op);

	m_queueMutex.unlock();
}
