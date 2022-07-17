// Copyright (c) 2022 SS

#include "stdafx.h"
#include "IsotopeDB.h"
#include "Model.h"
#include "StdioFileEx.h"
#include "NumString.h"
#include <map>
#include <exception>
#include <random>
#include "RandomEngine.h"

namespace Decay
{
	bool CIsotopeDB::Open(const CString& filename)
	{
		CCSVFile db;
		db.SetSeparator(_T(','));

		if (!db.Open(filename, std::ios_base::in, _SH_DENYWR))
			return false;

		clear();

		std::map<CString, CString> dataMap;
		
		CStringArray header;
		if (!db.ReadRow(header))
			return false;

		CStringArray rowData;
		while (db.ReadRow(rowData))
		{
			if (rowData.GetCount() != header.GetCount())
				continue;

			for (int col = 0; col < header.GetCount(); col++)
			{
				dataMap[header.GetAt(col)] = rowData.GetAt(col);
			}

			try
			{
				push_back(CIsotope(dataMap));
			}
			catch (std::exception e)
			{
				continue;
			}
		}

		return true;
	}

	CIsotopeDB::iterator CIsotopeDB::Find(UINT Z, UINT A /*= UINT_MAX*/)
	{
		for (auto it = begin(); it != end(); it++)
		{
			if (it->Z() == Z)
			{
				if (A == UINT_MAX || it->A() == A)
					return it;
			}
		}

		return end();
	}

	CIsotopeDB::iterator CIsotopeDB::Find(const CString& symbol, UINT A)
	{
		for (auto it = begin(); it != end(); it++)
		{
			if (it->Symbol().CompareNoCase(symbol) == 0 && it->A() == A)
				return it;
		}

		return end();
	}

	CIsotopeDB::iterator CIsotopeDB::Find(const CString& searchString)
	{
		struct SearchToken
		{
			CString text;
			bool isNumeric{ false };
			UINT value{ 0 };

			bool empty() const { return text.IsEmpty(); }
		};

		std::vector<SearchToken> tokens;

		for (int i = 0; i < searchString.GetLength(); i++)
		{
			TCHAR ch = searchString[i];
			if (_istalnum(ch))
			{
				if (tokens.empty())
					tokens.push_back(SearchToken());

				if (_istalpha(ch))
				{
					if (tokens.back().isNumeric && !tokens.back().empty())
						tokens.push_back(SearchToken());

					tokens.back().isNumeric = false;
				}
				else /*if (_istdigit(ch))*/
				{
					if (!tokens.back().isNumeric && !tokens.back().empty())
						tokens.push_back(SearchToken());

					tokens.back().isNumeric = true;
				}

				tokens.back().text += ch;
			}
			else
			{
				// End current token and push new one
				if (!tokens.empty())
					tokens.push_back(SearchToken());
			}
		}

		// remove blank tokens
		for (auto token = tokens.begin(); token != tokens.end(); /*nop*/)
		{
			if (token->empty())
				token = tokens.erase(token);
			else
			{
				if (token->isNumeric)
					StringToUInt(token->value, token->text);

				token++;
			}
		}

		auto matchIsot = end();
		UINT matchRank = 0;
		for (auto isot = begin(); isot != end(); isot++)
		{
			UINT rank = 0;
			for (auto token = tokens.begin(); token != tokens.end(); token++)
			{
				if (!token->isNumeric)
				{
					if (isot->Symbol() == token->text)
						rank += 3;
					else if (isot->Symbol().CompareNoCase(token->text) == 0)
						rank += 2;
				}
				else
				{
					if (isot->Z() == token->value)
					{
						if (std::distance(tokens.begin(), token) == 0)
							rank += 3;
						else
							rank += 1;
					}
					else if (isot->A() == token->value)
					{
						if (std::distance(tokens.begin(), token) == 0)
							rank += 2;
						else
							rank += 1;
					}
				}
			}

			if (rank > matchRank)
			{
				matchRank = rank;
				matchIsot = isot;
			}
		}

		return matchIsot;
	}

	CIsotopeDB::iterator CIsotopeDB::GetRandom()
	{
		std::vector<iterator> candidates;

		for (auto isot = begin(); isot != end(); isot++)
		{
			if (isot->HalfLife().valueSec >= Decay::CModel::kMinHalfLife &&
				isot->HalfLife().valueSec < Decay::CModel::kMaxHalfLife)
			{
				if (isot->DecayMode().mode.size() == 1 && 
					isot->DecayMode().mode.front().n == 1)
				{
					switch (isot->DecayMode().mode.front().mode)
					{
					case CIsotope::EDecayMode::alpha:
						if (Find(isot->Z() - 2, isot->A() - 4) != end())
							candidates.push_back(isot);
						break;

					case CIsotope::EDecayMode::beta_m:
						if (Find(isot->Z() + 1, isot->A()) != end())
							candidates.push_back(isot);
						break;

					case CIsotope::EDecayMode::beta_p:
					case CIsotope::EDecayMode::EC:
						if (Find(isot->Z() - 1, isot->A()) != end())
							candidates.push_back(isot);
						break;

					default:
						break;
					}
				}
			}
		}

		if (candidates.empty())
			return end();
		
		std::uniform_int_distribution<UINT> dist(0, candidates.size() - 1);
		UINT index = dist(CRandomEngine::Instance());

		return candidates[index];
	}

	void CIsotopeDB::PushFakeTable()
	{
		push_back(CIsotope());
	}

}

