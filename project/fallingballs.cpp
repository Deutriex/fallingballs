/**********************
 * Iago Brunherotto   *
 * RA: 22112047-0     *
 * Visual Studio 2008 *
 **********************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <sstream>
#include <iostream>
#include <algorithm> 
#include <deque>
#include <windows.h>
#include <gl.h>
#include <glu.h>
#include <glut.h>

using namespace std;

typedef void (*voidFn)();

typedef struct {
	float red;
	float green;
	float blue;
	float alpha;
} RGBColor;

typedef struct {
	float tempo;
	float tempoLimite;
} deltaTime;

deltaTime deltaT;
int bonusTimeMax;

typedef struct {
	int baseSpawnThreshold;
	int incStep;
} whiteBallSettings;

whiteBallSettings curDifficultyWB;
int whiteBallSpawnThreshold;
float slownessFactor;
int maximumFXDuration;
int defaultNoBalls;
int lsrFactor;

void timeFlowOut(int);
char * intToString(int);
void resumeOut();
void mainMenuOut();
void playOut();
void setEasyOut();
void setStandardOut();
void setHardOut();


float boundedNumber(float min, float num, float max){
	if (num < min) return min;
	if (num > max) return max;
	return num;
}

float maxBetween(float num1, float num2){
	if (num1 > num2) return num1;
	return num2;
}

class Programa;

deque<RGBColor> AritsuNoKaraa;


class Camera {
private:
	float eyex;
	float eyey;
	float eyez;
	float ctrx;
	float ctry;
	float ctrz;
	float upx;
	float upy;
	float upz;
public:
	Camera(float eyex, float eyey, float eyez, float ctrx, float ctry, float ctrz, float upx, float upy, float upz){
		this->eyex = eyex;
		this->eyey = eyey;
		this->eyez = eyez;
		this->ctrx = ctrx;
		this->ctry = ctry;
		this->ctrz = ctrz;
		this->upx = upx;
		this->upy = upy;
		this->upz = upz;
	}

	float getEyeX(){
		return eyex;
	}

	float getEyeY(){
		return eyey;
	}

	float getEyeZ(){
		return eyez;
	}

	float getCtrX(){
		return ctrx;
	}

	float getCtrY(){
		return ctry;
	}

	float getCtrZ(){
		return ctrz;
	}

	float getUpX(){
		return upx;
	}

	float getUpY(){
		return upy;
	}

	float getUpZ(){
		return upz;
	}
};

class RectContainer {
private:
	int x;
	int y;
	int h;
	int w;
public:
	RectContainer(int x, int y, int w, int h){
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	int getX(){
		return this->x;
	}

	int getY(){
		return this->y;
	}

	int getW(){
		return this->w;
	}

	int getH(){
		return this->h;
	}

	bool inBoundaries(int x, int y){
		return (x >= this->getX() && x <= this->getX()+this->getW()) && (y >= this->getY() && y <= this->getY()+this->getH());
	}

};

class ActionWord : public RectContainer {
private:
	char * word;
	void (*actionOnClick)();
	int state;
	char * flag;
	int currentWord;
	int z;

	deque<ActionWord *> actionWords;
	RGBColor * colorSch;

	void commomConstructor(char * word, void (*actionOnClick)()){
		this->word = word;
		this->actionOnClick = actionOnClick;
		this->state = 0;
		this->currentWord = 0;

		this->colorSch = (RGBColor *) malloc(sizeof(RGBColor)*3);
		
		RGBColor totalBlack;
		totalBlack.red = 0;
		totalBlack.green = 0;
		totalBlack.blue = 0;
		totalBlack.alpha = 1;

		this->colorSch[0] = totalBlack;
		this->z = -2147483648;

	}

	void DesenhaTexto(char *string, int x, int y, RGBColor fgColor){ 
		if (string == NULL) return;

		glPushMatrix();
		glColor4f(fgColor.red, fgColor.green, fgColor.blue, fgColor.alpha);
		glRasterPos2f(x, y+20);
		while( *string) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*string++); 
		glPopMatrix();
	} 

	void DesenhaTexto3D(char *string, int x, int y, int z, RGBColor fgColor){ 
		if (string == NULL) return;

		glPushMatrix();
		glColor4f(fgColor.red, fgColor.green, fgColor.blue, fgColor.alpha);
		glRasterPos3f(x, y+20, z);
		while( *string) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*string++); 
		glPopMatrix();
	} 

public:
	ActionWord(char * word, void (*actionOnClick)()) : RectContainer(0, 0, 0, 0) {
		this->commomConstructor(word, actionOnClick);
		this->flag = NULL;
	}

	ActionWord(char * word, void (*actionOnClick)(), int x, int y, int h, int w, char * flag) : RectContainer(x, y, h, w) {
		this->commomConstructor(word, actionOnClick);
		this->flag = flag;
	}

	ActionWord(deque<ActionWord *> actionWords, int x, int y, int h, int w, char * flag) : RectContainer(x, y, h, w) {
		char * word = actionWords[0]->getWord();
		this->actionWords = actionWords;
		this->actionOnClick = actionWords[0]->getCallBack();
		this->commomConstructor(word , actionOnClick);
		this->flag = flag;
	}

	ActionWord(char * word, int x, int y) : RectContainer(x, y, 0, 0){
		this->word = word;
		this->actionOnClick = NULL;
		this->commomConstructor(word, NULL);
	}

	ActionWord(char * word, int x, int y, int z) : RectContainer(x, y, 0, 0){
		this->word = word;
		this->actionOnClick = NULL;
		this->commomConstructor(word, NULL);
		this->z = z;
	}

	int getZ(){
		return this->z;
	}

	void setCallBack(void (*actionOnClick)()){
		this->actionOnClick = actionOnClick;
	}

	char * getWord(){
		return this->word;
	}

	char * getFlag(){
		return this->flag;
	}

	void setState(int state){
		this->state = state;
	}

	int getState(){
		return this->state;
	}

	voidFn getCallBack(){
		return this->actionOnClick;
	}

	void runCallBack(){
		if (this->actionOnClick == NULL) return;

		if (actionWords.size() != 0){
			this->currentWord = (this->currentWord+1)%this->actionWords.size();
			this->word = this->actionWords[currentWord]->getWord();
			this->actionOnClick = this->actionWords[currentWord]->getCallBack();
		}

		(*this->actionOnClick)();
	}

	void render(){
		RGBColor cor;

		if (this->actionOnClick != NULL && (this->state == 1 || this->state == 2)){

			cor.red = 1;
			cor.green = 1;
			cor.blue = 1;
			cor.alpha = 1;


			if (this->state == 1) glColor3f(0.5, 0.5, 0.5);
			if (this->state == 2) glColor3f(0, 0, 0);

			glBegin(GL_QUADS);
				glVertex3i(this->getX(),				this->getY()				,0);
				glVertex3i(this->getX(),				this->getY()+this->getH()	,0);
				glVertex3i(this->getX()+this->getW(),	this->getY()+this->getH()	,0);
				glVertex3i(this->getX()+this->getW(),	this->getY()				,0);
			glEnd();

		} else {
			cor = this->colorSch[0];
		}

		if (this->z == -2147483648) this->DesenhaTexto(this->word, this->getX(), this->getY(), cor);
		else this->DesenhaTexto3D(this->word, this->getX(), this->getY(), this->getZ(), cor);
	}

	void setColorScheme(int n, RGBColor color){
		if (n >= 0 && n <= 2){
			this->colorSch[n] = color;
		}
	}
};


class WordsArray {
private:
	deque<ActionWord *> actionWords;
	deque<int> iterators;

	WordsArray(){
		iterators.push_back(0);
	}

	int active;
	char * activeFlag;
public:
	static WordsArray * getInstance(){
		static WordsArray S;
		return &S;
	}

	int addIterator(){
		this->iterators.push_back(0);
		return this->iterators.size()-1;
	}

	void addWord(ActionWord * aw){
		this->actionWords.push_back(aw);
	}

	int firstWord(int fromIterator){
		this->iterators[fromIterator] = 0;
		return 0;
	}

	bool reachedLimit(int fromIterator){
		return actionWords.size() == this->iterators[fromIterator];
	}

	int nextWord(int fromIterator){

		do {
			this->iterators[fromIterator] += 1;
		} while (!this->reachedLimit(fromIterator) && strcmp(this->activeFlag, this->actionWords[this->iterators[fromIterator]]->getFlag()) != 0);

		return this->iterators[fromIterator];
	}


	ActionWord * getWordObject(int fromIterator){
		return this->actionWords[this->iterators[fromIterator]];
	}

	void activate(char * flag){
		this->active = 1;
		this->activeFlag = flag;
		int i;
		ActionWord * aw;

		for (i = 0; i < this->actionWords.size(); i++){
			aw = this->actionWords[i];
			if (strcmp(aw->getFlag(), this->activeFlag) == 0){
				aw->render();
			}
		}

	}

	void deactivate(){
		this->active = 0;
	}

	int isActive(){
		return this->active;
	}

};

WordsArray * wArray;

class Difficulty {
private:
	int spawnThreshold;
	int maxBallLength;
	int minSpeed;
	int maxSpeed;
	int bonusMaxTime;
	char * label;
	int baseWhiteBallSpawnThreshold;
	int whiteBallSpawnThresholdIncStep;
	int xtTimeStep;
	int plusBallsStep; 
public:
	Difficulty(){
		this->label = "Acabucuru";
		this->maxBallLength = 1;
		this->spawnThreshold = 0;
		this->minSpeed = 15;
		this->maxSpeed = 240;
	}

	Difficulty(char * label, int maxBallLength, int spawnThreshold, float minSpeed, float maxSpeed, int bonusMaxTime, int baseWhiteBallSpawnThreshold, int whiteBallSpawnThresholdIncStep, int xtTimeStep, int plusBallsStep){
		this->label = label;
		this->maxBallLength = maxBallLength;
		this->spawnThreshold = spawnThreshold;
		this->minSpeed = minSpeed;
		this->maxSpeed = maxSpeed;
		this->bonusMaxTime = bonusMaxTime;
		this->baseWhiteBallSpawnThreshold = baseWhiteBallSpawnThreshold;
		this->whiteBallSpawnThresholdIncStep = whiteBallSpawnThresholdIncStep;
		this->xtTimeStep = xtTimeStep;
		this->plusBallsStep = plusBallsStep;
	}

	int getPlusBallsStep(){
		return this->plusBallsStep;
	}

	int getXtTimeStep(){
		return this->xtTimeStep;
	}

	int getSpawnThreshold(){
		return this->spawnThreshold;
	}

	int getMaxBallLength(){
		return this->maxBallLength;
	}

	int getMinSpeed(){
		return this->minSpeed;
	}

	int getMaxSpeed(){
		return this->maxSpeed;
	}

	char * getLabel(){
		return this->label;
	}

	int getBonusMaxTime(){
		return this->bonusMaxTime;
	}

	int getBaseWhiteBallSpawnThreshold(){
		return this->baseWhiteBallSpawnThreshold;
	}

	int getWhiteBallSpawnThresholdIncStep(){
		return this->whiteBallSpawnThresholdIncStep;
	}
};

class ScoreData {
private:
	char date[50];
	char difficulty[50];
	int score;
public:
	ScoreData(char * date, char * difficulty, int score){
		strcpy(this->date, date);
		strcpy(this->difficulty, difficulty);
		this->score = score;
	}

	char * getDate(){
		return this->date;
	}

	char * getDifficulty(){
		return this->difficulty;
	}

	int getScore(){
		return this->score;
	}
};



class ScoreDataArray {
private:
	deque<ScoreData *> ScoreDataE;
	deque<ScoreData *> ScoreDataS;
	deque<ScoreData *> ScoreDataH;
	char difficulty[50];

	void renderHelper(deque<ScoreData *> sdArg){
		int i;
		int getMax = (int) boundedNumber(-1, sdArg.size(), 15);

		for (i = 0; i < getMax; i++){
			ActionWord * aw = new ActionWord(sdArg[i]->getDate(), 400, 40+25*i);
			aw->render();

			aw = new ActionWord(intToString(sdArg[i]->getScore()), 650, 40+25*i);
			aw->render();
		}
	}

public:
	ScoreDataArray (){
		
	}

	void setDifficulty(char * difficulty){
		strcpy(this->difficulty, difficulty);
	}

	char * getDifficulty(){
		return this->difficulty;
	}

	void addScoreData(ScoreData * novo){
		if (strcmp(novo->getDifficulty(), "Easy") == 0){
			this->ScoreDataE.push_back(novo);
		}

		if (strcmp(novo->getDifficulty(), "Standard") == 0){
			this->ScoreDataS.push_back(novo);
		}

		if (strcmp(novo->getDifficulty(), "Hard") == 0){
			this->ScoreDataH.push_back(novo);
		}
	}

	void reset(){
		ScoreDataE.clear();
		ScoreDataS.clear();
		ScoreDataH.clear();
	}


	void sortData(){
		struct comp {
			bool operator ()(ScoreData * a,  ScoreData * b){              
				return a->getScore() > b->getScore();        
			} 
		} funcione;

		sort(this->ScoreDataE.begin(), this->ScoreDataE.end(), funcione);
		sort(this->ScoreDataS.begin(), this->ScoreDataS.end(), funcione);
		sort(this->ScoreDataH.begin(), this->ScoreDataH.end(), funcione);
	}

	void render(){
		if (strcmp(this->getDifficulty(), "Easy") == 0){
			this->renderHelper(ScoreDataE);
		}

		if (strcmp(this->getDifficulty(), "Standard") == 0){
			this->renderHelper(ScoreDataS);
		}

		if (strcmp(this->getDifficulty(), "Hard") == 0){
			this->renderHelper(ScoreDataH);
		}
	}
};

ScoreDataArray * sdArray;



class Torus {
private:
	float relX;
	float relZ;
	int sensitivity;
	int innerBorderSize;
	int outerBorderSize;
	float tempo;
	float textFadeoutTime;
	float blackholeFadeinTime;
	float blackholeFadeoutTime;
	float AritsuFadeinTime;
	float AritsuFadeoutTime;
	int state;
	RGBColor color;
	char * text;
	int sizeLevel;
	bool gravDistortion;
	int gravDistortionLevel;
	bool AritsuColor;
	deque<RGBColor> AritsuNoKaraaT;

	Torus(){
		this->relX = 0;
		this->relZ = 0;
		this->sensitivity = 250;
		this->innerBorderSize = 30;
		this->outerBorderSize = 5;
		color.red = 1;
		color.green = 1;
		color.blue = 1;
		color.alpha = 1;
		this->text = (char*) malloc(sizeof(char)*20);
		strcpy(text, "");
		this->sizeLevel = 0;
		this->tempo = 0;
		this->gravDistortion = false;
		this->gravDistortionLevel = 0;
		this->AritsuColor = true;
		this->blackholeFadeinTime = 0;
		this->blackholeFadeoutTime = -1;
		this->AritsuFadeinTime = 0;
		this->AritsuFadeoutTime = -1;


		RGBColor red;
		red.red = 1;
		red.green = 0;
		red.blue = 0;
		AritsuNoKaraaT.push_back(red);

		RGBColor yellowishOrange;
		yellowishOrange.red = 1;
		yellowishOrange.green = 0.75;
		yellowishOrange.blue = 0;
		AritsuNoKaraaT.push_back(yellowishOrange);

		RGBColor green;
		green.red = 0;
		green.green = 1;
		green.blue = 0;
		AritsuNoKaraaT.push_back(green);

		RGBColor skyblue;
		skyblue.red = 0;
		skyblue.green = 0.5;
		skyblue.blue = 1;
		AritsuNoKaraaT.push_back(skyblue);

		RGBColor purple;
		purple.red = 0.75;
		purple.green = 0;
		purple.blue = 0.75;
		AritsuNoKaraaT.push_back(purple);

	}

	RGBColor getAritsuColorHelper(int i){
		float getPal = 3*this->tempo+i;
		while (getPal >= 5) getPal -= 5;
		RGBColor resC;
		int s = (int) getPal;
		int t = (s+1)%5;
		float getScale = getPal-s;

		resC.red = (1-getScale)*AritsuNoKaraaT[s].red + getScale*AritsuNoKaraaT[t].red;
		resC.green = (1-getScale)*AritsuNoKaraaT[s].green + getScale*AritsuNoKaraaT[t].green;
		resC.blue = (1-getScale)*AritsuNoKaraaT[s].blue + getScale*AritsuNoKaraaT[t].blue;
	
		return resC;
	}

public:
	static Torus * getInstance(){
		static Torus instance;
		return &instance;
	}

	bool isAritsuColorOn(){
		return this->AritsuColor;
	}

	void setAritsuColor(){
		this->AritsuColor = true;
		this->AritsuFadeinTime = this->tempo;
		this->AritsuFadeoutTime = 2147483647;
	}

	void clearAritsuColor(){
		this->AritsuColor = false;
		this->AritsuFadeoutTime = this->tempo+1;
	}

	void setGravDistortionLevel(int newL){
		this->gravDistortionLevel = newL;
	}

	void toggleGravDistortion(){
		this->gravDistortion = !this->gravDistortion;
	}

	void setGravDistortion(){
		this->gravDistortion = true;
		this->blackholeFadeinTime = this->tempo;
		this->blackholeFadeoutTime = 2147483647;
	}

	void clearGravDistortion(bool withFadeOut){
		this->gravDistortion = false;
		if (withFadeOut){
			this->blackholeFadeoutTime = this->tempo+1;
		} else {
			this->blackholeFadeinTime = -1;
			this->blackholeFadeoutTime = -2;
		}
	}

	float getTempo(){
		return this->tempo;
	}

	void setTempo(float newT){
		this->tempo = newT;
	}

	void setSizeLevel(int newL){
		this->sizeLevel = newL;
	}

	void setText(char * newT){
		this->text = newT;
		this->textFadeoutTime = this->tempo+3;
	}

	void setColor(RGBColor newC){
		this->color = newC;
	}

	void setRelCoords(float x, float z){
		this->relX = x;
		this->relZ = z;
	}

	int getInnerBorderSize(){
		if (sizeLevel == 0){
			return this->innerBorderSize;
		} else {
			return this->innerBorderSize+15*this->sizeLevel;
		}
	}

	int getOuterBorderSize(){
		return this->outerBorderSize;
	}

	int getX(){
		return this->sensitivity*this->relX;
	}

	int getZ(){
		return this->sensitivity*this->relZ;
	}

	void moveUp(){
		this->relZ = boundedNumber(-1, this->relZ-0.1, 1);
	}

	void moveDown(){
		this->relZ = boundedNumber(-1, this->relZ+0.1, 1);
	}

	void moveLeft(){
		this->relX = boundedNumber(-1, this->relX-0.1, 1);
	}

	void moveRight(){
		this->relX = boundedNumber(-1, this->relX+0.1, 1);
	}

	void render(){
		glColor4f(color.red, color.green, color.blue, color.alpha);

		glPushMatrix();
		glTranslatef(this->getX(), 0, this->getZ());
		glRotatef(-90, 1, 0, 0);
		glutSolidTorus(this->getOuterBorderSize(), this->getInnerBorderSize(), 32, 32);
		glPopMatrix();

		int i;
		int j;

		float fadeOutAlpha = this->blackholeFadeoutTime-this->tempo;

		if (this->gravDistortion || fadeOutAlpha > 0){

			float fadeInAlpha = this->tempo-this->blackholeFadeinTime;

			float alphaFadeIn = boundedNumber(0, fadeInAlpha, 1);
			float alphaFadeOut = boundedNumber(0, fadeOutAlpha, 1);

			for (i = 0; i < 5; i++){


				float alphaT = 0.5+0.5*sin(4*this->tempo);
				float alphaFunc = (0.5-(0.05*i))*alphaFadeIn*alphaFadeOut*alphaT;

				if (!this->AritsuColor || this->AritsuFadeoutTime <= 0){
					glColor4f(color.red, color.green, color.blue, alphaFunc);
				} else {
					RGBColor resC = getAritsuColorHelper(i);
					RGBColor resC2;
					float alphaA = alphaFadeIn*alphaFadeOut;
					resC2.red = (1-alphaA)*1 + alphaA*resC.red;
					resC2.green = (1-alphaA)*1 + alphaA*resC.green;
					resC2.blue = (1-alphaA)*1 + alphaA*resC.blue;

					glColor4f(resC2.red, resC2.green, resC2.blue, alphaFunc);

				}


				glPushMatrix();
				int aniFlow = 4*this->gravDistortionLevel*this->tempo;
				glTranslatef(this->getX(), (i+1)*20 - aniFlow%20, this->getZ());
				glRotatef(-90, 1, 0, 0);
				glutWireTorus(this->getOuterBorderSize()*(1+0.1*i), this->getInnerBorderSize()*(1+0.1*i), 8, 32);

				glPopMatrix();
			}

			for (j = 0; j < 5; j++){
				for (i = 0; i < 6; i++){

					float alphaFunc = (1-0.2*j)*alphaFadeIn*alphaFadeOut;

					glPushMatrix();
					float theta = (2*3.14159)*((i+this->gravDistortionLevel*this->tempo+0.2*j)/6.0);
					float radius = this->getInnerBorderSize() + this->getOuterBorderSize() + (2.0/3)*this->getInnerBorderSize()*(1+j);

					if (!this->AritsuColor || this->AritsuFadeoutTime <= 0){
						glColor4f(1, 1, 1, alphaFunc);
					} else {
						RGBColor resC = getAritsuColorHelper(i);
						RGBColor resC2;
						float alphaA = alphaFadeIn*alphaFadeOut;
						resC2.red = (1-alphaA)*1 + alphaA*resC.red;
						resC2.green = (1-alphaA)*1 + alphaA*resC.green;
						resC2.blue = (1-alphaA)*1 + alphaA*resC.blue;

						glColor4f(resC2.red, resC2.green, resC2.blue, alphaFunc);
					}

					float x = this->getX() + radius*cos(theta);
					float z = this->getZ() + radius*sin(theta);
					glTranslatef(x, 0, z);

					glutSolidSphere(((1.0/3)*this->getInnerBorderSize())*((5-j)/5.0), 32, 32);

					glPopMatrix();
				}
			}

		}


		float RTime = boundedNumber(0, this->textFadeoutTime-this->tempo, 3);
		float getScale = RTime/3;
		float flashAlpha = sin(3.14159*getScale);

		RGBColor white;
		white.alpha = flashAlpha;
		white.red = 1;
		white.green = 1;
		white.blue = 1;


		ActionWord * aw = new ActionWord(text, 0.5*this->getX()+40, 0, this->getZ());
		aw->setColorScheme(0, white);
		aw->render();
	}

};

Torus * torus;

class Ball {
private:
	int x;
	int z;
	RGBColor color;
	float iniSpace;
	float iniSpeed;
	float acceleration;
	int state;
	float tempo;
	int active;
	int sensitivity;
	int radius;
	Difficulty * difficultyObj;
	bool overTorus;
	int slownessLevel;
public:

	void spawn(){
		this->tempo = 0;
		this->state = 0;
		this->iniSpace = 242;
		this->iniSpeed = this->difficultyObj->getMinSpeed()+rand()%(this->difficultyObj->getMaxSpeed()-this->difficultyObj->getMinSpeed());
		this->acceleration = 5;

		int AritsuNoAlea = rand()%100;
		
		if (AritsuNoAlea < whiteBallSpawnThreshold) this->color = AritsuNoKaraa[AritsuNoKaraa.size()-1]; // white
		else this->color = AritsuNoKaraa[rand()%(AritsuNoKaraa.size()-1)];

		this->x = -this->sensitivity+rand()%(1+2*this->sensitivity);
		this->z = -this->sensitivity+rand()%(1+2*this->sensitivity);
		this->radius = 20;
		this->overTorus = false;
	}

	Ball(Difficulty * difficultyObj){
		this->sensitivity = 250 - 40*lsrFactor;
		this->difficultyObj = difficultyObj;
		if (difficultyObj != NULL){
			this->spawn();
		}
		this->active = 0;
		this->slownessLevel = 0;
	}

	void setSlownessLevel(int newL){
		this->slownessLevel = newL;
		this->setIniSpace(this->getBallSpaceEquation());
		this->setTempo(0);
	}

	void resetSlownessLevel(){
		this->slownessLevel = 0;
	}

	RGBColor getColor(){
		return this->color;
	}

	void setColor(RGBColor color){
		if (this != NULL){
			this->color = color;
		}
	}

	void setRadius(int radius){
		this->radius = radius;
	}

	float getBallSpaceEquation(){
		return this->iniSpace-(this->iniSpeed)*(this->tempo)-(this->acceleration)*pow(this->tempo,2);
	}

	double getEstimatedLandTime(){
		double a = -this->acceleration;
		double b = -this->iniSpeed;
		double c = this->iniSpace;

		return (-b - sqrt(b*b - 4*a*c))/(2*a);
	}

	void setIniSpace(float newIniSpace){
		this->iniSpace = newIniSpace;
	}

	int getX(){
		return this->x;
	}

	int getZ(){
		return this->z;
	}

	void setCoordinates(int x, int z){
		this->x = x;
		this->z = z;
	}

	int getActive(){
		return this->active;
	}

	void setActive(int active){
		this->active = active;
	}

	void setAcceleration(float acceleration){
		this->acceleration = acceleration;
	}

	void setSpeed(float speed){
		this->setIniSpace(this->getBallSpaceEquation());
		this->iniSpeed = speed;
		this->setTempo(0);
	}

	int getSpeed(){
		return this->iniSpeed*(1-0.16*this->slownessLevel) + this->acceleration*this->tempo;
	}
	
	float getTempo(){
		return this->tempo;
	}

	void setTempo(float tempo){
		this->tempo = tempo;
	}

	float getAlpha(){
		float alpha1 = boundedNumber(0, deltaT.tempoLimite-deltaT.tempo, 1);
		float alpha2 = boundedNumber(0, 1+getBallSpaceEquation()/50.0, 1);
		float alpha;

		if (alpha1 == 1){
			alpha = alpha2;
		} else {
			alpha = (alpha1+alpha2-abs(alpha1-alpha2))/2;
		}


		return alpha;
	}

	void render(){
		if (this->active){


			glPushMatrix();

			glTranslatef(this->x, this->getBallSpaceEquation(), this->z);
			if (this->state == 1){

				float sizeFactor = 1-this->getAlpha();
				float alphaFactor = sin(this->getAlpha()*3.14159);

				glColor4f(this->color.red, this->color.green, this->color.blue, alphaFactor);

				//glPushMatrix();
				glRotatef(-90, 1, 0, 0);
				glRotatef(360*this->getAlpha(), 0, 0, 1);

				glutWireTorus(5, 20*sizeFactor*this->getRadius(), 8, 32);
				//glPopMatrix();

				glRotatef(-360*this->getAlpha(), 0, 0, 1);
				glRotatef(90, 1, 0, 0);
			}


			glColor4f(this->color.red, this->color.green, this->color.blue, this->getAlpha());
			glutSolidSphere(this->radius, 32, 32);

			glPopMatrix();
		}
	}

	void setState(int state){
		this->state = state;
	}

	int getState(){
		return this->state;
	}

	int getRadius(){
		return this->radius;
	}

	bool getOverTorus(){
		return this->overTorus;
	}

	void toggleOverTorus(){
		this->overTorus = !(this->overTorus);
	}

};


class BallsArray {
private:
	deque<Ball*> Balls;
	deque<int> iterators;

	BallsArray(){
		this->iterators.push_back(0);
	}
public:
	static BallsArray * getInstance(){
		static BallsArray I;
		return &I;
	}

	int activeBallLength(){
		int active = 0;
		int i;
		for (i = 0; i < Balls.size(); i++){
			if (Balls[i]->getActive() == 1) active++;
		}

		return active;
	}

	int addIterator(){
		this->iterators.push_back(0);
		return this->iterators.size()-1;
	}

	void getRidOfTheBalls(){
		this->Balls.clear();
	}

	void defaultNoOfBalls(){
		this->Balls.erase(Balls.begin()+defaultNoBalls,Balls.end()-1);
	}

	void addBall(Ball * newBall){
		this->Balls.push_back(newBall);
	}

	int ballLength(){
		return this->Balls.size();
	}

	Ball * getCurrentBall(int fromIterator){
		return this->Balls[this->iterators[fromIterator]];
	}

	Ball * getBallNo(int frm){
		return this->Balls[frm];
	}

	int firstBall(int fromIterator){
		this->iterators[fromIterator] = 0;
		return 0;
	}

	int nextBall(int fromIterator){
		return ++(this->iterators[fromIterator]);
	}

	bool reachedLimit(int fromIterator){
		return this->iterators[fromIterator] == this->Balls.size();
	}

	void resetToInitialSettings(){
		int i;
		for (i = 0; i < this->Balls.size(); i++){
			
			if (i == 0) this->Balls[i]->setActive(1);
			else this->Balls[i]->setActive(0);

			this->Balls[i]->spawn();
		}
	}

};

class BonusFX {
private:
	float expireTime;
	int ID;
	int level;
	char * desc;
public:
	BonusFX(int ID, int level, float expireTime){
		this->ID = ID;
		this->level = level;
		this->expireTime = expireTime;
		desc = (char*) malloc(sizeof(char)*50);
	}

	void setDesc(char * newD){
		strcpy(this->desc, newD);
	}

	char * getDesc(){
		return this->desc;
	}

	int getID(){
		return this->ID;
	}

	int getLevel(){
		return this->level;
	}

	float getExpireTime(){
		return this->expireTime;
	}

	void increaseLevel(){
		this->level = (int) boundedNumber(1, this->level+1, 5);
	}

	void renewExpireTime(){
		this->expireTime = deltaT.tempo+maximumFXDuration;
	}

	void setLevel(int newL){
		this->level = newL;
	}

};

BallsArray * bArray;

class Programa {
private:
	int winHeight;
	int winWidth;
	int score;
	RGBColor bgColor;
	RGBColor oldBgColor;
	int consecutiveHits;
	deque<Difficulty *> difficultyLevels;
	deque<Camera *> cameras;
	int difficulty;
	int curCamera;
	float tempo;
	float tempoLimite;
	int refreshRate;
	bool paused;
	int screenId;
	boolean wasFileUpdated;
	int AritsuBonusState;
	deque<BonusFX *> persistentBonuses;
	deque<BonusFX *> temporaryBonuses;
	float tempoLimiteInextensivel;

	Programa(){
		FreeConsole();
		int i;
		this->score = 0;
		this->bgColor.red = 0;
		this->bgColor.blue = 0;
		this->bgColor.green = 0;
		this->bgColor.alpha = 1;
		this->consecutiveHits = 1;
		this->refreshRate = 60;
		this->paused = false;
		this->winWidth = 854;
		this->winHeight = 480;
		this->wasFileUpdated = false;
		this->AritsuBonusState = 0;

		sdArray = new ScoreDataArray();
		this->setScreenId(0);


		 float vetorDeCores[27] = {1, 0, 0, 1, 0.5, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0.5, 0, 0.5, 1, 1, 1};
		 RGBColor tempColor;

		 for (i = 0; i < 9; i++){
			 tempColor.red = vetorDeCores[3*i];
			 tempColor.green = vetorDeCores[3*i+1];
			 tempColor.blue = vetorDeCores[3*i+2];

 			 AritsuNoKaraa.push_back(tempColor);
		 }


		torus = Torus::getInstance();
		bArray = BallsArray::getInstance();
		wArray = WordsArray::getInstance();

		srand(time(NULL));
		this->tempo = 0;
		this->tempoLimiteInextensivel = 300;

		 difficultyLevels.push_back(new Difficulty("Easy",		2, 70, 15, 70, 40, 10, 8, 4, 1));
		 difficultyLevels.push_back(new Difficulty("Standard",	3, 50, 35, 100, 30, 8, 6, 3, 2));
 		 difficultyLevels.push_back(new Difficulty("Hard",		5, 30, 55, 150, 20, 5, 4, 2, 3));

		bArray->addIterator();
		bArray->addIterator();

		 this->setDifficulty(1);

		 this->curCamera = 0;
		 cameras.push_back(new Camera(0, 300, 0, 0, 0, 0, 0, 0, -1));
		 cameras.push_back(new Camera(0, -300, 0, 0, 0, 0, 0, 0, 1));

		 wArray->addWord(new ActionWord("Resume", resumeOut, 372+5, 190+10+25, 90, 25, "pause"));
		 wArray->addWord(new ActionWord("Main menu", mainMenuOut, 372+5, 190+10+25*2, 90, 25, "pause"));

		 wArray->addWord(new ActionWord("Main menu", mainMenuOut, 372+5, 205+10+25, 90, 25, "gameover"));

 		 wArray->addWord(new ActionWord("Play", playOut, 100, 100, 70, 25, "main"));

		 deque<ActionWord *> awD;
		 
  		 awD.push_back(new ActionWord("Standard", setStandardOut));
  		 awD.push_back(new ActionWord("Hard", setHardOut));
 		 awD.push_back(new ActionWord("Easy", setEasyOut));

 		 wArray->addWord(new ActionWord(awD, 220, 125, 100, 25, "main"));


		 glutTimerFunc(1000/this->getRefreshRate(), timeFlowOut, 1);
	}

	void setBgColor(RGBColor newC){
		this->bgColor = newC;
	}

	int getAritsuBonusState(){
		return this->AritsuBonusState;
	}

	void setAritsuBonusState(int newS){
		this->AritsuBonusState = newS;

		if (newS == 0){
			RGBColor white;
			white.red = 1;
			white.green = 1;
			white.blue = 1;
			white.alpha = 1;

			torus->setColor(white);
		}
	}

	void displayPauseScreen(){

		int w = 120;
		int h = 100;
		int x = (this->getWinWidth()-w)/2;
		int y = (this->getWinHeight()-h)/2;

		glColor4f(1, 1, 1, 0.7);

		glBegin(GL_QUADS);
			glVertex3i(x,y,0);
			glVertex3i(x,y+h,0);
			glVertex3i(x+w,y+h,0);
			glVertex3i(x+w,y,0);
		glEnd();

		glColor4f(0, 0, 0, 1);
		ActionWord * aw;

		RGBColor gray;
		gray.alpha = 1;
		gray.red = 0.3;
		gray.green = 0.3;
		gray.blue = 0.3;


		aw = new ActionWord("Paused", x+10, y+10);
		aw->setColorScheme(0, gray);
		aw->render();

		wArray->activate("pause");

	}

	void displayTimeoutScreen(){
		glColor4f(1, 1, 1, 0.7);

		int w = 120;
		int h = 70;
		int x = (this->getWinWidth()-w)/2;
		int y = (this->getWinHeight()-h)/2;

		glBegin(GL_QUADS);
			glVertex3i(x,y,0);
			glVertex3i(x,y+h,0);
			glVertex3i(x+w,y+h,0);
			glVertex3i(x+w,y,0);
		glEnd();

		glColor4f(0, 0, 0, 1);
		ActionWord * aw;

		RGBColor gray;
		gray.red = 0.3;
		gray.green = 0.3;
		gray.blue = 0.3;
		gray.alpha = 1;

		aw = new ActionWord("Game over", x+10, y+10);
		aw->setColorScheme(0, gray);
		aw->render();


		wArray->activate("gameover");
	}

	void displayHUD(){
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glDepthMask(GL_FALSE);  // disable writes to Z-Buffer

 		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_DEPTH_TEST);

		gluOrtho2D(0.0, this->getWinWidth(), this->getWinHeight(), 0.0);

		glPushMatrix();
		glClear(GL_DEPTH_BUFFER_BIT);

		glColor4f(1, 1, 1, 0.7);

		glBegin(GL_QUADS);
			glVertex3i(0,0,0);
			glVertex3i(this->getWinWidth(),0,0);
			glColor4f(1, 1, 1, 0.4);
			glVertex3i(this->getWinWidth(),30,0);
			glVertex3i(0,30,0);
		glEnd();


		glColor4f(0, 0, 0, 1);

		char * acabucuru;
		acabucuru = (char*) malloc(sizeof(char)*30);
		strcpy(acabucuru, "Score: ");
		acabucuru = strcat(acabucuru, intToString(this->score));

		ActionWord * aw;

		aw = new ActionWord(acabucuru, 10, 0);
		aw->render();

		char * acabucuru2;
		acabucuru2 = (char*) malloc(sizeof(char)*30);
		strcpy(acabucuru2, "Time: ");

		float invTime = this->tempoLimite-this->tempo;

		int boundedTime = (int) boundedNumber(0, invTime, this->tempoLimite);
		int minutes = boundedTime/60;
		int seconds = boundedTime%60;

		if (minutes < 10) acabucuru2 = strcat(acabucuru2, "0");
		acabucuru2 = strcat(acabucuru2, intToString(minutes));
		acabucuru2 = strcat(acabucuru2, ":");
		if (seconds < 10) acabucuru2 = strcat(acabucuru2, "0");
		acabucuru2 = strcat(acabucuru2, intToString(seconds));

		RGBColor red;
		red.red = 1;
		red.green = 0;
		red.blue = 0;
		red.alpha = 1;

		RGBColor black;
		black.red = 0;
		black.green = 0;
		black.blue = 0;
		black.alpha = 1;

		RGBColor resColor;
		resColor.alpha = 1;


		if (invTime >= 11.0) resColor = black;
		else if (invTime <= 10.0) resColor = red;
		else {
			float alpha;
			alpha = 1-(invTime-10.0);
			resColor.red = (1-alpha)*black.red + alpha*red.red;
			resColor.green = (1-alpha)*black.green + alpha*red.green;
			resColor.blue = (1-alpha)*black.blue + alpha*red.blue;
		}

		aw = new ActionWord(acabucuru2, this->getWinWidth()-100, 0);
		aw->setColorScheme(0, resColor);
		aw->render();

		if (this->consecutiveHits > 2){
			char * acabucuru3;
			acabucuru3 = (char*) malloc(sizeof(char)*50);
			strcpy(acabucuru3, intToString(-1+this->consecutiveHits));
			strcat(acabucuru3, " hits");

			aw = new ActionWord(acabucuru3, 400, 0);
			aw->render();
		}


		if (this->getAritsuBonusState() >= 1){ 
			glColor4f(1, 0, 0, 1);

			glBegin(GL_QUADS);
				glVertex3i(10,40,0);
				glVertex3i(10,60,0);
				glVertex3i(30,60,0);
				glVertex3i(30,40,0);
			glEnd();
		}


		if (this->getAritsuBonusState()/10 == 0){
			if (this->getAritsuBonusState() >= 2){ 
				glColor4f(0, 1, 0, 1);

				glBegin(GL_QUADS);
					glVertex3i(40,40,0);
					glVertex3i(40,60,0);
					glVertex3i(60,60,0);
					glVertex3i(60,40,0);
				glEnd();
			}
		}

		if (this->getAritsuBonusState()/10 == 1){
			if (this->getAritsuBonusState() >= 12){ 
				glColor4f(1, 1, 0, 1);

				glBegin(GL_QUADS);
					glVertex3i(40,40,0);
					glVertex3i(40,60,0);
					glVertex3i(60,60,0);
					glVertex3i(60,40,0);
				glEnd();
			}

			if (this->getAritsuBonusState() >= 13){ 
				glColor4f(0, 1, 0, 1);

				glBegin(GL_QUADS);
					glVertex3i(70,40,0);
					glVertex3i(70,60,0);
					glVertex3i(90,60,0);
					glVertex3i(90,40,0);
				glEnd();
			}
		}


		if (this->getAritsuBonusState()/10 == 2){
			if (this->getAritsuBonusState() >= 22){ 
				glColor4f(1, 0.5, 0, 1);

				glBegin(GL_QUADS);
					glVertex3i(40,40,0);
					glVertex3i(40,60,0);
					glVertex3i(60,60,0);
					glVertex3i(60,40,0);
				glEnd();
			}

			if (this->getAritsuBonusState() >= 23){ 
				glColor4f(1, 1, 0, 1);

				glBegin(GL_QUADS);
					glVertex3i(70,40,0);
					glVertex3i(70,60,0);
					glVertex3i(90,60,0);
					glVertex3i(90,40,0);
				glEnd();
			}


			if (this->getAritsuBonusState() >= 24){ 
				glColor4f(0, 1, 1, 1);

				glBegin(GL_QUADS);
					glVertex3i(100,40,0);
					glVertex3i(100,60,0);
					glVertex3i(120,60,0);
					glVertex3i(120,40,0);
				glEnd();
			}

			if (this->getAritsuBonusState() >= 25){ 
				glColor4f(0, 0, 1, 1);

				glBegin(GL_QUADS);
					glVertex3i(130,40,0);
					glVertex3i(130,60,0);
					glVertex3i(150,60,0);
					glVertex3i(150,40,0);
				glEnd();
			}

			if (this->getAritsuBonusState() >= 26){ 
				glColor4f(1, 0, 1, 1);

				glBegin(GL_QUADS);
					glVertex3i(160,40,0);
					glVertex3i(160,60,0);
					glVertex3i(180,60,0);
					glVertex3i(180,40,0);
				glEnd();
			}
		}


		int i;

		if (this->temporaryBonuses.size() > 0){
			glColor4f(1, 1, 1, 0.5);

			glBegin(GL_QUADS);
				glVertex3i(this->getWinWidth()-300,30,0);
				glVertex3i(this->getWinWidth(),30,0);
				glVertex3i(this->getWinWidth(),30+this->temporaryBonuses.size()*25+10,0);
				glVertex3i(this->getWinWidth()-300,30+this->temporaryBonuses.size()*25+10,0);
			glEnd();
		}

		for (i = 0; i < this->temporaryBonuses.size(); i++){

			float calcRTime = this->temporaryBonuses[i]->getExpireTime()-this->getTempo();

			RGBColor resColor2;
			resColor2.alpha = 1;

			if (calcRTime >= 6.0) resColor2 = black;
			else if (calcRTime <= 5.0) resColor2 = red;
			else {
				float alpha;
				alpha = 1-(calcRTime-5.0);
				resColor2.red = (1-alpha)*black.red + alpha*red.red;
				resColor2.green = (1-alpha)*black.green + alpha*red.green;
				resColor2.blue = (1-alpha)*black.blue + alpha*red.blue;
			}

			char * resT = (char*) malloc(sizeof(char)*60);
			strcpy(resT, this->temporaryBonuses[i]->getDesc());
			strcat(resT, " lv");
			strcat(resT, intToString(this->temporaryBonuses[i]->getLevel()));

			ActionWord * aw = new ActionWord(resT, this->getWinWidth()-290, 35+25*i);
			aw->setColorScheme(0, resColor2);
			aw->render();

			char * str = (char*) malloc(sizeof(char)*20);
			strcpy(str, "00:");

			if (calcRTime < 10) strcat(str, "0");
			strcat(str, intToString((int) calcRTime));


			aw = new ActionWord(str, this->getWinWidth()-50, 35+25*i);
			aw->setColorScheme(0, resColor2);
			aw->render();

		}



		if (this->wasTimeLimitExceeded()){

			this->displayTimeoutScreen();

		} else if (this->isPaused()){

			this->displayPauseScreen();

		} else {
			wArray->deactivate();
		}



		glMatrixMode(GL_PROJECTION);

		glDepthMask(GL_TRUE);  // disable writes to Z-Buffer
		glEnable(GL_DEPTH_TEST);
	}

	void DesenhaMenu(){

		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glDepthMask(GL_FALSE);  // disable writes to Z-Buffer

 		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_DEPTH_TEST);

		gluOrtho2D(0.0, this->getWinWidth(), this->getWinHeight(), 0.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);

		glEnable(GLUT_MULTISAMPLE);


		ActionWord * aw;
		int i;

		wArray->activate("main");

		RGBColor x;
		x.red = 0.5;
		x.green = 0.5;
		x.blue = 0.5;
		x.alpha = 1;

		RGBColor y;
		y.red = 0.7;
		y.green = 0.7;
		y.blue = 0.7;
		y.alpha = 1;

		RGBColor z;
		z.red = 1;
		z.green = 1;
		z.blue = 1;
		y.alpha = 1;

		aw = new ActionWord("Falling Balls", 100, 15);
		aw->setColorScheme(0, x);
		aw->render();

		aw = new ActionWord("Copyright 2014 Iago Brunherotto", 100, this->getWinHeight()-50);
		aw->setColorScheme(0, y);
		aw->render();

		aw = new ActionWord("Difficulty:", 100, 125);
		aw->setColorScheme(0, y);
		aw->render();

		aw = new ActionWord("Best scores in this difficulty", 400, 15);
		aw->setColorScheme(0, y);
		aw->render();

		sdArray->render();


		glutSwapBuffers();
	}

	void InicializaJogo(){
		this->tempo = 0;
		this->tempoLimite = 180;
		deltaT.tempoLimite = 180;
		this->paused = false;
		this->score = 0;
		this->wasFileUpdated = false;
		this->consecutiveHits = 1;
		bArray->resetToInitialSettings();
		this->AritsuBonusState = 0;
		torus->setText(NULL);
		torus->clearGravDistortion(false);
		torus->setGravDistortionLevel(0);


		persistentBonuses.clear();
		temporaryBonuses.clear();

		BonusFX * timeXt = new BonusFX(1, 1, -1);
		persistentBonuses.push_back(timeXt);
		BonusFX * bonusXt = new BonusFX(2, 1, -1);
		persistentBonuses.push_back(bonusXt);

		whiteBallSpawnThreshold = this->getDifficultyObject()->getBaseWhiteBallSpawnThreshold();

		maximumFXDuration = this->getDifficultyObject()->getBonusMaxTime();
		defaultNoBalls = this->getDifficultyObject()->getMaxBallLength();
		lsrFactor = 0;

	}

	void DesenhaJogo(){
		if (this->isPaused() || this->wasTimeLimitExceeded()){
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW); 			
		 } else {
 			glutSetCursor(GLUT_CURSOR_NONE); 
		 }

		 glEnable(GLUT_MULTISAMPLE);

		 int i;

		 GLfloat luzAmbiente[4] = {0.35,0.35,0.35,1.0}; 
		 GLfloat luzDifusa[4] = {0.7,0.7,0.7,1.0};
		 GLfloat luzEspecular[4] = {1.0,1.0,1.0,1.0};
 		 GLfloat posicaoLuz[4] = {0.0,50.0,50.0,1.0};

   		 glLoadIdentity();
		 glClearColor(this->bgColor.red, this->bgColor.green, this->bgColor.blue, 1.0f);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  		 glMatrixMode(GL_PROJECTION);
		 glViewport(0, 0, (GLsizei) this->getWinWidth(), (GLsizei) this->getWinHeight());
		 gluPerspective(90, (GLfloat) this->getWinWidth()/(GLfloat) this->getWinHeight(), 5, 800);

		 float maxY = -2147483647;
		 float calcEyeY;

		 for (i = bArray->firstBall(2); !bArray->reachedLimit(2); i = bArray->nextBall(2)){
			Ball * b = bArray->getCurrentBall(2);			 
 			maxY = maxBetween(maxY, b->getBallSpaceEquation());
		 }

		 /*
		 if (this->getCamera() == 0){
			 calcEyeY = maxBetween(this->getCameraObject()->getEyeY(), maxY);
		 } else {
			 calcEyeY = this->getCameraObject()->getEyeY();
		 }
		 */


		 gluLookAt(this->getCameraObject()->getEyeX(), this->getCameraObject()->getEyeY(), this->getCameraObject()->getEyeZ(),
			 this->getCameraObject()->getCtrX(), this->getCameraObject()->getCtrY(), this->getCameraObject()->getCtrZ(),
			 this->getCameraObject()->getUpX(), this->getCameraObject()->getUpY(), this->getCameraObject()->getUpZ());

			



 		 glEnable(GL_COLOR_MATERIAL);
		 glEnable(GL_LIGHTING);
		 glEnable(GL_LIGHT0);
		 glEnable(GL_DEPTH_TEST);
		 glShadeModel(GL_SMOOTH);

		 glMaterialfv(GL_LIGHT0,GL_SPECULAR,luzEspecular);
		 glLightModelfv(GL_LIGHT_MODEL_AMBIENT,luzAmbiente);

		 glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
		 glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
		 glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
		 glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

			 
		 glEnable (GL_BLEND);
 		 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		 glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);


		torus->render();

		for (i = bArray->firstBall(0); !bArray->reachedLimit(0); i = bArray->nextBall(0)){
			bArray->getCurrentBall(0)->render();
		}
		

		this->displayHUD();

		glutSwapBuffers();
	}

	void attemptToSpawnBalls(int ex, bool excCurrent){

		int spawnDecide;
		int newlySpanwedBall = 0;

		int j;

		BonusFX * underBonusMode = getTemporaryBonusObject(3);

		for (j = bArray->firstBall(2); !bArray->reachedLimit(2) && !newlySpanwedBall; j = bArray->nextBall(2)){
			if (bArray->getCurrentBall(2)->getActive() == 0 && j != ex){
				spawnDecide = rand()%100;

				if (spawnDecide < this->getDifficultyObject()->getSpawnThreshold() && underBonusMode == NULL){
					bArray->getCurrentBall(2)->setActive(0);
				} else {
					bArray->getCurrentBall(2)->setActive(1);
					bArray->getCurrentBall(2)->spawn();
					newlySpanwedBall = 1;
				}
			}
		}

		if (!excCurrent){
			spawnDecide = rand()%100;
			Ball * b = bArray->getBallNo(ex);

			if (bArray->activeBallLength() > 1 && underBonusMode == NULL){
				if (spawnDecide < this->getDifficultyObject()->getSpawnThreshold()){
					b->setActive(0);
				} else {
					b->setActive(1);
				}
			} else {
				b->setActive(1);
			}

			b->spawn();
		}



		double minimalTime = 2147483647;
		Ball * getBallObj = NULL;

		for (j = bArray->firstBall(2); !bArray->reachedLimit(2); j = bArray->nextBall(2)){
			if (bArray->getCurrentBall(2)->getActive() == 1){
				int getEstTime = bArray->getCurrentBall(2)->getEstimatedLandTime();
				if (getEstTime < minimalTime && getEstTime > 0 && bArray->getCurrentBall(2)->getBallSpaceEquation() >= 20){
					minimalTime = bArray->getCurrentBall(2)->getEstimatedLandTime();
					getBallObj = bArray->getCurrentBall(2);
				}
			}
		}
		
		int AritsuNoAlea = rand()%(this->getDifficultyObject()->getMaxBallLength());

		if (this->getAritsuBonusState() == 1){
			AritsuNoAlea = rand()%31;

			RGBColor orange;
			orange.alpha = 1;
			orange.red = 1;
			orange.green = 0.5;
			orange.blue = 0;

			RGBColor green;
			green.alpha = 1;
			green.red = 0;
			green.green = 1;
			green.blue = 0;

			RGBColor yellow;
			yellow.alpha = 1;
			yellow.red = 1;
			yellow.green = 1;
			yellow.blue = 0;

			if (AritsuNoAlea <= 24)	getBallObj->setColor(green);
			else if (AritsuNoAlea <= 29) getBallObj->setColor(yellow);
			else getBallObj->setColor(orange);
		}

		if (this->getAritsuBonusState() == 2){
			RGBColor blue;
			blue.alpha = 1;
			blue.red = 0;
			blue.green = 0;
			blue.blue = 1;

			getBallObj->setColor(blue);
		}

		if (this->getAritsuBonusState() == 12){
			RGBColor green;
			green.alpha = 1;
			green.red = 0;
			green.green = 1;
			green.blue = 0;

			getBallObj->setColor(green);
		}

		if (this->getAritsuBonusState() == 13){
			RGBColor blue;
			blue.alpha = 1;
			blue.red = 0;
			blue.green = 0;
			blue.blue = 1;

			getBallObj->setColor(blue);
		}

		
		if (this->getAritsuBonusState() == 22){
			RGBColor yellow;
			yellow.alpha = 1;
			yellow.red = 1;
			yellow.green = 1;
			yellow.blue = 0;

			getBallObj->setColor(yellow);
		}

		if (this->getAritsuBonusState() == 23){
			RGBColor cyan;
			cyan.alpha = 1;
			cyan.red = 0;
			cyan.green = 1;
			cyan.blue = 1;

			getBallObj->setColor(cyan);
		}

		if (this->getAritsuBonusState() == 24){
			RGBColor blue;
			blue.alpha = 1;
			blue.red = 0;
			blue.green = 0;
			blue.blue = 1;

			getBallObj->setColor(blue);
		}

		if (this->getAritsuBonusState() == 25){
			RGBColor fuchsia;
			fuchsia.alpha = 1;
			fuchsia.red = 1;
			fuchsia.green = 0;
			fuchsia.blue = 1;

			getBallObj->setColor(fuchsia);
		}

		if (this->getAritsuBonusState() == 26){
			RGBColor purple;
			purple.alpha = 1;
			purple.red = 0.5;
			purple.green = 0;
			purple.blue = 0.5;

			getBallObj->setColor(purple);
		}
	}

	BonusFX * actOverTemporaryBonuses(int ID){
		int i;
		for (i = 0; i < temporaryBonuses.size(); i++){
			if (temporaryBonuses[i]->getID() == ID){
				temporaryBonuses[i]->increaseLevel();
				temporaryBonuses[i]->renewExpireTime();
				return temporaryBonuses[i];
			}
		}

		BonusFX * BFX = new BonusFX(ID, 1, deltaT.tempo+maximumFXDuration);
		temporaryBonuses.push_back(BFX);
		return BFX;
	}

	BonusFX * getTemporaryBonusObject(int ID){
		int i;
		for (i = 0; i < temporaryBonuses.size(); i++){
			if (temporaryBonuses[i]->getID() == ID){
				return temporaryBonuses[i];
			}
		}

		return NULL;
	}

	void updateLeaderboard(){
		 FILE * recordToFile = fopen("leaderboard.txt", "a");

		 SYSTEMTIME str_t;
		 GetSystemTime(&str_t);

		 this->temporaryBonuses.clear();

		 fprintf(recordToFile, "\n");
		 int getDay = (int) str_t.wDay;
		 int getMonth = (int) str_t.wMonth;
		 int getYear = (int) str_t.wYear;
		 int getHour = (int) str_t.wHour;
		 int getMinute = (int) str_t.wMinute;
		 int getSecond = (int) str_t.wSecond;


		 if (getDay < 10) fprintf(recordToFile, "0");
		 fprintf(recordToFile, intToString(getDay));
		 fprintf(recordToFile, "/");

		 if (getMonth < 10) fprintf(recordToFile, "0");
		 fprintf(recordToFile, intToString(getMonth));
		 fprintf(recordToFile, "/");

		 fprintf(recordToFile, intToString(getYear));
		 fprintf(recordToFile, " ");


		 if (getHour < 10) fprintf(recordToFile, "0");
		 fprintf(recordToFile, intToString(getHour));
		 fprintf(recordToFile, ":");

		 if (getMinute < 10) fprintf(recordToFile, "0");
		 fprintf(recordToFile, intToString(getMinute));
		 fprintf(recordToFile, ":");

		 if (getSecond < 10) fprintf(recordToFile, "0");
		 fprintf(recordToFile, intToString(getSecond));


		 fprintf(recordToFile, "\t");
		 fprintf(recordToFile, this->getDifficultyObject()->getLabel());
		 fprintf(recordToFile, "\t");
		 fprintf(recordToFile, intToString(this->getScore()));


		 fclose(recordToFile);

		 this->wasFileUpdated = true;
	}
