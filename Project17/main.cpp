#define _CRT_SECURE_NO_WARNINGS      
#include"object.h"
#include <stdlib.h>       
#include<time.h>    
#include"particle.h"      
#include"texture.h"  

#include"snowflower.h"
#include"butterfly.h"
#include"collision.h"
const int skySizeX = 200;
const int skySizeY = 30;
const int skySizeZ = 200;

#define BUFSIZE 100  
emitter *e1;
emitter *e2;
snow* s;
butterflyManager* butt;

GLuint texture[10];
GLint houseList = 0;
GLint baseList = 0;
//����      
float whRatio;
int wHeight = 0;
int wWidth = 0;


//�ӵ�      
float center[] = { 0, 0, 0 };
float eye[] = { 0, 0, 50 };
GLfloat dx = 0, dy = 0, dz = 0;
GLfloat ax = 0, ay = 0, az = 0;
GLint mx = 0, my = 0;
GLint MouseDown = 0;
float ratio = 180;
bool isSnow = false;
bool snowflag = false;
//����obj
map<string, Object> housemap;
set<string> housename;
string house;
map<string, Material> house_matname;

map<string, Object> basemap;
set<string> basename;
string base;
map<string, Material> base_matname;

GLuint selectBuf[BUFSIZE];
int rotate_state = 1;
bool isRotate = false;


particle* init_particle()
{
	float size = rand() % 20 * 0.03f;
	unsigned char color[] = { 1.0f,0.0f,0.0f };
	float speed[] = { float(rand() % 10 - 4) / 800, float(rand() % 10 - 4) / 400, float(rand() % 10 - 4) / 800 };
	float acc[] = { 1.0f*(rand()%3 - 1)/90000,-4.9 / 4000 ,1.0f*(rand() % 3 - 1) / 90000 };
	float angle[] = { rand() % 360, rand() % 360 ,rand() % 360 };
	particle* p = new particle(vec(size,size,size),vec(speed),vec(acc),
	vec(angle), rand()%100,texture[0]);
	return p;
}

particle* init_flame()
{
	float size = rand() % 90 * 0.02f;
	float speed[] = { float(rand() % 10 - 4) / 1600, float(rand() % 10 - 4) / 800, float(rand() % 10 - 4) / 1600 };
	float acc[] = { 1.0f*(rand() % 3 - 1) / 9000000,4.9 / 4000000 ,1.0f*(rand() % 3 - 1) / 9000000 };
	float angle[] = { rand() % 360, rand() % 360 ,rand() % 360 };
	particle* p = new particle(vec(size, size, size), vec(speed), vec(acc),
		vec(angle), rand() % 50 + 10, texture[2]);
	return p;
}


GLint GenTableList()
{
	GLint lid = glGenLists(1);

	glNewList(lid, GL_COMPILE);
	glPushMatrix();
	glTranslatef(0, -1.0f*skySizeY / 2.0f - 2.0f, -1.0f*skySizeZ / 2.0f + 60.0f);
	glScalef(3.0f, 3.0f, 3.0f);
	glRotatef(90, 0, 1, 0);
	loadObj(housename, housemap, house_matname);
	glPopMatrix();

	glNewList(lid, GL_COMPILE);
	glPushMatrix();
	glTranslatef(5.0f, -1.0f*skySizeY / 2.0f,-23.0f);
	glScalef(0.8f, 0.8f, 0.8f);
	loadObj(basename, basemap,base_matname);
	glPopMatrix();

	glEndList();
	return lid;
}

int col = 4, num = 12;

