enum PITCH_SETS {CHROMATIC, DIATONIC, PENTATONIC, SEVENTH, TRIAD, FIFTH};
enum GLIDE_MODE_SET {ROOT, MAIN_GLIDE, OFFSET_GLIDE};

//name for serial monitoring
#define IDNAME(name) #name
const String pitchSetNames[] = {"CHROMATIC", "DIATONIC", "PENTATONIC", "SEVENTH", "TRIAD", "FIFTH"};
const String glideModeNames[] = {"ROOT", "MAIN_GLIDE", "OFFSET_GLIDE"};

byte pitchClasses[2][61]; //5 octaves + 1
PITCH_SETS curPitchSet[2] = {DIATONIC, DIATONIC};
GLIDE_MODE_SET glideMode = ROOT;
int glideTime[2] = {0,0};
int curRoot = 0;
int newRoot = 0;
byte newRootFlag = 0;

//scale definitions
const byte CHROMATIC_SCALE[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
const byte DIATONIC_SCALE[] = {0,2,4,5,7,9,11,12};
const byte PENTATONIC_SCALE[] = {0,1,2,4,5,7}; //index into diatonic scale
const byte SEVENTH_SCALE[] = {0,2,4,6,7}; ////index into diatonic scale
const byte TRIAD_SCALE[] = {0,2,4,7}; //index into diatonic scale
const byte FIFTH_SCALE[] = {0,4,7}; //index into diatonic scale

/* fillPitchClasses()
 *  - deprecated
 *  - orignally the idea was to keep octaves divided into 12 parts
 *  - and place scale interval changes at their respectavie position based on 12tet 
 *  - but was switched to spread interval changes evenly across octave
 *  - in order to facilitate both finding intervals as well as more regular 
 *  - interval changes when using e.g. ramps and triangle LFOs
 */
void fillPitchClasses(){
  for(int i=0;i<2;i++){
     byte curOctave = 0;
     byte curInterval = 0;
     Serial.println("scale " +String(i));
     byte scaleSize = sizeof(CHROMATIC_SCALE)-1;
     if(curPitchSet[i] == DIATONIC) scaleSize = sizeof(DIATONIC_SCALE)-1;
     else if(curPitchSet[i] == PENTATONIC) scaleSize = sizeof(PENTATONIC_SCALE)-1;
     else if(curPitchSet[i] == SEVENTH) scaleSize = sizeof(SEVENTH_SCALE)-1;
     else if(curPitchSet[i] == TRIAD) scaleSize = sizeof(TRIAD_SCALE)-1;

     if ( SERIAL_DEBUG) Serial.print(String(curPitchSet[i]) + ": ");
     
    for(int j=0;j<61;j++) {
      curOctave = floor(j/12);
      
      switch(curPitchSet[i]){
        case CHROMATIC:
        pitchClasses[i][j] = j%12;
        break;
  
        case DIATONIC:
        if((j%12) < DIATONIC_SCALE[curInterval+1]) pitchClasses[i][j] = curInterval;
        else pitchClasses[i][j] = ++curInterval;
        break;

        case PENTATONIC:
        if((j%12) < DIATONIC_SCALE[PENTATONIC_SCALE[curInterval+1]]) pitchClasses[i][j] = PENTATONIC_SCALE[curInterval];
        else pitchClasses[i][j] = PENTATONIC_SCALE[++curInterval];
        break;

        case SEVENTH:
        if((j%12) < DIATONIC_SCALE[SEVENTH_SCALE[curInterval+1]]) pitchClasses[i][j] = SEVENTH_SCALE[curInterval];
        else pitchClasses[i][j] = SEVENTH_SCALE[++curInterval];
        break;

        case TRIAD:
        if((j%12) < DIATONIC_SCALE[TRIAD_SCALE[curInterval+1]]) pitchClasses[i][j] = TRIAD_SCALE[curInterval];
        else pitchClasses[i][j] = TRIAD_SCALE[++curInterval];
        break;

        case FIFTH:
        if((j%12) < DIATONIC_SCALE[FIFTH_SCALE[curInterval+1]]) pitchClasses[i][j] = FIFTH_SCALE[curInterval];
        else pitchClasses[i][j] = FIFTH_SCALE[++curInterval];
        break;
      }
      pitchClasses[i][j] = pitchClasses[i][j] + curOctave*7;
      if ( SERIAL_DEBUG) Serial.print(String(pitchClasses[i][j]) + " ");
      if(j%12 == 11)  curInterval = 0;
    }
    if ( SERIAL_DEBUG) Serial.println("----");
  }
}//fillPitchClasses

void setCurScale(byte channel, PITCH_SETS scale){
  curPitchSet[channel]=scale;
  if ( SERIAL_DEBUG) Serial.println("_____scale " + pitchSetNames[curPitchSet[channel]]);
  fillPitchClasses();
}

byte getScaleSize(byte num, PITCH_SETS scale){
  byte scaleSize = sizeof(CHROMATIC_SCALE)-1;
     if(curPitchSet[num] == DIATONIC) scaleSize = sizeof(DIATONIC_SCALE)-1;
     else if(curPitchSet[num] == PENTATONIC) scaleSize = sizeof(PENTATONIC_SCALE)-1;
     else if(curPitchSet[num] == SEVENTH) scaleSize = sizeof(SEVENTH_SCALE)-1;
     else if(curPitchSet[num] == TRIAD) scaleSize = sizeof(TRIAD_SCALE)-1;
     return scaleSize;
}

int getChromaticPitch(byte num, int val){
  int output = 0;
  int size = getScaleSize(num,curPitchSet[num]);
  int degree = val%size;
  int octave = val/size;
  
  //int quantValChromatic = DIATONIC_SCALE[quantValPitch%7] + quantValPitch/7*12; //convert to chromatic
    
  switch(curPitchSet[num]){
    case CHROMATIC: output=val; break;
    case DIATONIC: output= DIATONIC_SCALE[degree]; break;
    case PENTATONIC: output = DIATONIC_SCALE[PENTATONIC_SCALE[degree]]; break;
    case SEVENTH: output = DIATONIC_SCALE[SEVENTH_SCALE[degree]]; break;
    case TRIAD: output = DIATONIC_SCALE[TRIAD_SCALE[degree]]; break;
    case FIFTH: output = DIATONIC_SCALE[FIFTH_SCALE[degree]]; break;
  }
  return output + octave*12;
}

/* readCV()
 *  main quantization function
 *  analyzes incoming CV to generate output quantized CV
 * 
 */
void readCV(){
    static int cvInput[2] = {0,0};

    //main CV for quantization
    int input = analogRead(cv2);
    if(abs(input-cvInput[0])>5) cvInput[0] = input; //ignore noise
    int quantValPitch = map(cvInput[0], 42, 600, 0, 4000); //scale to 4 octaves
    quantValPitch = constrain(quantValPitch,0,4000);
    int degreeBins = 1000/getScaleSize(0,curPitchSet[0]);
    //check for new note to apply new root
    static int rootPitch = 0;
    static byte prevPitch = 0;
    if( quantValPitch/degreeBins != prevPitch){
      prevPitch = quantValPitch/degreeBins;
      curRoot = newRoot;
      rootPitch = curRoot/135; //outputs 0-7
    }
    //quantValPitch += (curRoot);
    analogWrite(led[3],quantValPitch/40);
    //get diatonic pitch
    quantValPitch = constrain((quantValPitch%1000)/degreeBins,0,6) + getScaleSize(0,curPitchSet[0]) * (quantValPitch/1000);
    int quantValChromatic = getChromaticPitch(0, quantValPitch);

    //cv for offset
    //we need to know if there is a new offset in order to trigger glide...
    byte newOffsetFlag = 0;
    static int prevOffsetVal = 0;
    input = analogRead(cv1);
    if(abs(input-cvInput[1])>5) cvInput[1] = input;
    int offsetVal = map(cvInput[1], 50, 800, 0, 1000);
    offsetVal = constrain(offsetVal,0,1000);
    analogWrite(led[2],constrain(offsetVal>>4,0,255));
    offsetVal = pitchClasses[1][byte(floor(offsetVal/83))];
    if(offsetVal != prevOffsetVal){
      prevOffsetVal = offsetVal;
      newOffsetFlag = 1;
    }
    offsetVal += rootPitch;
    int offsetValPitch = quantValPitch;
    if(curPitchSet[0] == DIATONIC) offsetValPitch += offsetVal;
    else if(curPitchSet[0] == PENTATONIC) offsetValPitch = PENTATONIC_SCALE[quantValPitch%5] + (quantValPitch/5)*7  + offsetVal;
    else if(curPitchSet[0] == SEVENTH) offsetValPitch = SEVENTH_SCALE[quantValPitch%4] + (quantValPitch/4)*7  +offsetVal;
    else if(curPitchSet[0] == TRIAD) offsetValPitch = TRIAD_SCALE[quantValPitch%3] + (quantValPitch/3)*7  +offsetVal;
    else if(curPitchSet[0] == FIFTH) offsetValPitch = FIFTH_SCALE[quantValPitch%2] + (quantValPitch/2)*7  +offsetVal;
    int offsetValChromatic = getChromaticPitch(1, offsetValPitch); //convert to chromatic
    offsetValChromatic = offsetValChromatic - quantValChromatic;

    //calculate output pitch
    int pitchVal[] = {quantValChromatic,offsetValChromatic};
    static int curPitch[2] = {-1,-1};
    //Serial.println(String(curPitch[0]) + " " +  String(curPitch[1]));
    for(int i=0;i<2;i++){
      if(pitchVal[i] != curPitch[i]){
        if ( SERIAL_DEBUG) Serial.println("new Pitch " + String(i) + " " + String(pitchVal[i]) + " " + String(curPitch[i]));
        curPitch[i] = pitchVal[i];
        if(i==0) env.AR(attackTime,decayTime);

        //write new pitches as CV
        if(i==0)quantized.cv(map(curPitch[i],0,60,0,1023),glideTime[0]);
        else if(i==1 && newOffsetFlag == 1) {
          newOffsetFlag = 0;
          quantizeOffset.cv(map(curPitch[i],0,60,0,1023)+512, glideTime[1]);
        } 
        else if(i==1)  quantizeOffset.cv(map(curPitch[i],0,60,0,1023)+512, 0);
      }
    }
  
}
