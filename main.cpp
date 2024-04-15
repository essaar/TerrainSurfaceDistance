#include <iostream>
#include <fstream>
#include <math.h>
#include <map>
#include <glm/glm.hpp>

#define SIZE 512
#define SPATIAL_RES 30
#define HEIGHT_VAL 11

static glm::vec2 startCoord = {1, 2};
static glm::vec2 endCoord = {8, 10};

static char preData[SIZE][SIZE];
static char postData[SIZE][SIZE];

enum Bounds { MinX, MaxX, MinY, MaxY };

float getSlope(glm::vec2 a, glm::vec2 b)
{
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    float slope = (dx == 0) ? std::numeric_limits<float>::infinity() : (dy == 0) ? 0 : (dy / dx);
    return slope;
}

// Calculate distance between start and end coordinates given we know they make a horizontal line
std::pair<float, float> calculateDistanceHorizontal()
{
    std::pair<float, float> distances = {0, 0};

    for(int x = startCoord.x; x < endCoord.x - 1; x++)
    {
        // Pre-eruption
        float curHeight = preData[x][(int)startCoord.y];
        float nextHeight = preData[x + 1][(int)startCoord.y];
        float heightDiff = curHeight - nextHeight;
        distances.first += sqrt(pow(heightDiff * HEIGHT_VAL, 2) + pow(SIZE, 2));


        // Post-eruption
        curHeight = postData[x][(int)startCoord.y];
        nextHeight = postData[x + 1][(int)startCoord.y];
        heightDiff = curHeight - nextHeight;
        distances.second += sqrt(pow(heightDiff * HEIGHT_VAL, 2) + pow(SIZE, 2));
    }

    return distances;
}

// Calculate distance between start and end coordinates given we know they make a vertical line
std::pair<float, float> calculateDistanceVertical()
{
    std::pair<float, float> distances = {0, 0};

    for(int y = startCoord.y; y < endCoord.y - 1; y++)
    {
        // Pre-eruption
        float curHeight = preData[(int)startCoord.x][y];
        float nextHeight = preData[(int)startCoord.x][y + 1];
        distances.first += sqrt(pow((curHeight - nextHeight) * HEIGHT_VAL, 2) + pow(SIZE, 2));


        // Post-eruption
        curHeight = postData[(int)startCoord.x][y];
        nextHeight = postData[(int)startCoord.x][y + 1];
        distances.second += sqrt(pow((curHeight - nextHeight) * HEIGHT_VAL, 2) + pow(SIZE, 2));
    }

    return distances;
}

// Calculate distance between start and end coordinates given we know they make a diagonal line
std::pair<float, float> calculateDistanceDiagonal()
{
    std::pair<float, float> distances = {0, 0};

    // Move from right to left on x-axis and bottom to top on y-axis
    for(int x = std::max(startCoord.x, endCoord.x); x < std::min(startCoord.x, endCoord.x) + 1; x--)
    {
        for(int y = std::min(startCoord.y, endCoord.y); y < std::max(startCoord.y, endCoord.y) - 1; y++)
        {
            // Pre-eruption
            float curHeight = preData[x][y];
            float nextHeight = preData[x + 1][y + 1];
            distances.first += sqrt(pow((curHeight - nextHeight) * HEIGHT_VAL, 2) + pow(SIZE * sqrt(2), 2));    // Dialognal length = sqrt(2)[pythagorean theorm]


            // Post-eruption
            curHeight = postData[x][y];
            nextHeight = postData[x + 1][y + 1];
            distances.second += sqrt(pow((curHeight - nextHeight) * HEIGHT_VAL, 2) + pow(SIZE * sqrt(2), 2));
        }
    }

    return distances;
}

// Get intersection point given 2 lines defined by points a,b & c,d
std::pair<bool, glm::vec2> getIntersection(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d)
{
    std::pair<bool, glm::vec2> intersection {false, glm::vec2(-1)};

    float determinant = (a.x - b.x) * (c.y - d.y) - (a.y - b.y) * (c.x - d.x);

    if (determinant == 0)
    {
        return intersection;
    }
    else
    {
        float px = ((((a.x * b.y) - (a.y * b.x)) * (c.x - d.x)) - ((a.x - b.x) * ((c.x * d.y) - (c.y * d.x)))) / determinant;
        float py = ((((a.x * b.y) - (a.y * b.x)) * (c.y - d.y)) - ((a.y - b.y) * ((c.x * d.y) - (c.y * d.x)))) / determinant;

        intersection.first = true;
        intersection.second = glm::vec2(px, py);
    }
    return intersection;
}