void drawRect(GLuint texture, int i, int j)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);  //ѡ������texture[status]       

	const GLfloat x1 = -0.5, x2 = 0.5;
	const GLfloat y1 = -0.5, y2 = 0.5;
	const GLfloat x = 1.0 / col, y = 1.0 / (num / col);
	const GLfloat point[4][2] = { { x1,y1 },{ x2,y1 },{ x2,y2 },{ x1,y2 } };
	const GLfloat dir[4][2] = { { j*x,1 - (i + 1)*y },{ (j + 1)*x,1 - (i + 1)*y },{ (j + 1)*x ,1 - i*y },{ j*x,1 - i*y } };
	glBegin(GL_QUADS);

	for (int k = 0; k < 4; k++) {
		glTexCoord2fv(dir[k]);
		glVertex2fv(point[k]);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
void drawTop(GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);  //ѡ������texture[status]       
	int i = 0, j = 0;
	
	const GLfloat x = 1.0 / col, y = 1.0 / (num / col);
	const GLfloat x0[] = { 0,x,2 * x,3 * x,4 * x };
	const GLfloat x1 = -0.5f, x2 = 0.5f;
	const GLfloat y1 = -0.5f, y2 = 0.5f;
	const GLfloat point[12][2] = {
		{ x2,y1 },{ 0,0 },{ x1,y1 },
		{ x1,y1 },{ 0,0 },{ x1,y2 },
		{ x1,y2 },{ 0,0 },{ x2,y2 },
		{ x2,y2 },{ 0,0 },{ x2,y1 },
	};
	const GLfloat dir[12][2] = { { x0[0],1 - y },{ x0[2],1 },{ x0[1],1 - y },
	{ x0[1],1 - y },{ x0[2],1 },{ x0[2],1 - y },
	{ x0[2],1 - y },{ x0[2],1 },{ x0[3],1 - y },
	{ x0[3],1 - y },{ x0[2],1 },{ x0[4],1 - y } };
	
	glBegin(GL_TRIANGLES);

	for (int k = 0; k < 12; k++) {
		glTexCoord2fv(dir[k]);
		glVertex2fv(point[k]);
	}
	glEnd();
	

	glDisable(GL_TEXTURE_2D);
}

void drawBottom(GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);  //ѡ������texture[status]       
	int i = 0, j = 0;

	const GLfloat x = 1.0 / col, y = 1.0 / (num / col);
	const GLfloat x0[] = { 0,x,2 * x,3 * x,4 * x };
	const GLfloat x1 = -0.5f, x2 = 0.5f;
	const GLfloat y1 = -0.5f, y2 = 0.5f;
	const GLfloat point[12][2] = {
		{ x2,y1 },{ 0,0 },{ x1,y1 },
		{ x1,y1 },{ 0,0 },{ x1,y2 },
		{ x1,y2 },{ 0,0 },{ x2,y2 },
		{ x2,y2 },{ 0,0 },{ x2,y1 },
	};
	const GLfloat dir[12][2] = { { x0[0],y },{ x0[2],0 },{ x0[1],y },
	{ x0[1],y },{ x0[2],0 },{ x0[2],y },
	{ x0[2],y },{ x0[2],0 },{ x0[3],y },
	{ x0[3],y },{ x0[2],0 },{ x0[4],y } };

	glBegin(GL_TRIANGLES);

	for (int k = 0; k < 12; k++) {
		glTexCoord2fv(dir[k]);
		glVertex2fv(point[k]);
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);
}

