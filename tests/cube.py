from OpenGL.GL import *

def drawWireCube(L=0.5):
    
    glBegin(GL_LINE_LOOP)

    glVertex(-L,-L, L)
    glVertex(+L,-L, L)
    glVertex(+L,+L, L)
    glVertex(-L,+L, L)
    
    glEnd()
    
    glBegin(GL_LINE_LOOP)

    glVertex(-L,-L, -L)
    glVertex(+L,-L, -L)
    glVertex(+L,+L, -L)
    glVertex(-L,+L, -L)
     
    glEnd()
    
    glBegin(GL_LINES)
    
    glVertex(-L,-L,  L)
    glVertex(-L,-L, -L)

    glVertex(+L,-L,  L)
    glVertex(+L,-L, -L)
    
    glVertex(+L,+L,  L)
    glVertex(+L,+L, -L)
    
    glVertex(-L,+L,  L)
    glVertex(-L,+L, -L)
    
    glEnd()
