#ifndef FD__OPACITYDEPENDENT_H
#define FD__OPACITYDEPENDENT_H

/*
 * This class defines an abstract object whose opacity (or perhaps a multiplicative factor of it)
 * is dependent on opacity information from a container or owning object. It exposes a generic
 * interface for such objects that a owning class can use to set the opacity of many different
 * objects that implement the interface.
 */
class OpacityDependent {
 public:
    
    /* Function called by owner/containing object to instruct the dependent on its opacity.
     * Opacity should be a value between 0 and 1 where zero is transparent and one is opaque.
     */
    void setDependentOpacity(float opacity);

    /* Main point exposed to subclasses - return opacity configured by the owning object */
    float getDependentOpacity();

 protected:

    /* Protected constructor so only sub-classes can be instantiated */
    OpacityDependent();

 private:

    /* The opacity factor provided by the owner/containing object */
    float dependentOpacity;

};

#endif