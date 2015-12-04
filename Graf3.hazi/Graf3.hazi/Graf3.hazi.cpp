// Graf3.hazi.cpp : Defines the entry point for the console application.
//

//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fßggvÊnyt kivÊve, a lefoglalt adat korrekt felszabadítåsa nÊlkßl 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : <VEZETEKNEV(EK)> <KERESZTNEV(EK)>
// Neptun : <NEPTUN KOD>
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;

	Vector() {
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0) {
		x = x0; y = y0; z = z0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) { 	// dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) { 	// cross product
		return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	float Length() { return sqrt(x * x + y * y + z * z); }

	Vector normalize() {
		return (*this) * (1 / (Length()));
	}
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
	float r, g, b;

	Color() {
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0) {
		r = r0; g = g0; b = b0;
	}
	Color operator*(float a) {
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c) {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

const int screenWidth = 600;	// alkalmazås ablak felbontåsa
const int screenHeight = 600;


const int segments = 31;
const int numOfCpoints = 8;
const int numOfBeziers = 6;
long times[numOfBeziers] = { 1, 2, 3, 4, 5, 6 };

void drawSphere() {
	Vector p1, p2, prevp1, prevp2;
	glBegin(GL_QUAD_STRIP);
	for (int i = 2; i <= 51; i++) {
		float prevb = ((float)(i - 1)) / 50 * M_PI;
		float b = ((float)i) / 50 * M_PI;

		for (int j = 2; j <= 51; j++) {
			float preva = ((float)(j - 1) / 50)*M_PI*2.0f;
			float a = ((float)j / 50.0)*M_PI*2.0f;
			prevp1.x = sinf(prevb)*cosf(preva);
			prevp1.y = sinf(prevb)*sinf(preva);
			prevp1.z = cosf(prevb);

			prevp2.x = sinf(prevb)*cosf(a);
			prevp2.y = sinf(prevb)*sinf(a);
			prevp2.z = cosf(prevb);

			p1.x = sinf(b)*cosf(preva);
			p1.y = sinf(b)*sinf(preva);
			p1.z = cosf(b);

			p2.x = sinf(b)*cosf(a);
			p2.y = sinf(b)*sinf(a);
			p2.z = cosf(b);

			

			glNormal3f(prevp1.x, prevp1.y, prevp1.z);
			glVertex3f(prevp1.x, prevp1.y, prevp1.z);
			glNormal3f(prevp2.x, prevp2.y, prevp2.z);
			glVertex3f(prevp2.x, prevp2.y, prevp2.z);
			glNormal3f(p1.x, p1.y, p1.z);
			glVertex3f(p1.x, p1.y, p1.z);
			glNormal3f(p2.x, p2.y, p2.z);
			glVertex3f(p2.x, p2.y, p2.z);



		}
	}
	glEnd();
}

void drawHenger(float r = 1) {
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 50; i++) {
		for (int j = 0; j<2; j++) {
			float szog = 2.0f*M_PI / 50 * i;
			Vector p = Vector(r*cosf(szog), j, r*sinf(szog));
			Vector n = Vector(cosf(szog), 0, sinf(szog));
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(p.x, p.y, p.z);
		}
	}
	glEnd();
}

void drawKup() {
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 50; i++) {
		for (int j = 0; j < 2; j++) {
			float szog = 2.0f*((float)i / 50.0f)*M_PI;
			Vector p = Vector((float)j*cosf(szog), (float)(-j), (float)j*sinf(szog));
			glNormal3f(p.x, p.y, p.z);
			glVertex3f(p.x, p.y, p.z);
		}
	}
	glEnd();
}

int fakt(int n)
{
	int res = 1;
	for (int i = 1; i <= n; ++i)
	{
		res *= i;
	}
	return res;
}


struct BezierCurve {
	Vector points[segments+1];
	Vector cpoints[10];
	BezierCurve(){}
	BezierCurve(Vector cponts[]) {
		Bezier(cponts);
	}

	void Bezier(Vector cpoints[])
	{
		int actualcurvepoint = 0;
		for (int j = 0; j <= 30; ++j)
		{
			float t = float(j) / 30.0f;
			float n = numOfCpoints - 1;
			Vector result = Vector();
			for (int i = 0; i<numOfCpoints; ++i)
			{
				result = result + cpoints[i] * Bernstein(n, i, t);
			}
			points[actualcurvepoint++] = result;
		}

	}
	float Bernstein(int n, int i, float t)
	{
		float ber = (float(fakt(n)) / (float(fakt(i))*float(fakt(n - i))))*powf(t, i)*powf(1 - t, n - i);
		return ber;
	}
};

BezierCurve curves[numOfBeziers];

