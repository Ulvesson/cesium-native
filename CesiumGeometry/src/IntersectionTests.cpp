#include "CesiumGeometry/IntersectionTests.h"

#include "CesiumGeometry/Plane.h"
#include "CesiumGeometry/Ray.h"

#include <CesiumUtility/Math.h>

#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

using namespace CesiumUtility;

namespace CesiumGeometry {

/*static*/ std::optional<glm::dvec3>
IntersectionTests::rayPlane(const Ray& ray, const Plane& plane) noexcept {
  const double denominator = glm::dot(plane.getNormal(), ray.getDirection());

  if (glm::abs(denominator) < Math::Epsilon15) {
    // Ray is parallel to plane.  The ray may be in the polygon's plane.
    return std::optional<glm::dvec3>();
  }

  const double t =
      (-plane.getDistance() - glm::dot(plane.getNormal(), ray.getOrigin())) /
      denominator;

  if (t < 0) {
    return std::optional<glm::dvec3>();
  }

  return ray.getOrigin() + ray.getDirection() * t;
}

std::optional<glm::dvec2> IntersectionTests::rayEllipsoid(
    const Ray& ray,
    const glm::dvec3& radii) noexcept {
  if (radii.x == 0.0 || radii.y == 0.0 || radii.z == 0.0) {
    return std::nullopt;
  }

  glm::dvec3 inverseRadii = 1.0 / radii;

  glm::dvec3 origin = ray.getOrigin();
  glm::dvec3 direction = ray.getDirection();

  glm::dvec3 q = inverseRadii * origin;
  glm::dvec3 w = inverseRadii * direction;

  const double q2 = pow(length(q), 2);
  const double qw = dot(q, w);

  double difference, w2, product, discriminant, temp;
  if (q2 > 1.0) {
    // Outside ellipsoid.
    if (qw >= 0.0) {
      // Looking outward or tangent (0 intersections).
      return std::optional<glm::dvec2>();
    }

    // qw < 0.0.
    const double qw2 = qw * qw;
    difference = q2 - 1.0; // Positively valued.
    w2 = pow(length(w), 2);
    product = w2 * difference;

    if (qw2 < product) {
      // Imaginary roots (0 intersections).
      return std::optional<glm::dvec2>();
    }
    if (qw2 > product) {
      // Distinct roots (2 intersections).
      discriminant = qw * qw - product;
      temp = -qw + sqrt(discriminant); // Avoid cancellation.
      const double root0 = temp / w2;
      const double root1 = difference / temp;
      if (root0 < root1) {
        return glm::dvec2(root0, root1);
      }

      return glm::dvec2(root1, root0);
    }
    // qw2 == product.  Repeated roots (2 intersections).
    const double root = sqrt(difference / w2);
    return glm::dvec2(root, root);
  }
  if (q2 < 1.0) {
    // Inside ellipsoid (2 intersections).
    difference = q2 - 1.0; // Negatively valued.
    w2 = pow(length(w), 2);
    product = w2 * difference; // Negatively valued.

    discriminant = qw * qw - product;
    temp = -qw + sqrt(discriminant); // Positively valued.
    return glm::dvec2(0.0, temp / w2);
  }
  // q2 == 1.0. On ellipsoid.
  if (qw < 0.0) {
    // Looking inward.
    w2 = pow(length(w), 2);
    return glm::dvec2(0.0, -qw / w2);
  }
  // qw >= 0.0.  Looking outward or tangent.
  return std::optional<glm::dvec2>();
}

bool IntersectionTests::pointInTriangle(
    const glm::dvec2& point,
    const glm::dvec2& triangleVertA,
    const glm::dvec2& triangleVertB,
    const glm::dvec2& triangleVertC) noexcept {
  // Define the triangle edges.
  const glm::dvec2 ab = triangleVertB - triangleVertA;
  const glm::dvec2 bc = triangleVertC - triangleVertB;
  const glm::dvec2 ca = triangleVertA - triangleVertC;

  // Return false for degenerate triangles.
  if (glm::length2(ab) < CesiumUtility::Math::Epsilon8 ||
      glm::length2(bc) < CesiumUtility::Math::Epsilon8 ||
      glm::length2(ca) < CesiumUtility::Math::Epsilon8) {
    return false;
  }

  // Get the vector perpendicular to each triangle edge in the same 2D plane).
  const glm::dvec2 ab_perp(-ab.y, ab.x);
  const glm::dvec2 bc_perp(-bc.y, bc.x);
  const glm::dvec2 ca_perp(-ca.y, ca.x);

  // Get vectors from triangle corners to point.
  const glm::dvec2 av = point - triangleVertA;
  const glm::dvec2 cv = point - triangleVertC;

  // Project the point vectors onto the perpendicular vectors.
  const double v_proj_ab_perp = glm::dot(av, ab_perp);
  const double v_proj_bc_perp = glm::dot(cv, bc_perp);
  const double v_proj_ca_perp = glm::dot(cv, ca_perp);

  // The projections determine in what direction the point is from the triangle
  // edge. This will determine in or out, irrespective of winding.
  return (v_proj_ab_perp >= 0.0 && v_proj_ca_perp >= 0.0 &&
          v_proj_bc_perp >= 0.0) ||
         (v_proj_ab_perp <= 0.0 && v_proj_ca_perp <= 0.0 &&
          v_proj_bc_perp <= 0.0);
}

bool IntersectionTests::pointInTriangle(
    const glm::dvec3& point,
    const glm::dvec3& triangleVertA,
    const glm::dvec3& triangleVertB,
    const glm::dvec3& triangleVertC) noexcept {
  // PERFORMANCE_IDEA: Investigate if there is a faster algorithm that can do
  // this test, but without needing to compute barycentric coordinates
  glm::dvec3 unused;
  return IntersectionTests::pointInTriangle(
      point,
      triangleVertA,
      triangleVertB,
      triangleVertC,
      unused);
}

bool IntersectionTests::pointInTriangle(
    const glm::dvec3& point,
    const glm::dvec3& triangleVertA,
    const glm::dvec3& triangleVertB,
    const glm::dvec3& triangleVertC,
    glm::dvec3& barycentricCoordinates) noexcept {
  // PERFORMANCE_IDEA: If optimization is necessary in the future, there are
  // algorithms that avoid length computations (which involve square root
  // operations).

  // Define the triangle edges.
  const glm::dvec3 ab = triangleVertB - triangleVertA;
  const glm::dvec3 bc = triangleVertC - triangleVertB;

  const glm::dvec3 triangleNormal = glm::cross(ab, bc);
  const double lengthSquared = glm::length2(triangleNormal);
  if (lengthSquared < CesiumUtility::Math::Epsilon8) {
    // Don't handle invalid triangles.
    return false;
  }

  // Technically this is the area of the parallelogram between the vectors,
  // but triangles are 1/2 of the area, and the division will cancel out later.
  const double triangleAreaInv = 1.0 / glm::sqrt(lengthSquared);

  const glm::dvec3 ap = point - triangleVertA;
  const double triangleABPRatio =
      glm::length(glm::cross(ab, ap)) * triangleAreaInv;
  if (triangleABPRatio > 1) {
    return false;
  }

  const glm::dvec3 bp = point - triangleVertB;
  const double triangleBCPRatio =
      glm::length(glm::cross(bc, bp)) * triangleAreaInv;
  if (triangleBCPRatio > 1) {
    return false;
  }

  const double triangleCAPRatio = 1.0 - triangleABPRatio - triangleBCPRatio;
  if (triangleCAPRatio < 0) {
    return false;
  }

  barycentricCoordinates.x = triangleBCPRatio;
  barycentricCoordinates.y = triangleCAPRatio;
  barycentricCoordinates.z = triangleABPRatio;

  return true;
}

} // namespace CesiumGeometry