bool withinBounds(glm::vec2 coord, glm::vec4 bounds)
{
    return (coord.x >= bounds[MinX] && coord.x <= bounds[MaxX] && coord.y >= bounds[MinX] && coord.y <= bounds[MaxX]);
}

// Calculate distance between start and end coordinates piece-wise
// based on intersections with all horizontal, vertical and dialgonal lines withing bounds.
std::pair<float, float> calculateDistancebyIntersections(glm::vec4 bounds)
{
    std::pair<float, float> distances = {0, 0};

    std::map<float, glm::vec2> intersectionPtsbyDist;
    intersectionPtsbyDist.insert(std::pair<float, glm::vec2> (0, startCoord));  // add start coordinate

    // Find intersections with all horizontal lines within bounds
    for(int y = bounds[MinY]; y < bounds[MaxY]; y++)
    {
        glm::vec2 startPt (bounds[MinX], y);
        glm::vec2 endPt (bounds[MaxX], y);
        std::pair<bool, glm::vec2> intersection = getIntersection(startPt, endPt, startCoord, endCoord);
        if(intersection.first == true && withinBounds(intersection.second, bounds))
        {
            float distance = glm::length(intersection.second - startCoord);
            intersectionPtsbyDist.insert(std::pair<float, glm::vec2> (distance, intersection.second));
        }
    }

    // Find intersections with all vertical lines within bounds
    for(int x = bounds[MinX]; x < bounds[MaxX]; x++)
    {
        glm::vec2 startPt (x, bounds[MinY]);
        glm::vec2 endPt (x, bounds[MaxY]);
        std::pair<bool, glm::vec2> intersection = getIntersection(startPt, endPt, startCoord, endCoord);
        if(intersection.first == true && withinBounds(intersection.second, bounds))
        {
            float distance = glm::length(intersection.second - startCoord);
            intersectionPtsbyDist.insert(std::pair<float, glm::vec2> (distance, intersection.second));
        }
    }

    // Find intersections with all diagonal lines within bounds
    int x = bounds[MinX];
    int xBound = bounds[MinY];
    int y = bounds[MinY];
    int yBound = bounds[MinX];
    while(xBound <= bounds[MaxX] && yBound <= bounds[MaxY])
    {
        (x < bounds[MaxX]) ? x++ : xBound++;
        glm::vec2 startPt (x, xBound);

        (y < bounds[MaxY]) ? y++ : yBound++;
        glm::vec2 endPt (yBound, y);

        // FOR DEBUGGING
        // std::cout<<"\n("<<startPt.x<<", "<<startPt.y<<") "<<"("<<endPt.x<<", "<<endPt.y<<") "<<std::endl;

        std::pair<bool, glm::vec2> intersection = getIntersection(startPt, endPt, startCoord, endCoord);
        if(intersection.first == true && withinBounds(intersection.second, bounds))
        {
            float distance = glm::length(intersection.second - startCoord);
            intersectionPtsbyDist.insert(std::pair<float, glm::vec2> (distance, intersection.second));
        }
    }

    // Insert last piece of distance with end coordinate
        //-> giving NAN results coz some intersection points missing which mess up final calculations :(
    // float lastDist = glm::length(intersectionPtsbyDist.end()->second - endCoord);
    // intersectionPtsbyDist.insert(std::pair<float, glm::vec2> (lastDist, endCoord));

    // FOR DEBUGGING
    // for (auto const& [key, val] : intersectionPtsbyDist)
    // {
    //     std::cout << "" << key << ": (" << val.x << ", " << val.y << ")" << std::endl;
    // }

    // Calculate height at each point of intersection
    // then calculate distace bewteen consecutive intersection points along the surface
    glm::vec2 pt1 = intersectionPtsbyDist[0];
    for (auto const& [key, val] : intersectionPtsbyDist)
    {
        if(key == 0)
            continue;

        glm::vec2 pt2 = val;

        // FOR DEBUGGING
        // std::cout << "(" << pt1.x << ", " << pt1.y << ") : (" << pt2.x << ", " << pt2.y << ")" << std::endl;

        glm::vec2 neighbor1 = glm::vec2(glm::ceil(pt2.x), glm::floor(pt2.y));
        glm::vec2 neighbor2 = glm::vec2(glm::floor(pt2.x), glm::ceil(pt2.y));

        // Pre-eruption
        float height1 = preData[(int)neighbor1.x][(int)neighbor1.y];
        float height2 = preData[(int)neighbor2.x][(int)neighbor2.y];
        float t = glm::length(neighbor1 - pt2)/glm::length(neighbor1 - neighbor2);
        float heightOfIntersectionPt = glm::mix(height1, height2, t);
        float deltaHeight = heightOfIntersectionPt - preData[(int)pt1.x][(int)pt1.y];
        distances.first += sqrt(pow(deltaHeight * HEIGHT_VAL, 2) + pow(glm::length(pt2 - pt1) * SIZE, 2));    // pythagorean theorm

        // Post-eruption
        height1 = postData[(int)neighbor1.x][(int)neighbor1.y];
        height2 = postData[(int)neighbor2.x][(int)neighbor2.y];
        t = glm::length(neighbor1 - pt2)/glm::length(neighbor1 - neighbor2);
        heightOfIntersectionPt = glm::mix(height1, height2, t);
        deltaHeight = heightOfIntersectionPt - preData[(int)pt1.x][(int)pt1.y];
        distances.second += sqrt(pow(deltaHeight * HEIGHT_VAL, 2) + pow(glm::length(pt2 - pt1) * SIZE, 2));

        pt1 = pt2;
    }

    return distances;
}

