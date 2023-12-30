#include "Runtime.h"

#include "TickUpdater.h"

void runtime::Boot()
{
	TickUpdaterTypeDef::GetTypeDef();
}
