#include "materials/Material.h"

// Perhaps this isn't the best of ideas? Theres nothing to chain with materials...
// The only thing this helps with is to return a shared pointer when creating a material.
// Can probably be replaced by a `std::shared_ptr(builder)` where `builder` returns a reference.
namespace rt {
  template <typename T = Material> class MaterialBuilder {
    T* material;
  
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
