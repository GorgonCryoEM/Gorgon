#ifndef GORGON_RENDERER_H
#define GORGON_RENDERER_H

#include <string>

using namespace std;

namespace wustl_mm {
	namespace Visualization {	
		class Renderer {
		public:
			virtual void Center();
			virtual void Draw();
			virtual void Focus();
			virtual void LoadFile(string fileName);
			virtual string GetSupportedFileFormats();

			void SetDrawBoundingBox(bool drawBoundingBox);
			bool GetDrawBoundingBox();

		protected:
			bool drawBoundingBox;
		};

		void Renderer::Center() {
		}

		void Renderer::Draw() { 
		}

		void Renderer::Focus() {
		}

		void Renderer::LoadFile(string fileName) {
		}

		string Renderer::GetSupportedFileFormats() {
			return "All Files (*.*)";
		}

		void Renderer::SetDrawBoundingBox(bool drawBoundingBox) {
			this->drawBoundingBox = drawBoundingBox;
		}

		bool Renderer::GetDrawBoundingBox() {
			return drawBoundingBox;
		}
	}
}


#endif