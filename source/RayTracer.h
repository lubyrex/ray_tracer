/**
  \file RayTracer.h

  Specifies procedures for raytracing in image rendering
 */
#pragma once
#include <G3D/G3DAll.h>

 /** \brief Application framework. */
class RayTracer {
protected:
    shared_ptr<Scene> scene;
    shared_ptr<TriTree> tris;
    float epsilon;


    /** Called by traceImage()
        Calls intersects()
        Returns nullptr if no intersection is found */
    shared_ptr<Surfel> traceOneRay(Point2int32 point, const Rect2D& plane, const shared_ptr<Camera>& camera, const shared_ptr<Image>& img);

    /** Calls intersectSphere()
        Calls intersectTriangle()
        Returns nullptr if no intersection is found */
        //shared_ptr<Surfel> intersects( const Ray& ray/*, const TriTree& tris*/) const;
    shared_ptr<Surfel> intersects(const Point3& origin, const Vector3& direction/*, const TriTree& tris*/) const;


    /** Returns nullptr if no intersecting surface is found */
    shared_ptr<Surfel> intersectSphere(const Sphere& sphere, const Ray& ray, const Color3& color) const;

    /** \copybrief intersectSphere*/
    //shared_ptr<Surfel> intersectTriangle(const CPUVertexArray& vertexArray, int triIndex, const Ray& ray) const;
    bool intersectTriangle(const Point3& P, const Vector3& w, const Point3 V[3], float b[3], float& t) const;

    Radiance3 RayTracer::shade(const shared_ptr<Surfel>& surfel, const Array<shared_ptr<Light>>& lights, const Vector3& w_o) const;

    Radiance3 RayTracer::L_i(const Point3& X, const Vector3& wi,const Array<shared_ptr<Light>>& lights) const;

    Radiance3 L_o(const shared_ptr<Surfel>& surfelX, const Vector3& wo, const Array<shared_ptr<Light>>& lights, int numScattering) const;//L_o(const shared_ptr<Surfel>& surfel, const Point3& X, const shared_ptr<Light>& light, const Vector3& w_o, bool scattered) const;

    Radiance3 doIndirectLight(const shared_ptr<Surfel>& surfelX, const Vector3& wo, const Array<shared_ptr<Light>>& lights) const;

    bool isVisible(const Point3& X, const Point3& Y/*const shared_ptr<Surfel>& surfel, const Point3& X, const Vector3& w_i*/) const;
public:
    RayTracer(const shared_ptr<Scene>& scene, float e);
    /** Driver function to traceOneRay, traces a ray through
   the center of each of the image pixels */
    void traceImage(const shared_ptr<Camera>& camera,/* const shared_ptr<Scene>& scene,*/ const shared_ptr<Image>& img);
};
