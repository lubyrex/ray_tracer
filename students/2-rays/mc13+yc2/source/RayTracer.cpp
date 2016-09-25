/** \file RayTracer.cpp */
#include "RayTracer.h"
#include "Math.h"

RayTracer::RayTracer(const shared_ptr<Scene>& s, float e, bool mult) :
    m_scene(s), m_epsilon(e), m_spheresOn(false), m_concurrent(mult) {};

#define numIndRays 4
void RayTracer::traceImage(const shared_ptr<Camera>& camera,/* const shared_ptr<Scene>& m_scene,*/const shared_ptr<Image>& img) {
    // Nested for loops over pixel array
        // call traceOneRay for each pixel
        // Adjust pixel Radiance through shading 
        // change image

    Array<shared_ptr<Surface>> surfs; // Pass Arrays down 
    m_scene->onPose(surfs);
    m_triTree = shared_ptr<TriTree>(new TriTree());
    m_triTree->setContents(surfs);
    const int width = img->width();
    const int height = img->height();

    Array<shared_ptr<Light>> lights;
    m_scene->getTypedEntityArray(lights);

    Vector2 dimensions((float)width, (float)height);
    Rect2D plane(dimensions);
    for (Point2int32 pixel; pixel.y < height; ++pixel.y) {
        for (pixel.x = 0; pixel.x < width; ++pixel.x) {
            Ray ray(camera->worldRay(pixel.x, pixel.y, plane));
            // Point2int32 pixel(x, y);
            Radiance3 sum(0, 0, 0);
            for (int i(0); i < 4; ++i) {
                ray = ray.bumpedRay(m_epsilon);
                sum += L_i(ray.origin(), ray.direction(), lights, numIndRays, 1);
            };
            img->set(pixel.x, pixel.y, sum / 4);
        };
    };



};

// Adapted from C++ Direct Illumination [_rn_dirctIllm] from http://graphicscodex.com 
Radiance3 RayTracer::L_i(const Point3& X, const Vector3& wi, const Array<shared_ptr<Light>>& lights, int numScattering, int depth) const {
    // Find the first intersection 
    shared_ptr<Surfel> surfelY = intersects(X, wi);

    if (notNull(surfelY)) {
        // Compute the light leaving Y, which is the same as
        // the light entering X when the medium is non-absorptive
        return L_o(surfelY, -wi, lights, numScattering, depth);
    }
    else {
        return Radiance3(0, 0, 0);
    }
}

shared_ptr<Surfel> RayTracer::intersects(const Point3& P, const Vector3& w) const {
    //  return m_triTree->intersectRay(Ray(P,w));
    TriTree::Hit hit;
    shared_ptr<UniversalSurfel> surfel(new UniversalSurfel);

    CPUVertexArray vertexArray(m_triTree->vertexArray());
    //Vector3 w(direction);
    //Point3 P(origin);

    float t(0);
    float b[3];
    Point3 V[3];

    float d(inf());
    for (int i(0); i < m_triTree->size(); ++i) {
        Tri tri(m_triTree->operator[](i));
        V[0] = tri.position(vertexArray, 0);
        V[1] = tri.position(vertexArray, 1);
        V[2] = tri.position(vertexArray, 2);


        if (intersectTriangle(P, w, V, b, t)) {
            if (t < d) {
                d = t;
                hit.distance = t;
                hit.backface = (w.dot(tri.normal(vertexArray)) > 0);
                hit.triIndex = i;
                hit.u = b[0];
                hit.v = b[1];
            };
        };

    };

    if (d < inf()) {
        return m_triTree->sample(hit);
    }
    else {
        return nullptr;
    };
};

bool RayTracer::intersectTriangle(const Point3& P, const Vector3& w, const Point3 V[3], float b[3], float& t) const {
    const Vector3& e1(V[1] - V[0]);
    const Vector3& e2(V[2] - V[0]);
    const Vector3& n(e1.cross(e2).direction());

    const Vector3& q(w.cross(e2));
    float a(e1.dot(q));

    if (n.dot(w) >= 0 || abs(a) <= m_epsilon) {
        return nullptr;
    };


    const Vector3& s((P - V[0]) / a);

    const Vector3& r(s.cross(e1));

    b[0] = s.dot(q);
    b[1] = r.dot(w);
    b[2] = 1 - b[0] - b[1];

    if (b[0] < 0 || b[1] < 0 || b[2] < 0) {
        return nullptr;
    };

    t = e2.dot(r);
    return (t >= 0);
};

shared_ptr<Surfel> RayTracer::intersectSphere(const Sphere& sphere, const Ray& ray, const Color3& color) const {
    const Vector3& w(ray.direction());
    Point3 P(ray.origin());
    Point3 C(sphere.center);
    float r(sphere.radius);
    float b = 2.0*w.dot(P - C);
    float c = (P - C).dot(P - C) - r*r;
    float inRadical = b*b - 4 * c;
    float t(0.0);

    if (inRadical < 0 || sphere.contains(P)) {
        return nullptr;
    }
    else {
        float t_0 = (-b + sqrt(inRadical)) / 2.0;
        float t_1 = (-b - sqrt(inRadical)) / 2.0;
        t = t_0 < t_1 ? t_0 : t_1;
        Point3 X(P + t*w);

        shared_ptr<UniversalSurfel> surfel(new UniversalSurfel());

        Color3 color(0, 0, 0);
        surfel->lambertianReflectivity = color;

        const Vector3& normal((X - C).direction());
        surfel->geometricNormal = normal;
        surfel->shadingNormal = normal;
        surfel->position = X;

        return surfel;
    };
};

