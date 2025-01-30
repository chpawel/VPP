# basic simulation showing sphere falling ball gravity,
# bouncing against another sphere representing the support
os.environ['OMP_NUM_THREADS']='4'
import math
import random

##parametr z konsoli najczesciej mnoznik
mnoznik = 1
if len(sys.argv)== 2:
 mnoznik = float(sys.argv[1])

# DATA COMPONENTS

# wspolczynniki tlumienia
cn0=50000 
cs0=50000
# wspolczynniki kohezji
nch=1e15
sch=1e15

folia=['folia',Vector3(0,1,0)]
kula=['kula',Vector3(1,1,1)]

#######################################
################################
mass_scale = 1#100 


promien_k = 0.00593/2  #promien ziaren
promien_f = 0.00593/2/3  #promien folii [m] grubosc folii 0.00015

gr_f = 0.00015 # grubosc folii [m]
young_f = 2*1.2218e8*gr_f/(promien_f) # [Pa]
poisson_f = 0.25
density_f = 1000/promien_f * gr_f * mass_scale # [kg/m^3/mass_scale]

young_k = 2.7e9 # [Pa]
poisson_k = 0.25
density_k = 2689 * mass_scale # [kg/m^3/mass_scale]

frictAngle = atan(0.9)# po ulozeniu kul zmiana tarcia.
frictAngleFoil = 0 #tarcie folii
ratio_upak =  0.65 #0.62 #ratio upakowania /ile kul ma byc wygenerowanych 




srednica_probki = 0.051#/2   # [m]
wysokosc_probki = 0.105#/2 # [m]

wys_fixed = 1 # liczba kul folii nieruchomych
wys_sciany_doc = 8*promien_k #wysokosc sciany dociskajacej



tmp_srednica = srednica_probki - 2.*promien_f
promien_probki = tmp_srednica / 2.
obw_probki = 2*math.pi*promien_probki


#######################################
###############################

#########################################
#########DO UZUPELNIENIA ################
pressureDirection=Vector3(0,0,1)
pAtm = 100000 #[Pa]
pressureValue = mnoznik * pAtm # [Pa] 
#########################################


l_kul = int(math.floor(obw_probki/(2.*promien_f)))+1 #liczba kul w obwodzie

#odejmuje wysokosc kul zafiksowanych
wysokosc_probki = wysokosc_probki - wys_fixed * promien_f*2

wys = int(math.floor(wysokosc_probki/(promien_f*2))) # wysokosc folii (liczba srednic kul)

k_wys = wys*2*promien_f*1.25 # wysokosc kul do dosypania [m]

wys=wys+1



#########################################
nazwaPliku = "wyniki/frictionS/pila_Ampl_1cm_upak_65_popr_cylWfolii_predkosc_m8_Knratio0p2_press_"+str(mnoznik)+"_"+str(pressureValue)+"_"+str(l_kul)+"_"+str(wys)+"_"
nazwaPlikuIter = "wyniki/frictionS/pilaIter_Ampl_1cm_upak_65_popr_cylWfolii_predkosc_m8_Knratio0p2_press_"+str(mnoznik)+"_"+str(pressureValue)+"_"+str(l_kul)+"_"+str(wys)+"_"
############### Parametry zapisu c++ ########




#####################
startZ = 2*promien_k # poczatek wysokosci ustawinia kul

v_probki = math.pi * pow(srednica_probki/2,2) * wysokosc_probki
v_kuli = 4.0/3.0 * math.pi * pow(promien_k,3)
liczbaKulDuzych = (ratio_upak * v_probki) / v_kuli


print("\n \n ratio zadane")
print ratio_upak
print("Vprobki")
print v_probki
print("Vkuli")
print v_kuli
print("promien kuli")
print promien_k
print("wysokosc probki")
print wysokosc_probki
print("promien probki")
print promien_probki

print("\n\nliczba kul w rdzeniu dokladnie")
print liczbaKulDuzych
liczbaKulDuzych = round(liczbaKulDuzych,0)
print("liczba kul w rdzeniu")
print liczbaKulDuzych

print("ratio wyliczone")
print liczbaKulDuzych * v_kuli /v_probki * 100