int main()
{
    std::ifstream prefile("../../pre.data");
    std::ifstream postfile("../../post.data");

    if(prefile && postfile)
    {
        // Read data from files
        prefile.seekg (0, prefile.end);
        int length = prefile.tellg();
        prefile.seekg (0, prefile.beg);
        char * preBuffer = new char [length];
        prefile.read (preBuffer,length);

        postfile.seekg (0, postfile.end);
        length = postfile.tellg();
        postfile.seekg (0, postfile.beg);
        char * postBuffer = new char [length];
        postfile.read (postBuffer,length);

        // Store read data into 2D arrays
        for(int i = 0; i < SIZE * SIZE; i++)
        {
            preData[i / SIZE][i % SIZE] = preBuffer[i];
            postData[i / SIZE][i % SIZE] = postBuffer[i];
        }

        prefile.close();
        postfile.close();
        delete[] preBuffer;
        delete[] postBuffer;

        // Get coordinate bounds for calculations
        glm::vec4 bounds = {
            std::min(startCoord.x, endCoord.x), //MinX
            std::max(startCoord.x, endCoord.x), //MaxX
            std::min(startCoord.y, endCoord.y), //MinY
            std::max(startCoord.y, endCoord.y)  //MaxY
        };

        float totSurfDistPre = 0;
        float totSurfDistPost = 0;

        // Case 1 : direction is parallel to X-axis
        if (bounds[MinY] == bounds[MaxY])
        {
            std::pair<float, float> distances = calculateDistanceHorizontal();
            totSurfDistPre += distances.first;
            totSurfDistPost += distances.second;
        }

        // Case 2 : direction is parallel to Y-axis
        else if (bounds[MinX] == bounds[MaxX])
        {
            std::pair<float, float> distances = calculateDistanceHorizontal();
            totSurfDistPre += distances.first;
            totSurfDistPost += distances.second;
        }

        // Case 3 : direction is diagonal (x =- y)
        else if (getSlope(startCoord, endCoord) == -1)
        {
            std::pair<float, float> distances = calculateDistanceDiagonal();
            totSurfDistPre += distances.first;
            totSurfDistPost += distances.second;
        }

        // Case 4 : all other directions
        else
        {
            std::pair<float, float> distances = calculateDistancebyIntersections(bounds);
            totSurfDistPre = distances.first;
            totSurfDistPost = distances.second;
        }

        std::cout<<"Surface distance from point A("<<startCoord.x<<", "<<startCoord.y<<") and point B("<<endCoord.x<<", "<<endCoord.y<<") is as follows:"<<std::endl;
        std::cout<<"\nPre-eruption = "<<totSurfDistPre<<"m"<<std::endl;
        std::cout<<"\nPost-eruption = "<<totSurfDistPost<<"m"<<std::endl;
        std::cout<<"\nDifference = "<<(totSurfDistPost - totSurfDistPre)<<"m"<<std::endl;
    }
    else
    {
        std::cout<<"Files not found."<<std::endl;
        return -1;
    }

    return 0;
}
