#include "VppSaveInteractions.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/vpp/VppState.hpp>
#include<pkg/vpp/VppState.hpp>
#include <vector>

#include<fstream>
	
				
void VppSaveInteractions::action(){
	//
	
	static vector<DaneInter> ListaDaneEnd; // tablica trzymajace dane koniec INterakcje
	static vector<DaneInter> ListaDaneSave; // vector do zapisu 
	static vector<DaneInter> ListaDaneSr;
	static vector<DaneInter> ListaDaneAll; // to save all interactions
	static int fileNumber; // indeks pliku - kazdy pomiar do innego pliku
	static double timeFirst; // czas pierwszego zapisu sredniej
	double dTime; // roznica czasow zapisu
	static int iterSrTmp;
	static int iterSaveTmp;
	static double lastTime; // czas startu zapisu sluzacy do obliczen zmiany
	static double calculationTime; // czas liczenia
	static bool calculate = false; // save is on
	
	static int saveAll; // if save all is on
	static double timeFirstAll=0; //first time of saveAll

	//start zapisu po okreslonym czasie
	if((scene->time - lastTime)>deltaTime or timeFirst== 0){
		calculate = true;
		//cout << "warunek czasu spelniony" <<endl;
	}
	
	
	if((scene->time > timeStart) and calculate and engineOn){
		//nr_zapisu
		static int idSave;
		//cout << "zapisuje"<<endl;
		//cout << "wlaczam zapis"<< scene->time<<endl;
		
		ofstream outfile;
	
	
		if(scene->time > timeStartAll and timeFirstAll==0 and saveAll==0) 
			saveAll = 1;
		
		
		
	
		static bool header=true;
		if(header){
			//cout << "zapisuje naglowek interakcje"<<endl;
			outfile.open(fileName, ios_base::app);
			outfile << "ListaDaneSave[i].id1;id2;time;typeId1;typeId2;positionId1[0];positionId1[1];positionId1[2];orientationId1(0,0);orientationId1(0,1);orientationId1(0,2);orientationId1(1,0);orientationId1(1,1);orientationId1(1,2);orientationId1(2,0);orientationId1(2,1);orientationId1(2,2);positionId2[0];positionId2[1];;orientationId2(0,0);orientationId2(0,1);orientationId2(0,2);orientationId2(1,0);orientationId2(1,1);orientationId2(1,2);orientationId2(2,0);orientationId2(2,1);orientationId2(2,2);normForce[0];normForce[1];normForce[2];shearForce[0];shearForce[1];shearForce[2];normal[0];normal[1];normal[2];r1;r2;shearForceEquation[0];shearForceEquation[1];shearForceEquation[2]";
			outfile.close();
			header=false;
			//cout << "Startuje zapis do pliku."<<ListaDaneEnd.size() <<endl;
		}
	
	
				DaneInter interTmp;
		
		
		// steps 0 - pierwszy wpis; 1 - kolejny
			if(scene->interactions->size() > 0) // sprawdzenie czy sa interakcje w analizie i dopiero pozniej uruchomienie zapisywania			
				if(iterSrTmp == 0 ){	
				 ListaDaneEnd.reserve(scene->interactions->size());
				 ListaDaneSr.resize(scene->interactions->size());
				//cout << "Rezerwuje pierwszy zapis" << endl; 
			// zapisuje dane z cial pierwszy pomiar 
				//zapis pierwszego czasu
				timeFirst =  scene->time;
				//to samo tylko interakcje
				FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
					if(I->isReal()){
						const shared_ptr<Body>& b1 = Body::byId(I->getId1(),scene);
						const shared_ptr<Body>& b2 = Body::byId(I->getId2(),scene);
					
						//cout << "PIERWSZY ZAPIS "<< scene->interactions->size() << "czas startu" << timeFirst << endl;
					
						State* st1 = static_cast<State*>(b1->state.get());
						State* st2 = static_cast<State*>(b2->state.get());
					
						ScGeom6D* geom  = YADE_CAST<ScGeom6D*> (I->geom.get());
						ScGeom* geomScG=YADE_CAST<ScGeom*>(I->geom.get());
						CohFrictPhys* physFP = YADE_CAST<CohFrictPhys*>(I->phys.get());

						//get radius of shperes;
						Real r1 = (static_cast<Sphere*>(b1->shape.get()))->radius;	
						Real r2 = (static_cast<Sphere*>(b2->shape.get()))->radius;

						interTmp.id1=b1->id;
						interTmp.id2=b2->id;
						interTmp.r1=r1;
						interTmp.r2=r2;
						interTmp.normForce=physFP->normalForce;
						interTmp.shearForce=physFP->shearForce;
						interTmp.normal=geomScG->normal;
						interTmp.time = scene->time;
						interTmp.dW = I->work;
						
						interTmp.connectionOk = true; //ustawinie w pierwszym miejscu ze polaczenie jest prawdziwe, wystarczy jedno zeranie i nie bedziemy brali go pod uwage
					
					
						interTmp.typeId1 = b1->material->label;
						interTmp.typeId2 = b2->material->label;
						interTmp.time = scene->time;
						interTmp.positionId1 = st1->pos;
					 	interTmp.orientationId1 = st1->ori;
					 	interTmp.positionId2 = st2->pos;
					 	interTmp.orientationId2 = st2->ori;
					
					
					
						ListaDaneEnd.push_back(interTmp);
						//cout << b1->id << " " << b2->id<< " " << interTmp.shearForceEquation << endl; 
					
						if(saveAll==1){
							timeFirstAll =  scene->time;
							ListaDaneAll.push_back(interTmp);
						}
					
					}
				}
			
				//PrintIter(ListaDaneEnd[0]);
				//cout << "Pierwszy zapisssss " <<ListaDaneEnd.size() << endl;
				iterSrTmp=1;
			
			}else if(ListaDaneEnd.size()>0){//koniec pierwszego zapisu
				calculationTime =  scene->time;
	
			//cout << "kOLEJNE ZAPISYsss "<< ListaDaneEnd.size() << endl;
			
			for(int i=0;i<ListaDaneEnd.size();i++){
			
				//PrintIter(ListaDaneEnd[i]);
				//cout << endl << endl << ListaDaneEnd[i].id1<< endl << ListaDaneEnd[i].id2 << endl << endl;
			
				const shared_ptr<Interaction>& Int = scene->interactions->find(ListaDaneEnd[i].id1,ListaDaneEnd[i].id2);
			
			
			
				if(Int and Int->isReal() and ListaDaneEnd[i].connectionOk) // 
				{ //cout << "kOLEJNE ZAPISY INTERAKCJA POPRAWN "<< scene->interactions->size() << endl;
					//buduje nowy obiekt tmp aby latwiej przypisac srednie i pozniej dopisac jako ostani lelement
						const shared_ptr<Body>& b1 = Body::byId(ListaDaneEnd[i].id1);
						const shared_ptr<Body>& b2 = Body::byId(ListaDaneEnd[i].id2);
					
						State* st1 = static_cast<State*>(b1->state.get());
						State* st2 = static_cast<State*>(b2->state.get());
					
						//pobranie geometrii oraz phys
						CohFrictPhys* physFP = YADE_CAST<CohFrictPhys*>(Int->phys.get());
						ScGeom6D* geomScG=YADE_CAST<ScGeom6D*>(Int->geom.get());
				
						//get radius of shperes;
						Real r1 = (static_cast<Sphere*>(b1->shape.get()))->radius;	
						Real r2 = (static_cast<Sphere*>(b2->shape.get()))->radius;

						
						interTmp.id1=b1->id;
						interTmp.id2=b2->id;
						interTmp.r1=r1;
						interTmp.r2=r2;
						interTmp.normForce=physFP->normalForce;
						interTmp.shearForce=physFP->shearForce;
						interTmp.normal=geomScG->normal;
						

						
					
						//interTmp.size = 1;
					
					
						interTmp.typeId1 = b1->material->label;
						interTmp.typeId2 = b2->material->label;
						interTmp.time = scene->time;
						interTmp.positionId1 = st1->pos;
					 	interTmp.orientationId1 = st1->ori;
					 	interTmp.positionId2 = st2->pos;
					 	interTmp.orientationId2 = st2->ori;
		
						//roznica czasow
						dTime = scene->time - ListaDaneEnd[i].time;
						//cout << "iiiii="<<i << endl<< endl;
						//PrintIter(ListaDaneSr[i]);
						//suma do sredniej 
					
						ListaDaneSr[i].typeId1 = b1->material->label;
						ListaDaneSr[i].typeId2 = b2->material->label;
						ListaDaneSr[i].id1=b1->id;
						ListaDaneSr[i].id2=b2->id;
						ListaDaneSr[i].r1=r1;
						ListaDaneSr[i].r2=r2;
					
						ListaDaneSr[i].positionId1 += (ListaDaneEnd[i].positionId1 + interTmp.positionId1)/2.  * dTime;
						ListaDaneSr[i].positionId2 += (ListaDaneEnd[i].positionId2 + interTmp.positionId2)/2.  * dTime;
					
						ListaDaneSr[i].orientationId1 += (ListaDaneEnd[i].orientationId1 + interTmp.orientationId1)/2.  * dTime;	
						ListaDaneSr[i].orientationId2 += (ListaDaneEnd[i].orientationId2 + interTmp.orientationId2)/2.  * dTime;			
					
						ListaDaneSr[i].normForce += (ListaDaneEnd[i].normForce + interTmp.normForce)/2.  * dTime;		
						ListaDaneSr[i].shearForce += (ListaDaneEnd[i].shearForce + interTmp.shearForce)/2.  * dTime;	
						ListaDaneSr[i].normal += (ListaDaneEnd[i].normal + interTmp.normal)/2.  * dTime;
						ListaDaneSr[i].dW += Int->work;
						ListaDaneSr[i].dWAccum = Int->workAccum;
						ListaDaneSr[i].connectionOk=true;
						//PrintIter(ListaDaneSr[i]);
					
						//if(b->id == 2920)
						//	cout << dTime << " Pozycja 2920 =" << ListaDaneSr[b->id].position << endl;
					
						if(saveAll==1){
							ListaDaneAll.push_back(interTmp);
						}
				
						//nadpisanie jako ostatni element
						ListaDaneEnd[i] = interTmp;
					} else
					{
						ListaDaneEnd[i].connectionOk=false; //usuniecie z obliczen polaczenia zerwanego
						ListaDaneSr[i].connectionOk=false;
						//cout << "Zerwane polaczenie id1= "<< ListaDaneEnd[i].id1 << " id2= " << ListaDaneEnd[i].id2 << endl; 
					}
				}
			}//koniec kolejnych zapisow
	
	
		
			
		//calculate avarege of all elements
		 
		if((scene->time-timeFirst) > timeSr and ListaDaneSr.size() > 0 ){
		
		//cout << "Srednia "<< scene->interactions->size();
			dTime = ListaDaneEnd[0].time - timeFirst;
			for(int i=0; i<ListaDaneSr.size();i++)
				if(ListaDaneSr[i].connectionOk)				
				{
				ListaDaneSr[i].time = (calculationTime + timeFirst)/2.;
				ListaDaneSr[i].positionId1 /= dTime;
				ListaDaneSr[i].positionId2 /= dTime;
				
				ListaDaneSr[i].orientationId1/= dTime;
				ListaDaneSr[i].orientationId2/= dTime;
				
				ListaDaneSr[i].normForce /= dTime;
				ListaDaneSr[i].shearForce /= dTime;
				ListaDaneSr[i].normal /= dTime;
				ListaDaneSr[i].dW /= dTime;
				
				//add to save list
				ListaDaneSave.push_back(ListaDaneSr[i]);
				
				//ListaDaneEnd.clear();
				
			}


			
			//clear ListaDaneSr becouse all avarage data are in table to save
			ListaDaneSr.clear();
			ListaDaneEnd.clear();
			iterSrTmp = 0;
			calculate = false; // wylaczenie liczenia sredniej
			lastTime = calculationTime -timeSr/2.; //ustawienie ostatniego czasu sredniej jako ostatni czas
			iterSaveTmp++;
			//cout << "Licze srednia"<< endl;
		
		
				if (saveAll==1){
			
			string fileNameTmp = fileName+"_All";
			outfile.open(fileNameTmp, ios_base::app);
			//outfile.precision(15); //dodane do cout
			for(int i=0; i<ListaDaneAll.size();i++){
				outfile  << std::setprecision(15) << ListaDaneAll[i].id1 << ";"
		<< ListaDaneAll[i].id2 << ";" 
		<< ListaDaneAll[i].time << ";"
		<< ListaDaneAll[i].typeId1<< ";"
		<< ListaDaneAll[i].typeId2<< ";"
		<< ListaDaneAll[i].positionId1[0]<< ";"
		<< ListaDaneAll[i].positionId1[1]<< ";"
		<< ListaDaneAll[i].positionId1[2]<< ";"
		<< ListaDaneAll[i].orientationId1(0,0)<<";"
		<< ListaDaneAll[i].orientationId1(0,1)<<";"
		<< ListaDaneAll[i].orientationId1(0,2)<<";"
		<< ListaDaneAll[i].orientationId1(1,0)<<";"
		<< ListaDaneAll[i].orientationId1(1,1)<<";"
		<< ListaDaneAll[i].orientationId1(1,2)<<";"
		<< ListaDaneAll[i].orientationId1(2,0)<<";"
		<< ListaDaneAll[i].orientationId1(2,1)<<";"
		<< ListaDaneAll[i].orientationId1(2,2)<<";"
		<< ListaDaneAll[i].positionId2[0]<< ";"
		<< ListaDaneAll[i].positionId2[1]<< ";"
		<< ListaDaneAll[i].positionId2[2]<< ";"
		<< ListaDaneAll[i].orientationId2(0,0)<<";"
		<< ListaDaneAll[i].orientationId2(0,1)<<";"
		<< ListaDaneAll[i].orientationId2(0,2)<<";"
		<< ListaDaneAll[i].orientationId2(1,0)<<";"
		<< ListaDaneAll[i].orientationId2(1,1)<<";"
		<< ListaDaneAll[i].orientationId2(1,2)<<";"
		<< ListaDaneAll[i].orientationId2(2,0)<<";"
		<< ListaDaneAll[i].orientationId2(2,1)<<";"
		<< ListaDaneAll[i].orientationId2(2,2)<<";"
		<< ListaDaneAll[i].normForce[0]<<";"
		<< ListaDaneAll[i].normForce[1]<<";"
		<< ListaDaneAll[i].normForce[2]<<";"
		<< ListaDaneAll[i].shearForce[0]<<";"
		<< ListaDaneAll[i].shearForce[1]<<";"
		<< ListaDaneAll[i].shearForce[2]<<";"
		<< ListaDaneAll[i].normal[0]<<";"
		<< ListaDaneAll[i].normal[1]<<";"
		<< ListaDaneAll[i].normal[2]<<";"
		<< ListaDaneAll[i].r1<<";"
		<< ListaDaneAll[i].r2<<";"
		<< ListaDaneAll[i].dW<<";"
		<< ListaDaneAll[i].dWAccum<<";"
				<<"\n";

			}
			outfile.close();
			saveAll=2;
		}//end saveAll
		
		cout << "Do zapisu po Interakcjach pozostalo " << iterSave - iterSaveTmp << endl;
		
		} //end srednia
	
	

	
		if(iterSaveTmp>iterSave){
				string fileNameTmp = fileName+std::to_string(fileNumber);
				outfile.open(fileNameTmp, ios_base::app);
				//outfile.precision(15); //dodane do cout
				string save="";  
				for(int i=0; i<ListaDaneSave.size();i++){
					if(ListaDaneSave[i].connectionOk)
						outfile  << std::setprecision(15) << ListaDaneSave[i].id1 << ";"
						<< ListaDaneSave[i].id2 << ";" 
						<< ListaDaneSave[i].time << ";"
						<< ListaDaneSave[i].typeId1<< ";"
						<< ListaDaneSave[i].typeId2<< ";"
						<< ListaDaneSave[i].positionId1[0]<< ";"
						<< ListaDaneSave[i].positionId1[1]<< ";"
						<< ListaDaneSave[i].positionId1[2]<< ";"
						<< ListaDaneSave[i].orientationId1(0,0)<<";"
						<< ListaDaneSave[i].orientationId1(0,1)<<";"
						<< ListaDaneSave[i].orientationId1(0,2)<<";"
						<< ListaDaneSave[i].orientationId1(1,0)<<";"
						<< ListaDaneSave[i].orientationId1(1,1)<<";"
						<< ListaDaneSave[i].orientationId1(1,2)<<";"
						<< ListaDaneSave[i].orientationId1(2,0)<<";"
						<< ListaDaneSave[i].orientationId1(2,1)<<";"
						<< ListaDaneSave[i].orientationId1(2,2)<<";"
						<< ListaDaneSave[i].positionId2[0]<< ";"
						<< ListaDaneSave[i].positionId2[1]<< ";"
						<< ListaDaneSave[i].positionId2[2]<< ";"
						<< ListaDaneSave[i].orientationId2(0,0)<<";"
						<< ListaDaneSave[i].orientationId2(0,1)<<";"
						<< ListaDaneSave[i].orientationId2(0,2)<<";"
						<< ListaDaneSave[i].orientationId2(1,0)<<";"
						<< ListaDaneSave[i].orientationId2(1,1)<<";"
						<< ListaDaneSave[i].orientationId2(1,2)<<";"
						<< ListaDaneSave[i].orientationId2(2,0)<<";"
						<< ListaDaneSave[i].orientationId2(2,1)<<";"
						<< ListaDaneSave[i].orientationId2(2,2)<<";"
						<< ListaDaneSave[i].normForce[0]<<";"
						<< ListaDaneSave[i].normForce[1]<<";"
						<< ListaDaneSave[i].normForce[2]<<";"
						<< ListaDaneSave[i].shearForce[0]<<";"
						<< ListaDaneSave[i].shearForce[1]<<";"
						<< ListaDaneSave[i].shearForce[2]<<";"
						<< ListaDaneSave[i].normal[0]<<";"
						<< ListaDaneSave[i].normal[1]<<";"
						<< ListaDaneSave[i].normal[2]<<";"
						<< ListaDaneSave[i].r1<<";"
						<< ListaDaneSave[i].r2<<";"
						<< ListaDaneSave[i].dW<<";"
						<< ListaDaneSave[i].dWAccum<<";"
								<<"\n";

				}
				outfile.close();
			ListaDaneSave.clear();
			iterSaveTmp = 0;
			
			fileNumber++; //zwiekszam numer pliku
			savedFiles++; 
		}
		
			
		}//end timeStrat
		
};


