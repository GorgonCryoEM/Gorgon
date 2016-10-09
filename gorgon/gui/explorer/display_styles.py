from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


def wireframe():
    glPolygonMode(GL_FRONT, GL_LINE)
    glPolygonMode(GL_BACK, GL_LINE)
    

def flat():
    glPolygonMode(GL_FRONT, GL_FILL)
    glPolygonMode(GL_BACK, GL_FILL)
    glShadeModel(GL_FLAT)
    

def smooth():
    glPolygonMode(GL_FRONT, GL_FILL)
    glPolygonMode(GL_BACK, GL_FILL)
    glShadeModel(GL_SMOOTH)
