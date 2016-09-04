#include "header.h"
#include "param.h"

int ParError;
const Param<int> dummy("error", 0,10,1, &ParError);

bool ParamBase::bChangedAny = true;	//1st update