void VppSaveInteractions::PrintIter(DaneInter iter)
{
	cout << iter.id1 << ";"
		<< iter.id2 << ";" 
		<< iter.time << ";"
		<< iter.typeId1<< ";"
		<< iter.typeId2<< ";"
		<< iter.positionId1[0]<< ";"
		<< iter.positionId1[1]<< ";"
		<< iter.positionId1[2]<< ";"
		<< iter.orientationId1(0,0)<<";"
		<< iter.orientationId1(0,1)<<";"
		<< iter.orientationId1(0,2)<<";"
		<< iter.orientationId1(1,0)<<";"
		<< iter.orientationId1(1,1)<<";"
		<< iter.orientationId1(1,2)<<";"
		<< iter.orientationId1(2,0)<<";"
		<< iter.orientationId1(2,1)<<";"
		<< iter.orientationId1(2,2)<<";"
		<< iter.positionId2[0]<< ";"
		<< iter.positionId2[1]<< ";"
		<< iter.positionId2[2]<< ";"
		<< iter.orientationId2(0,0)<<";"
		<< iter.orientationId2(0,1)<<";"
		<< iter.orientationId2(0,2)<<";"
		<< iter.orientationId2(1,0)<<";"
		<< iter.orientationId2(1,1)<<";"
		<< iter.orientationId2(1,2)<<";"
		<< iter.orientationId2(2,0)<<";"
		<< iter.orientationId2(2,1)<<";"
		<< iter.orientationId2(2,2)<<";"
		<< iter.normForce[0]<<";"
		<< iter.normForce[1]<<";"
		<< iter.normForce[2]<<";"
		<< iter.shearForce[0]<<";"
		<< iter.shearForce[1]<<";"
		<< iter.shearForce[2]<<";"
		<< iter.normal[0]<<";"
		<< iter.normal[1]<<";"
		<< iter.normal[2]<<";"
		<< "koniec linii " << endl;
		};

Real VppSaveInteractions::CountdW(CohFrictPhys* physFP, ScGeom6D* geom, int id){

	Real dW = 0;
	
	const Vector3r& dus = geom->shearIncrement();
	Real un     = geom->penetrationDepth;
	Real Fn     = physFP->kn*(un-physFP->unp);
	Real Fs     = physFP->shearForce.norm();
	Real maxFs = Fn * physFP->tangensOfFrictionAngle;
					

	Real ds =  (Fs-maxFs)/physFP->ks;

	

	if (ds > 0 ){
		dW  = ds * maxFs;  
	} 
	if(id==1380){
	cout << "ds=" << ds <<" un=" << un <<" Fn=" << Fn  <<" Fs=" << Fs <<" maxFs=" << maxFs <<" tangensOfFrictionAngle=" << physFP->tangensOfFrictionAngle << endl;
	cout << dW << " " << ds << endl;
	}
	return dW;
};

YADE_PLUGIN((VppSaveInteractions));
CREATE_LOGGER(VppSaveInteractions);


