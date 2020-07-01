#include "strings.h"

std::vector<ULONG> get_symbol_positions(const CString &string, char symbol)
{
	std::vector<ULONG> to_return;

	for (ULONG i = 0; i < string.Length(); i++)
	{
		if (string.GetAt(i) == symbol)
		{
			to_return.push_back(i);
		}
	}

	return to_return;
}