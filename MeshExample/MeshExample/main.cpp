#define _CRT_SECURE_NO_WARNINGS 

#include "mesh.h"
#include "glew.h"
#include "glut.h"
#include "FreeImage.h"
#include <iostream>
#include "ShaderLoader.h"

using namespace std;

class LIGHT{
public:
	GLfloat light_specular[4];
	GLfloat light_diffuse[4];
	GLfloat light_ambient[4];
	GLfloat light_position[4];
};
vector<LIGHT> lv;
GLfloat env_ambient[4];
void light_loader(char *filename){
	char token[100]; 
	FILE *fp = fopen(filename, "r");
	LIGHT li;
	if (!fp){
		cout << "Fail to open light file\n";
		return;
	}
	while (!feof(fp)){
		fscanf(fp, "%s", token);
		if (strcmp(token, "light") == 0){
			fscanf(fp, "%f %f %f", &li.light_position[0], &li.light_position[1], &li.light_position[2]);
			li.light_position[3] = 1.0;
			fscanf(fp, "%f %f %f", &li.light_ambient[0], &li.light_ambient[1], &li.light_ambient[2]);
			li.light_ambient[3] = 1.0;
			fscanf(fp, "%f %f %f", &li.light_diffuse[0], &li.light_diffuse[1], &li.light_diffuse[2]);
			li.light_diffuse[3] = 1.0;
			fscanf(fp, "%f %f %f", &li.light_specular[0], &li.light_specular[1], &li.light_specular[2]);
			li.light_specular[3] = 1.0;
			lv.push_back(li);
		}
		else if (strcmp(token, "ambient") == 0){
			fscanf(fp, "%f %f %f", &env_ambient[0], &env_ambient[1], &env_ambient[2]);
			env_ambient[3] = 1.0;
		}
	}
	fclose(fp);
}



class VIEW{
public:
	GLdouble eyex, eyey, eyez , cenx, ceny ,cenz , vupx , vupy , vupz;
	GLdouble fovy, neard, fard; 
	GLint portx, porty, w, h;
	
};
VIEW view;
void view_loader(char *filename){
	char token[100];
	FILE *fp = fopen(filename, "r");
	if (!fp){
		cout << "Fail to open view file\n";
		return;
	}
	while (!feof(fp)){
		fscanf(fp, "%s", token);
		if (strcmp(token, "eye") == 0){
			fscanf(fp, "%lf %lf %lf", &view.eyex, &view.eyey, &view.eyez);
		}
		else if (strcmp(token, "vat") == 0){
			fscanf(fp, "%lf %lf %lf", &view.cenx, &view.ceny, &view.cenz);
		}
		else if (strcmp(token, "vup") == 0){
			fscanf(fp, "%lf %lf %lf", &view.vupx, &view.vupy, &view.vupz);
		}
		else if (strcmp(token, "fovy") == 0){
			fscanf(fp, "%lf", &view.fovy);
		}
		else if (strcmp(token, "dnear") == 0){
			fscanf(fp, "%lf", &view.neard);
		}
		else if (strcmp(token, "dfar") == 0){
			fscanf(fp, "%lf", &view.fard);
		}
		else if (strcmp(token, "viewport") == 0){
			fscanf(fp, "%d %d %d %d", &view.portx, &view.porty, &view.w, &view.h);
		}
	}
	fclose(fp);
}
class TEX_NAME{
public:
	int tex_type; 
	char tex_name[6][100];
};
class OBJECT{
public:
	mesh *object;
	GLfloat sx, sy, sz, angle, rx, ry, rz, tx, ty, tz;
	int tex_type ;
	int tex_index;

};


vector<TEX_NAME> tex;
vector<OBJECT> ob;

