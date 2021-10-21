
#include <TM1637Display.h>
TM1637Display display1(13, 12);
TM1637Display display2(9, 10);

// [ DECLARATIONS ]

// Constants
const byte tmax = 20; // Durée maximale du feu rouge
const byte tmin = 10; // Durée minimale du feu rouge

// Variables
byte i; // Utilisable pour les boucles
byte vr; // Voie du feu rouge (0 = voie A (1+3) || 1 = voie B (2+4))
byte fj; // Feu jaune (0 = off || 1 = on)
unsigned long t1, t2, t3; // Manipulation des délais
word tf; // Durée du feu rouge
word nvmax;  // Nombre maximal de voitures
word nv[4]; // Nombre de voitures dans les 2 voies
boolean o[6]; // Booleéns pour les pushButtons des incrémenteurs/décrémenteurs
byte cmp[6] = { A0, A1, A2, A3, A4, A5 }; // Entrées des incrémenteurs/décrémenteursnt 
/*
 * cmp[0] = incrémenteur 1
 * cmp[1] = incrémenteur 2
 * cmp[2] = décrémenteur 1 + décrémenteur 2
 * cmp[3] = incrémenteur 3
 * cmp[4] = incrémenteur 4
 * cmp[5] = décrémenteur 3 + décrémenteur 4
 */
byte led[8] = { 2,3,4,5,6,7,8,9 }; // Sorties des LEDs
/* 
 * led[0] = R1 + R3
 * led[1] = R2 + R4
 * led[2] = V2 + V4
 * led[3] = V1 + V3
 * led[4] = J2 + J4
 * led[5] = J1 + J3
 * led[6] = PV1 + PR2 + PV3 + PR4
 * led[7] = PR1 + PV2 + PR3 + PV4
 */

// [ FONCTIONS ]

// Verification
word check(word t) 
{
  if (t < 13) return 10;
  else if (t > 57) return 60;
  return t;       
}

// Calcul temps [ Feux vert + jaune ; (rouge) ]
word temps(word v)
{ 
  word tc = 3*v + 2 + 3; // +3 du feu jaune
  if ((tc % 5) > 2) return check(tc - (tc % 5) + 5);
  else return check(tc - (tc % 5));
}

// PushButton ( retourne vrai au front descendant )
boolean pushButton(int b)
{
  int s = digitalRead(cmp[b]);
  if (!s && o[b]) { o[b]= !o[b]; return true; }
  o[b] = s;
  return false;
}

void setup() {

  display1.setBrightness(0x0a);
  display2.setBrightness(0x0a);
  
  for(i = 0; i<6; i++) pinMode(cmp[i], INPUT); // Entrées incrémenteurs/décrémenteurs
  for(i = 0; i<8; i++) { pinMode(led[i], OUTPUT); digitalWrite(led[i], LOW); } // Sorties LEDs

  // [ ETAT INITIAL ]
  vr = 0; // voie 1 + voie 3 = feu rouge
  nvmax = 0;
  // nv = { 0, 0, 0, 0 };
  // o = { 0, 0, 0, 0, 0, 0 };
  tf = 10; // 30 secondes
  fj = 0;
  digitalWrite(led[vr], HIGH);
  digitalWrite(led[vr+2], HIGH);
  digitalWrite(led[vr+6], HIGH);
}

void loop() {

  t2 = millis() - t1;
  nvmax = max(nv[vr], nv[vr+2]);
  display1.showNumberDec(t2/1000);

  if (t2 >= (tf*1000)) 
  {
  t1 = millis();
  tf = temps(nvmax);
  
  // Changement des feux
  digitalWrite(led[!vr], HIGH);
  digitalWrite(led[!vr+2], HIGH);
  digitalWrite(led[!vr+6], HIGH);
  digitalWrite(led[vr], LOW);
  digitalWrite(led[vr+4], LOW);
  digitalWrite(led[vr+6], LOW);
  vr = !vr;
  fj = !fj;
  
  }
  else if ((t2>=((tf*1000)-3000)) && !fj)
  {
    digitalWrite(led[vr+4], HIGH);
    digitalWrite(led[vr+2], LOW);
    fj = !fj;
  }

  // [ INCREMENTEURS / DECREMENTEURS ]
  
  // Incrémenteur 1

  if (pushButton(0)) nv[0] ++;
  
  // Incrémenteur 2
  if (pushButton(1)) nv[1] ++;
  
  // Décrémenteur 1 + 2
  if (pushButton(2)) if (nv[!vr]>0) nv[!vr] --;
  
  // Incrémenteur 3
  if (pushButton(3)) nv[2] ++;
  
  // Incrémenteur 4
  if (pushButton(4)) nv[3] ++;
  
  // Décrémenteur 3 + 4
  if (pushButton(5)) if (nv[!vr+2]>0) nv[!vr+2] --;


} 
