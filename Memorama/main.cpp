//
//  main.cpp
//  Memorama
//
//  Created by manolo on 2/6/16.
//  Copyright © 2016 manolo. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <GLUT/GLUT.h>
#include <vector>
#include "math.h"
#include <math.h>
#include <string>
using namespace std;

#pragma mark - global vars

typedef enum { playing, paused, finished } State;

State state = paused;
char numbers[8][2] = {"0", "1", "2", "3", "4", "5", "6", "7"};
bool correct[16] = {false};
int correctPairs = 0;
int selected[3] = {0, -1, -1};
int order[16] = {1, 3, 5, 4, 2, 6, 7, 0, 4, 3, 1, 2, 0, 5, 7, 6};
bool highlighted[16] = {false};

bool drawSolutionSwitch = false;
int lastHighlightedCardIndex = -1;

int timerTotal = 0, screenWidth = 800, screenHeight = 500, turns = 0;

#pragma mark - aux

int cardForX(int x) {
	int width = screenWidth/16;
	return (x/width)%width;
}

void draw3dString (void *font, char *str, float x, float y, float scale) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScaled(scale, scale, scale);
	
	for (int i = 0; str[i] != '\0'; i++) {
		glutStrokeCharacter(font, str[i]);
	}
	glPopMatrix();
}

vector<GLubyte> formatTime() {
	int mins = timerTotal / 600;
	int secs = (timerTotal - (600*mins)) / 10;
	int secs1 = secs / 10;
	int secs2 = secs % 10;
	int decs = timerTotal % 10;
	
	vector<GLubyte> str;
	if (mins > 9) {
		str.push_back((mins/10) + 48);
	}
	str.push_back((mins%10) + 48);
	str.push_back(':');
	str.push_back(secs1 + 48);
	str.push_back(secs2 + 48);
	str.push_back('.');
	str.push_back(decs + 48);
	
//	for(vector<GLubyte>::iterator it = str.begin(); it < str.end(); it++) {
//		cout << *it;
//	}
//	cout << endl;
	
	return str;
}

void timer(int i) {
	if (state != playing) {
		return;
	}
	timerTotal++;
	formatTime();
	glutPostRedisplay();
	glutTimerFunc(100, timer, 0);
}

void drawNumber(int i, int position) {
	glColor3f(1, 1, 1);
	int x = position*(screenWidth/16);
	int y = 0.06*screenHeight;
	draw3dString(GLUT_STROKE_MONO_ROMAN, numbers[i], x+((screenWidth/16)/5), y, 0.3);
}

void drawSolution() {
    glColor3f(0.7, 0.7, 0.7);
    for (int i = 0; i < 16; i++) {
        draw3dString(GLUT_STROKE_MONO_ROMAN, numbers[order[i]], i*(screenWidth/16)+(screenWidth/16 / 5), screenHeight * 0.15, 0.1);
    }
}

void paintCards() {
	int w = screenWidth/16;
	// cout << w << endl;
	for (int i=0; i<16; i+=2) {
        if (highlighted[i] && !correct[i]){
            glColor3ub(100, 128, 194);
        } else {
            glColor3ub(4, 24, 36);
        }
		glRectd(i*w, 0, (i+1)*w, screenHeight/5);
	}
	
	for (int i=1; i<16; i+=2) {
        if (highlighted[i] && !correct[i]) {
            glColor3ub(100, 128, 194);
        } else {
            glColor3ub(8, 50, 76);
        }
		
		glRectd(i*w, 0, (i+1)*w, screenHeight/5);
	}
	
	for (int i=1; i<=selected[0]; i++) {
		glColor3ub(20, 128, 194);
		glRectd(selected[i]*w, 0, (selected[i]+1)*w, screenHeight/5);
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	
//	Timer
	glColor3f(1, 1, 1);
	glRasterPos2i(screenWidth*0.925, screenHeight*0.94);
	vector<GLubyte> time = formatTime();
	for(vector<GLubyte>::iterator it = time.begin(); it < time.end(); it++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *it);
	}
	
	paintCards();
    
    // drawing solution
    if (drawSolutionSwitch) {
        drawSolution();
    }
    
	for (int i=0; i<16; i++) {
		if (correct[i]) {
			drawNumber(order[i], i);
		}
	}
	
	for (int i=1; i<=selected[0]; i++) {
		drawNumber(order[selected[i]], selected[i]);
	}
	
	char msg[100] = "";
	glColor3f(1, 1, 1);
	switch (state) {
	case playing:
		sprintf(msg, "Turnos: %i", turns/2);
		draw3dString(GLUT_STROKE_ROMAN, msg, 10, 450, 0.3);
		break;
	case paused:
		sprintf(msg, "Credits: Luis Lamadrid [A01191158] & Manuel Sanudo [A01192241]");
		draw3dString(GLUT_STROKE_ROMAN, msg, 20, 300, 0.15);
		sprintf(msg, "I - iniciar, P - pausa, R - reiniciar, esc - salir");
		draw3dString(GLUT_STROKE_ROMAN, msg, 20, 260, 0.15);
		break;
	case finished:
		sprintf(msg, "Felicidades! Lo lograste en %i turnos.", turns/2);
		draw3dString(GLUT_STROKE_ROMAN, msg, 30, 300, 0.3);
		break;
	}
	
	glutSwapBuffers();
}

