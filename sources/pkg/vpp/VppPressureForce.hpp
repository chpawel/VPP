#pragma once
#include <core/GlobalEngine.hpp>
#include <stdexcept>


class VppPressureForce: public GlobalEngine{
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(VppPressureForce,GlobalEngine,"Class for applying pressure.",
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VppPressureForce);

