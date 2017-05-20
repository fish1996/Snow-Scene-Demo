#include"snowflower.h"

const int skySizeY = 30;

snowflower::snowflower(float _x,float _z,const vec& v,const vec& a)
{
	x = _x;
	z = _z;
	size = v;
	angle = a;
}

snowflower::snowflower()
{

}
void snowflower::draw(GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);  //—°‘ÒŒ∆¿Ìtexture[status]       
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

void snowflower::show(int texture)
{
	glPushMatrix();
	glTranslatef(x, -1.0f*skySizeY/2,z);
	glRotatef(angle.x, 1, 0, 0);
	glRotatef(angle.y, 0, 1, 0);
	glRotatef(angle.z, 0, 0, 1);
	glScalef(size.x,size.y,size.z);
	draw(texture);
	glPopMatrix();
}

snow::snow(int t) {
	count = 0;
	texture = t;
}


void snow::add(float x, float z, const vec& v,const vec& a)
{
	snowflower sn(x, z, v, a);
	s.push_back(sn);
	count++;
	if (count == 100) {
		count = 0;
		list<snowflower>::iterator it;
		for (it = s.begin(); count!=99 && it!=s.end(); ) {
			it = s.erase(it);
			count++;
		}
		count = 0;
	}
}

void snow::remove()
{
	static int cnt = 0;
	cnt++;
	list<snowflower>::iterator it = s.begin();
	if (cnt == 5 && it != s.end()) {
		cnt = 0;
		s.erase(it);
	}
}

void snow::show()
{
	list<snowflower>::iterator it;
	for (it = s.begin(); it != s.end(); it++) {
		it->show(texture);
	}
}