void drawRect(GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);  //ѡ������texture[status]       
	const GLfloat x1 = -0.5, x2 = 0.5;
	const GLfloat y1 = -0.5, y2 = 0.5;
	const GLfloat point[4][2] = { { x1,y1 },{ x2,y1 },{ x2,y2 },{ x1,y2 } };
	int dir[4][2] = { { 0,0 },{ 1,0 },{ 1,1 },{ 0,1 } };
	glBegin(GL_QUADS);

	for (int i = 0; i < 4; i++) {
		glTexCoord2iv(dir[i]);
		glVertex2fv(point[i]);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

bool isDead(particle* p)
{
	const vec& place = p->getPlace();
	if (place.y < -1.0f*skySizeY / 2) {
		s->add(place.x, place.z, p->getSize(),p->getAngle());
		return true;
	}
	return false;
}

void drawScene()
{
//	drawLine();

	static int cnt = 0;
	glInitNames();//��ʼ������  
	glPushName(0);//��ʼ������ջ  
	if (isRotate && rotate_state ==1 && ratio >= 120) ratio -= 0.05f;
	else if(isRotate && rotate_state == 2 && ratio <=180)ratio += 0.05f;
	else if (isRotate && (ratio < 120||ratio>180)) {
		isRotate = false;
		if (rotate_state == 1) {
			rotate_state = 2;
		}
		else if (rotate_state == 2) {
			rotate_state = 1;
		}
	}
	
	glCallList(houseList);

	glCallList(baseList);
	glLoadName(1);//��������  
	//��
	glPushMatrix();
	glTranslatef(-17.0f,-9.0f, -23.0f);
	glRotatef(180, 1, 0, 0);
	glRotatef(180, 0, 0, 1);
	glRotatef(ratio, 0, 1, 0);
	glTranslatef(5.0f, 0,0);
	glScalef(10, 11, 1);
	drawRect(texture[4]);
	glPopMatrix();
	glPopName();
	//�ذ�
	glPushMatrix();
	glTranslatef(0.0f, -1.0f*skySizeY / 2.0f, 0.0f);
	glRotatef(270, 1, 0, 0);
	glScalef(skySizeX, skySizeZ, 1);
	drawBottom(texture[1]);
	glPopMatrix();

	//�컨��	
	glPushMatrix();
	glTranslatef(0.0f, 1.0f*skySizeY / 2.0f, 0.0f);
	glRotatef(270, 1, 0, 0);
	glScalef(skySizeX, skySizeZ, 1);
	drawTop(texture[1]);
	glPopMatrix();

	//ǽ�ڣ�ǰ��
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f*skySizeZ / 2.0);
	glRotatef(180, 1, 0, 0);
	glRotatef(180, 0, 0, 1);
	glRotatef(180, 0, 1, 0);
	glScalef(skySizeX, skySizeY, 1);
	drawRect(texture[1],1,2);
	glPopMatrix();

	//ǽ�ڣ���
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f*skySizeZ / 2.0f);
	glRotatef(180, 0, 1, 0);
	glScalef(skySizeX, skySizeY, 1);
	drawRect(texture[1],1,0);
	glPopMatrix();

	//ǽ�ڣ���
	glPushMatrix();
	glTranslatef(-1.0f*skySizeX / 2.0f, 0.0f, 0.0f);
	glRotatef(90, 0, 1, 0);
	glScalef(skySizeZ, skySizeY, 1);
	drawRect(texture[1],1,1);
	glPopMatrix();

	//ǽ�ڣ��ң�
	glPushMatrix();
	glTranslatef(1.0f*skySizeX / 2.0f, 0.0f, 0.0f);
	glRotatef(270, 0, 1, 0);
	glScalef(skySizeZ, skySizeY, 1);
	drawRect(texture[1],1,3);
	glPopMatrix();
	static int last_t = 0;
	//	int t = timer->GetTime();
	static int t = 0;
	glEnable(GL_BLEND);
	if (isSnow) {
		snowflag = true;
		t++;
		if (t % 10 == 0 && t != last_t) {
			e1->update();
			last_t = t;
			if (t == 100) {
				t = 0;
				last_t = 0;
			}
		}
		else {
			e1->show();
		}
	}
	else if(snowflag){
		if (t >= 10) {
			e1->fade();
			s->remove();
			t = 0;
		}
		e1->show();
		t++;
	}
	s->show();
	
	e2->show();
	e2->update();
	cnt++;

	butt->show();
	glDisable(GL_BLEND);
}

void updateView(int height, int width)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);//���þ���ģʽΪͶӰ           
	glLoadIdentity();   //��ʼ������Ϊ��λ����              
	whRatio = (GLfloat)width / (GLfloat)height;  //������ʾ����             
	gluPerspective(45, whRatio, 1,400); //͸��ͶӰ  
	glMatrixMode(GL_MODELVIEW);  //���þ���ģʽΪģ��        
}

void reshape(int width, int height)
{
	if (height == 0) { //����߶�Ϊ0     
		height = 1;   //�ø߶�Ϊ1��������ַ�ĸΪ0������          
	}
	wHeight = height;
	wWidth = width;
	updateView(wHeight, wWidth); //�����ӽ�          
}

void idle()
{
	glutPostRedisplay();
}

void init(void)
{
	srand(unsigned(time(nullptr)));
	glClearColor(1.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0, 1.0, 1.0, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	float pos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float white[] = { 255,255,255 };
	float dimwhite[] = { 0.2*255,0.2*255,0.2*255 };
	//Set up light
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_AMBIENT, dimwhite);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glEnable(GL_LIGHT1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);
	readObj(house, "obj\\house.obj", housemap, housename, house_matname);
	readObj(house, "obj\\base.obj", basemap, basename, base_matname);
	houseList = GenTableList();
	baseList = houseList + 1;
	BuildTexture("snowflower.jpg", texture[0]);
	BuildTexture("background.jpg", texture[1]);
	BuildTexture("spark.bmp", texture[2]);
	BuildTexture("packet.jpg", texture[3]);
	BuildTexture("door.jpg", texture[4]);
	BuildTexture("Butterfly1.bmp",texture[5]);
	BuildTexture("Butterfly2.bmp", texture[6]);
	BuildTexture("Butterfly3.bmp", texture[7]);

	s = new snow(texture[0]);

	e1 = new emitter(1000, -100, 100, 15, 15,-100,100);
	e1->emit(init_particle,isDead);

	e2 = new emitter(400,1.5f, 2.5f, -8.0f, -7.5f,-19.5f, -20.5f);
	e2->emit(init_flame);
	
	int* tex = new int[3];
	tex[0] = texture[5];
	tex[1] = texture[6];
	tex[2] = texture[7];

	butt = new butterflyManager(tex);
	delete tex;

}

