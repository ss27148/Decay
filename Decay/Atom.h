// Copyright (c) 2022 SS

#pragma once

namespace Decay
{
	class CAtom
	{
	public:
		CAtom(UINT row, UINT col) :
			m_row(row), m_col(col)
		{	}

		inline bool IsActive() const { return m_active; }
		inline void SetActive(bool active) { m_active = active; }
		inline void Flip() { m_active = false; }

		inline UINT Row() const { return m_row; }
		inline UINT Col() const { return m_col; }

	private:
		bool m_active{ true };
		UINT m_row;
		UINT m_col;
	};
}