#########################################
#parametry ruchu sciany gornej
amplituda = 0.01 #0.05 * wysokosc_probki #1*promien_f # [m]
omega = 2.84 * 5  # [1/s]
time = 0. # [s]
idGora = 0 # id sciany do dociskania


velPila = 0.000833333 * 8 # [m/s] predkosc obnizania przy wymuszeniu pila maszyna 50 mm na min
posPila = amplituda / 2. #start ruchu

direct = -1 #kierunek ruchu -1 do dolu + do gory


vObnizenia = 0.005 *10 # [m/s]
timeToStart = 0#0.1 # czas ktory jest odczekiwany do startu - czas zerowany jezeli kula nie przeskoczy w tym czasie zadna - obnizam folie [s]
timeToStartPressure = 1. #czas ktory jest odczekiwany do startu po wlaczeniu cisnienia [s]
dtimeToStart = 0 # [s]
tmpDtimeToStart = 0 #tmp dla czasu - czas z kroku poprzedniego [s]
stage = 0 #0 - usypywanie, 2 - obnizenie sciany , 4 - ruch calosci

relGora = -promien_f # procent obnizenia gornej sciany przy dociskaniu
relFolia = 0.007 # ile mm obnizenia folii i sciany (tak jak przy wkladaniu do maszyny)

#parametry zmiany czasu obliczen 
timeChange=0.13 #po jakim czasie ma byc zmienione O.dt
timeChangeValue = 1 #ile razy ma byc zmniejszone -1 - zmiana nie nastepuje

O.materials.append(VppCohFrictMat(young=young_f, poisson=poisson_f, density=density_f, frictionAngle=radians(0),normalCohesion=nch, shearCohesion=sch, momentRotationLaw=True, knRatio=0.2,foilType=1, label='folia'))

O.materials.append(FrictMat(young=1e9, poisson=.25, frictionAngle=0, density=1000, label='frictionlessWalls'))

O.materials.append(CohFrictMat(young=young_k, poisson=poisson_k, density=density_k, frictionAngle=radians(0),normalCohesion=0, shearCohesion=0, momentRotationLaw=False, label='kula'))

#momentRotationLaw = False - wtedy mamy obroty;

import math
mask_f = 3 #         1 1 0
mask_wd = 2#         0 1 0
mask_wg = 4#         0 0 1
mask_k=10 #          0 1 0 1  maska kul przy ustawianiu
mask_ku= 14 #        0 1 1 1    maska kuli po przekroczeniu scisny gornej
maska_cyl = 8 #      0 0 0 1   maska_cylindra 
maska_cylfoil = 1 #  1 0 0   maska_cylindra wewnatrz folii
#relGora = (wys+1 + wys_fixed) * promien_f * (relGora/100.) # odleglosc obnizenia [m]
#relFolia = (wys+1 + wys_fixed) * promien_f * (relFolia/100.) # odleglosc obnizenia folii [m]



idFoliiStart = 0
idFoliiKoniec = 0

idFoliiDolStart = 0
idFoliiDolKoniec = 0

promien = promien_f/sin(pi/l_kul) # [m]
fi =  2*pi / l_kul # [rad]
fi2 = 0 # [rad] do obrotu kul miedzy warstwamifo	

promien = promien * 0.99 # [m]
dtt = 0 # zmienna do przechowania czasu [s]
#fi = 5.81




#print ("Promien_probki= " + str(promien))
print (" Wysokosc_probki= " + str((wys+wys_fixed)*promien_f*2))
print ("Promien_probki_folii= " + str(promien_f))
print (" Modul kuli folii= " + str(young_f))
wys = wys+1 # korekcja wysokosci aby bylo tyle kul ile zadeklarowalismy wysokosc folii [liczba kul]


######################################################
### USTAWIENIE PIERWSZEJ NIERUCHOMEJ WARSTWY FOLII
######################################################
idFoliiDolStart = 0
for k in range(l_kul):
	x = promien*cos(k*fi)
	y = promien*sin(k*fi)
	O.bodies.append([
    	 utils.sphere(center=(x,y,2*promien_f),radius=promien_f,fixed=True,material=folia[0],color=folia[1],mask=mask_f),
	])
	O.bodies[-1].state.pressureDirection = pressureDirection
	#O.bodies[-1].state.pressureValue = pressureValue
