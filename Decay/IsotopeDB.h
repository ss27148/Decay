// Copyright (c) 2022 SS

#pragma once
#include <vector>
#include "Isotope.h"

namespace Decay
{
	class CIsotopeDB : private std::vector<CIsotope>
	{
	public:
		typedef std::vector<CIsotope>::iterator iterator;

		bool Open(const CString& filename);

		iterator Find(UINT Z, UINT A = UINT_MAX);
		iterator Find(const CString& symbol, UINT A);
		iterator Find(const CString& searchString);

		static CIsotopeDB& Instance()
		{
			static CIsotopeDB db;
			return db;
		}

		iterator begin() { return std::vector<CIsotope>::begin(); }
		iterator end() { return std::vector<CIsotope>::end(); }

		CIsotope& at(UINT index) { return std::vector<CIsotope>::at(index); }

		CIsotopeDB::iterator GetRandom();

		UINT size() const { return std::vector<CIsotope>::size(); }

		void PushFakeTable();
	};
}