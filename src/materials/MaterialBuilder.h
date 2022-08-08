#include "materials/Material.h"

namespace rt {
  template <typename T = Material> class MaterialBuilder {
    Material* material;
  
    MaterialBuilder(T *newMaterial) : material(newMaterial)  {}
  
    public:
      template<typename... Args> MaterialBuilder(Args... args): material(new T(args...)) {}
      
      
      MaterialBuilder& setTexture(sPtr<Texture> texture) {
        material->albedo = texture;
        return *this;
      }

      MaterialBuilder& setTexture(vec3 color) {
        material->albedo = ms<SolidColor>(color);
        return *this;
      }
  
      sPtr<T> build() {
        return std::shared_ptr<T>(material);
      }
   };
}
