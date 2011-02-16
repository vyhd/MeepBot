#include "ISAAC.h"
#include <cstring>

ISAAC::ISAAC( uint32_t seed[8] )
{
	/* clear the PRNG context */
	memset( &m_ctx, 0, sizeof(m_ctx) );

	/* copy the seed data into the results array */
	for( unsigned i = 0; i < 8; i++ )
		m_ctx.randrsl[i] = seed[i];

	/* init the context, using randrsl as a seed */
	ISAAC_Init( &m_ctx, int(true) );
}

uint32_t ISAAC::GetValue()
{
	return ISAAC_Rand(&m_ctx);
}
