#include "VppSave.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/vpp/VppState.hpp>
#include<pkg/vpp/VppCohesiveFrictionalContactLaw.hpp>
#include<pkg/vpp/VppState.hpp>
#include <vector>

#include<fstream>
	
				
void VppSave::action(){
	//
	cout.precision(15); // set cout precision for calss

	static vector<Dane> ListaDaneEnd; // tablica trzymajace dane koniec
	static vector<Dane> ListaDaneSave; // vector do zapisu 
	static vector<Dane> ListaDaneSr;
	static vector<DaneIter> ListaDaneIter;
	static int iterSrTmp;
	static int iterSaveTmp;
	static int fileNumber; // indeks pliku - kazdy pomiar do innego pliku
	static double timeFirst; // czas pierwszego zapisu sredniej
	static double calculationTime; // czas liczenia
	double dTime; // roznica czasow zapisu
	
	static double lastTime; // czas startu zapisu
	static bool calculate = false;

	//save all zapis
	static int saveAll; // if save all is on
	static double timeFirstAll=0; //first time of saveAll
	static vector<Dane> ListaDaneAll; // to save all 
	
	//file declaration	
	ofstream outfile;


	if(scene->time > timeStartAll and timeFirstAll==0 and saveAll==0 and engineOn) {
			timeFirstAll =  scene->time;
			saveAll = 1;
			cout << "**Startuje zapis SaveAllBodies**"<<endl;
	}

	if((scene->time - lastTime)>deltaTime or timeFirst== 0){
		calculate = true;
	}
	

	
	if((scene->time > timeStart) and calculate and engineOn){
		//nr_zapisu
		static int idSave;
		//cout << "wlaczam zapis"<< scene->time<<endl;
		//cout << "do zapisu " << iterSave - iterSaveTmp << endl;
		
	
		static bool header=true;
		if(header){
			outfile.open(fileName, ios_base::app);
			outfile<<	info << endl;
			outfile<<	 "id;type;time;position0;position1;position2;orient00;orient01;orient02;orient10;orient11;orient12;orient20;orient21;orient22;normStress0;normStress1;normStress2;shearStress0;shearStress1;shearStress2;force0;force1;force2"<<endl;
			outfile.close();
			header=false;
			cout << "Startuje zapis do pliku."<< endl;
			cout << timeStart <<" timeStart" << endl;
			cout << deltaTime <<" deltaTime" << endl;
			cout << timeSr <<" timeSr" << endl;
			cout << iterSave <<" iterSave" << endl;
			
		}
	
		Dane tmp;
	
		//licze tensorynaprezenia wszystkich cial
		vector<bodyState> bState;
		getStressForEachBody(bState);
	
		
		
		// steps 0 - pierwszy wpis; 1 - kolejny
		if(iterSrTmp == 0){	
			 ListaDaneEnd.resize(scene->bodies->size());
			 ListaDaneSr.resize(scene->bodies->size());
			 
		// zapisuje dane z cial pierwszy pomiar 
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if(State* st = dynamic_cast<State*>(b->state.get()))
				if(b->groupMask!=0) {
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
					timeFirst =  scene->time;
					
					
					ListaDaneEnd[b->id] = tmp;
					//przypisanie stalych elementow
					ListaDaneSr[b->id].type = tmp.type;
					ListaDaneSr[b->id].dt=  tmp.dt;
					ListaDaneSr[b->id].time= tmp.time;
					ListaDaneSr[b->id].id = tmp.id;
					ListaDaneSr[b->id].timeStartAvarage = timeFirst;
					ListaDaneSr[b->id].work=st->work;
					ListaDaneSr[b->id].dEnergyYade=st->dEnergyYade;
					ListaDaneSr[b->id].dEnergyVel=st->dEnergyVel;
					ListaDaneSr[b->id].dEnergyAngVel=st->dEnergyAngVel;
					ListaDaneSr[b->id].dEnergyFoil=st->dEnergyFoil;
					
				};
			};
			iterSrTmp++;
			cout << "Pierwszy zapis po cialach" << endl;
		}else{//koniec pieierwszego zapisu
			calculationTime =  scene->time; // zapamietanie czasu zapisu - w wyniu bedzie ostatni czas 			
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
				if(State* st = dynamic_cast<State*>(b->state.get()))
				if(b->groupMask!=0) {
					
					//buduje nowy obiekt tmp aby latwiej przypisac srednie i pozniej dopisac jako ostani lelement
					tmp.type = b->material->label;
					tmp.dt = scene->dt;
					tmp.time = scene->time;
					tmp.id = b->id;
					tmp.position = st->pos;
					tmp.orientation = st->ori;
					tmp.normStress = bState[b->id].normStress;
					tmp.shearStress = bState[b->id].shearStress;
					tmp.force = scene->forces.getForce(b->id);
		
					//roznica czasow
					dTime = scene->time - ListaDaneEnd[b->id].time;
					//suma do sredniej 
					ListaDaneSr[b->id].position += (ListaDaneEnd[b->id].position + tmp.position)/2.  * dTime;	
					ListaDaneSr[b->id].orientation += (ListaDaneEnd[b->id].orientation + tmp.orientation)/2.  * dTime;		
					ListaDaneSr[b->id].normStress += (ListaDaneEnd[b->id].normStress + tmp.normStress)/2.  * dTime;
					ListaDaneSr[b->id].shearStress += (ListaDaneEnd[b->id].shearStress + tmp.shearStress)/2.  * dTime;
					ListaDaneSr[b->id].force += (ListaDaneEnd[b->id].force + tmp.force)/2.  * dTime;
					
					
					//if(b->id == 1000){
					//	cout << dTime << " Pozycja  "<< b->id <<"  " << ListaDaneSr[b->id].position << endl;
					//	cout << " ListaDaneEnd[b->id].time " << ListaDaneEnd[b->id].time << endl;
					//}
				
					//nadpisanie jako ostatni element
					ListaDaneEnd[b->id] = tmp;
					
					
				}

			}
				
		}//koniec kolejnych zapisow
			

		
		//calculate avarege of all elements
		 
		if((scene->time-timeFirst) > timeSr){
			
			for(int i=0; i<ListaDaneSr.size();i++){
				dTime = ListaDaneEnd[i].time - timeFirst;
				ListaDaneSr[i].time = (calculationTime + timeFirst)/2.;
				ListaDaneSr[i].position /= dTime;
				ListaDaneSr[i].orientation /= dTime;
				ListaDaneSr[i].normStress /= dTime;
				ListaDaneSr[i].shearStress /= dTime;
				ListaDaneSr[i].force /= dTime;
				

				//zapisanie ostatniego czasu z lliczenia sredniej
				ListaDaneSr[i].timeEndAvarage = scene->time;
				
				
				//add to save list
				ListaDaneSave.push_back(ListaDaneSr[i]);
				
			}
			
		
			
			//avarage
//			for(int i=0; i<ListaDaneSr.size();i++){
//				ListaDaneSr[i].time = ListaDaneSr[i].time ;//  ListaDaneSr[i].dt;
//				ListaDaneSr[i].position = ListaDaneSr[i].position / ListaDaneSr[i].dt;
//				ListaDaneSr[i].orientation = ListaDaneSr[i].orientation / ListaDaneSr[i].dt;
//				ListaDaneSr[i].normStress = ListaDaneSr[i].normStress / ListaDaneSr[i].dt;
//				ListaDaneSr[i].shearStress = ListaDaneSr[i].shearStress / ListaDaneSr[i].dt;
//				ListaDaneSr[i].force = ListaDaneSr[i].force / ListaDaneSr[i].dt;
//			
//				//add to save list
//				ListaDaneSave.push_back(ListaDaneSr[i]);
//			}
		/*
			DaneIter tmpIter;
			FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
				if(I->isReal()){
					const shared_ptr<Body>& b1 = Body::byId(I->getId1(),scene);
					const shared_ptr<Body>& b2 = Body::byId(I->getId2(),scene);
					
					//pobranie geometrii oraz phys
					const FrictPhys* physFP = YADE_CAST<FrictPhys*>(I->phys.get());
					ScGeom* geomScG=YADE_CAST<ScGeom*>(I->geom.get());
				
					tmpIter.b1=b1->id;
					tmpIter.b2=b2->id;
					tmpIter.time = ListaDaneSr[b1->id].time;
					ListaDaneIter.push_back(tmpIter);
					
					//cout << b1->id << " " << b2->id<< " " << physFP->normalForce << endl; 
					
				}
			}
	*/		
			//clear ListaDaneSr becouse all avarage data are in table to save
			ListaDaneSr.clear();
			iterSrTmp = 0;
			calculate = false; // wylaczenie liczenia sredniej
			lastTime = calculationTime-timeSr/2.; //ustawienie ostatniego czasu sredniej jako ostatni czas z przesunieciem o polowe czasu ze sredniej
			iterSaveTmp++;
			//cout << "Licze srednia"<< endl;
			cout << "Zapis po cialach " << scene->time << "Do zapisu pozostalo "<< iterSave - iterSaveTmp << endl;
		}
	
	

	
		if(iterSaveTmp>iterSave){
			if(ListaDaneSave.size()>2){
				string fileNameTmp = fileName+std::to_string(fileNumber);
				outfile.open(fileNameTmp, ios_base::app);
				//outfile.precision(15); //dodane do cout 
				string save="";
				for(int i=0; i<ListaDaneSave.size();i++){
			
					outfile << std::setprecision(15) 
					<< ListaDaneSave[i].id << ";" //1
					<< ListaDaneSave[i].type <<";" //2
					<< ListaDaneSave[i].time <<";" //3
					<< ListaDaneSave[i].position[0]<< ";" //4
					<< ListaDaneSave[i].position[1]<<";" //5
					<< ListaDaneSave[i].position[2]<<";" //6
					<< ListaDaneSave[i].orientation(0,0)<<";" //7
					<< ListaDaneSave[i].orientation(0,1)<<";" //8
					<< ListaDaneSave[i].orientation(0,2)<<";" //9
					<< ListaDaneSave[i].orientation(1,0)<<";" //10
					<< ListaDaneSave[i].orientation(1,1)<<";" //11
					<< ListaDaneSave[i].orientation(1,2)<<";" //12
					<< ListaDaneSave[i].orientation(2,0)<<";" //13
					<< ListaDaneSave[i].orientation(2,1)<<";" //14
					<< ListaDaneSave[i].orientation(2,2)<<";" //15
					<< ListaDaneSave[i].normStress[0]<<";" //16
					<< ListaDaneSave[i].normStress[1]<<";" //17
					<< ListaDaneSave[i].normStress[2]<<";" //18
					<< ListaDaneSave[i].shearStress[0]<<";" //19
					<< ListaDaneSave[i].shearStress[1]<<";" //20
					<< ListaDaneSave[i].shearStress[2]<<";" //21
					<< ListaDaneSave[i].force[0]<<";" //22
					<< ListaDaneSave[i].force[1]<<";" //23
					<< ListaDaneSave[i].force[2]<<";" //24
					<< ListaDaneSave[i].work<<";" //25
					<< ListaDaneSave[i].dEnergyYade<<";"    //26
					<< ListaDaneSave[i].dEnergyVel<<";"     //27
					<< ListaDaneSave[i].dEnergyAngVel<<";"	//28	
					<< ListaDaneSave[i].dEnergyFoil<<";"	//29		
					<< "czasy;"
					<< ListaDaneSave[i].timeStartAvarage<<";"
					<< ListaDaneSave[i].timeEndAvarage<<";"
					<<"\n";

				}
				outfile.close();
			
				ListaDaneSave.clear();
				/*string fileNameIter = fileName+"iter"+std::to_string(fileNumber);
		
				//save interactions
				outfile.open(fileNameIter, ios_base::app);
				for(int i=0; i<ListaDaneIter.size();i++){
					outfile << ListaDaneIter[i].time << ";"
						<< ListaDaneIter[i].b1 << ";"
						<<  ListaDaneIter[i].b2 << ";"
						<<"\n";
				}
				ListaDaneIter.clear();
				outfile.close();
				*/
				iterSaveTmp = 0;
				
				fileNumber++; //zwiekszam numer pliku 

				
					
			}
		}
	}//end save by body engine on


	//saveAll data in every step					
		if(saveAll==1 and (scene->time - timeFirstAll) < deltaTimeAll){
			Dane tmpAll;
 			
			//licze tensorynaprezenia wszystkich cial
			vector<bodyState> bState;
			getStressForEachBody(bState);

			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if(State* st = dynamic_cast<State*>(b->state.get()))
				if(b->groupMask!=0 and  (b->id >= idMinAll and b->id<=idMaxAll)) {
					tmpAll.type = b->material->label;
					tmpAll.dt = scene->dt;
					tmpAll.time = scene->time;
					tmpAll.id = b->id;
					tmpAll.position = st->pos;
					tmpAll.orientation = st->ori;
					tmpAll.normStress = bState[b->id].normStress;
					tmpAll.shearStress = bState[b->id].shearStress;
					tmpAll.force = scene->forces.getForce(b->id);
					
					ListaDaneAll.push_back(tmpAll);
										
					
				};
			}//end foreach body
		}//end save all
		


		if (saveAll==1 and (scene->time - timeFirstAll) > deltaTimeAll){
			string fileNameTmp = fileName+"_All";
			outfile.open(fileNameTmp, ios_base::app);
			//outfile.precision(15); dopisane do cout
			for(int i=0; i<ListaDaneAll.size();i++){
			outfile << std::setprecision(15) << ListaDaneAll[i].id << ";"
			<< ListaDaneAll[i].type <<";"
			<< ListaDaneAll[i].time <<";"
			<< ListaDaneAll[i].position[0]<< ";"
			<< ListaDaneAll[i].position[1]<<";"
			<< ListaDaneAll[i].position[2]<<";"
			<< ListaDaneAll[i].orientation(0,0)<<";"
			<< ListaDaneAll[i].orientation(0,1)<<";"
			<< ListaDaneAll[i].orientation(0,2)<<";"
			<< ListaDaneAll[i].orientation(1,0)<<";"
			<< ListaDaneAll[i].orientation(1,1)<<";"
			<< ListaDaneAll[i].orientation(1,2)<<";"
			<< ListaDaneAll[i].orientation(2,0)<<";"
			<< ListaDaneAll[i].orientation(2,1)<<";"
			<< ListaDaneAll[i].orientation(2,2)<<";"
			<< ListaDaneAll[i].normStress[0]<<";"
			<< ListaDaneAll[i].normStress[1]<<";"
			<< ListaDaneAll[i].normStress[2]<<";"
			<< ListaDaneAll[i].shearStress[0]<<";"
			<< ListaDaneAll[i].shearStress[1]<<";"
			<< ListaDaneAll[i].shearStress[2]<<";"
			<< ListaDaneAll[i].force[0]<<";"
			<< ListaDaneAll[i].force[1]<<";"
			<< ListaDaneAll[i].force[2]<<";"
			<<"\n";

		}
			outfile.close();
			saveAll=2;
		}//end saveAll
		
		
};
////////////////////////////////////////////////////
void VppSave::getStressForEachBody(vector<bodyState>& bodyStates){
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



YADE_PLUGIN((VppSave));
CREATE_LOGGER(VppSave);