idFoliiDolKoniec  = O.bodies[-1].id

print("poczatek obreczy dol")
print(idFoliiDolStart)
print("koniec obreczy dol")
print(idFoliiDolKoniec)

	
######################################################
### USTAWIENIE POZOSTALYCH WARSTW FOLII
######################################################
y=0
x = 0
######################################################
### USTAWIENIE POZOSTALYCH WARSTW FOLII
######################################################
for k in range(l_kul):
	for z in range(2,wys):
		if(k%2 == 0 and z%2 ==0):
			continue
		else:
			x = promien*(cos(k*fi))
			y = promien*(sin(k*fi))
			O.bodies.append([
	  # fixed: particle's position in space will not change (support)
	  		 utils.sphere(center=(x,y,z*promien_f*2*1.),radius=promien_f,fixed=False,material=folia[0],color=folia[1],mask=mask_f),
			])
			O.bodies[-1].state.pressureDirection = pressureDirection
			
	
######################################################
### USTAWIENIE OSTATNICH NIERUCHOMYCH WARSTW FOLII
######################################################	
idFoliiStart = O.bodies[-1].id+1

for z in range(0,wys_fixed):
	for k in range(l_kul):
		x = promien*cos(k*fi)
		y = promien*sin(k*fi)
		O.bodies.append([
	     	 utils.sphere(center=(x,y,(wys*promien_f*2*1.0)),radius=promien_f,fixed=True,material=folia[0],color=(1,1,1),mask=mask_f),
		])	
		O.bodies[-1].state.pressureDirection = pressureDirection


idFoliiKoniec  = O.bodies[-1].id

print("poczatek obreczy")
print(idFoliiStart)
print("koniec obreczy")
print(idFoliiKoniec)

######################################################
### USTAWIENIE sciany na gorze jezeli sciana ma byc ustawiona nad forlia to w tym miejscu
######################################################
#O.bodies.append(utils.wall(max([b.state.pos[2] + wys_sciany_doc for b in O.bodies if isinstance(b.shape,Sphere)]),axis=2,sense=-1,material='frictionlessWalls',mask = mask_wg))
#idGora = O.bodies[-1].id


		
######################################################
### USTAWIENIE sciany na dole
######################################################
O.bodies.append(utils.wall(min([b.state.pos[2]-b.shape.radius for b in O.bodies if isinstance(b.shape,Sphere)]),axis=2,sense=1,material='frictionlessWalls',mask = mask_wd))
idDol = O.bodies[-1].id

def set_spheres():
	global startZ
	ile_kul = 0 #licze ile kul ustawilem 
	h = 0 #ile obreczy zrobionych
	pr=promien #lokalny promien
	
	while(ile_kul<liczbaKulDuzych):
		r = promien - 2.*promien_f-1.5*promien_k # promien do ustawiania kul
		while (r>2.0*promien_k):
			obw = 2.0*pi*r #obwod do ustawiania kul
			l_k = int(math.floor(obw/(2.*promien_k))) # liczba kul w obwodzie
			fi =  2.0*pi / l_k # [rad]
			z_fi = random.uniform(0.,pi) # przesuniecie w obrocie kul
			for k in range(l_k):
				x = r*cos(k*fi+z_fi)
				y = r*sin(k*fi+z_fi)
				if ile_kul<liczbaKulDuzych:				
					O.bodies.append([
		    	 		 utils.sphere(center=(x,y,startZ),radius=promien_k,fixed=False,material=kula[0],color=(0,0,1),mask=mask_k),
					])
					ile_kul+=1
			#print("ile_kul")
			#print[ile_kul]
			#print[random.uniform(0.1*2.*promien_k,2.*promien_k)]
			r -=  (2.*promien_k)#+random.uniform(0.1*2.*promien_k,2.*promien_k)) # promien do ustawiania kul	
		startZ+=2.1*promien_k

######################################################
### dodanie kul duzych
######################################################

first = O.bodies[-1].id+1#len(O.bodies)-1 #do zmian parametrow kuli pierwszy element
from yade import pack
sp=pack.SpherePack()

