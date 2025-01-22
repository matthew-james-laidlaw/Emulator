#include "Processor.h"

int main()
{
	std::array<u8, 65536> memory;
	Processor processor(memory);
	return 0;
}
