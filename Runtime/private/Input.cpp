#include "Input.h"

namespace
{
    runtime::Input m_input;
}

runtime::Input& runtime::GetInput()
{
    return m_input;
}