#c1=pack.SpherePack([((0,0,0),.0001),((.00005,0,0),.0001),((0.0001,0,0),.0001)])
#sp.makeClumpCloud((-promien+2*promien_k+2*promien_f,-promien+2*promien_k+2*promien_f,promien_k),(promien-2*promien_k-2*promien_f,promien-2*promien_k-2*promien_f,promien_f*(wys+wys_fixed)*2+k_wys),[c1],periodic=True,num=500000)	
#sp.toSimulation()

#sp.makeCloud((-promien*sqrt(2)+3*promien_k+2*promien_f,-promien*sqrt(2)+3*promien_k+2*promien_f,promien_k),(promien*sqrt(2)-3*promien_k-2*promien_f,promien*sqrt(2)-2*promien_k-3*promien_f,promien_f*(wys+wys_fixed)*4+k_wys),rMean=promien_k,rRelFuzz=0,porosity=0.8)

dlugoscBokuChmury = 0.8*promien-2.5*promien_k-2*promien_f
wysokoscChmury = 2*wysokosc_probki#((liczbaKulDuzych * (4./3 * 3.14 * pow(promien_k,3)))/pow(dlugoscBokuChmury,2))


###################
#####Do odblokowania###
#print "wysokoscChmury= "+str(wysokoscChmury)

#sp.makeCloud((-dlugoscBokuChmury,-dlugoscBokuChmury,3*promien_k),(dlugoscBokuChmury,dlugoscBokuChmury,wysokoscChmury),rMean=promien_k,rRelFuzz=0,num=int(liczbaKulDuzych))


#sp.toSimulation()

set_spheres()

last = O.bodies[-1].id  #do zmian parametrow kuli ostatni element

for k in range(first,last+1):
	 #O.bodies[k].material =  O.materials[1] #zmiana materialu kuli
	 O.bodies[k].mask = mask_k

print("Liczba kul dodanych w rdzeniu")
liczba_kul_dodanych = last-first
print liczba_kul_dodanych

######################################################
### USTAWIENIE sciany na gorze jezeli sciana ma byc ustawiona nad forlia to w tym miejscu
######################################################
O.bodies.append(utils.wall(max([b.state.pos[2] + wys_sciany_doc for b in O.bodies if isinstance(b.shape,Sphere)]),axis=2,sense=-1,material='frictionlessWalls',mask = mask_wg))
idGora = O.bodies[-1].id
print "********Id gora **********"
print idGora

######################################################
### dodanie cylindra do usypywania
######################################################
idCylinderStart = O.bodies[-1].id+1

O.bodies.append(geom.facetCylinder((0,0,wysokosc_probki+wysokoscChmury/2+promien_k*2-relFolia), promien-promien_f*2,wysokoscChmury,material='frictionlessWalls', wallMask=4))

idCylinderKoniec = O.bodies[-1].id

print "********Poczatek cylindra **********"
print idCylinderStart
print "********Koniec cylindra ************"
print idCylinderKoniec

for k in range(idCylinderStart,idCylinderKoniec+1):
	O.bodies[k].mask = maska_cyl
	print("Id Cylinder do usypawnia kul = ",k)

######################################################
### dodanie cylindra do folii
######################################################
idCylinderFoliaStart = O.bodies[-1].id+1

O.bodies.append(geom.facetCylinder((0,0,wysokosc_probki/2), promien-promien_f,wysokosc_probki,material='frictionlessWalls', wallMask=4 ))

idCylinderFoliaKoniec = O.bodies[-1].id

for k in range(idCylinderFoliaStart,idCylinderFoliaKoniec+1):
	O.bodies[k].mask = maska_cylfoil
	print("Id Cylinder do ustawienia folii = ",k)


print "********Poczatek cylindra folii**********"
print idCylinderFoliaStart
print "********Koniec cylindra folii************"
print idCylinderFoliaKoniec



######################################################
### Ustawienie czasu
######################################################
O.dt=utils.PWaveTimeStep()


def ChangeTime():
	global timeChange,timeChangeValue

	if timeChangeValue >0 and O.time >= timeChange :
		print("Zmiana O.dt z "+str(O.dt)+ " na mniejszy ")		
		O.dt=O.dt/timeChangeValue
		timeChangeValue = -1
		print(str(O.dt))
		