void scene_loader(char *filename){
	char token[100];
	int obj_num = 0;
	OBJECT obj ;
	TEX_NAME temp;
	FILE *fp = fopen(filename, "r");
	if (!fp){
		cout << "Fail to open scene file\n";
		return ;
	}
	while (!feof(fp)){
		fscanf(fp ,"%s", token); 
		if (strcmp(token, "model") == 0){
			fscanf(fp, "%s", token);
			obj.object = new mesh(token);
			fscanf(fp, "%f %f %f %f %f %f %f %f %f %f", &obj.sx, &obj.sy, &obj.sz , &obj.angle , &obj.rx , &obj.ry , &obj.rz ,&obj.tx , &obj.ty , &obj.tz );
			obj.tex_type = temp.tex_type;
			if (obj.tex_type == 2)
				obj.tex_index = obj_num - 2;
			else obj.tex_index = obj_num - 1;
			ob.push_back(obj);
		}
		else if (strcmp(token, "no-texture") == 0){
			temp.tex_type = 0;
		}
		else if (strcmp(token, "single-texture") == 0){
			temp.tex_type = 1;
			fscanf(fp ,"%s", temp.tex_name[0]);
			tex.push_back(temp);
			obj_num++;
		}
		else if (strcmp(token, "multi-texture") == 0){
			temp.tex_type = 2;
			fscanf(fp ,"%s", temp.tex_name[0]);
			tex.push_back(temp);
			fscanf(fp ,"%s", temp.tex_name[0]);
			tex.push_back(temp);
			obj_num += 2;
		}
		else if (strcmp(token, "cube-map") == 0){
			temp.tex_type = 3;
			for (int j = 0; j < 6; j++){
				fscanf(fp ,"%s", temp.tex_name[j]);
			}
			tex.push_back(temp);
			obj_num++;
		}

	}
	fclose(fp);
}


unsigned int texObject[100];
int level = 0;
void mouse(int , int);
void trace(int, int);
void keyboard(unsigned char , int  , int );
void light();
void display();
void draw_obj(mesh* , int , int);
void reshape(GLsizei , GLsizei );
void LoadTexture();
void LoadShaders();

int MyShader;
int main(int argc, char** argv)
{
	scene_loader("as3.scene");
	light_loader("as3.light");
	view_loader("as3.view");


	glutInit(&argc, argv);
	glutInitWindowSize(view.w, view.h);
	glutInitWindowPosition(view.portx, view.porty);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Mesh Example");
	
	glewInit();
	FreeImage_Initialise();
	glGenTextures(100 , texObject) ;
	LoadTexture();
	FreeImage_DeInitialise();
	LoadShaders();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse);
	glutPassiveMotionFunc(trace);
	glutMainLoop();

	return 0;
}

void LoadShaders(){
	MyShader = glCreateProgram();
	if (MyShader != 0)
	{
		ShaderLoad(MyShader, "subdivision.vert", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "subdivision.geom", GL_GEOMETRY_SHADER);
		ShaderLoad(MyShader, "subdivision.frag", GL_FRAGMENT_SHADER);
	}
}
void light()
{

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	for (int i = 0; i < lv.size(); i++){
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lv[i].light_position);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, lv[i].light_diffuse);
		glLightfv(GL_LIGHT0+i, GL_SPECULAR, lv[i].light_specular);
		glLightfv(GL_LIGHT0+i, GL_AMBIENT, lv[i].light_ambient);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, env_ambient);
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	// viewport transformation
	glViewport(view.portx, view.porty,  view.w , view.h);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view.fovy, (GLfloat)view.w/(GLfloat)view.h, view.neard, view.fard);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	view.eyex, view.eyey, view.eyez,// eye
				view.cenx, view.ceny, view.cenz,     // center
				view.vupx, view.vupy, view.vupz);    // up
	
	//注意light位置的設定，要在gluLookAt之後
	light();
	glUseProgram(MyShader);
	GLint location = glGetUniformLocation(MyShader, "colorTexture");
	if (location == -1) printf("Cant find texture name: colorTexture\n");
	else glUniform1i(location, 0);

	location = glGetUniformLocation(MyShader, "level");
	if (location == -1) printf("Cant find level\n");
	else glUniform1i(location, level);

	location = glGetUniformLocation(MyShader, "Radius");
	if (location == -1) printf("Cant find radius\n");
	else glUniform1f(location, 1.0f);
	
	GLfloat f[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	location = glGetUniformLocation(MyShader, "Centroid");
	if (location == -1) printf("Cant find centroid\n");
	else glUniform4fv(location,1,f);
	
	for (int i = 0; i < ob.size(); i++){
		glPushMatrix();
		glTranslatef(ob[i].tx, ob[i].ty, ob[i].tz);
		glRotatef(ob[i].angle, ob[i].rx, ob[i].ry, ob[i].rz);
		glScalef(ob[i].sx, ob[i].sy, ob[i].sz); 	
			draw_obj(ob[i].object ,ob[i].tex_type , ob[i].tex_index );
		glPopMatrix();
	}

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	view.w = w;
	view.h = h;
}

