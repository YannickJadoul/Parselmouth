#ifndef INC_PRAAT_UTILS_H
#define INC_PRAAT_UTILS_H

#include "sys/Thing.h"
#undef I
#undef trace

class MelderInfoInterceptor
{
public:
	MelderInfoInterceptor() : m_string(), m_divertInfo(&m_string) {}
	std::string get() { return std::string(Melder_peek32to8(m_string.string)); }

private:
	autoMelderString m_string;
	autoMelderDivertInfo m_divertInfo;
};

#endif // INC_PRAAT_UTILS_H