// Adapted from C++ Direct Illumination [_rn_dirctIllm] from http://graphicscodex.com 
Radiance3 RayTracer::L_o(const shared_ptr<Surfel>& surfelX, const Vector3& wo, const Array<shared_ptr<Light>>& lights, int numScattering, int depth) const {
    // Begin with the emitted radiance
    Radiance3 L = surfelX->emittedRadiance(wo);
    const Point3& X = surfelX->position;
    const Vector3& n = surfelX->shadingNormal;

    // Add direct illumination from each light
    for (int i = 0; i < lights.size(); ++i) {
        const shared_ptr<Light> light(lights[i]);
        const Point3& Y(light->position().xyz());

        if (isVisible(X, Y)) {
            const Vector3& wi((Y - X*(light->position().w)).direction());
            Biradiance3& Bi(light->biradiance(X));
            const Color3& f(surfelX->finiteScatteringDensity(wi, wo));
            L += Bi * f * abs(wi.dot(n)) + surfelX->reflectivity(Random::threadCommon()) * 0.05f;
        };

        Radiance3 indirectLight = Radiance3::black();
        if (depth > 0) {
            for (int j(0); j < numScattering; ++j) {
               indirectLight += doIndirectLight(surfelX, wo, lights, depth);
            };
            indirectLight/=(float)numScattering;
            L+= indirectLight;
        };
    };

    return L;
}

Radiance3 RayTracer::doIndirectLight(const shared_ptr<Surfel>& surfelX, const Vector3& wo, const Array<shared_ptr<Light>>& lights, int depth) const {
    Radiance3 r(0, 0, 0);
    const Vector3& wi(Vector3::cosHemiRandom(surfelX->shadingNormal, Random::threadCommon()).direction());
    const Color3& f(surfelX->finiteScatteringDensity(wi, wo));
    r += L_i(surfelX->position, wi, lights, 1, depth - 1)*f;
    return r;
};

bool RayTracer::isVisible(const Point3& X, const Point3& Y) const {
    Vector3 w_i((Y - X).direction());
    Point3 origin(Y + m_epsilon*w_i);
    shared_ptr<Surfel> intersect(intersects(origin, (-1)*w_i));
    return (intersect != nullptr) && ((intersect->position - X).length() <= m_epsilon);
};

/*

Radiance3 RayTracer::L_o(const shared_ptr<Surfel>& surfel, const Point3& X, const shared_ptr<Light>& light, const Vector3& w_o,  bool scatter) const {
    Vector4 Y(light->position());
    Vector3& w_i((Y.xyz() - X*Y.w).direction());
    const Vector3& n(surfel->shadingNormal);
    if (scatter) {
        w_i = w_i.hemiRandom(n, Random::threadCommon()).direction();
    }
    if (isVisible(surfel, Y.xyz(), w_i)) {
        float compFactor(abs(w_i.dot(n)));
        Radiance3 f(surfel->finiteScatteringDensity(w_i, w_o));
        return (light->biradiance(X)*f*compFactor);
    }
    else {
        return Radiance3(0, 0, 0);
    };
};
*/

/*
Radiance3 RayTracer::shade(const shared_ptr<Surfel>& surfel, const Array<shared_ptr<Light>>& lights, const Vector3& w_o) const {
    if (notNull(surfel)) {
        Point3 X(surfel->position);
        Radiance3 L_e(surfel->emittedRadiance(w_o));
        Radiance3 L_d(0, 0, 0);
        Radiance3 L_s(0, 0, 0);


        for (int i(0); i < lights.size(); ++i) {
            shared_ptr<Light> light(lights[i]);
            L_d += L_o(surfel, X, light, w_o, false) + surfel->reflectivity(Random::threadCommon()) * 0.05f;
            //L_s += L_o(surfel, X, light, w_o, true);
        };
        return L_e + L_d + L_s;

    }
    else {
        return Radiance3(0, 0, 0);
    };
};

Radiance3 RayTracer::traceOneRay(Point2int32 pixel, const Rect2D& plane, const shared_ptr<Camera>& camera, const shared_ptr<Image>& img) {
    Ray ray(camera->worldRay(pixel.x, pixel.y, plane));
    Array<shared_ptr<Light>> lights;
    m_scene->getTypedEntityArray(lights);

    shared_ptr<Surfel> surfel(intersects(ray.bumpedRay(m_epsilon).origin(), ray.direction()));
    Radiance3 pixelValue(0, 0, 0);
    if (notNull(surfel)) {
        pixelValue = L_o(surfel, (-1)*ray.direction(), lights, 2);
    }


    //L_o(intersects(ray.bumpedRay(m_epsilon).origin(),ray.direction()), (-1)*ray.direction(), lights)/*shade(intersects(ray), lights, (-1)*ray.direction()));

    return pixelValue;
    //Trace a single ray from the center of the specified pixel

    // for loop over primitive arrays
    // return foundTriangle, or foundSphere accordingly
};
*/
