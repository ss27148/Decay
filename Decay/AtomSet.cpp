// Copyright (c) 2022 SS

#include "stdafx.h"
#include "AtomSet.h"

namespace Decay
{

	void CAtomSet::Initialize(UINT rows, UINT cols)
	{
		m_rows = rows;
		m_cols = cols;

		reserve(m_rows * m_cols);

		for (UINT row = 0; row < m_rows; row++)
		{
			for (UINT col = 0; col < m_cols; col++)
			{
				push_back(CAtom(row, col));
			}
		}

		m_posDist.param(decltype(m_posDist)::param_type(0, m_rows * m_cols - 1));

		std::shuffle(begin(), end(), CRandomEngine::Instance());
	}

	void CAtomSet::Reset()
	{
		for (auto it = begin(); it != end(); ++it)
			it->SetActive(true);
	}

	UINT CAtomSet::RandomIndex()
	{
		return m_posDist(CRandomEngine::Instance());
	}

	CAtom& CAtomSet::RandomAtom()
	{
		return at(RandomIndex());
	}

}

