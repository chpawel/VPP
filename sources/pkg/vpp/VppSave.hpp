#pragma once
#include <core/GlobalEngine.hpp>
#include <stdexcept>

#include<pkg/dem/Shop.hpp>

class VppSave: public GlobalEngine{
	
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
		double timeStartAvarage;
		double timeEndAvarage;
		double work;	
		double dEnergyVel;
		double dEnergyAngVel;
		double dEnergyYade;
		double dEnergyFoil;
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
	
	YADE_CLASS_BASE_DOC_ATTRS(VppSave,GlobalEngine,"Class for daving data.",
	((int,iterSave,50,,"save file iteration"))
	((double,timeSr,0.002,,"time to calculate avarage"))
	((double,timeStart,0.03,,"time to start"))
	((double,deltaTime,0.003,,"time between calculate"))
	((double,timeStartAll,0.03,,"time to start saving all interactions"))
	((double,deltaTimeAll,0.03,,"time how long to save"))
	((int,idMinAll,0,,"minimal ID to save all"))
	((int,idMaxAll,1,,"maximal ID to save all"))
	((string,fileName,"data.txt",,"file name"))
	((string,info,"",,"info from python about scenerio"))
	((bool,engineOn,false,,"default to off"))
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VppSave);

