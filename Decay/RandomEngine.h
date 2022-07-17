// Copyright (c) 2022 SS

#pragma once
#include <random>

class CRandomEngine : public std::mt19937
{
public:
	CRandomEngine() : std::mt19937(std::random_device{}())
	{
	}

	static CRandomEngine& Instance() { static CRandomEngine instance; return instance; }
};