void reset() {
	state = paused;
	timerTotal = 0;
	correctPairs = 0;
	selected[0] = 0;
    drawSolutionSwitch = false;
	for (int i=0; i<16; i++) {
		correct[i] = false;
	}
	
    // randomize order
    int tmp, randIndex;
    for (int i = 0; i < 16; i++) {
        randIndex = random() % (int) 16;
        tmp = order[i];
        order[i] = order[randIndex];
        order[randIndex] = tmp;
    }
    turns = 0; // turn reset
}

void keyboard(unsigned char keyPressed, int mouseX, int mouseY) {
	switch (keyPressed) {
		case 'i': case 'I':
			if (state == paused) {
				state = playing;
				timer(0);
			}
			break;
		case 'p': case 'P':
			if (state == playing) {
				state = paused;
				glutPostRedisplay();
			}
			break;
		case 'r': case 'R':
			reset();
			glutPostRedisplay();
			break;
        case 'a': case 'A':
            drawSolutionSwitch = !drawSolutionSwitch;
            glutPostRedisplay();
            break;
		case 27:// Escape key
			exit(0);
		default:
			break;
	}
}

bool click(int x, int y) {
	if (y <= screenHeight/5) {
		int card = cardForX(x);
		if (!correct[card] && card != selected[1]) {
			selected[0] = 1+(selected[0])%2;
			selected[selected[0]] = card;
			return true;
		}
	}
	return false;
}

void onMouseMotion(int x, int y) {
    // printf("X: %d, Y: %d\n", x, y);
    if (y >= 400 && y <= 500 && state == playing){ // to be changed to variable
        highlighted[lastHighlightedCardIndex] = false;
        lastHighlightedCardIndex = cardForX(x);
        highlighted[lastHighlightedCardIndex] = true;
    } else {
        highlighted[lastHighlightedCardIndex] = false;
    }
}

void mouse(int button, int buttonState, int x, int y) {
	if (state != playing) {
		return;
	}
	y = screenHeight-y;
	if (buttonState == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			if (click(x, y)) {
				turns++;
				if (selected[0] == 2) {
					if (order[selected[1]] == order[selected[2]]) {
						correct[selected[1]] = true;
						correct[selected[2]] = true;
						correctPairs++;
						if (correctPairs == 8) {
							state = finished;
							selected[0] = 0;
							glutPostRedisplay();
						}
					}
				}
			}
		}
	}
}

void reshape(int newWidth,int newHeight) {
//	screenWidth = newWidth;
//	screenHeight = newHeight;
	glViewport(0, 0, newWidth, newHeight);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluOrtho2D(0, 800, 0, 500);
	
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char *argv[]) {
	reset();
	
	glutInit(&argc, argv);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Memorama");
	glClearColor(0.12, 0.34, 0.56, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
	gluOrtho2D(0, screenWidth, 0, screenHeight);
    
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
    glutPassiveMotionFunc( onMouseMotion );
	glutTimerFunc(33,timer,1);
	glutMainLoop();
	return 0;
}