void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//�����ɫ����Ȼ���     
	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   //��ʼ������Ϊ��λ����          
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);
	glPushMatrix();
	glTranslatef(dx, dy, dz);
	glRotatef(ax, 1.0f, 0.0f, 0.0f);
	glRotatef(ay, 0.0f, 1.0f, 0.0f);
	drawScene();//���Ƴ��� 
	glPopMatrix();

	glutSwapBuffers();//����������      
}

void myMouse(int button, int state, int x, int y)
{
	
	if (button == GLUT_LEFT_BUTTON && button == GLUT_DOWN) {
		//�洢�ӽ�  
		GLint viewport[4];

		glSelectBuffer(BUFSIZE, selectBuf);//ѡ�񷵻����ݵ�����  
		glGetIntegerv(GL_VIEWPORT, viewport);//�õ��ӽ�  
		glRenderMode(GL_SELECT);//����ѡ��ģʽ  

		glMatrixMode(GL_PROJECTION);//���þ���ģʽΪͶӰ  
		glPushMatrix();//������ջ  
		glLoadIdentity();//��ʼ��Ϊ��λ����  

		gluPickMatrix((GLdouble)x,
			(GLdouble)(viewport[3] - y), 1, 1, viewport);//����ѡ�����  

		gluPerspective(45, whRatio, 1, 400); //͸��ͶӰ   
		
		redraw(); //�ػ�  

		int hits = glRenderMode(GL_RENDER); //������Ⱦģʽ���õ�ѡ���������  

		if (hits >= 1) //���ѡ�������������1  
		{
			int choose = selectBuf[3]; //�õ�ѡ����������  
			int depth = selectBuf[1]; //�õ�ѡ���������  

			for (int i = 0; i < hits; i++)
			{
				if (selectBuf[i * 4 + 1] < (GLuint)depth)//��ȡ�����С������(selectBuff�ǰ���ID��С�������е�)  
				{
					choose = selectBuf[i * 4 + 3];
					depth = selectBuf[i * 4 + 1];
				}
			}
			if (choose == 1) {
				isRotate = true;
			}
		}
		//���þ���ģʽΪͶӰ  
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();//�׳�����  
					  //���þ���ģʽΪģ��  
		glMatrixMode(GL_MODELVIEW);
	}
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q': {exit(0); break; } //�˳�  

	case 'a': { //����  
		printf("e\n");
		move(eye, center, direction::left);
		break;
	}
	case 'd': { //����  
		move(eye, center, direction::right);
		break;
	}
	case 'w': { //ǰ��  
		move(eye, center, direction::front);
		break;
	}
	case 's': {  //����  
		move(eye, center, direction::back);
		break;
	}

	case 'j': {//�ӽ�����  
		rotate(eye, center, direction::left);
		break;
	}
	case 'l': {//�ӽ�����  
		rotate(eye, center, direction::right);
		break;
	}
	case 'i': {//�ӽ�����  
		center[1] += 0.1f;
		break;
	}
	case 'k': {//�ӽ�����  
		center[1] -= 0.1f;
		break;
	}
	case 'o': {
		s->remove();
		break;
	}
	case 'n': {
		if (!isSnow) {
			e1->reset();
		}
		isSnow = true;
		break;
	}
	case 'm': {
		isSnow = false;
		break;
	}
	case 'z': {
		butt->isDraw = true;
		break;
	}
	case 'x': {
		butt->isDraw = false;
		break;
	}
	}
	outCollisionTest(eye, center, -skySizeX / 2, skySizeX / 2, -skySizeZ / 2, skySizeZ / 2);
	inCollisionTest(eye, center, -40, -50,10,0);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);//��glut�ĳ�ʼ��             
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	//��ʼ����ʾģʽ:RGB��ɫģ�ͣ���Ȳ��ԣ�˫����               
	glutInitWindowSize(800, 600);//���ô��ڴ�С             
	int windowHandle = glutCreateWindow("Simple GLUT App");//���ô��ڱ���               
	glutDisplayFunc(redraw); //ע����ƻص�����             
	glutReshapeFunc(reshape);   //ע���ػ�ص�����    

	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);

	glutIdleFunc(idle);//ע��ȫ�ֻص�����������ʱ����           
	init();
	glutMainLoop();  // glut�¼�����ѭ��           
	system("pause");
	return 0;
}