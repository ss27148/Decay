// Copyright (c) 2022 SS

#include "stdafx.h"
#include "Isotope.h"
#include "NumString.h"
#include <exception>
#include "IsotopeDB.h"

namespace Decay
{
	CIsotope::CIsotope()
	{
		m_symbol = _T("Ac");

		m_Z.N = 89;
		m_A.N = 222;

		m_mass.mass = 222.017750;
		m_mass.text = _T("222.017750");

		m_decayMode.mode.push_back(EDecayMode::alpha);

		m_halfLife.unit = HLUnit::s;
		m_halfLife.value = 5.015;
		m_halfLife.valueSec = 5.015;
		m_halfLife.text = _T("5.015");
	}

	CIsotope::CIsotope(std::map<CString, CString>& data)
	{
		m_symbol = data[_T("symbol")];

		UINT N;
		if (!StringToUInt(N, data[_T("z")]))
			throw std::exception("Invalid Z");
		m_Z.Set(N);

		if (!StringToUInt(N, data[_T("n")]))
			throw std::exception("Invalid N");
		m_A.Set(m_Z.N + N);

		m_mass.Set(data[_T("atomic_mass")]);

		m_decayMode.Set(data[_T("decay_1")], data[_T("half_life")]);

		m_halfLife.Set(data[_T("half_life")], data[_T("unit_hl")], data[_T("half_life_sec")]);

		m_D = data[_T("abundance")];
	}

	double CIsotope::Q(CIsotopeDB& isotDB) const
	{
		CIsotope* product = Product(isotDB);
		if (!product)
			return 0;

		if (m_decayMode.mode.size() != 1)
			return 0;

		UINT n = m_decayMode.mode[0].n;
		double dm = 0;

		switch (m_decayMode.mode[0].mode)
		{
		case EDecayMode::alpha:
		{
			CIsotopeDB::iterator alpha = isotDB.Find(2, 4);
			if (alpha == isotDB.end())
				return 0;

			dm = Mass().mass - (alpha->Mass().mass * n + product->Mass().mass);
		}
			break;

		case EDecayMode::beta_m:
			dm = Mass().mass - product->Mass().mass;
			break;

		case EDecayMode::beta_p:
		case EDecayMode::EC:
			dm = Mass().mass - product->Mass().mass;
			break;

		case EDecayMode::p:
		{
			CIsotopeDB::iterator proton = isotDB.Find(1, 1);
			if (proton == isotDB.end())
				return 0;

			dm = Mass().mass - (proton->Mass().mass * n + product->Mass().mass);
		}
			break;

		case EDecayMode::n:
		{
			CIsotopeDB::iterator neutron = isotDB.Find(0, 1);
			if (neutron == isotDB.end())
				return 0;

			dm = Mass().mass - (neutron->Mass().mass * n + product->Mass().mass);
		}
		break;

		case EDecayMode::SF:
		case EDecayMode::stable:
		default:
			break;
		}

		return dm * 931.5 / 1e6; // dm in microAMU
	}

	CIsotope* CIsotope::Product(CIsotopeDB& isotDB) const
	{
		if (m_decayMode.mode.size() != 1)
			return nullptr;

		CIsotopeDB::iterator product;
		UINT n = m_decayMode.mode[0].n;

		switch (m_decayMode.mode[0].mode)
		{
		case EDecayMode::alpha:
			if ((product = isotDB.Find(m_Z.N - 2 * n, m_A.N - 4 * n)) != isotDB.end())
				return &(*product);
			break;

		case EDecayMode::beta_m:
			if ((product = isotDB.Find(m_Z.N + n, m_A.N)) != isotDB.end())
				return &(*product);
			break;

		case EDecayMode::beta_p:
		case EDecayMode::EC:
			if ((product = isotDB.Find(m_Z.N - n, m_A.N)) != isotDB.end())
				return &(*product);
			break;

		case EDecayMode::p:
			if ((product = isotDB.Find(m_Z.N - n, m_A.N - n)) != isotDB.end())
				return &(*product);
			break;

		case EDecayMode::n:
			if ((product = isotDB.Find(m_Z.N, m_A.N - n)) != isotDB.end())
				return &(*product);
			break;

		case EDecayMode::SF:
		case EDecayMode::stable:
		default:
			break;
		}

		return nullptr;
	}

	void CIsotope::CAtomicMass::Set(const CString& _text)
	{
		CString massamu = _text;
		massamu.Trim();

		if (!massamu.IsEmpty())
		{
			int dppos = massamu.Find(_T('.'));
			if (dppos != -1)
				massamu.Delete(dppos);
			else
				dppos = massamu.GetLength();

			massamu.Insert(dppos - 6, _T('.'));

			if (!StringToDouble(mass, _text))
				throw std::exception("Invalid Mass");
		}

		text = massamu;
		text.FreeExtra();
	}

