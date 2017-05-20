#include<stdlib.h>  
#include"particle.h"  

void particle::draw(GLuint texture)
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

void particle::draw()
{
	const GLfloat x1 = -0.5, x2 = 0.5;
	const GLfloat y1 = -0.5, y2 = 0.5;
	const GLfloat point[4][2] = { { x1,y1 },{ x2,y1 },{ x2,y2 },{ x1,y2 } };
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glVertex2fv(point[i]);
	}
	glEnd();
}

void particle::show()
{
	glDepthMask(GL_FALSE);
	glPushMatrix();
	glTranslatef(place.x, place.y, place.z);
	glRotatef(angle.x, 1, 0, 0);
	glRotatef(angle.y, 0, 1, 0);
	glRotatef(angle.z, 0, 0, 1);
	glScalef(size.x, size.y, size.z);
	draw(texture);
	glPopMatrix();
	glDepthMask(GL_TRUE);
}

void particle::update() 
{
	if (has_tex) {
		glDepthMask(GL_FALSE);
		glPushMatrix();
		glTranslatef(place.x, place.y, place.z);
		glRotatef(angle.x, 1, 0, 0);
		glRotatef(angle.y, 0, 1, 0);
		glRotatef(angle.z, 0, 0, 1);
		glScalef(size.x, size.y, size.z);
		draw(texture);
		glPopMatrix();

		place.y += speed.y;
		place.x += speed.x;
		place.z += speed.z;

		speed.y += acc.y;
		speed.x += acc.x;
		speed.z += acc.z;
		life -= 0.2f;
		glDepthMask(GL_TRUE);
	}
	else {
		glPushMatrix();
		glColor3f(color.x, color.y, color.z);
		glTranslatef(place.x, place.y, place.z);
		glRotatef(angle.x, 1, 0, 0);
		glRotatef(angle.y, 0, 1, 0);
		glRotatef(angle.z, 0, 0, 1);
		glScalef(size.x, size.y, size.z);
		draw();
		glPopMatrix();

		place.y += speed.y;
		place.x += speed.x;
		place.z += speed.z;

		speed.y += acc.y;
		speed.x += acc.x;
		speed.z += acc.z;

		life -= 0.2f;
		glColor3f(0, 0, 0);
	}
}

particle::particle() {}

particle::particle(vec _size, vec _speed, vec _acc, vec _angle,
	float _life, unsigned int _texture)
{
	size = _size;
	speed = _speed;
	acc = _acc;
	life = _life;
	texture = _texture;
	angle = _angle;
	has_tex = true;
}

const vec& particle::getPlace()
{
	return place;
}

const vec& particle::getSize()
{
	return size;
}

const vec& particle::getAngle()
{
	return angle;
}

particle::particle(vec _size, vec _speed, vec _acc, vec _angle,
	float _life, vec _color)
{
	size = _size;
	speed = _speed;
	acc = _acc;
	life = _life;
	color = _color;
	angle = _angle;
	has_tex = false;
}

void emitter::emit(particle* (init)(), bool(*judge)(particle*)){
	isforever = true;
	for (int i = 0; i < speed; i++) {
		f = init;
		isDead = judge;
		p[i] = init();
		int place_x = rand() % speed;
		int place_y = rand() % speed;
		int place_z = rand() % speed;
		p[i]->place.x = 1.0f*place_x / speed*(x2 - x1) + x1;
		p[i]->place.y = 1.0f*place_y / speed*(y2 - y1) + y1;
		p[i]->place.z = 1.0f*place_z / speed*(z2 - z1) + z1;
	}
}

void emitter::emit(particle* (init)())
{
	isforever = false;
	for (int i = 0; i < speed; i++) {
		f = init;
		p[i] = init();
		int place_x = rand() % speed;
		int place_y = rand() % speed;
		int place_z = rand() % speed;
		p[i]->place.x = 1.0f*place_x / speed*(x2 - x1) + x1;
		p[i]->place.y = 1.0f*place_y / speed*(y2 - y1) + y1;
		p[i]->place.z = 1.0f*place_z / speed*(z2 - z1) + z1;
	}
}


void emitter::show()
{
	for (int i = 0; i < speed; i++) {
		if (!dead[i]) {
			p[i]->show();
		}
	}
}

void emitter::update() {
	for (int i = 0; i < speed; i++) {
		p[i]->update();
		if (p[i]->life < 0|| isforever && isDead(p[i])) {
			delete p[i];
			p[i] = f();
			int place_x = rand() % speed;
			int place_y = rand() % speed;
			int place_z = rand() % speed;
			p[i]->place.x = 1.0f*place_x / speed*(x2 - x1) + x1;
			p[i]->place.y = 1.0f*place_y / speed*(y2 - y1) + y1;
			p[i]->place.z = 1.0f*place_z / speed*(z2 - z1) + z1;
		}
	}
}

void emitter::reset()
{
	for (int i = 0; i < speed; i++) {
		dead[i] = false;
	}
	for (int i = 0; i < speed; i++) {
		p[i] = f();
		int place_x = rand() % speed;
		int place_y = rand() % speed;
		int place_z = rand() % speed;
		p[i]->place.x = 1.0f*place_x / speed*(x2 - x1) + x1;
		p[i]->place.y = 1.0f*place_y / speed*(y2 - y1) + y1;
		p[i]->place.z = 1.0f*place_z / speed*(z2 - z1) + z1;
	}
	num = speed;
}

void emitter::fade() {
	if (num == 0)return;
	for (int i = 0; i < speed; i++) {
		if (dead[i] == true)continue;
		p[i]->update();
		if (isDead(p[i])) {
			delete p[i];
			num--;
			dead[i] = true;
		}
	}
}

emitter::emitter(int _speed, float _x1,
	float _x2, float _y1, float _y2,float _z1,float _z2) {
	speed = _speed;
	x1 = _x1;
	x2 = _x2;
	y1 = _y1;
	y2 = _y2;
	z1 = _z1;
	z2 = _z2;
	p = new particle*[speed];
	dead = new bool[speed];
	for (int i = 0; i < speed; i++) {
		dead[i] = false;
	}
	num = speed;
}