public:
	bool wasTimeLimitExceeded(){
		return this->tempoLimite <= this->tempo;
	}

	int getScreenId(){
		return this->screenId;
	}

	void setScreenId(int newScreenId){
		this->screenId = newScreenId;

		if (this->screenId == 0){
			this->inicializaMenu();
		}

		if (this->screenId == 1){
			this->InicializaJogo();
		}
	}

	void inicializaMenu(){
		sdArray->reset();

		FILE * retLeaderboard = fopen("leaderboard.txt", "r");

		if (retLeaderboard == NULL){
			FILE * retLeaderboardCreate = fopen("leaderboard.txt", "w");
			fprintf(retLeaderboardCreate, "%s", NULL);
			fclose(retLeaderboardCreate);
		}

		char lDate[50];
		char lDifficulty[20];
		int lScore;

		if (retLeaderboard != NULL){
			while (fscanf(retLeaderboard, "%[^\t]s", lDate) != EOF){

				fscanf(retLeaderboard, "%s", lDifficulty);
				fscanf(retLeaderboard, "%d", &lScore);

				ScoreData * SD = new ScoreData(lDate, lDifficulty, lScore);
				sdArray->addScoreData(SD);
			}
			sdArray->sortData();
		}

	}

	bool isPaused(){
		return this->paused;
	}

	void pause(){
		this->paused = true;
	}

	void resume(){
		this->paused = false;
	}

	int getWinHeight(){
		return this->winHeight;
	}

	int getWinWidth(){
		return this->winWidth;
	}

	void setWinSize(int newW, int newH){
		this->winWidth = newW;
		this->winHeight = newH;
	}

	void setDifficulty(int difficulty){
		this->difficulty = difficulty;
		sdArray->setDifficulty( this->getDifficultyObject()->getLabel());
		bonusTimeMax = this->getDifficultyObject()->getBonusMaxTime();

		 Ball * objetoEsferico;

		 bArray->getRidOfTheBalls();
		 int i;

		 for (i = 0; i < this->getDifficultyObject()->getMaxBallLength(); i++){
			 objetoEsferico = new Ball(this->getDifficultyObject());
			 if (i == 0) objetoEsferico->setActive(1);
			 else objetoEsferico->setActive(0);
			 bArray->addBall(objetoEsferico);
		 }

	}

	void setRefreshRate(int n){
		this->refreshRate = n;
	}

	int getRefreshRate(){
		return this->refreshRate;
	}

	Difficulty * getDifficultyObject(){
		return difficultyLevels[this->difficulty];
	}

	float getTempo(){
		return this->tempo;
	}

	void setTime(float tempo){
		this->tempo = tempo;
		deltaT.tempo = tempo;
	}

	void incremementConsecutiveHits(){
		this->consecutiveHits++;
	}
	
	void resetConsecutiveHits(){
		this->consecutiveHits = 1;
	}

	int getConsecutiveHits(){
		return this->consecutiveHits;
	}

	void setWinHeight(int newH){
		this->winHeight = newH;
	}

	void setWinWidth(int newW){
		this->winWidth = newW;
	}

	static Programa * getInstance(){
		static Programa instancia;
		return &instancia;
	}

	float getTempoLimite(){
		return this->tempoLimite;
	}

	int getScore(){
		return this->score;
	}

	void setScore(int newScore){
		this->score = newScore;
	}

	void setCamera(int newCameraId){
		this->curCamera = newCameraId;
	}

	int getCamera(){
		return this->curCamera;
	}

	Camera * getCameraObject(){
		return this->cameras[this->curCamera];
	}

	void displayDecider(){
		if (this->screenId == 0){
			this->DesenhaMenu();
		}

		if (this->screenId == 1){
			this->DesenhaJogo();
		}
	}

	void timeFlow(int t){
		if (this->getScreenId() != 1){
			t = 0;
		}

		int i;
		int j;
		this->setTime((1.0*t)/this->getRefreshRate());


		for (i = 0; i < this->temporaryBonuses.size(); i++){
			if (this->getTempo() >= this->temporaryBonuses[i]->getExpireTime()){

				if (this->temporaryBonuses[i]->getID() == 2){
					torus->setSizeLevel(0);
				}

				if (this->temporaryBonuses[i]->getID() == 3){
					RGBColor black;
					black.red = 0;
					black.green = 0;
					black.blue = 0;
					black.alpha = 1;

					this->setBgColor(black);
				}

				if (this->temporaryBonuses[i]->getID() == 4){
					torus->clearGravDistortion(true);
					torus->setGravDistortionLevel(0);
				}

				if (this->temporaryBonuses[i]->getID() == 5){
					whiteBallSpawnThreshold = this->getDifficultyObject()->getBaseWhiteBallSpawnThreshold();
				}

				if (this->temporaryBonuses[i]->getID() == 9){
					for (j = bArray->firstBall(2); !bArray->reachedLimit(2); j = bArray->nextBall(2)){
						Ball *  b = bArray->getCurrentBall(2);
						b->setSlownessLevel(0);
					}
				}

				if (this->temporaryBonuses[i]->getID() == 10){
					bArray->defaultNoOfBalls();
				}

				if (this->temporaryBonuses[i]->getID() == 11){
					maximumFXDuration = this->getDifficultyObject()->getBonusMaxTime();
				}

				if (this->temporaryBonuses[i]->getID() == 12){
					lsrFactor = 0;
				}

				this->temporaryBonuses.erase(this->temporaryBonuses.begin()+i);
				i--;
			}
		}

		torus->setTempo((1.0*t)/this->getRefreshRate());

		bool found2 = false;
		bool found3 = false;
		bool found4 = false;
		bool found5 = false;
		bool found8 = false;
		bool found9 = false;
		bool found11 = false;
		bool found12 = false;

		int level4 = 0;
		int level8 = 0;
		int level12 = 0;

		for (i = 0; i < this->temporaryBonuses.size(); i++){
			if (this->temporaryBonuses[i]->getID() == 2){
				torus->setSizeLevel(this->temporaryBonuses[i]->getLevel());
				found2 = true;
			}

			if (this->temporaryBonuses[i]->getID() == 3){

				RGBColor black;
				black.red = 0;
				black.green = 0;
				black.blue = 0;

				RGBColor skyblue;
				skyblue.red = 0;
				skyblue.green = 0.5;
				skyblue.blue = 1;

				RGBColor orange;
				orange.red = 1;
				orange.green = 0.25;
				orange.blue = 0;

				RGBColor navy;
				navy.red = 0;
				navy.green = 0;
				navy.blue = 0.5;

				float getScale = this->temporaryBonuses[i]->getExpireTime()-this->getTempo();
				getScale = maximumFXDuration-getScale;
				getScale /= maximumFXDuration;

				RGBColor resultingColor;

				if (getScale >= 0 && getScale < 0.1){

					if (getScale <= 1.1/this->refreshRate){
						this->oldBgColor = this->bgColor;
					}

					float alpha = getScale*10;
					resultingColor.red = this->oldBgColor.red*(1-alpha) + skyblue.red*alpha;
					resultingColor.green = this->oldBgColor.green*(1-alpha) + skyblue.green*alpha;
					resultingColor.blue = this->oldBgColor.blue*(1-alpha) + skyblue.blue*alpha;
				} else if (getScale >= 0.1 && getScale < 0.6){
						resultingColor = skyblue;
				} else if (getScale >= 0.6 && getScale < 0.7){
					float alpha = (getScale-0.6)*10;
					resultingColor.red = skyblue.red*(1-alpha) + orange.red*alpha;
					resultingColor.green = skyblue.green*(1-alpha) + orange.green*alpha;
					resultingColor.blue = skyblue.blue*(1-alpha) + orange.blue*alpha;
				} else if (getScale >= 0.7 && getScale < 0.8){
					resultingColor = orange;
				} else if (getScale >= 0.8 && getScale < 0.9){
					float alpha = (getScale-0.8)*10;
					resultingColor.red = orange.red*(1-alpha) + navy.red*alpha;
					resultingColor.green = orange.green*(1-alpha) + navy.green*alpha;
					resultingColor.blue = orange.blue*(1-alpha) + navy.blue*alpha;
				} else if (getScale >= 0.9 && getScale < 1){
					float alpha = (getScale-0.9)*10;
					resultingColor.red = navy.red*(1-alpha) + black.red*alpha;
					resultingColor.green = navy.green*(1-alpha) + black.green*alpha;
					resultingColor.blue = navy.blue*(1-alpha) + black.blue*alpha;
				}

				resultingColor.alpha = 1;

				found3 = true;



				this->setBgColor(resultingColor);
			}

			if (this->temporaryBonuses[i]->getID() == 4){
				found4 = true;
				level4 = this->temporaryBonuses[i]->getLevel();
			}

			if (this->temporaryBonuses[i]->getID() == 5){
				found5 = true;
			}

			if (this->temporaryBonuses[i]->getID() == 8){
				found8 = true;
				level8 = this->temporaryBonuses[i]->getLevel();
			}

			if (this->temporaryBonuses[i]->getID() == 9){

				int j;

				for (j = bArray->firstBall(2); !bArray->reachedLimit(2); j = bArray->nextBall(2)){
					Ball * b = bArray->getCurrentBall(2);
					b->setSlownessLevel(this->temporaryBonuses[i]->getLevel());
				}

				found9 = true;
			}

			if (this->temporaryBonuses[i]->getID() == 11){
				maximumFXDuration = this->getDifficultyObject()->getBonusMaxTime() + this->getDifficultyObject()->getXtTimeStep() * this->temporaryBonuses[i]->getLevel();
				found11 = true;
			}

			if (this->temporaryBonuses[i]->getID() == 12){
				lsrFactor = this->temporaryBonuses[i]->getLevel();
			}
		}


		if (!found3 || !found4){
			torus->clearAritsuColor();
		}


		 if (this->wasTimeLimitExceeded()){

			 torus->setGravDistortion();
			 torus->setGravDistortionLevel(0);

			 if (!this->wasFileUpdated){
				 this->updateLeaderboard();
			 }
		 }

		if (this->isPaused()){
			return;
		}


		for (i = bArray->firstBall(1); !bArray->reachedLimit(1); i = bArray->nextBall(1)){
			Ball *  b = bArray->getCurrentBall(1);

			if (b->getActive() == 1){
				b->setTempo(b->getTempo()+1.0/this->getRefreshRate());

				float insideTorusThreshold = torus->getInnerBorderSize()-b->getRadius();
				float xDim = b->getX()-torus->getX();
				float zDim = b->getZ()-torus->getZ();
				float inTorusBorderThreshold = torus->getInnerBorderSize()+torus->getOuterBorderSize();

				if (b->getBallSpaceEquation() > 0 && b->getBallSpaceEquation() <= torus->getOuterBorderSize()){
						if (pow(xDim, 2) + pow(zDim, 2) > pow(insideTorusThreshold, 2) && pow(xDim, 2) + pow(zDim, 2) <= pow(inTorusBorderThreshold, 2)){
								//b->setIniSpace(b->getBallSpaceEquation());
								b->setSpeed(-0.6*b->getSpeed());
								//b->setTempo(0);
								this->attemptToSpawnBalls(i, true);
						}
				}

				if (b->getBallSpaceEquation() >= 0 && found4){
					if (pow(xDim, 2) + pow(zDim, 2) <= pow(insideTorusThreshold, 2)){
						if (b->getOverTorus() == false){
							b->setIniSpace(b->getBallSpaceEquation());
							b->setTempo(0);

							float newSpeed = b->getSpeed()*(1+level4);
							if (newSpeed < 0) newSpeed *= -1;

							b->setSpeed(newSpeed);
							b->setAcceleration(5+5*level4);
							b->toggleOverTorus();
						}
					} else {
						if (b->getOverTorus() == true){
							b->setIniSpace(b->getBallSpaceEquation());
							b->setTempo(0);
							b->setSpeed(b->getSpeed()/(1+0.5*level4));
							b->setAcceleration(5);
							b->toggleOverTorus();
						}						
					}
				}

				if (found8){
					b->setRadius(20-2*level8);
				} else {
					b->setRadius(20);
				}


				if (b->getBallSpaceEquation() <= 0){

					if (b->getState() == 0){

						if (pow(xDim, 2) + pow(zDim, 2) <= pow(insideTorusThreshold, 2)){

							RGBColor c = b->getColor();
							float RTime = this->tempoLimite-this->tempo;

							if (c.red == 1.0 && c.green == 1.0 && c.blue == 1.0){

								int c;
								int inLevel5 = 0;


								int AritsuNoAlea = -1;

									do {
										if (RTime >= maximumFXDuration){
											AritsuNoAlea = rand()%13;

											if (AritsuNoAlea == 7){
												for (c = 0; c < temporaryBonuses.size(); c++){
													if (this->temporaryBonuses[c]->getLevel() == 5){
														inLevel5++;
													}
												}
											}

										} else {
											AritsuNoAlea = rand()%2;
										}

									} while (
										(AritsuNoAlea == 0 && this->tempoLimite >= this->tempoLimiteInextensivel)
									 || (temporaryBonuses.size() < 1 && (AritsuNoAlea == 6 || AritsuNoAlea == 7))
									 || (AritsuNoAlea == 7 && inLevel5 == temporaryBonuses.size()));
								
								if (AritsuNoAlea == 0){

									int bonusTime = rand()%persistentBonuses[0]->getLevel();
									int baseBonusTime;

									char * str = (char*) malloc(sizeof(char)*20);

									if (RTime >= maximumFXDuration){
										baseBonusTime = 5+2.5*bonusTime;
									} else {
										baseBonusTime = 15+3.75*bonusTime;
									}

									this->tempoLimite = boundedNumber(0, this->tempoLimite+baseBonusTime, this->tempoLimiteInextensivel);
									deltaT.tempoLimite = boundedNumber(0, deltaT.tempoLimite+baseBonusTime, this->tempoLimiteInextensivel);
									persistentBonuses[0]->increaseLevel();


									strcpy(str, "+00:");
									if (baseBonusTime < 10);
									strcpy(str, "0");
									strcat(str, intToString(baseBonusTime));

									torus->setText(str);

								} else if (AritsuNoAlea == 1){

									int bonusScore = 1+rand()%persistentBonuses[1]->getLevel();

									int baseBonusScore;

									if (RTime >= maximumFXDuration){
										baseBonusScore = 200*bonusScore;
									} else {
										baseBonusScore = 500 + 100*bonusScore;
									}

									this->setScore(this->getScore()+baseBonusScore);
									persistentBonuses[1]->increaseLevel();

									char * str = (char*) malloc(sizeof(char)*20);
									strcpy(str, "+");
									strcat(str, intToString(baseBonusScore));
									strcat(str, " bonus");

									torus->setText(str);


								} else if (AritsuNoAlea == 2){

									BonusFX * BFX = this->actOverTemporaryBonuses(2);
									BFX->setDesc("Larger torus");
									torus->setText("Larger torus");

								} else if (AritsuNoAlea == 3){

									BonusFX * BFX = this->actOverTemporaryBonuses(3);
									BFX->setDesc("Bonus mode");
									torus->setText("Bonus mode");

									BonusFX * getFX4 = getTemporaryBonusObject(4);
									if (getFX4 != NULL) torus->setAritsuColor();

								} else if (AritsuNoAlea == 4){

									BonusFX * BFX = this->actOverTemporaryBonuses(4);
									BFX->setDesc("Blackhole torus");
									torus->setText("Blackhole torus");
									torus->setGravDistortion();
									torus->setGravDistortionLevel(BFX->getLevel());

									BonusFX * getFX3 = getTemporaryBonusObject(3);
									if (getFX3 != NULL) torus->setAritsuColor();

								} else if (AritsuNoAlea == 5){

									BonusFX * BFX = this->actOverTemporaryBonuses(5);
									BFX->setDesc("More white balls");
									torus->setText("More white balls");

									whiteBallSpawnThreshold = this->getDifficultyObject()->getBaseWhiteBallSpawnThreshold()+this->getDifficultyObject()->getWhiteBallSpawnThresholdIncStep()*BFX->getLevel();
								} else if (AritsuNoAlea == 6){

									int i;

									for (i = 0; i < temporaryBonuses.size(); i++){
										temporaryBonuses[i]->renewExpireTime();
									}

									torus->setText("Extended effects");
								} else if (AritsuNoAlea == 7){

									int p;
									char * str = (char*) malloc(sizeof(char)*50);
									int i = rand()%temporaryBonuses.size();

									while (temporaryBonuses[i]->getLevel() == 5){
										i = (i+1)%temporaryBonuses.size();
									}
	
									p = temporaryBonuses[i]->getLevel();
									temporaryBonuses[i]->setLevel(5);

									strcpy(str, temporaryBonuses[i]->getDesc());
									strcat(str, " maxed out");

									if (temporaryBonuses[i]->getID() == 10){
										int comp = 5-p;

										for (i = 0; i < (this->getDifficultyObject()->getPlusBallsStep())*comp; i++){
											Ball * b = new Ball(this->getDifficultyObject());
											bArray->addBall(b);
											b->setActive(1);
											b->spawn();
										}
									}


									torus->setText(str);
								} else if (AritsuNoAlea == 8){

									BonusFX * BFX = this->actOverTemporaryBonuses(8);
									BFX->setDesc("Smaller balls");
									torus->setText("Smaller balls");

								} else if (AritsuNoAlea == 9){

									BonusFX * BFX = this->actOverTemporaryBonuses(9);
									BFX->setDesc("Slower balls");
									torus->setText("Slower balls");

									slownessFactor = 1+0.5*BFX->getLevel();

									int i;

									for (i = bArray->firstBall(2); !bArray->reachedLimit(2); i = bArray->nextBall(2)){
										Ball *  b = bArray->getCurrentBall(2);
										b->setSlownessLevel(BFX->getLevel());
									}

								} else if (AritsuNoAlea == 10){

									int iniLevel;

									BonusFX * BFX0 = this->getTemporaryBonusObject(10);
									if (BFX0 == NULL) iniLevel = 0;
									else iniLevel = BFX0->getLevel();


									BonusFX * BFX = this->actOverTemporaryBonuses(10);
									BFX->setDesc("More balls");
									torus->setText("More balls");

									int i;
									int comp = BFX->getLevel()-iniLevel;

									for (i = 0; i < (this->getDifficultyObject()->getPlusBallsStep())*comp; i++){
										Ball * b = new Ball(this->getDifficultyObject());
										bArray->addBall(b);
										b->setActive(1);
										b->spawn();
									}


								} else if (AritsuNoAlea == 11){

									BonusFX * BFX = this->actOverTemporaryBonuses(11);
									BFX->setDesc("More effect time");
									torus->setText("More effect time");

									maximumFXDuration = this->getDifficultyObject()->getBonusMaxTime() + this->getDifficultyObject()->getXtTimeStep()*BFX->getLevel();

								} else if (AritsuNoAlea == 12){

									BonusFX * BFX = this->actOverTemporaryBonuses(12);
									BFX->setDesc("Limited spawn range");
									torus->setText("Limited spawn range");

									lsrFactor = BFX->getLevel();

								}

							} else {
								int baseScore = 10;
								if (b->getSpeed() >= 60) baseScore += (int) (b->getSpeed() - 60)/9;
								baseScore *= 5;

								int newScore = baseScore*((1.0*this->getConsecutiveHits())/(this->getConsecutiveHits()+4));

								bool found3 = false;
								int level3 = 0;

								for (i = 0; i < this->temporaryBonuses.size() && !found3; i++){
									if (this->temporaryBonuses[i]->getID() == 3){
										found3 = true;
										level3 = this->temporaryBonuses[i]->getLevel();
									}
								}

								if (level3 == 1) newScore *= 1.2;
								if (level3 == 2) newScore *= 1.5;
								if (level3 == 3) newScore *= 1.7;
								if (level3 == 4) newScore *= 2.0;
								if (level3 == 5) newScore *= 2.5;

								this->setScore(this->getScore()+newScore);

								char * str = (char*) malloc(sizeof(char)*20);
								strcpy(str, "+");
								strcat(str, intToString(newScore));

								torus->setText(str);
							}

							this->incremementConsecutiveHits();



							if (this->getAritsuBonusState() == 0){

								if (c.red == 1 && c.green == 0 && c.blue == 0){
									this->setAritsuBonusState(1);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}

							} else if (this->getAritsuBonusState() == 1){
								if (c.red == 0 && c.green == 1 && c.blue == 0){
									RGBColor blue;
									blue.red = 0;
									blue.green = 0;
									blue.blue = 1;
									blue.alpha = 1;

									this->setAritsuBonusState(2);
									torus->setColor(blue);

								} else if (c.red == 1 && c.green == 1 && c.blue == 0){
									RGBColor green;
									green.red = 0;
									green.green = 1;
									green.blue = 0;
									green.alpha = 1;

									this->setAritsuBonusState(12);
									torus->setColor(green);

								} else if (c.red == 1 && c.green == 0.5 && c.blue == 0){
									RGBColor yellow;
									yellow.red = 1;
									yellow.green = 1;
									yellow.blue = 0;
									yellow.alpha = 1;
									torus->setColor(yellow);

									this->setAritsuBonusState(22);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 2){
								if (c.red == 0 && c.green == 0 && c.blue == 1){
									this->setScore(this->getScore()+250);

									char * str = (char*) malloc(sizeof(char)*20);
									strcpy(str, "+");
									strcat(str, intToString(250));
									strcat(str, " bonus");
									torus->setText(str);

									this->setAritsuBonusState(0);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 12){
								if (c.red == 0 && c.green == 1 && c.blue == 0){

									RGBColor blue;
									blue.red = 0;
									blue.green = 0;
									blue.blue = 1;
									blue.alpha = 1;
									torus->setColor(blue);

									this->setAritsuBonusState(13);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 13){
								if (c.red == 0 && c.green == 0 && c.blue == 1){
									this->setScore(this->getScore()+400);

									char * str = (char*) malloc(sizeof(char)*20);
									strcpy(str, "+");
									strcat(str, intToString(400));
									strcat(str, " bonus");
									torus->setText(str);

									this->setAritsuBonusState(0);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 22){
								if (c.red == 1 && c.green == 1 && c.blue == 0){

									RGBColor cyan;
									cyan.red = 0;
									cyan.green = 1;
									cyan.blue = 1;
									cyan.alpha = 1;
									torus->setColor(cyan);

									this->setAritsuBonusState(23);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 23){
								if (c.red == 0 && c.green == 1 && c.blue == 1){

									RGBColor blue;
									blue.red = 0;
									blue.green = 0;
									blue.blue = 1;
									blue.alpha = 1;
									torus->setColor(blue);

									this->setAritsuBonusState(24);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 24){
								if (c.red == 0 && c.green == 0 && c.blue == 1){

									RGBColor fuchsia;
									fuchsia.red = 1;
									fuchsia.green = 0;
									fuchsia.blue = 1;
									fuchsia.alpha = 1;
									torus->setColor(fuchsia);


									this->setAritsuBonusState(25);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 25){
								if (c.red == 1 && c.green == 0 && c.blue == 1){

									RGBColor violet;
									violet.red = 0.5;
									violet.green = 0;
									violet.blue = 0.5;
									violet.alpha = 1;
									torus->setColor(violet);


									this->setAritsuBonusState(26);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							} else if (this->getAritsuBonusState() == 26){
								if (c.red == 0.5 && c.green == 0 && c.blue == 0.5){
									this->setScore(this->getScore()+1000);

									char * str = (char*) malloc(sizeof(char)*20);
									strcpy(str, "+");
									strcat(str, intToString(1000));
									strcat(str, " bonus");
									torus->setText(str);



									this->setAritsuBonusState(0);
								} else if (!found3) {
									this->setAritsuBonusState(0);
								}
							}



						} else {
							int i;

							BonusFX * get3 = this->getTemporaryBonusObject(3);

							if (get3 == NULL){
								this->resetConsecutiveHits();
								this->setAritsuBonusState(0);
							}
						}

						b->setState(1);

					} else if (b->getState() == 1 && b->getAlpha() == 0){
						this->attemptToSpawnBalls(i, false);
					}
				}
			}
		}
	}

	

};

Programa * prog;


char * intToString(int inp){
	char * sc;
	int i;
	int t;
	t = inp;
	int r;
	i = 0;
	int a;
	int ia;
	char tmpch;

	sc = (char*) malloc(sizeof(char)*20);

	if (t == 0){
		sc[0] = 48;
		i = 1;
	}

	while (t != 0){
		r = t%10;

		sc[i] = (char) 48+r;

		t = t/10;
		i++;
	}

	for (a = 0; a < i/2; a++){
		ia = i-a-1;
		tmpch = sc[ia];
		sc[ia] = sc[a];
		sc[a] = tmpch;
	}

	sc[i] = 0;
	return sc;
}


void timeFlowOut(int t){
	prog->timeFlow(t);
	int nextTime;

	if (prog->getScreenId() == 0) nextTime = 0;
	else if (prog->isPaused()) nextTime = t;
	else nextTime = t+1;

	glutTimerFunc(1000/prog->getRefreshRate(), timeFlowOut, nextTime);
	glutPostRedisplay();
}

void AlteraTamanhoJanela(GLsizei wNew, GLsizei hNew)
{
     // Evita a divisao por zero
     if(hNew == 0) hNew = 1;

	  prog->setWinWidth(wNew);
	  prog->setWinHeight(hNew);

	 glutPostRedisplay();
}


void GerenciaMouse(int button, int state, int x, int y)
{
	ActionWord * aw;
	int i;

	if (wArray->isActive()){
		if (state == GLUT_UP && button == GLUT_LEFT_BUTTON){
			for (i = wArray->firstWord(0); !wArray->reachedLimit(0); i = wArray->nextWord(0)){
				aw = wArray->getWordObject(0);
				if (aw->inBoundaries(x, y)) {
					aw->runCallBack();
				}
			}
		}
	}
}

// Função callback chamada para gerenciar eventos de teclado
void GerenciaTeclado(unsigned char key, int x, int y)
{
	if (prog->getScreenId() == 1){
		if (key == 27){ // Tecla ESC
			if (!prog->isPaused()) prog->pause();
			else prog->resume();
		}
	}

}

void GerenciaTeclado2(int key, int x, int y){

	if (!prog->wasTimeLimitExceeded()){
		if (!prog->isPaused()){

			if (key == GLUT_KEY_LEFT) torus->moveLeft();
			if (key == GLUT_KEY_RIGHT) torus->moveRight();

			if (key == GLUT_KEY_UP){
				if (prog->getCamera() == 0) torus->moveUp();
				if (prog->getCamera() == 1) torus->moveDown();
			}

			if (key == GLUT_KEY_DOWN){
				if (prog->getCamera() == 0) torus->moveDown();
				if (prog->getCamera() == 1) torus->moveUp();
			}
		}

		if (key == GLUT_KEY_F2){
			prog->setCamera(1-prog->getCamera());
		}
	}


	glutPostRedisplay();
}


void marcaActionWords(int x, int y, int newState){
	ActionWord * aw;
	int i;

	for (i = wArray->firstWord(0); !wArray->reachedLimit(0); i = wArray->nextWord(0)){
		aw = wArray->getWordObject(0);
		if (aw->inBoundaries(x, y)) {
			aw->setState(newState);
		} else {
			aw->setState(0);
		}
	}
}

void GerenciaMouseMovendoComum(int x, int y){
	float curx;
	float cury;

	if (prog->getCamera() == 1){
		y = prog->getWinHeight()-y;
	}

	curx = (float) 2*(((float) x/prog->getWinWidth())-0.5);
	cury = (float) 2*(((float) y/prog->getWinHeight())-0.5);

	torus->setRelCoords(curx, cury);
}

void GerenciaMouseMovendo(int x, int y){
	if (wArray->isActive()){
		marcaActionWords(x, y, 1);
		return;
	}

	GerenciaMouseMovendoComum(x, y);
	glutPostRedisplay();
}

void GerenciaMouseMovendoClicando(int x, int y){
	if (wArray->isActive()){
		marcaActionWords(x, y, 2);
		return;
	}

	GerenciaMouseMovendoComum(x, y);
	glutPostRedisplay();
}


void Desenha(){
	prog->displayDecider();
}

void resumeOut(){
	prog->resume();
}

void mainMenuOut(){
	prog->setScreenId(0);
}

void playOut(){
	prog->setScreenId(1);
}

void setEasyOut(){
	prog->setDifficulty(0);
	sdArray->setDifficulty("Easy");
}

void setStandardOut(){
	prog->setDifficulty(1);
	sdArray->setDifficulty("Standard");
}

void setHardOut(){
	prog->setDifficulty(2);
	sdArray->setDifficulty("Hard");
}

int main(){
	 prog = Programa::getInstance();
	 glutInitWindowSize(prog->getWinWidth(),prog->getWinHeight());
	 int x = (glutGet(GLUT_SCREEN_WIDTH)-prog->getWinWidth())/2;
	 int y = (glutGet(GLUT_SCREEN_HEIGHT)-prog->getWinHeight())/2;

	 glutInitWindowPosition(x,y);

     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
     glutCreateWindow("Falling balls"); //Toorasu-joo no booru
	 glutReshapeFunc(AlteraTamanhoJanela);
	 glutMouseFunc(GerenciaMouse);
	 glutPassiveMotionFunc(GerenciaMouseMovendo);
	 glutMotionFunc(GerenciaMouseMovendoClicando);
	 glutKeyboardFunc(GerenciaTeclado);
	 glutSpecialFunc(GerenciaTeclado2);
	 glutDisplayFunc(Desenha);
     glutReshapeFunc(AlteraTamanhoJanela);
     glutMainLoop();
}
