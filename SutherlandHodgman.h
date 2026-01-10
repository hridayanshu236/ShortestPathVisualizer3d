#ifndef SUTHERLAND_HODGMAN_H
#define SUTHERLAND_HODGMAN_H

#include <vector>
#include <glm/glm.hpp>

class SutherlandHodgman
{
public:
    // Clip a polygon against a rectangular boundary
    static std::vector<glm::vec2> ClipPolygon(
        const std::vector<glm::vec2>& polygon,
        const glm::vec2& minBound,
        const glm::vec2& maxBound
    );

    // Clip a polygon against a circular boundary
    static std::vector<glm::vec2> ClipPolygonCircular(
        const std::vector<glm::vec2>& polygon,
        const glm::vec2& center,
        float radius,
        int segments = 32
    );

private:
    // Clip against a single edge
    static std::vector<glm::vec2> ClipAgainstEdge(
        const std::vector<glm::vec2>& polygon,
        const glm::vec2& edgeStart,
        const glm::vec2& edgeEnd
    );

    // Check if point is inside the edge (left side)
    static bool IsInside(const glm::vec2& point, const glm::vec2& edgeStart, const glm::vec2& edgeEnd);

    // Calculate intersection point
    static glm::vec2 Intersect(
        const glm::vec2& p1, const glm::vec2& p2,
        const glm::vec2& edgeStart, const glm::vec2& edgeEnd
    );
};

#endif