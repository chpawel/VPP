#pragma once
#include <core/GlobalEngine.hpp>
#include <stdexcept>
#include <vector>
#include<pkg/dem/Shop.hpp>
#include<pkg/vpp/VppCohesiveFrictionalContactLaw.hpp>

class VppSaveInteractions: public GlobalEngine{
	
	struct DaneInter{
		int id1;
		int id2;
		double time;
		double r1;
		double r2;
		string typeId1;
		string typeId2;
		Vector3r positionId1;
		Matrix3r orientationId1;
		Vector3r positionId2;
		Matrix3r orientationId2;
		Vector3r normForce;
		Vector3r shearForce;
		Real dW;
		Real dWAccum;
		Vector3r normal;
		bool connectionOk;
	};
			
	virtual void action();
	
	void  PrintIter(DaneInter iter);

	Real CountdW(CohFrictPhys* physFP, ScGeom6D* geom, int id=0);
	
	//void  PrintIter2(std::vector<DaneInter> &iter);
	
	YADE_CLASS_BASE_DOC_ATTRS(VppSaveInteractions,GlobalEngine,"Class for daving data.",
	((int,iterSave,50,,"save file iteration"))
	((double,timeSr,0.002,,"time to calculate avarage"))
	((double,timeStart,0.03,,"time to start"))
	((double,timeStartAll,0.03,,"time to start saving all interactions"))
	((double,deltaTimeAll,0.03,,"time how long to save"))
	((double,deltaTime,0.003,,"time between calculate"))
	((string,fileName,"data.txt",,"file name"))
	((bool,engineOn,false,,"default to off"))
	((int,savedFiles,0,,"how many files were saved - it is for exit function"))
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VppSaveInteractions);