	void CIsotope::CDecayMode::Set(const CString& _text, const CString& _halfLifeText)
	{
		if (_halfLifeText == _T("STABLE"))
		{
			mode.push_back(EDecayMode::stable);
			return;
		}

		UINT n = 1;
		for (int i = 0; i < _text.GetLength(); i++)
		{
			TCHAR ch = _text[i];

			if (_istdigit(ch))
			{
				n = ch - '0';
			}
			else if (_istspace(ch))
			{
				// continue;
			}
			else
			{
				if (_text[i] == 'A')
					mode.push_back(CDecayProduct(EDecayMode::alpha, n));
				else if (_text[i] == 'B')
				{
					if (_text[i + 1] == '+')
						mode.push_back(CDecayProduct(EDecayMode::beta_p, n));
					else if (_text[i + 1] == '-')
						mode.push_back(CDecayProduct(EDecayMode::beta_m, n));
				}
				else if (_text[i] == 'E' && _text[i + 1] == 'C' && _text[i + 2] != '+')
				{
					mode.push_back(CDecayProduct(EDecayMode::EC, n));
				}
				else if (_text[i] == 'P')
				{
					mode.push_back(CDecayProduct(EDecayMode::p, n));
				}
				else if (_text[i] == 'N')
				{
					mode.push_back(CDecayProduct(EDecayMode::n, n));
				}
				else if (_text[i] == 'S' && _text[i + 1] == 'F')
				{
					mode.push_back(CDecayProduct(EDecayMode::SF, n));
				}

				n = 1;
			}
		}

	}

	CString CIsotope::CDecayMode::Format() const
	{
		CString str;
		for (size_t i = 0; i < mode.size(); i++)
		{
			if (!str.IsEmpty())
				str += _T(", ");

			if (mode[i].n > 1)
				str.AppendFormat(_T("%d"), mode[i].n);

			switch (mode[i].mode)
			{
			case Decay::CIsotope::EDecayMode::alpha:
				str += _T("\x03b1");
				break;
			case Decay::CIsotope::EDecayMode::beta_m:
				str += _T("\x03b2\x2013");
				break;
			case Decay::CIsotope::EDecayMode::beta_p:
				str += _T("\x03b2+");
				break;
			case Decay::CIsotope::EDecayMode::EC:
				str += _T("EC");
				break;
			case Decay::CIsotope::EDecayMode::n:
				str += _T("n");
				break;
			case Decay::CIsotope::EDecayMode::p:
				str += _T("p");
				break;
			case Decay::CIsotope::EDecayMode::SF:
				str += _T("SF");
				break;
			case Decay::CIsotope::EDecayMode::stable:
				str = _T("STABLE");
				break;
			default:
				break;
			}
		}

		return str;
	}

	void CIsotope::CHalfLife::Set(const CString& _valueText, const CString& _unitText, const CString& _valueSecText)
	{
		CString valueText(_valueText);
		CString unitText(_unitText);
		CString valueSecText(_valueSecText);
		valueText.Trim();
		unitText.Trim();
		valueSecText.Trim();

		if (valueText == _T("STABLE"))
		{
			value = 0;
			valueSec = 0;
			text = "";
			unit = HLUnit::s;
			return;
		}
		else if (valueText.IsEmpty())
		{
			value = 0;
			valueSec = 0;
			text = "";
			unit = HLUnit::unknown;
			return;
		}

		if (!StringToDouble(value, valueText))
		{
			value = 0;
			valueSec = 0;
			text = "";
			unit = HLUnit::unknown;
			return;
		}

		if (!StringToDouble(valueSec, valueSecText))
		{
			value = 0;
			valueSec = 0;
			unit = HLUnit::unknown;
			return;
		}


		if (unitText == _T("ys"))
			unit = HLUnit::ys;
		else if (unitText == _T("zs"))
			unit = HLUnit::zs;
		else if (unitText == _T("as"))
			unit = HLUnit::as;
		else if (unitText == _T("fs"))
			unit = HLUnit::fs;
		else if (unitText == _T("ps"))
			unit = HLUnit::ps;
		else if (unitText == _T("ns"))
			unit = HLUnit::ns;
		else if (unitText == _T("us"))
			unit = HLUnit::us;
		else if (unitText == _T("ms"))
			unit = HLUnit::ms;
		else if (unitText == _T("s"))
			unit = HLUnit::s;
		else if (unitText == _T("m"))
			unit = HLUnit::m;
		else if (unitText == _T("h"))
			unit = HLUnit::h;
		else if (unitText == _T("d"))
			unit = HLUnit::d;
		else if (unitText == _T("Y"))
			unit = HLUnit::y;

		if (unitText.Find(_T("eV")) == -1)
		{
			text = valueText;
		}
		else
		{
			// T1/2(s) = (h-bar) * ln2 / Gamma = 4.562*10-22 / Gamma(MeV)
			// T1/2 - half-life
			// h-bar - reduced Plank constant 
			// Gamma - total width
			value = valueSec;
			double exponent = log10(valueSec);
			if (exponent < -21)
			{
				value *= 1e24;
				unit = HLUnit::ys;
			}
			else if (exponent >= -21 && exponent < -18)
			{
				value *= 1e21;
				unit = HLUnit::zs;
			}
			else if (exponent >= -18 && exponent < -15)
			{
				value *= 1e18;
				unit = HLUnit::as;
			}
			else if (exponent >= -15 && exponent < -12)
			{
				value *= 1e15;
				unit = HLUnit::fs;
			}
			else if (exponent >= -12 && exponent < -9)
			{
				value *= 1e12;
				unit = HLUnit::ps;
			}
			else if (exponent >= -9 && exponent < -6)
			{
				value *= 1e9;
				unit = HLUnit::ns;
			}
			else
			{
				unit = HLUnit::s;
			}

			text.Format(_T("%g"), value);
		}

	}

}

