#pragma once
#include<lib/serialization/Serializable.hpp>
#include<lib/multimethods/Indexable.hpp>
#include<core/Dispatcher.hpp>
#include<core/State.hpp>


class VppState: public State{
	public:
	int neighbours[6]={-1,-1,-1,-1,-1,-1};
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(VppState,State,"Vpp of a body (spatial configuration, internal variables).",
		((double,pressureValue,0,,"Value of pressure"))
		((Vector3r,pressureDirection,Vector3r::Zero(),,"Direction of pressure"))
		((Vector3r,pressureForce,Vector3r::Zero(),,"PressureForce"))
		((Vector3r,surfaceVec,Vector3r::Zero(),,"Vector of surface - sum of croos with neighbours"))
		((Vector3r,surfaceVecTmp,Vector3r::Zero(),,"Vector of surface - sum of croos with neighbours"))
		((int,neighbour0,-1,,"id of neighbour 1 from table"))
		((int,neighbour1,-1,,"id of neighbour 2 from table"))
		((int,neighbour2,-1,,"id of neighbour 3 from table"))
		((int,neighbour3,-1,,"id of neighbour 4 from table"))
		((int,neighbour4,-1,,"id of neighbour 5 from table"))
		((int,neighbour5,-1,,"id of neighbour 6 from table"))
		((bool,pressure,false,,"add pressure??")),
 		createIndex();
        );
	REGISTER_CLASS_INDEX(VppState,State);
};
REGISTER_SERIALIZABLE(VppState);

