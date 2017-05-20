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
//视区      
float whRatio;
int wHeight = 0;
int wWidth = 0;


//视点      
float center[] = { 0, 0, 0 };
float eye[] = { 0, 0, 50 };
GLfloat dx = 0, dy = 0, dz = 0;
GLfloat ax = 0, ay = 0, az = 0;
GLint mx = 0, my = 0;
GLint MouseDown = 0;
float ratio = 180;
bool isSnow = false;
bool snowflag = false;
//加载obj
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
	glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]       

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
	glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]       
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
	glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]       
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
	glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]       
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
	glInitNames();//初始化名字  
	glPushName(0);//初始化名字栈  
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
	glLoadName(1);//加载名字  
	//门
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
	//地板
	glPushMatrix();
	glTranslatef(0.0f, -1.0f*skySizeY / 2.0f, 0.0f);
	glRotatef(270, 1, 0, 0);
	glScalef(skySizeX, skySizeZ, 1);
	drawBottom(texture[1]);
	glPopMatrix();

	//天花板	
	glPushMatrix();
	glTranslatef(0.0f, 1.0f*skySizeY / 2.0f, 0.0f);
	glRotatef(270, 1, 0, 0);
	glScalef(skySizeX, skySizeZ, 1);
	drawTop(texture[1]);
	glPopMatrix();

	//墙壁（前）
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f*skySizeZ / 2.0);
	glRotatef(180, 1, 0, 0);
	glRotatef(180, 0, 0, 1);
	glRotatef(180, 0, 1, 0);
	glScalef(skySizeX, skySizeY, 1);
	drawRect(texture[1],1,2);
	glPopMatrix();

	//墙壁（后）
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f*skySizeZ / 2.0f);
	glRotatef(180, 0, 1, 0);
	glScalef(skySizeX, skySizeY, 1);
	drawRect(texture[1],1,0);
	glPopMatrix();

	//墙壁（左）
	glPushMatrix();
	glTranslatef(-1.0f*skySizeX / 2.0f, 0.0f, 0.0f);
	glRotatef(90, 0, 1, 0);
	glScalef(skySizeZ, skySizeY, 1);
	drawRect(texture[1],1,1);
	glPopMatrix();

	//墙壁（右）
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
	glMatrixMode(GL_PROJECTION);//设置矩阵模式为投影           
	glLoadIdentity();   //初始化矩阵为单位矩阵              
	whRatio = (GLfloat)width / (GLfloat)height;  //设置显示比例             
	gluPerspective(45, whRatio, 1,400); //透视投影  
	glMatrixMode(GL_MODELVIEW);  //设置矩阵模式为模型        
}

void reshape(int width, int height)
{
	if (height == 0) { //如果高度为0     
		height = 1;   //让高度为1（避免出现分母为0的现象）          
	}
	wHeight = height;
	wWidth = width;
	updateView(wHeight, wWidth); //更新视角          
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色和深度缓存     
	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   //初始化矩阵为单位矩阵          
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);
	glPushMatrix();
	glTranslatef(dx, dy, dz);
	glRotatef(ax, 1.0f, 0.0f, 0.0f);
	glRotatef(ay, 0.0f, 1.0f, 0.0f);
	drawScene();//绘制场景 
	glPopMatrix();

	glutSwapBuffers();//交换缓冲区      
}

void myMouse(int button, int state, int x, int y)
{
	
	if (button == GLUT_LEFT_BUTTON && button == GLUT_DOWN) {
		//存储视角  
		GLint viewport[4];

		glSelectBuffer(BUFSIZE, selectBuf);//选择返回数据的数组  
		glGetIntegerv(GL_VIEWPORT, viewport);//得到视角  
		glRenderMode(GL_SELECT);//进入选择模式  

		glMatrixMode(GL_PROJECTION);//设置矩阵模式为投影  
		glPushMatrix();//矩阵入栈  
		glLoadIdentity();//初始化为单位矩阵  

		gluPickMatrix((GLdouble)x,
			(GLdouble)(viewport[3] - y), 1, 1, viewport);//设置选择矩阵  

		gluPerspective(45, whRatio, 1, 400); //透视投影   
		
		redraw(); //重绘  

		int hits = glRenderMode(GL_RENDER); //进入渲染模式，得到选中物体个数  

		if (hits >= 1) //如果选中物体个数大于1  
		{
			int choose = selectBuf[3]; //得到选中物体名字  
			int depth = selectBuf[1]; //得到选中物体深度  

			for (int i = 0; i < hits; i++)
			{
				if (selectBuf[i * 4 + 1] < (GLuint)depth)//获取深度最小的物体(selectBuff是按照ID从小到大排列的)  
				{
					choose = selectBuf[i * 4 + 3];
					depth = selectBuf[i * 4 + 1];
				}
			}
			if (choose == 1) {
				isRotate = true;
			}
		}
		//设置矩阵模式为投影  
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();//抛出矩阵  
					  //设置矩阵模式为模型  
		glMatrixMode(GL_MODELVIEW);
	}
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q': {exit(0); break; } //退出  

	case 'a': { //左移  
		printf("e\n");
		move(eye, center, direction::left);
		break;
	}
	case 'd': { //右移  
		move(eye, center, direction::right);
		break;
	}
	case 'w': { //前移  
		move(eye, center, direction::front);
		break;
	}
	case 's': {  //后移  
		move(eye, center, direction::back);
		break;
	}

	case 'j': {//视角左移  
		rotate(eye, center, direction::left);
		break;
	}
	case 'l': {//视角右移  
		rotate(eye, center, direction::right);
		break;
	}
	case 'i': {//视角上移  
		center[1] += 0.1f;
		break;
	}
	case 'k': {//视角上移  
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
	glutInit(&argc, argv);//对glut的初始化             
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	//初始化显示模式:RGB颜色模型，深度测试，双缓冲               
	glutInitWindowSize(800, 600);//设置窗口大小             
	int windowHandle = glutCreateWindow("Simple GLUT App");//设置窗口标题               
	glutDisplayFunc(redraw); //注册绘制回调函数             
	glutReshapeFunc(reshape);   //注册重绘回调函数    

	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);

	glutIdleFunc(idle);//注册全局回调函数：空闲时调用           
	init();
	glutMainLoop();  // glut事件处理循环           
	system("pause");
	return 0;
}