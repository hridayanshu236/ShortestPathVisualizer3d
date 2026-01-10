#include "SutherlandHodgman.h"

std::vector<glm::vec2> SutherlandHodgman::ClipPolygon(
    const std::vector<glm::vec2>& polygon,
    const glm::vec2& minBound,
    const glm::vec2& maxBound)
{
    if (polygon.empty())
        return polygon;

    std::vector<glm::vec2> output = polygon;

    // Clip against left edge
    output = ClipAgainstEdge(output,
        glm::vec2(minBound.x, minBound.y),
        glm::vec2(minBound.x, maxBound.y));

    // Clip against right edge
    output = ClipAgainstEdge(output,
        glm::vec2(maxBound.x, maxBound.y),
        glm::vec2(maxBound.x, minBound.y));

    // Clip against bottom edge
    output = ClipAgainstEdge(output,
        glm::vec2(maxBound.x, minBound.y),
        glm::vec2(minBound.x, minBound.y));

    // Clip against top edge
    output = ClipAgainstEdge(output,
        glm::vec2(minBound.x, maxBound.y),
        glm::vec2(maxBound.x, maxBound.y));

    return output;
}

std::vector<glm::vec2> SutherlandHodgman::ClipPolygonCircular(
    const std::vector<glm::vec2>& polygon,
    const glm::vec2& center,
    float radius,
    int segments)
{
    if (polygon.empty())
        return polygon;

    std::vector<glm::vec2> output = polygon;

    // Create circle as a polygon and clip against each edge
    float angleStep = 2.0f * 3.14159f / segments;

    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        glm::vec2 p1(
            center.x + radius * cos(angle1),
            center.y + radius * sin(angle1)
        );
        glm::vec2 p2(
            center.x + radius * cos(angle2),
            center.y + radius * sin(angle2)
        );

        output = ClipAgainstEdge(output, p1, p2);

        if (output.empty())
            break;
    }

    return output;
}

std::vector<glm::vec2> SutherlandHodgman::ClipAgainstEdge(
    const std::vector<glm::vec2>& polygon,
    const glm::vec2& edgeStart,
    const glm::vec2& edgeEnd)
{
    if (polygon.size() < 3)
        return polygon;

    std::vector<glm::vec2> output;

    for (size_t i = 0; i < polygon.size(); i++)
    {
        glm::vec2 current = polygon[i];
        glm::vec2 next = polygon[(i + 1) % polygon.size()];

        bool currentInside = IsInside(current, edgeStart, edgeEnd);
        bool nextInside = IsInside(next, edgeStart, edgeEnd);

        if (currentInside)
        {
            output.push_back(current);

            if (!nextInside)
            {
                // Current inside, next outside - add intersection
                output.push_back(Intersect(current, next, edgeStart, edgeEnd));
            }
        }
        else if (nextInside)
        {
            // Current outside, next inside - add intersection
            output.push_back(Intersect(current, next, edgeStart, edgeEnd));
        }
    }

    return output;
}

bool SutherlandHodgman::IsInside(
    const glm::vec2& point,
    const glm::vec2& edgeStart,
    const glm::vec2& edgeEnd)
{
    // Cross product to determine which side of the edge the point is on
    return ((edgeEnd.x - edgeStart.x) * (point.y - edgeStart.y) -
        (edgeEnd.y - edgeStart.y) * (point.x - edgeStart.x)) >= 0;
}

glm::vec2 SutherlandHodgman::Intersect(
    const glm::vec2& p1, const glm::vec2& p2,
    const glm::vec2& edgeStart, const glm::vec2& edgeEnd)
{
    glm::vec2 dc = glm::vec2(edgeEnd.x - edgeStart.x, edgeEnd.y - edgeStart.y);
    glm::vec2 dp = glm::vec2(p1.x - p2.x, p1.y - p2.y);

    float n1 = (edgeStart.x - p1.x) * dc.y - (edgeStart.y - p1.y) * dc.x;
    float n2 = dp.x * dc.y - dp.y * dc.x;

    if (abs(n2) < 0.0001f)
        return p1; // Parallel lines

    float t = n1 / n2;

    return glm::vec2(p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y));
}