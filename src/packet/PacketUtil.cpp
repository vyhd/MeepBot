#include "packet/PacketUtil.h"
#include "util/StringUtil.h"	// for split()
#include <cstddef>

using namespace std;

/* a packet is suffixed with "\n"; we can use that
 * as a criterion for determining multiple packets. */
bool PacketUtil::NeedsSplit( const string& in )
{
	// if the first "\n" is not at the end, we have multiple packets.
	return in.length() - 1 != in.find_first_of("\n");
}

void PacketUtil::Split( const string& in, vector<string>& add )
{
	// packets are delimited by a newline.
	StringUtil::Split( in, add, '\n' );
}