void draw_obj(mesh *object , int type , int index){
	int lastMaterial = -1;
	for (size_t i = 0; i < object->fTotal; ++i)
	{
		// set material property if this face used different material
		if (lastMaterial != object->faceList[i].m)
		{
			lastMaterial = (int)object->faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT, object->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, object->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, object->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &object->mList[lastMaterial].Ns);

			//you can obtain the texture name by object->mList[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}
		if (type == 0){
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				//textex corrd. object->tList[object->faceList[i][j].t].ptr
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		else if (type == 1){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[index]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glTexCoord2fv(object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (type == 2){
			//bind texture0
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D , texObject[index]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

			//bind texture1
			glActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[index + 1]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glMultiTexCoord2fv(GL_TEXTURE0,object->tList[object->faceList[i][j].t].ptr);
				glMultiTexCoord2fv(GL_TEXTURE1 ,object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();

			//unbind texture1
			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			//unbind texture0
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			
		}
		else if (type == 3){
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[index]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glTexCoord2fv(object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();

			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		
	}
}

int current = 3;
int oldx = 0, oldy = 0;
void mouse(int x, int y){
	if (current > ob.size()) return;
	ob[current - 1].tx += 0.05*(x - oldx);      //0.05 is mouse sensitivity
	oldx = x;
	ob[current - 1].ty -= 0.05*(y - oldy);
	oldy = y;
	glutPostRedisplay();
}
void trace(int x, int y){
	oldx = x;
	oldy = y;
}
void keyboard(unsigned char key, int x, int y){ 
	float v_x = view.eyex - view.cenx;
	float v_y = view.eyey - view.ceny;
	float v_z = view.eyez - view.cenz;
	if (key <= '9' && key >= '1'){ 
		level = key - 1 - '0';
	}
	else if (key == 'f'){
		lv[0].light_position[1] -= 1;
	}
	else if (key == 'r'){
		lv[0].light_position[1] += 1;
	}
	else if (key == 'w'){ 
		view.eyex -= v_x * 0.3;
		view.eyey -= v_y * 0.3;
		view.eyez -= v_z * 0.3;
	}
	else if (key == 'a'){
		GLfloat user_define_matrix[16] = { v_x, 0,0,0,
										   v_y,0,0,0,
										   v_z,0,0,0, 
										   0,0,0,0 };
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadMatrixf(user_define_matrix);
			glRotatef(15.0, view.vupx, view.vupy, view.vupz);
			glGetFloatv(GL_MODELVIEW_MATRIX, user_define_matrix);
		glPopMatrix();

		view.eyex = view.cenx + user_define_matrix[0]; 
		view.eyey = view.ceny + user_define_matrix[4];
		view.eyez = view.cenz + user_define_matrix[8];
		//cout << view.eyex << " " << view.eyey << " " << view.eyez << endl;

	}
	else if (key == 's'){
		view.eyex += v_x * 0.3;                 //0.3 is zoom sensitivity
		view.eyey += v_y * 0.3;
		view.eyez += v_z * 0.3;
	}
	else if (key == 'd'){
		GLfloat user_define_matrix[16] = { v_x, 0, 0, 0,
			v_y, 0, 0, 0,
			v_z, 0, 0, 0,
			0, 0, 0, 0 };

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadMatrixf(user_define_matrix);
			glRotatef(-15.0, view.vupx, view.vupy, view.vupz);
			glGetFloatv(GL_MODELVIEW_MATRIX, user_define_matrix);
		glPopMatrix();

		view.eyex = view.cenx + user_define_matrix[0];
		view.eyey = view.ceny + user_define_matrix[4];
		view.eyez = view.cenz + user_define_matrix[8];
		//cout << view.eyex << " " << view.eyey << " " << view.eyez << endl;
	}
	glutPostRedisplay();
}
void LoadTexture(){
	for (int i = 0; i < tex.size(); i++){
		if (tex[i].tex_type < 3){
			FIBITMAP *pImage = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[0], 0), tex[i].tex_name[0]);
			FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
			int iWidth = FreeImage_GetWidth(p32BitsImage);
			int iHeight = FreeImage_GetHeight(p32BitsImage);

			glBindTexture(GL_TEXTURE_2D, texObject[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			
			glGenerateMipmap(GL_TEXTURE_2D);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			
			FreeImage_Unload(p32BitsImage);
			FreeImage_Unload(pImage);
		}
		else{
			FIBITMAP* pImage1 = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[0], 0), tex[i].tex_name[0]);
			FIBITMAP* p32BitsImage1 = FreeImage_ConvertTo32Bits(pImage1);
			int iWidth1 = FreeImage_GetWidth(p32BitsImage1);
			int iHeight1 = FreeImage_GetHeight(p32BitsImage1);
			FIBITMAP *pImage2 = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[1], 0), tex[i].tex_name[1]);
			FIBITMAP *p32BitsImage2 = FreeImage_ConvertTo32Bits(pImage2);
			int iWidth2 = FreeImage_GetWidth(p32BitsImage2);
			int iHeight2 = FreeImage_GetHeight(p32BitsImage2);
			FIBITMAP *pImage3 = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[2], 0), tex[i].tex_name[2]);
			FIBITMAP *p32BitsImage3 = FreeImage_ConvertTo32Bits(pImage3);
			int iWidth3 = FreeImage_GetWidth(p32BitsImage3);
			int iHeight3 = FreeImage_GetHeight(p32BitsImage3);
			FIBITMAP *pImage4 = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[3], 0), tex[i].tex_name[3]);
			FIBITMAP *p32BitsImage4 = FreeImage_ConvertTo32Bits(pImage4);
			int iWidth4 = FreeImage_GetWidth(p32BitsImage4);
			int iHeight4 = FreeImage_GetHeight(p32BitsImage4);
			FIBITMAP *pImage5 = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[4], 0), tex[i].tex_name[4]);
			FIBITMAP *p32BitsImage5 = FreeImage_ConvertTo32Bits(pImage5);
			int iWidth5 = FreeImage_GetWidth(p32BitsImage5);
			int iHeight5 = FreeImage_GetHeight(p32BitsImage5);
			FIBITMAP *pImage6 = FreeImage_Load(FreeImage_GetFileType(tex[i].tex_name[5], 0), tex[i].tex_name[5]);
			FIBITMAP *p32BitsImage6 = FreeImage_ConvertTo32Bits(pImage6);
			int iWidth6 = FreeImage_GetWidth(p32BitsImage6);
			int iHeight6 = FreeImage_GetHeight(p32BitsImage6);

			glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, iWidth1, iHeight1,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage1));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, iWidth2, iHeight2,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage2));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, iWidth3, iHeight3,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage3));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, iWidth4, iHeight4,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage4));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, iWidth5, iHeight5,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage5));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, iWidth6, iHeight6,
				0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage6));
			
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			FreeImage_Unload(p32BitsImage6);
			FreeImage_Unload(pImage6);
			FreeImage_Unload(p32BitsImage5);
			FreeImage_Unload(pImage5);
			FreeImage_Unload(p32BitsImage4);
			FreeImage_Unload(pImage4);
			FreeImage_Unload(p32BitsImage3);
			FreeImage_Unload(pImage3);
			FreeImage_Unload(p32BitsImage2);
			FreeImage_Unload(pImage2);
			FreeImage_Unload(p32BitsImage1);
			FreeImage_Unload(pImage1);
		}
	}
}