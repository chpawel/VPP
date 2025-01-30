#include "VppSaveID.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/vpp/VppState.hpp>
#include<pkg/vpp/VppCohesiveFrictionalContactLaw.hpp>
#include<pkg/vpp/VppState.hpp>
#include <vector>

#include<fstream>
	
				
void VppSaveID::action(){
	//
	
	static vector<Dane> ListaDaneEnd; // tablica trzymajace dane koniec
	static vector<Dane> ListaDaneSave; // vector do zapisu 
	static vector<Dane> ListaDaneSr;
	vector<DaneIter> ListaDaneIter;
	static int iterSrTmp;
	static int iterSaveTmp;
	static int fileNumber; // indeks pliku - kazdy pomiar do innego pliku
	static double timeFirst; // czas pierwszego zapisu sredniej
	double dTime; // roznica czasow zapisu
	
	static double lastTime; // czas startu zapisu
	static int record;
	
	
	if((scene->time > timeStart) and record==0){
		//nr_zapisu
		static int idSave;
		
		ofstream outfile;
	
		static bool header=true;
		if(header){
			outfile.open(fileName, ios_base::app);
			outfile << "id;type;time;position0;position1;position2;orient00;orient01;orient02;orient10;orient11;orient12;orient20;orient21;orient22;normStress0;normStress1;normStress2;shearStress0;shearStress1;shearStress2;force0;force1;force2"<<endl;
			outfile.close();
			header=false;
			cout << "Startuje zapis do pliku ID. "<< endl;
			
			timeFirst = scene->time;
		}
	
		Dane tmp;
	
		//licze tensorynaprezenia wszystkich cial
		vector<bodyState> bState;
		getStressForEachBody(bState);
	
		
		
		
			 
		// zapisuje dane z cial pierwszy pomiar 
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if(State* st = dynamic_cast<State*>(b->state.get()))
				if(b->groupMask!=0 and b->id == idBody) {
					tmp.type = b->material->label;
					tmp.dt = scene->dt;
					tmp.time = scene->time;
					tmp.id = b->id;
					tmp.position = st->pos;
					tmp.orientation = st->ori;
					tmp.normStress = bState[b->id].normStress;
					tmp.shearStress = bState[b->id].shearStress;
					tmp.force = scene->forces.getForce(b->id);
					//zapis pierwszego czasu
					
					
					ListaDaneSave.push_back(tmp);
					
				};
			};
			
		
	
		cout << scene->time-timeFirst<< endl;

	
		if(scene->time-timeFirst>saveTime){
			string fileNameTmp = fileName+std::to_string(fileNumber);
			outfile.open(fileNameTmp, ios_base::app);
			string save="";
			for(int i=0; i<ListaDaneSave.size();i++){
			
				outfile << ListaDaneSave[i].id << ";"
				<< ListaDaneSave[i].type <<";"
				<< ListaDaneSave[i].time <<";"
				<< ListaDaneSave[i].position[0]<< ";"
				<< ListaDaneSave[i].position[1]<<";"
				<< ListaDaneSave[i].position[2]<<";"
				<< ListaDaneSave[i].orientation(0,0)<<";"
				<< ListaDaneSave[i].orientation(0,1)<<";"
				<< ListaDaneSave[i].orientation(0,2)<<";"
				<< ListaDaneSave[i].orientation(1,0)<<";"
				<< ListaDaneSave[i].orientation(1,1)<<";"
				<< ListaDaneSave[i].orientation(1,2)<<";"
				<< ListaDaneSave[i].orientation(2,0)<<";"
				<< ListaDaneSave[i].orientation(2,1)<<";"
				<< ListaDaneSave[i].orientation(2,2)<<";"
				<< ListaDaneSave[i].normStress[0]<<";"
				<< ListaDaneSave[i].normStress[1]<<";"
				<< ListaDaneSave[i].normStress[2]<<";"
				<< ListaDaneSave[i].shearStress[0]<<";"
				<< ListaDaneSave[i].shearStress[1]<<";"
				<< ListaDaneSave[i].shearStress[2]<<";"
				<< ListaDaneSave[i].force[0]<<";"
				<< ListaDaneSave[i].force[1]<<";"
				<< ListaDaneSave[i].force[2]<<";"
				<<"\n";

			}
			outfile.close();
			
			ListaDaneSave.clear();
			
			record = 1;
			
			cout<< "koniec zapisu ID" << endl;
			
		}
	
		}//end timeStrat
		
};
////////////////////////////////////////////////////
void VppSaveID::getStressForEachBody(vector<bodyState>& bodyStates){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	bodyStates.resize(scene->bodies->size());
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		Vector3r normalStress,shearStress;
		if(!I->isReal()) continue;
		
		const FrictPhys* physFP = YADE_CAST<FrictPhys*>(I->phys.get());
		ScGeom* geomScG=YADE_CAST<ScGeom*>(I->geom.get());
		
		const Body::id_t id1=I->getId1(), id2=I->getId2();
		
		if((physFP) and (geomScG)){
			Real minRad=(geomScG->radius1<=0?geomScG->radius2:(geomScG->radius2<=0?geomScG->radius1:min(geomScG->radius1,geomScG->radius2)));
			Real crossSection=Mathr::PI*pow(minRad,2);
				
			normalStress=((1./crossSection)*geomScG->normal.dot(physFP->normalForce))*geomScG->normal;
			for(int i=0; i<3; i++){
				int ix1=(i+1)%3,ix2=(i+2)%3;
				shearStress[i]=geomScG->normal[ix1]*physFP->shearForce[ix1]+geomScG->normal[ix2]*physFP->shearForce[ix2];
				shearStress[i]/=crossSection;
			}
			bodyStates[id1].normStress+=normalStress;
			bodyStates[id2].normStress+=normalStress;
			bodyStates[id1].shearStress+=shearStress;
			bodyStates[id2].shearStress+=shearStress;
		}
	}
}



YADE_PLUGIN((VppSaveID));
CREATE_LOGGER(VppSaveID);

