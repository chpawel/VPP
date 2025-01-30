#include "VppPressureForce.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/vpp/VppState.hpp>
#include<pkg/vpp/VppCohesiveFrictionalContactLaw.hpp>


void VppPressureForce::action(){
	//clear surface vector for all VPP bodies
	//FOREACH(const shared_ptr<Body>& b, *scene->bodies){
	//	if(VppState* st = dynamic_cast<VppState*>(b->state.get())){
	//		st->surfaceVec = Vector3r::Zero();
	//	}
	//}

	//calculate new surface vector for all VPP bodies
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		//calculations of body
		if(VppState* st = dynamic_cast<VppState*>(b->state.get())){
			//clear the surface vector befor calculations for all bodies
			st->surfaceVec = Vector3r::Zero();
			if(st->pressure){
				VppCohFrictMat* mat = static_cast<VppCohFrictMat*>(b->material.get());
				switch(mat->foilType){
					case 1:{
						for(int i=0; i<4; i++){
							int idNeighbour1 = st->neighbours[i];
							int idNeighbour2 = st->neighbours[(i+1)%4];
							if(idNeighbour1>-1&&idNeighbour2>-1){
								const shared_ptr<Body>& b1 = Body::byId(idNeighbour1);
								const shared_ptr<Body>& b2 = Body::byId(idNeighbour2);
						
								VppState* st1 = static_cast<VppState*>(b1->state.get());
								VppState* st2 = static_cast<VppState*>(b2->state.get());
						
								Vector3r pos1 = st1->pos;
								Vector3r pos2 = st2->pos;
				
								Vector3r d1 =  pos1 - st->pos;
								Vector3r d2 =  pos2 - st->pos;
				
								Vector3r surface = d1.cross(d2);
						
								st->surfaceVecTmp=surface;
								//st1->surfaceVec+=1./3 * surface;
								//st2->surfaceVec+=1./3 * surface;
								st->surfaceVec+=surface;
				
							}
						}
					break;
					}//end case 1
					case 2:{
						for(int i=0; i<6; i++){
							int idNeighbour1 = st->neighbours[i];
							int idNeighbour2 = st->neighbours[(i+1)%6];
							if(idNeighbour1>-1&&idNeighbour2>-1){
								const shared_ptr<Body>& b1 = Body::byId(idNeighbour1);
								const shared_ptr<Body>& b2 = Body::byId(idNeighbour2);
						
								VppState* st1 = static_cast<VppState*>(b1->state.get());
								VppState* st2 = static_cast<VppState*>(b2->state.get());
						
								Vector3r pos1 = st1->pos;
								Vector3r pos2 = st2->pos;
				
								Vector3r d1 =  pos1 - st->pos;
								Vector3r d2 =  pos2 - st->pos;
				
								Vector3r surface = d1.cross(d2);
						
								st->surfaceVecTmp=1./6 * surface;
								//st1->surfaceVec+=1./3 * surface;
								//st2->surfaceVec+=1./3 * surface;
								st->surfaceVec+=1./6 * surface;
							}
						}
					break;
					}
				}//end switch
			} //end if pressure
		}
	}
/*
//calculate new surface vector for all VPP bodies
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(VppState* st = dynamic_cast<VppState*>(b->state.get())){
			for(int i=0; i<4; i++){
				int idNeighbour1 = st->neighbours[i];
				int idNeighbour2 = st->neighbours[(i+1)%4];
				if(idNeighbour1>-1&&idNeighbour2>-1){
					const shared_ptr<Body>& b1 = Body::byId(idNeighbour1);
					const shared_ptr<Body>& b2 = Body::byId(idNeighbour2);
					
					VppState* st1 = static_cast<VppState*>(b1->state.get());
					VppState* st2 = static_cast<VppState*>(b2->state.get());
					
					st->surfaceVec+=1./3 * st->surfaceVecTmp;
					st->surfaceVec+=1./3 * st1->surfaceVecTmp;
					st->surfaceVec+=1./3 * st2->surfaceVecTmp;
				
				}	
			}
		}
	}
*/	
	//add force
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(VppState* st = dynamic_cast<VppState*>(b->state.get())){
			Vector3r force = st->pressureValue * st->surfaceVec;
			scene->forces.addForce(b->id,force);
			st->pressureForce = force;
			//zapianie pracy wykonanej przez folie
			Vector3r s = st->vel * scene->dt;
			st->dEnergyFoil += force.dot(s);
			//scene->forces.sync();
			//cout << " id= " << b->id << "force=" << force << " work=" <<  st->dEnergyFoil<< endl;
		}
	}
	scene->forces.sync();
		
};
YADE_PLUGIN((VppPressureForce));
CREATE_LOGGER(VppPressureForce);