######################################################
### Funkcja zmieniajaca polozenie folii - wstepne ugiecie
######################################################
def ChangePosFoil():
	global time,stage
	if stage == 1:
		if O.bodies[idFoliiStart].state.refPos[2] - O.bodies[idFoliiStart].state.pos[2] < relFolia:
			for k in range(idFoliiStart,idFoliiKoniec+1):
				#O.forces.addMove(k,(0,0,-vObnizenia*O.dt))
				O.bodies[k].state.vel[2]=-vObnizenia #zmiana bo addMove ma blad
			#O.forces.addMove(idGora,(0,0,-vObnizenia*O.dt))
		else:
			for k in range(idFoliiStart,idFoliiKoniec+1):
				O.bodies[k].state.vel[2]=0			
			stage = 2
			print "Stage 2"
			
################################


######################################################
### Funkcja obnizajaca scine - wstepne ugiecie
######################################################
def PreChangePos():
	global stage
	if stage == 2:
		O.bodies[idGora].state.vel[2]=-vObnizenia
		#O.forces.addMove(idGora,(0,0,-vObnizenia*O.dt))
		if (O.bodies[idFoliiStart].state.pos[2] - (O.bodies[idGora].state.pos[2]) + relGora ) > 0:
			O.bodies[idGora].state.vel[2] = 0
			stage =3 
			print "Stage 3"
			print O.time 
			O.materials[2].frictionAngle = frictAngle
			for k in range(idCylinderStart,idCylinderKoniec+1):
					O.bodies[k].mask = 0					
					O.bodies.erase(k)
			
	
######################################################
### Funkcja wlaczajaca cisnienie folii
######################################################
def PressureOn():
	global tmpDtimeToStart, stage
	if stage == 3:
		for k in range(len(O.bodies)-1):
			if O.bodies[k] is None:
				continue
			#print O.bodies[k].isClumpMember
			if O.bodies[k].mask == mask_f:
				if not O.bodies[k].isClumpMember:
					#print O.bodies[k].isClumpMember
					O.bodies[k].state.pressureValue = pressureValue
		tmpDtimeToStart = O.time # czas wlaczenia stage 2 - aby miec poczatek dodawania cisnienia
		stage = 4
		print "Stage 4"
		print O.time
		
	
######################################################
### Funkcja czekajaca na wlaczenie ruchu po wlaczeniu cisnienia folii
######################################################
def PressureTimeOn():
	global tmpDtimeToStart, stage, timeToStartPressure				
	if stage == 4:
		if (timeToStartPressure + tmpDtimeToStart) < O.time:
				stage = 5
				print "Stage 5"
				#kasowanie cylindra wewnatrz folii
				for k in range(idCylinderFoliaStart,idCylinderFoliaKoniec+1):
					O.bodies[k].mask = 0					
					O.bodies.erase(k)
	
				saveIter.engineOn=True
				saveBody.engineOn=True
				print O.time

######################################################
### Funkcja zmieniajaca id kul po przekroczeniu gornej sciany - tak zeby sie nie wysypywaly
######################################################
def ChangeMask():
	global dtimeToStart, stage, tmpDtimeToStart, dtt,resetNeighbour
	if stage ==0:
		for k in range(first,last+1):
			if O.bodies[k].mask == mask_k:
				if (O.bodies[k].state.pos[2]+(1.002*promien_k))<O.bodies[idGora].state.pos[2]:
					O.bodies[k].mask = mask_ku
					O.bodies[k].shape.color=(1,0,0)
					tmpDtimeToStart = O.time #jezeli kula przeskoczyla pobieram aktualny czas
					dtt = O.dt
					
			
		if dtimeToStart > timeToStart:
					#O.bodies[k].mask = 0
					#O.bodies[k].shape.color=(1,1,1)
					stage = 1
					for k in range(first,last+1):
						if O.bodies[k].mask == mask_k:					
							print "Stage 1 koniec usypywania kasuje id "+ str(k) + "mask" + str(O.bodies[k].mask)	
							#O.bodies.erase(O.bodies[k].id)							
					print "Stage 1 " + str(O.time)
					#O.dt = 0.1 * dtt