void catmullrom()
{
	Vector v[numOfBeziers];
	Vector vnext[numOfBeziers];
	v[0] = Vector();
	vnext[0] = Vector();
	v[numOfBeziers - 1] = Vector();
	vnext[numOfBeziers - 1] = Vector();
	glColor3f(0, 1, 0);
	glBegin(GL_QUAD_STRIP);
	for (int k = 1; k < segments-1; k++) {
		for (int j = 1; j < numOfBeziers - 1; ++j)
		{

			v[j] = ((curves[j+1].points[k] - curves[j].points[k])*0.5*(1.0f / (times[j + 1] - times[j])) + (curves[j].points[k] - curves[j-1].points[k])*(1.0f / (times[j] - times[j - 1])));
			vnext[j] = ((curves[j + 1].points[k+1] - curves[j].points[k+1])*0.5*(1.0f / (times[j + 1] - times[j])) + (curves[j].points[k+1] - curves[j - 1].points[k+1])*(1.0f / (times[j] - times[j - 1])));
		}
		for (int i = 0; i < numOfBeziers - 1; ++i)
		{
			float dif_t = float(times[i + 1] - times[i]);
			Vector a0 = curves[i].points[k];
			Vector a1 = v[i];
			Vector a2 = (curves[i+1].points[k] - curves[i].points[k])*3.0f*(1.0f / powf(dif_t, 2)) - (v[i + 1] + v[i] * 2.0f)*(1.0f / dif_t);
			Vector a3 = (curves[i].points[k] - curves[i + 1].points[k])*2.0f*(1.0f / powf(dif_t, 3)) + (v[i + 1] + v[i])*(1.0f / powf(dif_t, 2));
			Vector a0next = curves[i].points[k+1];
			Vector a1next = vnext[i];
			Vector a2next = (curves[i + 1].points[k+1] - curves[i].points[k+1])*3.0f*(1.0f / powf(dif_t, 2)) - (vnext[i + 1] + vnext[i] * 2.0f)*(1.0f / dif_t);
			Vector a3next = (curves[i].points[k+1] - curves[i + 1].points[k+1])*2.0f*(1.0f / powf(dif_t, 3)) + (vnext[i + 1] + vnext[i])*(1.0f / powf(dif_t, 2));


			Vector dirbezierseg=
			for (float j = 0; j <= dif_t; j += dif_t / 50)
			{
				Vector pointofcurve = a3*powf(j, 3) + a2*powf(j, 2) + a1*j + a0;
				Vector dirofcatcurveseg = (a3*powf(j, 2)*3 + a2*j*2 + a1).normalize();
				Vector pointofnextcurve = a3next*powf(j, 3) + a2next*powf(j, 2) + a1next*j + a0next;
				glVertex3f(pointofcurve.x, pointofcurve.y, pointofcurve.z);
				glVertex3f(pointofnextcurve.x, pointofnextcurve.y, pointofnextcurve.z);
			}
		}
	}
	glEnd();
}


void makechickbody() {
	Vector cponts[] = { Vector(1,0,0), Vector(1,0,1),Vector(0.5,0,1), Vector(-0.5,0,1), Vector(-1,0,1), Vector(-1,0,0), Vector(-1,0,-1), Vector(1,0,-1) };
	curves[0] = BezierCurve(cponts);
	Vector cponts1[] = { Vector(2,3,0), Vector(2,3,2),Vector(2,3,4), Vector(-6,3,4), Vector(-6,3,2), Vector(-6,3,0), Vector(-6,3,-2), Vector(1,3,-2) };
	curves[1] = BezierCurve(cponts1);
	Vector cponts2[] = { Vector(5,6,0), Vector(5,6,2),Vector(5,6,4), Vector(-3,6,4), Vector(-3,6,2), Vector(-3,6,0), Vector(-3,6,-2), Vector(5,6,-2) };
	curves[2] = BezierCurve(cponts2);
	Vector cponts3[] = { Vector(2,9,0), Vector(2,9,2),Vector(2,9,4), Vector(-6,9,4), Vector(-6,9,2), Vector(-6,9,0), Vector(-6,9,-2), Vector(2,9,-2) };
	curves[3] = BezierCurve(cponts3);
	Vector cponts4[] = { Vector(5,12,0), Vector(5,12,2),Vector(5,12,4), Vector(-3,12,4), Vector(-3,12,2), Vector(-3,12,0), Vector(-3,12,-2), Vector(5,12,-2) };
	curves[4] = BezierCurve(cponts4);
	Vector cponts5[] = { Vector(1,3,0), Vector(4,3,2),Vector(4,3,4), Vector(-1,3,1), Vector(-1,3,1), Vector(-4,3,0), Vector(-3,3,-2), Vector(5,3,-2) };
	curves[5] = BezierCurve(cponts5);
}



										// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, screenWidth, screenHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	float position[] = { 7.0f,6.0f, 17.0f, 0.0f };
	float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float atten[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, atten);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(11, 11, 11, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.01, 100);
	glMatrixMode(GL_MODELVIEW);
	makechickbody();

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	//glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	// Majd rajzolunk egy kek haromszoget
	float ambient[4] = { 0, 0, 0.9f, 1.0f };
	float diffuse[4] = { 0, 0.9, 0.0f, 1.0f };
	float specular[4] = { 0.9, 0, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 25);
	//glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(10.0f, 0.0f, 0);
	glVertex3f(-10.0f, 0.0f, 0);
	glEnd();
	//glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 10.0f, 0);
	glVertex3f(0.0f, -10.0f, 0);
	glEnd();
	//glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 10);
	glVertex3f(0.0f, 0.0f, -10);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-10.0f, 10.0f, -10);
	glVertex3f(0.0f, 0.0f, 0);
	glEnd();
	

	glPushMatrix();
	//glScalef(2, 2, 2);
	glRotatef(90, 0, 1, 0);
	glRotatef(50, 1, 0, -1);
	catmullrom();
	glPopMatrix();
	/*glBegin(GL_TRIANGLES);
	glNormal3f(1, 1, 1);
	glVertex3f(5.0f, 0.0f, 0);
	glNormal3f(1, 1, 1);
	glVertex3f(0.0f, 5.0f, 0);
	glNormal3f(1, 1, 1);
	glVertex3f(0.0f, 0.0f, 5);
	glEnd();*/
	
	/*glPushMatrix();
	glScalef(4, 4, 4);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 5, 0);
	glScalef(4, 4, 4);
	drawHenger();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(8, 4, 0);
	//glRotatef(180, 0, 0, 1);
	glScalef(1, 4, 1);
	drawKup();
	glPopMatrix();*/

	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	if (key == 'd') glutPostRedisplay(); 		// d beture rajzold ujra a kepet

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay();					 // Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
	glutInit(&argc, argv); 				// GLUT inicializalasa
	glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
	glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();					// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}