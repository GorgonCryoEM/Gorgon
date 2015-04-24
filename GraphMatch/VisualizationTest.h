#ifndef _VISUALIZATION_TEST_
#define _VISUALIZATION_TEST_

#include "BackEndInterface.h"
#include <GL/glut.h>

namespace wustl_mm {
	namespace GraphMatch {


		void executeTest(int argc, char** argv);

		// OpenGL Visualization methods
		void vIdle();
		void vDrawScene(); 
		void vDrawScene2();
		void vResize(GLsizei, GLsizei);
		void vKeyboard(unsigned char cKey, int iX, int iY);
		void vSpecial(int iKey, int iX, int iY);

		BackEndInterface * _backEnd;

		static const GLfloat afAmbientWhite [] = {0.25, 0.25, 0.25, 1.00}; 
		static const GLfloat afAmbientRed   [] = {0.25, 0.00, 0.00, 1.00}; 
		static const GLfloat afAmbientGreen [] = {0.00, 0.25, 0.00, 1.00}; 
		static const GLfloat afAmbientBlue  [] = {0.00, 0.00, 0.25, 1.00}; 
		static const GLfloat afDiffuseWhite [] = {0.75, 0.75, 0.75, 1.00}; 
		static const GLfloat afDiffuseRed   [] = {0.75, 0.00, 0.00, 1.00}; 
		static const GLfloat afDiffuseGreen [] = {0.00, 0.75, 0.00, 1.00}; 
		static const GLfloat afDiffuseBlue  [] = {0.00, 0.00, 0.75, 1.00}; 
		static const GLfloat afSpecularWhite[] = {1.00, 1.00, 1.00, 1.00}; 
		static const GLfloat afSpecularRed  [] = {1.00, 0.25, 0.25, 1.00}; 
		static const GLfloat afSpecularGreen[] = {0.25, 1.00, 0.25, 1.00}; 
		static const GLfloat afSpecularBlue [] = {0.25, 0.25, 1.00, 1.00}; 

		int mainWindow, subWindow;

		void executeTest(int argc, char** argv) {
			_backEnd = new BackEndInterface();
			_backEnd->SetConstantsFromFile(argv[1]);
			_backEnd->LoadSequenceGraph();
			_backEnd->LoadSkeletonGraph();

			float afPropertiesAmbient [] = {0.50, 0.50, 0.50, 1.00}; 
			float afPropertiesDiffuse [] = {0.75, 0.75, 0.75, 1.00}; 
			float afPropertiesSpecular[] = {1.00, 1.00, 1.00, 1.00}; 

			int iWidth = 640.0; 
			int iHeight = 480.0; 

			glutInit(&argc, argv);
			glutInitWindowPosition( 0, 0);
			glutInitWindowSize(iWidth, iHeight);
			glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
			mainWindow = glutCreateWindow( "Topography Hunter Visualization" );
			glutDisplayFunc( vDrawScene );
			glutIdleFunc( vIdle );
			glutReshapeFunc( vResize );
			glutKeyboardFunc( vKeyboard );
			glutSpecialFunc( vSpecial );

			glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
			glClearDepth( 1.0 ); 

			glEnable(GL_DEPTH_TEST); 
			glEnable(GL_LIGHTING);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glLightfv( GL_LIGHT0, GL_AMBIENT,  afPropertiesAmbient); 
			glLightfv( GL_LIGHT0, GL_DIFFUSE,  afPropertiesDiffuse); 
			glLightfv( GL_LIGHT0, GL_SPECULAR, afPropertiesSpecular); 
			glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0); 

			glEnable( GL_LIGHT0 ); 

			
			glMaterialfv(GL_BACK,  GL_AMBIENT,   afAmbientGreen); 
			glMaterialfv(GL_BACK,  GL_DIFFUSE,   afDiffuseGreen); 
			glMaterialfv(GL_FRONT, GL_AMBIENT,   afAmbientBlue); 
			glMaterialfv(GL_FRONT, GL_DIFFUSE,   afDiffuseBlue); 
			glMaterialfv(GL_FRONT, GL_SPECULAR,  afSpecularWhite); 
			glMaterialf( GL_FRONT, GL_SHININESS, 25.0); 
			

			vResize(iWidth, iHeight); 

			glutMainLoop(); 
		}

		void vResize( GLsizei iWidth, GLsizei iHeight ) 
		{ 
				GLfloat fAspect, fHalfWorldSize = (1.4142135623730950488016887242097/2); 

				glViewport( 0, 0, iWidth, iHeight ); 
				glMatrixMode (GL_PROJECTION);
				glLoadIdentity ();

				if(iWidth <= iHeight)
				{
						fAspect = (GLfloat)iHeight / (GLfloat)iWidth;
						glOrtho(-fHalfWorldSize, fHalfWorldSize, -fHalfWorldSize*fAspect,
								fHalfWorldSize*fAspect, -10*fHalfWorldSize, 10*fHalfWorldSize);
				}
				else
				{
						fAspect = (GLfloat)iWidth / (GLfloat)iHeight; 
						glOrtho(-fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -fHalfWorldSize,
								fHalfWorldSize, -10*fHalfWorldSize, 10*fHalfWorldSize);
				}
		 
				glMatrixMode( GL_MODELVIEW );
		}

		void vKeyboard(unsigned char cKey, int iX, int iY) {}

		void vSpecial(int iKey, int iX, int iY) {}

		void vIdle() {
				glutPostRedisplay();
		}

		void vDrawScene() 
		{ 
				static float fPitch = 0.0;
				static float fYaw   = 0.0;
				static float fTime = 0.0;

				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

				glPushMatrix(); 
				/*
				float x = 0;
				float y = 0; 
				float z = 100;

				gluLookAt(x, y, z, x, y, 0, 0, 1, 0);
				*/

				if(true) //bSpin)
				{
						fPitch += 4.0;
						fYaw   += 2.5;
				}
				if(false) //bMove)
				{
						fTime  += 0.025;
				}

				glTranslatef(0.0, 0.0, -1.0);  
				glRotatef( -fPitch, 1.0, 0.0, 0.0);
				glRotatef(     0.0, 0.0, 1.0, 0.0);
				glRotatef(    fYaw, 0.0, 0.0, 1.0);

				/*
				glPushAttrib(GL_LIGHTING_BIT);
						glDisable(GL_LIGHTING);
						glColor3f(1.0, 1.0, 1.0);
						glutWireCube(1.0); 
				glPopAttrib(); 
				*/

				glPushMatrix(); 
				//glTranslatef(-0.5, -0.5, -0.5);
						_backEnd->DrawSkeleton();
				glPopMatrix(); 


				glPopMatrix(); 

				glutSwapBuffers(); 
		}

		void vDrawScene2() {
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			_backEnd->DrawSequence();
		}
	}
}
#endif
