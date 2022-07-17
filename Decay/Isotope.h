// Copyright (c) 2022 SS

#pragma once
#include <vector>
#include <map>

namespace Decay
{
	class CIsotopeDB;

	class CIsotope
	{
	public:
		enum class HLUnit
		{
			ys,      // yoctoseconds
			zs,      // zeptoseconds
			as,      // attoseconds
			fs,      // femtoseconds
			ps,      // picoseconds
			ns,      // microseconds
			us,      // microseconds
			ms,      // milliseconds
			s,       // seconds
			m,       // minutes
			h,       // hours
			d,       // days
			y,       // years
			unknown,
		};

		enum class EDecayMode
		{
			stable,
			alpha,   // alpha
			beta_m,  // beta-
			beta_p,  // beta+
			EC,      // electron capture
			p,       // proton emission
			n,       // neutron emission
			SF,      // spontaneous fission
		};

		struct CAtomicNumber
		{
			void Set(UINT _N)
			{
				N = _N;
			}

			operator UINT() const { return N; }

			UINT N{ 0 };
		};

		struct CAtomicMass
		{
			void Set(const CString& _text);

			double mass{ 0 };
			CString text;
		};

		struct CDecayProduct
		{
			CDecayProduct(EDecayMode _mode, UINT _n = 1) : mode(_mode), n(_n) { }
			EDecayMode mode;
			UINT n;
		};

		struct CDecayMode
		{
			void Set(const CString& _text, const CString& _halfLifeText);

			CString Format() const;

			std::vector<CDecayProduct> mode;
		};

		struct CHalfLife
		{
			void Set(const CString& _valueText, const CString& _unitText, const CString& _valueSecText);

			double value{ 0 };
			double valueSec{ 0 };
			HLUnit unit{ HLUnit::s };
			CString text;
		};

		CIsotope();

		CIsotope(std::map<CString, CString>& data);

		const CAtomicNumber& Z() const { return m_Z; }
		const CAtomicNumber& A() const { return m_A; }
		const CString& Symbol() const { return m_symbol; }
		const CDecayMode& DecayMode() const { return m_decayMode; }
		const CAtomicMass& Mass() const { return m_mass; }
		const CHalfLife& HalfLife() const { return m_halfLife; }
		const CString& D() const { return m_D; }

		double DecayConstant() const { return log(2.0) / m_halfLife.valueSec; }
		double Lifetime() const { return m_halfLife.valueSec / log(2.0); }
		
		bool IsStable() const { return m_decayMode.mode.empty() || m_decayMode.mode[0].mode == EDecayMode::stable; }

		double Q(CIsotopeDB& isotDB) const;
		CIsotope* Product(CIsotopeDB& isotDB) const;
	private:
		CAtomicNumber m_Z;
		CAtomicNumber m_A;
		CString m_symbol;
		CAtomicMass m_mass;
		CDecayMode m_decayMode;
		CHalfLife m_halfLife;
		CString m_D;

	};
}