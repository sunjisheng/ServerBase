#pragma once
#include "TimeClock.h"
namespace Minicat
{
	class CRandom
	{
	public:
		static	void SRand()
		{
			m_holdrand = (unsigned int)CTimeClock::GetTime();
		}

		static int Rand()
		{
			return (((m_holdrand = m_holdrand * 0x015a4e35L + 1L) >> 16) & 0x7fff);
		}

		static int Rand(int nMax)
		{
			if (0 == nMax)
			{
				return 0;
			}
			return Rand() % nMax;
		}

		static unsigned int m_holdrand;
	};
}