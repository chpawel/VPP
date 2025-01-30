#pragma once
#include <core/GlobalEngine.hpp>
#include <stdexcept>


class VppNeighbours: public GlobalEngine{
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(VppNeighbours,GlobalEngine,"Class for applying user-defined control on particles.",
	((double,timeStart,0,,"Time when function starts"))
	((bool,reset,false,,"Reset function - again find neighbours"))		
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VppNeighbours);

