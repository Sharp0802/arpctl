#include "service/runopt.h"

RunOption::RunOption(___DTO__RunOption_ dto) : _dto(dto)
{
	_global = this;
}

const RunOption& RunOption::Global()
{
	return *_global;
}
