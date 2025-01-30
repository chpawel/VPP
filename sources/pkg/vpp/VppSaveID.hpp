#pragma once
#include <core/GlobalEngine.hpp>
#include <stdexcept>

#include<pkg/dem/Shop.hpp>

class VppSaveID: public GlobalEngine{
	
	struct Dane{
		int id;
		double dt;
		double time;
		string type;
		Vector3r position;
		Matrix3r orientation;
		Vector3r normStress;
		Vector3r shearStress;
		Vector3r force;
	};
	struct DaneIter{
		int b1;
		int b2;
		double time;
	};
		
	struct bodyState{
		Vector3r normStress, shearStress;
		bodyState (){
			normStress = Vector3r(0.0,0.0,0.0);
			shearStress = Vector3r(0.0,0.0,0.0);
		};
	};		
	
	
	virtual void action();
	virtual void getStressForEachBody(vector<bodyState>& bodyStates);
	
	YADE_CLASS_BASE_DOC_ATTRS(VppSaveID,GlobalEngine,"Class for daving data.",
	((double,saveTime,0.2,,"save file iteration"))
	((double,timeStart,0.3,,"time to start"))
	((int,idBody,0,,"id of body to collect data"))
	((string,fileName,"data.txt",,"file name"))
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VppSaveID);

