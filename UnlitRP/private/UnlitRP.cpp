#include "UnlitRP.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}


void rendering::unlit_rp::Boot()
{
}


#undef THROW_ERROR