#include "CommonBase.h"
#include "GlobalFunction.h"
#include "CommonType.h"
namespace Minicat
{
	#define SINGLE_ONE_BIT 0x80
	#define BLOCK_SIZE 512
	#define MOD_SIZE 448
	#define APP_SIZE 64
	#define BITS 8
	// MD5 Chaining Variable 
	#define A 0x67452301UL
	#define B 0xEFCDAB89UL
	#define C 0x98BADCFEUL
	#define D 0x10325476UL

	typedef struct
	{
		char *message;
		uint64 length;
	}STRING;

	const uint X[4][2] = {{0, 1}, {1, 5}, {5, 3}, {0, 7}};
	// Constants for MD5 transform routine.
	const uint S[4][4] = {{ 7, 12, 17, 22 },
	{ 5, 9, 14, 20 },
	{ 4, 11, 16, 23 },
	{ 6, 10, 15, 21 }};
	// F, G, H and I are basic MD5 functions.
	uint F( uint X, uint Y, uint Z )
	{
		return ( X & Y ) | ( ~X & Z );
	}

	uint G( uint X, uint Y, uint Z )
	{
		return ( X & Z ) | ( Y & ~Z );
	}

	uint H( uint X, uint Y, uint Z )
	{
		return X ^ Y ^ Z;
	}

	uint I( uint X, uint Y, uint Z ) 
	{
		return Y ^ ( X | ~Z );
	}
	// rotates x left s bits.
	uint rotate_left( uint x, uint s )
	{
		return ( x << s ) | ( x >> ( 32 - s ) );
	}
	// Pre-processin
	uint count_padding_bits ( uint length )
	{
		//uint div = length * BITS / BLOCK_SIZE;
		uint mod = length * BITS % BLOCK_SIZE;
		uint c_bits;
		if ( mod == 0 )
			c_bits = MOD_SIZE;
		else
			c_bits = ( MOD_SIZE + BLOCK_SIZE - mod ) % BLOCK_SIZE;
		return c_bits / BITS;
	}
	STRING append_padding_bits (const char * argv )
	{
		uint msg_length = strlen ( argv );
		uint bit_length = count_padding_bits ( msg_length );
		uint64 app_length = msg_length * BITS;
		STRING string;
		string.message = (char *)malloc(msg_length + bit_length + APP_SIZE / BITS);
		// Save message
		strncpy ( string.message, argv, msg_length );
		// Pad out to mod 64.
		memset ( string.message + msg_length, 0, bit_length );
		string.message [ msg_length ] = SINGLE_ONE_BIT;
		// Append length (before padding).
		memmove ( string.message + msg_length + bit_length, (char *)&app_length, sizeof( uint64 ) );
		string.length = msg_length + bit_length + sizeof( uint64 );
		return string;
	}


	void _GetMD5(char szMD5[16], const char *szSrc)
	{
		STRING string;
		uint w[16];
		uint chain[4];
		uint state[4];
		uint( *auxi[ 4 ])(uint, uint, uint) = { F, G, H, I };
		int roundIdx;
		int sIdx;
		int wIdx;
		int i;
		int j;
	
		string = append_padding_bits (szSrc);
		// MD5 initialization.
		chain[0] = A;
		chain[1] = B;
		chain[2] = C;
		chain[3] = D;
		for ( j = 0; j < (int)string.length; j += BLOCK_SIZE / BITS)
		{
			memmove ( (char *)w, string.message + j, BLOCK_SIZE / BITS );
			memmove ( state, chain, sizeof(chain) );
			for ( roundIdx = 0; roundIdx < 4; roundIdx++ )
			{
				wIdx = X[ roundIdx ][ 0 ];
				sIdx = 0;
				for ( i = 0; i < 16; i++ )
				{
					// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
					// Rotation is separate from addition to prevent recomputation.
					state[sIdx] = state [ (sIdx + 1) % 4 ] + rotate_left ( state[sIdx] + (*auxi[ roundIdx ])( state[(sIdx+1) % 4], state[(sIdx+2) % 4], state[(sIdx+3) % 4]) + w[ wIdx ] + (uint)floor((1ULL << 32) * fabs(sin((double)(roundIdx * 16 + i + 1 ))) ), S[ roundIdx ][ i % 4 ]);
					sIdx = ( sIdx + 3 ) % 4;
					wIdx = ( wIdx + X[ roundIdx ][ 1 ] ) & 0xF;
				}
			}
			chain[ 0 ] += state[ 0 ];
			chain[ 1 ] += state[ 1 ];
			chain[ 2 ] += state[ 2 ];
			chain[ 3 ] += state[ 3 ];
		}
		memmove ( szMD5 + 0, (char *)&chain[0], sizeof(uint) );
		memmove ( szMD5 + 4, (char *)&chain[1], sizeof(uint) );
		memmove ( szMD5 + 8, (char *)&chain[2], sizeof(uint) );
		memmove ( szMD5 + 12, (char *)&chain[3], sizeof(uint) );
	} 


	void GetMD5(char szMD5[32], const char *szSrc)
	{
		char bufTemp[16];
		_GetMD5(bufTemp, szSrc);
		Hex2Str(szMD5, (const char*)bufTemp, 16);
	}
};