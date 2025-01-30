#include "VppNeighbours.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/vpp/VppState.hpp>
#include<pkg/vpp/VppCohesiveFrictionalContactLaw.hpp>


void VppNeighbours::action(){
	static bool done = false;
	static int licznik = 10;
	
	if(reset == true){
		done=false;
		cout << "Reset Neighbour function"<< endl;	
	}

	if(scene->time < timeStart){
		cout << "nie dzialam"<< endl;		
		return ;
	}
	

	if(!done){
		licznik--;
		cout << licznik<< endl;
		//lookinf for all neighbours 
		FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
			if(I->isReal()){
				const shared_ptr<Body>& b1 = Body::byId(I->getId1(),scene);
				const shared_ptr<Body>& b2 = Body::byId(I->getId2(),scene);
				shared_ptr<Body> b[2] = {b1,b2};
				if(b1->isStandalone() and b2->isStandalone()){
					
					
						//cout << "Clumped!! b1" <<b1->isClumpMember()<< endl;
						//cout << "Clumped!! b2" <<b2->isClumpMember()<< endl;
					
					
					if(VppState* st1 = dynamic_cast<VppState*>(b1->state.get())){
						if(VppState* st2 = dynamic_cast<VppState*>(b2->state.get())){
							//VppState* st[2] = {static_cast<VppState*>(b[0]->state.get()),static_cast<VppState*>(b[1]->state.get())};
				
							VppState* st[2] = {st1,st2};
				
							for(int i=0;i<2;i++){
								//setting indexes
								int i1 = i;
								int i2 = 1-i;
								bool add  = true;
								for(int k=0;k<6;k++){
									if(st[i1]->neighbours[k] == b[i2]->id){
										add = false;
										break;
									}
								}
								if(add){
									for(int k=0;k<6;k++){
										if(st[i1]->neighbours[k] ==-1){
											st[i1]->neighbours[k] = b[i2]->id;
											break;
										}
									}
								}
							}
						}//vpp2
					}//vpp1
				}//isclump
			}//isreal
		}//interactions
//	
//	
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if(!(b->isClumpMember())){             
				if(VppState* st = dynamic_cast<VppState*>(b->state.get())){
				Sphere* sph = static_cast<Sphere*>(Body::byId(b->id,scene)->shape.get());
				double r = sph->radius;
				int tmp[6]={-1,-1,-1,-1,-1,-1};
				int elTmp=0;
				
				VppCohFrictMat* mat = static_cast<VppCohFrictMat*>(b->material.get());
				
				if(VppState* st = dynamic_cast<VppState*>(b->state.get()))
				switch(mat->foilType){
					case 1:{ //4 naighbours foil
						if(st->neighbours[0]!=-1){
							tmp[0] = st->neighbours[0];
							st->neighbour0 = st->neighbours[0];

								for(int k=1; k<4; k++){
									if(st->neighbours[k]==-1)
										break;
							
							
									const shared_ptr<Body>& b1 = Body::byId(tmp[0]);
									const shared_ptr<Body>& b2 = Body::byId(st->neighbours[k]);
					
									VppState* st1 = static_cast<VppState*>(b1->state.get());
									VppState* st2 = static_cast<VppState*>(b2->state.get());
						
									Vector3r pos1 = st1->pos;
									Vector3r pos2 = st2->pos;
					
									Vector3r d1 =  pos1 - st->pos;
									Vector3r d2 =  pos2 - st->pos;
						
									//liczone kiedy by podany pressureDirection jako wektor
									//Vector3r d3 =  st->pos-(pos1.dot(st->pressureDirection)*st->pressureDirection);
									//zmiana na punkt jako pressureDirection
									Vector3r d3 =st->pos - st->pressureDirection;
						
									Vector3r cross = d1.cross(d2);
									double dotDirec = cross.dot(d3); // aby wynik by jak przy korzystaniu z wektora
						
									//weryfikacja roznicy czy nie wieksza niz promien
									if(((pos1-pos2).norm()) > (2*r * sqrt(2) * 1.001)){
											tmp[2]=b2->id;
											st->neighbour2 = b2->id;
									}else if(dotDirec<0){
										tmp[1]=b2->id;
										st->neighbour1 = b2->id;
									}
									else{
										tmp[3]=b2->id;
										st->neighbour3 = b2->id;
									}
							
							
								} //end four neighbours
								}
								
								break;
							}//end case four neighbours
							case 2:{
							if(st->neighbours[0]!=-1){
								tmp[0] = st->neighbours[0];
								st->neighbour0 = st->neighbours[0];
	
								for(int k=1; k<6; k++){
								//cout << "Przed ustawianiem"<< st->neighbours[k];
									if(st->neighbours[k]==-1)
										break;
//							
//									
									const shared_ptr<Body>& b1 = Body::byId(tmp[0]);
									const shared_ptr<Body>& b2 = Body::byId(st->neighbours[k]);
//					
									VppState* st1 = static_cast<VppState*>(b1->state.get());
									VppState* st2 = static_cast<VppState*>(b2->state.get());
//						
									Vector3r pos1 = st1->pos;
									Vector3r pos2 = st2->pos;
//					
									Vector3r d1 =  pos1 - st->pos;
									Vector3r d2 =  pos2 - st->pos;
//									
//									//if(b->id==2285){
//									//	Sphere* sph = static_cast<Sphere*>(Body::byId(b->id,scene)->shape.get());
//									//	sph->color = Vector3r(1,1,1);	
//									//}
									Vector3r d3 =  st->pos-(pos1.dot(st->pressureDirection)*st->pressureDirection);
//						
									Vector3r cross = d1.cross(d2);
									double dotDirec = cross.dot(d3);
									//cout << "Licze dla folie 6"<<endl;
//									
									Vector3r crossTmp = cross / (d1.norm() * d2.norm());
//									//okreslenie kata - dla naprzeciwko bliki zero
									double sin = cross.norm() / (d1.norm() * d2.norm());
//									
//									
//									//if(b->id==2285){
//									//	Sphere* sph = static_cast<Sphere*>(Body::byId(b->id,scene)->shape.get());
//									//	sph->color = Vector3r(1,1,1);
//									//	cout << " n1="<<b1->id << " n2="<< b2->id << " sin="<< sin << " dpos="<< (pos1-pos2).norm() << endl;  	
//									//}
//									
									//weryfikacja roznicy czy nie wieksza niz promien
									if( ((pos1-pos2).norm()) < 4* r * 0.999 and  sin<0.15 ){
											tmp[3]=b2->id;
											st->neighbour3 = b2->id;
										}else if(dotDirec < 0 and ((pos1-pos2).norm()) < 2*r *sqrt(2)*1.001){
											tmp[1]=b2->id;
											st->neighbour1 = b2->id;
										}
										else if(dotDirec > 0 and ((pos1-pos2).norm()) < 2*r *sqrt(2)*1.001){
											tmp[5]=b2->id;
											st->neighbour5 = b2->id;
										}else if(dotDirec < 0 and ((pos1-pos2).norm()) > 2*r *sqrt(2)*1.001){
											tmp[2]=b2->id;
											st->neighbour2 = b2->id;
										}else if(dotDirec > 0 and ((pos1-pos2).norm()) > 2*r *sqrt(2)*1.001){
											tmp[4]=b2->id;
											st->neighbour4 = b2->id;
										}
									}
//							
//								
								}//end if
//								break;
								}//end case six neighbours
							}//end switch	
			
				
				
//					//copy new neighbours to table
						for(int i=0; i<6; i++){
						//	cout << b->id <<" " << st->neighbours[i] << endl;;
							st->neighbours[i] = tmp[i]; 
						}
					}//end if
					}//end dynamic cast
				}//end foreach
//					
					if(licznik == 0){	
						done=true;
						cout <<"Koniec ustawiania sasiadow"<< endl;
						//break;
					}
		
//		
//			
//	
//		
//selecting only spheres with four neighbours
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){             
		if(!(b->isClumpMember())){
		if(VppState* st = dynamic_cast<VppState*>(b->state.get())){	
			Sphere* sph = static_cast<Sphere*>(Body::byId(b->id,scene)->shape.get());
			
			VppCohFrictMat* mat = static_cast<VppCohFrictMat*>(b->material.get());
			
			bool	set = true;	
			switch(mat->foilType){
					case 1:{
						for(int k=0; k<4; k++){
							if(st->neighbours[k]==-1)
								set = false;
						}
		
						if(set){
							st->pressure = true;
							sph->color = Vector3r(0,0,1);		
						}
			
			}case 2:{
				for(int k=0; k<6; k++){
					if(st->neighbours[k]==-1)
						set = false;
				}

				if(set){
					st->pressure = true;
					sph->color = Vector3r(0,0,1);		
				}
			
				}
			}	
		}
		}//isclump
	}
	}
};
YADE_PLUGIN((VppNeighbours));
CREATE_LOGGER(VppNeighbours);
