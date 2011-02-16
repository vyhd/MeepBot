#ifndef ISAAC_H
#define ISAAC_H

#include <stdint.h>

// avoids namespace pollution
namespace ISAACDef
{
	#include "ISAAC/rand.h"
}

class ISAAC
{
public:
	ISAAC( uint32_t seed[8] );

	// returns a random value
	uint32_t GetValue();

private:
	ISAACDef::randctx m_ctx;
};

#endif // ISAAC_H
