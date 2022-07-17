// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the Chromium LICENSE file.

// Copyright (c) 2017 SS

#pragma once

#include <limits>

template <class ObserverType, bool check_empty = false>
class CObserverList
{
	//friend class CObserverList<ObserverType, check_empty>::Iterator;

private:
	typedef CArray<ObserverType*> ListType;

public:
	// Enumeration of which observers are notified.
	enum NotificationType
	{
		// Specifies that any observers added during notification are notified.
		// This is the default type if non type is provided to the constructor.
		NOTIFY_ALL,

		// Specifies that observers added while sending out notification are not
		// notified.
		NOTIFY_EXISTING_ONLY
	};

	CObserverList() : m_notify_depth(0), m_type(NOTIFY_ALL) {}
	CObserverList(NotificationType type) : m_notify_depth(0), m_type(type) {}
	~CObserverList()
	{
		// When check_empty is true, assert that the list is empty on destruction.
		if (check_empty)
		{
			Compact();
			ASSERT(m_observers.GetCount() == 0);
		}
	}

	INT_PTR Find(ObserverType* obs)
	{
		for (INT_PTR i = 0; i < m_observers.GetCount(); i++)
		{
			if (m_observers[i] == obs)
				return i;
		}
		return -1;
	}

	// Add an observer to the list.
	void AddObserver(ObserverType* obs)
	{
		// Observers can only be added once!
		ASSERT(Find(obs) == -1);
		m_observers.Add(obs);
	}

	// Remove an observer from the list.
	void RemoveObserver(ObserverType* obs)
	{
		INT_PTR i = Find(obs);
		if (i != -1)
		{
			if (m_notify_depth)
				m_observers[i] = NULL;
			else
				m_observers.RemoveAt(i);
		}
	}

	int GetCount() const { return (int) m_observers.GetCount(); }

	ObserverType* GetElementAt(int index) const { return m_observers[index]; }

	// An iterator class that can be used to access the list of observers.  See
	// also the FOREACH_OBSERVER macro defined below.
	class Iterator
	{
	public:
		Iterator(const CObserverList<ObserverType>& list)
			: m_list(list),
			m_index(0),
			m_maxIndex(list.m_type == NOTIFY_ALL ?
				0x7fffffff :
				list.GetCount())
		{
			++m_list.m_notify_depth;
		}

		~Iterator()
		{
			if (--m_list.m_notify_depth == 0)
				m_list.Compact();
		}

		ObserverType* GetNext()
		{
			ListType& observers = m_list.m_observers;
			// Advance if the current element is null
			int maxIndex = (std::min)(m_maxIndex, m_list.GetCount());
			while (m_index < maxIndex && !observers[m_index])
				++m_index;
			return m_index < maxIndex ? observers[m_index++] : NULL;
		}

	private:
		const CObserverList<ObserverType>& m_list;
		int m_index;
		int m_maxIndex;
	};

private:

	void Compact() const
	{
		INT_PTR i = m_observers.GetCount() - 1;
		while (i >= 0)
		{
			if (m_observers[i] == NULL)
				m_observers.RemoveAt(i);
			--i;
		}
	}

	// These are marked mutable to facilitate having NotifyAll be const.
	mutable ListType m_observers;
	mutable int m_notify_depth;
	NotificationType m_type;

};

#define FOR_EACH_OBSERVER(ObserverType, observer_list, func)	\
	do {														\
	CObserverList<ObserverType>::Iterator it(observer_list);		\
	ObserverType* obs;											\
	while ((obs = it.GetNext()) != NULL)						\
		obs->func;												\
	} while (0)