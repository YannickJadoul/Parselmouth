#ifndef INC_MELDER_INFO_INTERCEPTOR_H
#define INC_MELDER_INFO_INTERCEPTOR_H

#include <sys/melder.h>
#include "sys/melder.h"
#include "UndefPraatMacros.h"

class MelderInfoInterceptor
{
public:
	MelderInfoInterceptor() : m_string(), m_divertInfo(&m_string) {}
	std::u32string string() { return m_string.string; }
	std::string bytes() { return Melder_peek32to8(m_string.string); }
	std::string get() { return Melder_peek32to8(m_string.string); }

private:
	autoMelderString m_string;
	autoMelderDivertInfo m_divertInfo;
};

#endif // INC_MELDER_INFO_INTERCEPTOR_H