#					print O.dt
					#O.gravity=O.gravity/mass_scale
		dtimeToStart = O.time - tmpDtimeToStart
		
######################################################
### Funkcja zmieniajaca polozenie gornej sciany
######################################################
def ChangePos():
	global time
	if stage == 5:
		#dS  = amplituda * sin( omega * (time+O.dt)+pi/2) - amplituda * sin( omega * time+pi/2)
		dS  = amplituda * sin( omega * (time+O.dt)+pi) - amplituda * sin( omega * time+pi)
		O.forces.addMove(idGora,(0,0,dS))	
		for k in range(idFoliiStart,idFoliiKoniec+1):
			O.forces.addMove(k,(0,0,dS))
		time = time + O.dt





def ChangePosPila():
        global time,direct,posPila,amplituda
        if stage == 5:
                dz = direct * velPila * O.dt   
                posPila = posPila + dz 
                if posPila < 0 and direct ==-1:
                        direct*=-1
			#O.pause()
                if posPila > amplituda and direct == 1:
                        direct*=-1
			#O.pause()
		
		velZ = direct * velPila
                ds = Vector3(0,0,dz)
                for k in range(idFoliiStart,idFoliiKoniec+1):
                        #O.forces.addMove(k,(ds))
			O.bodies[k].state.vel[2]=velZ
                #O.forces.addMove(idGora,(ds))
		O.bodies[idGora].state.vel[2]=velZ




######################################################
### Funkcja zmieniajaca polozenie gornej sciany
######################################################
def PauseCheck():
	global pauseOn, savedFiles, EndTime
	if O.time > EndTime and pauseOn == True:
		O.pause()
		print("Koniec symulacji, aby uruchomic ponownie zmien EndTime = nowy czas i pauseOn = False") 
	if savedFiles == saveIter.savedFiles:
		O.exitNoBacktrace()
	


print("DT do zapisu")
step = O.dt
PiterSave =500 #ile zapisow w pliku
PtimeSr = 0.005#100*step
PtimeStart = 0.0 #start zapisu
PdeltaTime = (4*(20/8)-(PtimeSr * PiterSave) ) / PiterSave

#sinus (((1/omega)*2*pi) - (100*PtimeSr)) / 100 #1000*step #skok miedzy zapisami

###Zapis wszystkich danych w kazdym kroku
TimeStartAllIter =10000000.26  
DeltaTimeAllIter= step*100

TimeStartAllBody =0.
DeltaTimeAllBody= 3*(PtimeSr + PdeltaTime)



pauseOn = True # jezeli true to wlaczy sie pausa symaulacji - zmiana na false pozwoli na uruchomienie recznie

savedFiles = 2 # ile ma byc wykonanych zapisow EndTime musi byc wiekszy niz czas do zapisow
EndTime = 1000#PtimeStart + IleZapisow *(((PiterSave) * (PdeltaTime+PtimeSr))+PdeltaTime)




	

print("okres")
print((1/omega)*2*pi)

print("PtimeSr")
print(PtimeSr)

print("PdeltaTime")
print(PdeltaTime)


print("koniec zapisu")
print(EndTime)
#########################################


print("Ilosc w symulacji")
print(O.bodies[-1].id - 2)


##########INFO############
info = nazwaPliku +"\n"
info += "Promien probki =" + str(promien_probki) +"\n"
info += "Wysokosc probki = " + str(wysokosc_probki) +"\n"

info += "mass_scale =" + str(mass_scale) +"\n"
info += "promien_k = " + str(promien_k) +"\n"
info += "promien_f = " + str(promien_f) +"\n"

info += "gr_f = " + str(gr_f) +"\n"
info += "young_f = " + str(young_f) +"\n"
info += "poisson_f = " + str(poisson_f) +"\n"
info += "density_f = " + str(density_f) +"\n"

info += "young_k = " + str(young_k) +"\n"
info += "poisson_k = " + str(poisson_k) +"\n"
info += "density_k = " + str(density_k) +"\n"

info += "frictAngle = " + str(frictAngle) +"\n"

info += "Poczatek obreczy  = " + str(idFoliiStart) +"\n"
info += "Koniec obreczy = " + str(idFoliiKoniec) +"\n"
info += "wysokosc fixed" + str(wys_fixed) +"\n"
info += "wysokosc " + str(wys) +"\n"

