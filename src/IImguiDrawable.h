#pragma once

namespace rt {
  class IImguiDrawable {
  public:
  
    // For normal use cases of the interface 
    // For example: one class that wishes to use polymorphism
    virtual void OnDerivedImgui() {};
  
    // For when you have a base class that has some logic that it wants
    // to run before/after its child class implementation
    // For example: If you want to implement a texture changer that's shared across
    // All implementations of materials, you run the texture changer code, then the 
    // Material's specific ImGui code.
    
    // It's expected that this funcion calls `OnDerivedImgui` with some extra stuff
    // Common between child classes of the class that implements this. (For example, materials)
    virtual void OnBaseImgui() {};
  };
} // namespace rt