// Copyright (c) 2022 SS

#pragma once
#include <list>
#include "RandomEngine.h"
#include "AtomSet.h"
#include "IsotopeDB.h"
#include "ObserverList.h"

namespace Decay
{

class CDecayObserver
{
public:
	virtual void UpdateAtomState(const CAtom& atom) { }
	virtual void IsotopeChanged() { }
	virtual void DecayReset() { }
	virtual void DecayStarting() { }
	virtual void PassComplete() { }
	virtual void DecayStopped() { }
};

enum class EAlgorithm
{
	CoinFlip,
	Approximation
};

enum class EMode
{
	HalfLife,
	DecayConstant
};

enum class EState
{
	Initial,
	Running,
	Stopped
};

class CModel
{
public:
	CModel(EAlgorithm algorithm = EAlgorithm::CoinFlip);
	~CModel() {}

public:
	void AddObserver(CDecayObserver& observer);
	void RemoveObserver(CDecayObserver& observer);

public:
	void Reset();
	void Start();
	void Stop();

protected:
	void Initialize();

	bool Pass();
	bool DoCoinFlipPass();
	bool DoApproximationPass();
	bool AdvanceTime();
	bool Step(CAtom& decayAtom);

public:
	static constexpr double kMinHalfLife = 1.;
	static constexpr double kMaxHalfLife = 10.;

	static constexpr double kMinTime = 1.;
	static constexpr double kMaxTime = 49. * 24. * 3600.;
	static constexpr double kMinTimeResolution = 0.01;

	static constexpr double kMinPracticalTime = 1.;
	static constexpr double kMaxPracticalTime = 10.;

public:
	CIsotope* Isotope() { return m_isot; }
	void SetIsotope(CIsotope* isot);

	EMode Mode() const { return m_mode; }
	void SetMode(EMode mode);

	CAtomSet& Atoms() { return m_atoms; }

	UINT TotalAtoms() const { return m_totalAtoms; }
	UINT RemainingAtoms() const { return m_remainingAtoms; }
	UINT DecayedAtoms() const { return m_totalAtoms - m_remainingAtoms; }

	double Time() const { return m_t; }
	void SetTime(double t) { m_t = t; }

	double TimeLapse() const { return m_tmax; }
	void SetTimeLapse(double t) { m_tmax = t; }

	EState State() const { return m_state; }

	bool IsRunning() const { return m_state == EState::Running; }

protected:
	double round_dt(double dt) const;

protected:
	EAlgorithm m_algorithm{ EAlgorithm::CoinFlip };
	EMode m_mode{ EMode::HalfLife };
	EState m_state{ EState::Initial };

	CIsotope* m_isot{ nullptr };

	CAtomSet m_atoms;
	UINT m_totalAtoms{ 0 };
	UINT m_halfOfAllAtoms{ 0 };
	UINT m_remainingAtoms{ 0 };

	double m_decayConst{ 0 };
	std::uniform_real_distribution<double> m_decayDist;

	double m_p{ 0 };
	double m_t{ 0 };
	double m_dt{ 0 };
	double m_tmax{ 1.0 };

	CObserverList<CDecayObserver> m_observers;
};



}


