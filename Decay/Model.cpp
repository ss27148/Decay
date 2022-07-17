// Copyright (c) 2022 SS

#include "StdAfx.h"
#include "Model.h"
#include "PhysConst.h"
#include "NumString.h"
#include "mathx.h"
#include "Resource.h"
#include <exception>

namespace Decay
{


CModel::CModel(EAlgorithm algorithm) :
	m_algorithm(algorithm),
	m_decayDist(0.0, 1.0)
{
	Initialize();
}

void CModel::AddObserver(CDecayObserver& observer)
{
	if (m_observers.Find(&observer) < 0)
		m_observers.AddObserver(&observer);
}

void CModel::RemoveObserver(CDecayObserver& observer)
{
	m_observers.RemoveObserver(&observer);
}

void CModel::Initialize()
{
	constexpr UINT rows = 100;
	constexpr UINT cols = 100;

	m_atoms.Initialize(rows, cols);

	m_totalAtoms = rows * cols;
	m_halfOfAllAtoms = m_totalAtoms / 2;

	Reset();
}

void CModel::Reset()
{
	double halfLife = m_isot ? m_isot->HalfLife().valueSec : 5.0;

	m_decayConst = log(2.0) / halfLife;

	if (m_algorithm == EAlgorithm::CoinFlip)
	{
		double lifetime = halfLife / log(2.0);
		m_dt = round_dt(lifetime / 100.0);
		m_dt = (std::min)(kMinTimeResolution, m_dt);

		m_p = m_decayConst * m_dt;
	}
	else
	{
		// We choose a time step such that during this time interval, 
		// on average, no more than 1 atom would decay.
		m_dt = round_dt(1.0 / (m_totalAtoms * m_decayConst));
		m_dt = (std::min)(kMinTimeResolution, m_dt);

		m_p = m_decayConst * m_dt * m_totalAtoms;
	}
	
	m_t = 0.0;

	m_remainingAtoms = m_totalAtoms;

	m_atoms.Reset();

	m_state = EState::Initial;

	FOR_EACH_OBSERVER(CDecayObserver, m_observers, DecayReset());
}

void CModel::Start()
{
	switch (m_state)
	{
	case EState::Running:
		return;
	case EState::Stopped:
		Reset();
	default:
		break;
	}

	ASSERT(m_state == EState::Initial);

	m_state = EState::Running;

	ULONGLONG startTime = GetTickCount64();

	FOR_EACH_OBSERVER(CDecayObserver, m_observers, DecayStarting());

	while (true)
	{
		if (Pass())
			break;

		int timeDelta = static_cast<int>(m_t * 1000.0 - (GetTickCount64() - startTime));

		if (timeDelta > 0)
			Sleep(timeDelta);
	}

	Stop();

	FOR_EACH_OBSERVER(CDecayObserver, m_observers, DecayStopped());

	TRACE(_T("STOPPED\n"));
}

void CModel::Stop()
{
	m_state = EState::Stopped;
}

bool CModel::Pass()
{
	if (m_algorithm == EAlgorithm::CoinFlip)
		DoCoinFlipPass();
	else
		DoApproximationPass();


	FOR_EACH_OBSERVER(CDecayObserver, m_observers, PassComplete());

	if (m_state == EState::Stopped)
	{
		TRACE("COMPLETE: Stop requested by user\n");
		return true;
	}

	return false;
}

bool CModel::DoCoinFlipPass()
{
	for (UINT i = 0; i < m_totalAtoms; ++i)
	{
		if (Step(m_atoms.At(i)))
			return true;
	}

	return AdvanceTime();
}

bool CModel::DoApproximationPass()
{
	const UINT dN = static_cast<UINT>(kMinTimeResolution / m_dt);

	for (UINT i = 0; i < dN; ++i)
	{
		if (Step(m_atoms.RandomAtom()))
			return true;

		if (AdvanceTime())
			return true;
	}

	return false;
}

bool CModel::AdvanceTime()
{
	m_t += m_dt;

	if (m_mode == EMode::DecayConstant)
	{
		if (m_tmax > kMinTimeResolution)
		{
			if (m_t >= m_tmax)
			{
				m_t = m_tmax;
				TRACE("COMPLETE: t = tmax\n");
				Stop();
				return true;
			}
		}
	}

	return false;
}

bool CModel::Step(CAtom& decayAtom)
{
	if (m_decayDist(CRandomEngine::Instance()) <= m_p)
	{
		if (decayAtom.IsActive())
		{
			decayAtom.Flip();
			--m_remainingAtoms;

			FOR_EACH_OBSERVER(CDecayObserver, m_observers, UpdateAtomState(decayAtom));


			if (m_mode == EMode::HalfLife)
			{
				if (m_remainingAtoms == m_halfOfAllAtoms)
				{
					TRACE("COMPLETE: N = N0 / 2\n");
					Stop();
					return true;
				}
			}
			else if (m_mode == EMode::DecayConstant)
			{
				if (m_remainingAtoms == 0)
				{
					TRACE("COMPLETE: N = 0\n");
					Stop();
					return true;
				}
			}
		}
	}

	return false;
}

void CModel::SetIsotope(CIsotope* isot)
{
	if (isot == m_isot)
		return;

	if (isot &&
		isot->HalfLife().valueSec >= kMinTime &&
		isot->HalfLife().valueSec < kMaxTime)
	{
		m_isot = isot;

		FOR_EACH_OBSERVER(CDecayObserver, m_observers, IsotopeChanged());

		Reset();
	}
}

void CModel::SetMode(EMode mode)
{
	if (IsRunning())
		return;

	EMode prevMode = m_mode;
	m_mode = mode;

	if (prevMode != m_mode && m_state != EState::Initial)
		Reset();
}

double CModel::round_dt(double dt) const
{
	double exponent = pow(10.0, floor(log10(dt)));
	return floor(dt / exponent) * exponent;
}

}

