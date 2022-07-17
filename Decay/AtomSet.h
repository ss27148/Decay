// Copyright (c) 2022 SS

#pragma once
#include <vector>
#include "RandomEngine.h"
#include "Atom.h"

namespace Decay
{
	class CAtomSet : private std::vector<CAtom>
	{
	public:
		typedef std::vector<CAtom>::iterator iterator;

		void Initialize(UINT rows, UINT cols);

		UINT Rows() const { return m_rows; }
		UINT Cols() const { return m_cols; }

		UINT Count() const { return size(); }

		void Reset();

		UINT RandomIndex();

		CAtom& RandomAtom();

		CAtom& At(UINT index) { return at(index); }
		const CAtom& At(UINT index) const { return at(index); }

	private:
		std::uniform_int_distribution<UINT> m_posDist;
		UINT m_rows{ 0 };
		UINT m_cols{ 0 };

	};

}