info += "Poczatek obreczy dol = " + str(idFoliiDolStart) +"\n"
info += "Koniec obreczy dol = " + str(idFoliiDolKoniec) +"\n"

info += "ID sciany gornej "+ str(idGora) +"\n"
info += "ID sciany dolnej "+ str(idDol) +"\n"
info += "Ilosc w symulacji " + str(O.bodies[-1].id - 2) +"\n"
info += "Ilosc kul wygenerowanych w rdzeniu " + str(liczba_kul_dodanych) +"\n"

info += "parametry ruchu sciany gornej\n"
info += "Amplituda"+ str(amplituda) +"\n"
info += "Omega " +str(omega)+"\n"

info += "step " +str(step) +"\n"
info += "PiterSave " +str(PiterSave) +"\n"
info += "PtimeSr " +str(PtimeSr) +"\n"
info += "PtimeStart " +str(PtimeStart) +"\n"
info += "PdeltaTime " +str(PdeltaTime) +"\n"


################################
################################
# FUNCTIONAL COMPONENTS
stage = 0
# simulation loop -- see presentation for the explanation
O.engines=[
	VppSaveInteractions(label='saveIter',fileName=nazwaPlikuIter, iterSave= PiterSave, timeSr = PtimeSr, timeStart = PtimeStart, deltaTime= PdeltaTime,timeStartAll = TimeStartAllIter,deltaTimeAll = DeltaTimeAllIter ),
	VppSave(label='saveBody',info = info, fileName=nazwaPliku, iterSave= PiterSave, timeSr = PtimeSr, timeStart = PtimeStart, deltaTime= PdeltaTime,timeStartAll = TimeStartAllBody,deltaTimeAll = DeltaTimeAllBody,idMinAll=3160, idMaxAll=3197),
   ForceResetter(),
   VppNeighbours(),
   VppPressureForce(),
   PyRunner(command='ChangeMask()',iterPeriod=100),
   PyRunner(command='ChangeTime()',iterPeriod=100),
   PyRunner(command='PreChangePos()',iterPeriod=1),
   PyRunner(command='ChangePosFoil()',iterPeriod=1),
   PyRunner(command='ChangePosPila()',iterPeriod=1),
   PyRunner(command='PressureOn()',iterPeriod=100),
   PyRunner(command='PressureTimeOn()',iterPeriod=100),
   PyRunner(command='PauseCheck()',iterPeriod=100),
   InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=1.4),Bo1_Wall_Aabb(),Bo1_Facet_Aabb()]),
   InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom6D(interactionDetectionFactor=1.4),
		Ig2_Wall_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom6D()],
		[Ip2_FrictMat_FrictMat_FrictPhys(),
		Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=True),
		VppIp2_VppCohFrictMat_VppCohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False),
		VppIp2_CohFrictMat_VppCohFrictMat_CohFrictPhys(setCohesionNow=False,setCohesionOnNewContacts=False)#,
		#VppPressure_VppCohFrictMat_VppCohFrictMat()
		],
		[Law2_ScGeom_FrictPhys_CundallStrack(),
		VppLaw2_ScGeom6D_CohFrictPhys_CohesionMoment(
			useIncrementalForm=False, #useIncrementalForm is turned on as we want plasticity on the contact moments
			always_use_moment_law=True, #if we want "rolling" friction even if the contact is not cohesive (or cohesion is broken), we will have to turn this true somewhere
			shearBreaksCohesion = False,
			label='cohesiveLaw')]
	),
   # apply gravity force to particles
   GravityEngine(gravity=(0,0,-9.81)),
   # damping: numerical dissipation of energy
   NewtonIntegrator(damping=0.2)
]

# set timestep to a fraction of the critical timestep
# the fraction is very small, so that the simulation is not too fast
# and the motion can be observed


##########################URUCHOMIENIE ZAPISU OD RAZU
#saveIter.engineOn=True
#saveBody.engineOn=True



# save the simulation, so that it can be reloaded later, for experimentation
O.saveTmp()
yade.qt.Controller()#, yade.qt.View();
O.run()


