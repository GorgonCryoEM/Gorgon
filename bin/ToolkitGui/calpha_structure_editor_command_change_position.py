import math
from PyQt4 import QtGui, QtCore
from Explorer.libs import Vec3


class CAlphaStructureEditorCommandChangePosition(QtGui.QUndoCommand):

    def __init__(self, viewer, editor, doTranslate, translateVector, doRotate, rotateCenter, rotateAxis, rotateOldAngle, rotateNewAngle, rotateFieldNum):
        QtGui.QUndoCommand.__init__(self)
        self.viewer = viewer
        self.editor = editor
        self.renderer = viewer.renderer
        self.doTranslate = doTranslate
        self.translateVector = translateVector
        self.doRotate = doRotate
        self.rotateCenter = rotateCenter
        self.rotateAxis = rotateAxis
        self.rotateOldAngle = rotateOldAngle
        self.rotateNewAngle = rotateNewAngle
        self.rotateNewAngleValue = math.pi*rotateNewAngle/180
        self.rotateFieldNum = rotateFieldNum
    
    def redo(self):
        self.editor.undoInProgress = True
        if self.doTranslate:
            self.renderer.selectionMove(self.translateVector)
        if self.doRotate:
            self.renderer.selectionRotate(self.rotateCenter, self.rotateAxis, self.rotateNewAngleValue - self.rotateOldAngle)
            if(self.rotateFieldNum == 'roll'):
                self.editor.roll = self.rotateNewAngleValue
            elif (self.rotateFieldNum == 'pitch'):
                self.editor.pitch = self.rotateNewAngleValue
            elif (self.rotateFieldNum == 'yaw'):
                self.editor.yaw = self.rotateNewAngleValue

        self.editor.posMoveDict[self.rotateFieldNum].setValue(self.rotateNewAngle)
        self.viewer.emitModelChanged()
        self.editor.undoInProgress = False
        
    def undo(self):
        self.editor.undoInProgress = True
        if self.doTranslate:
            self.renderer.selectionMove(Vector3DFloat(-self.translateVector.x(), -self.translateVector.y(), -self.translateVector.z()))
        if self.doRotate:
            self.renderer.selectionRotate(self.rotateCenter, self.rotateAxis, self.rotateOldAngle - self.rotateNewAngleValue)
            if(self.rotateFieldNum == 'roll'):
                self.editor.roll = self.rotateNewAngleValue
            elif (self.rotateFieldNum == 'pitch'):
                self.editor.pitch = self.rotateNewAngleValue
            elif (self.rotateFieldNum == 'yaw'):
                self.editor.yaw = self.rotateNewAngleValue
        
        self.editor.posMoveDict[self.rotateFieldNum].setValue(self.rotateOldAngle)
        self.viewer.emitModelChanged()
        self.editor.undoInProgress = False
