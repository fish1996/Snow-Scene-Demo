#include <stdlib.h>  
#include "butterfly.h"
#include <time.h>
 
spline* butterfly::line;
int butterfly::texture[3];
butterfly::butterfly(int i)
{
	id = i;
}

void butterfly::show()
{
	
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, tex);
	glPushMatrix();
	glTranslatef(x, y, z);	
	glRotatef(45.0f, 1.0f, 0.0f, 0.0f);						
	glRotatef((spinz), 0.0f, 0.0f, 1.0f);		

	glBegin(GL_TRIANGLES);									
																			  
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);	
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, flap);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);		
																	
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);			
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);		
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, flap);	

	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void butterfly::update()
{
	//printf("%d:%f %f\n",id, x, y);
	if (index<line->size())index++;			
	spinz += spinzi;						
	flap += fi;						
	x = line->getX(index) + dx;
	y = line->getY(index) + dy;
	if (index >= line->size()){
		set(0);						
	}

	if ((flap > 1.0f) || (flap < -1.0f))	
	{
		fi = -fi;							
	}

}

void butterfly::set(int _index)						
{
	tex =texture[rand()%3];							
	z = rand()%20 - 10;
	spinzi = (rand() % 10000) / 5000.0f - 1.0f;		
	flap = 0.0f;									
	fi = 0.05f + (rand() % 100) / 1000.0f;			
	index = _index;				
	x = line->getX(index) ;							
	y = line->getY(index) ;
	dx = rand() % 10 - 5;
	dy = rand() % 10 - 5;
}

butterflyManager::butterflyManager(int* tex)
{
	isDraw = false;
	genPath();
	isGen = false;
	butterfly butt(0);
	obj.push_back(butt);
	int id;
	
	for (int i = 0; i < 3; i++) {
		butterfly::texture[i] = tex[i];
	}
	for (int i = 0; i < obj.size(); i++) {
		id = rand() % 3;
		obj[i].set(0);
	}
}

void butterflyManager::show()
{
	if (!isDraw)return;
	static int count[20] = { 0 };
	static int count2 = 0;
	count2++;
	for (int i = 0; i < obj.size(); i++) {
		count[i]++;
		obj[i].show();
		if (count[i] == 20) {
			count[i] = 0;
			obj[i].update();
		}
		if (count2 == 100&&obj.size() < 20) {
			
			count2 = 0;
//		printf("size = %d\n", obj.size());
			butterfly butt(obj.size());
			butt.set(0);
			obj.push_back(butt);
		}
	}
}

void butterflyManager::genPath()
{
	p = new point[20];
	p[0].setPoint(-100, 5);
	p[1].setPoint(-90, 7);
	p[2].setPoint(-80, 3);
	p[3].setPoint(-70, 0);
	p[4].setPoint(-60, 2);
	p[5].setPoint(-50, 4);
	p[6].setPoint(-40, 5);
	p[7].setPoint(-30, 7);
	p[8].setPoint(-20, 5);
	p[9].setPoint(-10, 8);
	p[10].setPoint(0, 5);
	p[11].setPoint(10, 5);
	p[12].setPoint(20, 8);
	p[13].setPoint(30, 9);
	p[14].setPoint(40, 10);
	p[15].setPoint(50, 11);
	p[16].setPoint(60, 13);
	p[17].setPoint(70, 14);
	p[18].setPoint(80, 13);
	p[19].setPoint(90, 10);
	line = new spline(p, 20, 20, 0.6);
	butterfly::line = line;
}