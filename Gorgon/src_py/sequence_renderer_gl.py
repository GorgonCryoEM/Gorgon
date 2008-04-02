from PyQt4 import QtOpenGL, QtCore

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber",
                            "PyOpenGL must be installed to run this example.",
                            QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,
                            QtGui.QMessageBox.NoButton)
    sys.exit(1)
    
class SequenceRendererGL(QtOpenGL.QGLWidget):
    def __init__(self, app, back_end, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)
        self.back_end = back_end
        self.app = app
    
    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(100, 100)

    def initializeGL(self):
        glClearColor( 0.0, 0.0, 0.0, 1.0 )
        glClearDepth( 5.0 )
        
    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
            
        self.drawScene()      

    def drawScene(self):
        #Scale from 100 by 10 input to whatever screen size

        #glScale(self.width()/100.0,self.height()/10.0,0)
        #self.back_end.DrawSequence(self.width(), self.height())
        self.back_end.DrawSequence()
        #glRectf(0,0,20,5)
       
    def pickObject(self, x, y):
        SIZE = 100
        viewport = list(glGetIntegerv(GL_VIEWPORT))

        glSelectBuffer(SIZE)
        glRenderMode(GL_SELECT)

        glInitNames()

        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPickMatrix(x, viewport[3]-y, 10, 10, viewport)
        gluPerspective(45, self.ratio, 0.1, 1000)

        self.drawScene()
        
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glFlush()

        bufferStack = glRenderMode(GL_RENDER)
        for hit_record in bufferStack:
            min_depth, max_depth, names = hit_record
            namelist = list(names)
            for n in namelist:
                print n
            print min_depth, max_depth, names
        
    def resizeGL(self, width, height):
        glViewport(0,0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, width, 0, height)
        
        
    def mousePressEvent(self, event):        
        #self.pickObject(event.x(), event.y())
        pass
    
    def mouseMoveEvent(self, event):
        self.updateGL()
