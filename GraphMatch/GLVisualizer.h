#ifndef GL_VISUALIZER_H_
#define GL_VISUALIZER_H_

#include <GL/glut.h>

using namespace std;

public class GLVisualizer {
public:
	// METHODS
	void DrawSkeleton(StandardGraph* skeleton);
	void DrawSequence(StandardGraph* sequence);
};

void GLVisualizer::DrawSkeleton(StandardGraph* skeleton) {}
void GLVisualizer::DrawSequence(StandardGraph* sequence) {}

#endif
