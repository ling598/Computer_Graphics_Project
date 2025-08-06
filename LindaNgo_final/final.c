#include "CSCIx229.h"
#include <time.h>
#include <stdio.h>
#ifdef __APPLE__ 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double angleX = -17;
double angleY = -5.0;
double zoom = 60;
void transformObject(float translateX, float translateY, float translateZ, float rotateAngleY, float rotateAngleZ, float scaleX, float scaleY, float scaleZ);
void drawText(const char* text, void* font, float red, float green, float blue);
int viewMode = 0;
GLfloat eyeX = 0, eyeY = 5, eyeZ = 60, eyeX2 = -3.5; // Initial eye position
float minZ = -30.0, maxZ = 150.0; // Forward and backward boundaries
float minX = -100.0, maxX = 100.0; // Left and right boundaries
int windowWidth = 800;  // Default width
int windowHeight = 600; // Default height

int axes=0;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int obj=0;        //  Scene/opbject selection
double asp=1;     //  Aspect ratio
double dim=3;     //  Size of world
// Light values
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   75;  // Light distance
int inc       =  3;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  65;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =  30;  // Elevation of light
unsigned int texture[9]; // Texture names
unsigned int rock[6]; // Display list ID


void reshape(int width, int height) {
    if (height == 0) height = 1;
    // Set the viewport to cover the entire window
    glViewport(0, 0, width, height);

    // Store the new window dimensions
    windowWidth = width;
    windowHeight = height;
    glMatrixMode(GL_MODELVIEW);
}

void drawFloor(float radius, float height) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[3]);

    const int sides = 100; // Resolution for the circle

    //bottom face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0); // Normal pointing downwards for the bottom face (fixed)
    glTexCoord2f(0.5f, 0.5f); // Center of the texture (UV coordinate for the center)
    glVertex3f(0.0f, -height / 2, 0.0f); // Center of the circle
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)sides;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);

        // Map the texture to the outer edge of the circle
        float u = 0.5f + 0.5f * cosf(angle);
        float v = 0.5f + 0.5f * sinf(angle);
        glTexCoord2f(u, v); // Texture coordinates for the edge of the circle
        glVertex3f(x, -height / 2, z); // Outer points of the bottom circle
    }
    glEnd();

    // top face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0); // Normal pointing upwards for the top face (fixed)
    glTexCoord2f(0.5f, 0.5f); // Center of the texture (UV coordinate for the center)
    glVertex3f(0.0f, height / 2, 0.0f); // Center of the circle
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)sides;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);

        // Map the texture to the outer edge of the circle
        float u = 0.5f + 0.5f * cosf(angle);
        float v = 0.5f + 0.5f * sinf(angle);
        glTexCoord2f(u, v); // Texture coordinates for the edge of the circle
        glVertex3f(x, height / 2, z); // Outer points of the top circle
    }
    glEnd();

    // Draw the side (wall) of the platform
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)sides;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);

        // Calculate normal for the side
        float normalX = cosf(angle);
        float normalZ = sinf(angle);

        // Texture coordinates for the side wall (linear along the height)
        float u = (float)i / (float)sides;  // From 0 to 1 across the edge
        float v1 = 0.0f; // Bottom edge of the side
        float v2 = 1.0f; // Top edge of the side

        // Bottom side vertex
        glNormal3f(normalX, 0, normalZ);  // Normal vector along the surface of the side
        glTexCoord2f(u, v1); // Texture coordinate for bottom edge
        glVertex3f(x, -height / 2, z); // Bottom edge

        // Top side vertex
        glNormal3f(normalX, 0, normalZ);  // Normal vector along the surface of the side
        glTexCoord2f(u, v2); // Texture coordinate for top edge
        glVertex3f(x, height / 2, z); // Top edge
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


void platform(float outerRadius, float innerRadius, float height) {
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Color for the bottom and top faces (light lavender)
    //glColor3f(0.82f, 0.74f, 0.98f);  // Set a color for the faces (light lavender)

    const int sides = 100;

    // Draw the bottom face (inner and outer circle)
    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0, -1, 0); // Normal pointing downwards for the bottom face
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float outerX = outerRadius * cosf(angle);
        float outerZ = outerRadius * sinf(angle);
        float innerX = innerRadius * cosf(angle);
        float innerZ = innerRadius * sinf(angle);

        float texCoord = (float)i / (float)sides; // Texture coordinate for wrapping

        glTexCoord2f(texCoord, 1.0f); glVertex3f(innerX, -height / 2, innerZ); // Inner circle
        glTexCoord2f(texCoord, 0.0f); glVertex3f(outerX, -height / 2, outerZ); // Outer circle
    }
    glEnd();

    // Draw the top face (inner and outer circle)
    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0, 1, 0); // Normal pointing upwards for the top face
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float outerX = outerRadius * cosf(angle);
        float outerZ = outerRadius * sinf(angle);
        float innerX = innerRadius * cosf(angle);
        float innerZ = innerRadius * sinf(angle);

        float texCoord = (float)i / (float)sides; // Texture coordinate for wrapping

        glTexCoord2f(texCoord, 1.0f); glVertex3f(innerX, height / 2, innerZ); // Inner circle
        glTexCoord2f(texCoord, 0.0f); glVertex3f(outerX, height / 2, outerZ); // Outer circle
    }
    glEnd();

    // Draw the outer side wall (between outer radius and top/bottom)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float outerX = outerRadius * cosf(angle);
        float outerZ = outerRadius * sinf(angle);

        float texCoord = (float)i / (float)sides; // Texture coordinate for wrapping

        // Outer side
        glNormal3f(cosf(angle), 0, sinf(angle)); // Smooth normal for outer side
        glTexCoord2f(texCoord, 0.0f); glVertex3f(outerX, -height / 2, outerZ); // Outer edge at bottom
        glTexCoord2f(texCoord, 1.0f); glVertex3f(outerX, height / 2, outerZ); // Outer edge at top
    }
    glEnd();

    // Draw the inner side wall (between inner radius and top/bottom)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float innerX = innerRadius * cosf(angle);
        float innerZ = innerRadius * sinf(angle);

        float texCoord = (float)i / (float)sides; // Texture coordinate for wrapping

        // Inner side
        glNormal3f(-cosf(angle), 0, -sinf(angle)); // Smooth normal for inner side
        glTexCoord2f(texCoord, 0.0f); glVertex3f(innerX, -height / 2, innerZ); // Inner edge at bottom
        glTexCoord2f(texCoord, 1.0f); glVertex3f(innerX, height / 2, innerZ); // Inner edge at top
    }
    glEnd();

    // Disable texturing for other parts (if necessary)
    glDisable(GL_TEXTURE_2D);
}

void window(float outerRadius, float innerRadius, float height) {
    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);  // Bind the window texture

    const int sides = 100;  // Resolution for the circle
    const float windowHeight = height * 0.2f;  // Height of each window (20% of cylinder height)
    const float windowWidth = 0.17f;  // Width of the window in terms of the cylinder's circumference (17% of total circumference)

    // Draw windows as rectangles around the cylinder wall
    const int windowCount = sides / 5;  // Draw about 5 windows around the cylinder
    for (int i = 0; i < windowCount; i++) {
        // Calculate the angle where the window starts
        float angleStart = 2.0f * 3.1415926f * (float)i / (float)windowCount;
        float angleEnd = angleStart + windowWidth;

        // Calculate coordinates for the four corners of the window on the outer and inner wall
        float xOuter1 = outerRadius * cosf(angleStart);
        float zOuter1 = outerRadius * sinf(angleStart);
        float xOuter2 = outerRadius * cosf(angleEnd);
        float zOuter2 = outerRadius * sinf(angleEnd);

        float xInner1 = innerRadius * cosf(angleStart);
        float zInner1 = innerRadius * sinf(angleStart);
        float xInner2 = innerRadius * cosf(angleEnd);
        float zInner2 = innerRadius * sinf(angleEnd);

        // Normals for outer and inner walls (perpendicular to the surface)
        float normalOuterX = cosf(angleStart); // Normal for outer wall pointing radially outward
        float normalOuterZ = sinf(angleStart);

        float normalInnerX = -cosf(angleStart); // Normal for inner wall pointing radially inward
        float normalInnerZ = -sinf(angleStart);

        // Outer wall window (normal pointing outward)
        glBegin(GL_QUADS);
        glNormal3f(normalOuterX, 0.0f, normalOuterZ);
        // Bottom of the window (outer wall)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xOuter1, -height / 2 + windowHeight / 2, zOuter1);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xOuter2, -height / 2 + windowHeight / 2, zOuter2);
        // Top of the window (outer wall)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xOuter2, height / 2 - windowHeight / 2, zOuter2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xOuter1, height / 2 - windowHeight / 2, zOuter1);
        glEnd();

        // Inner wall window (normal pointing inward)
        glBegin(GL_QUADS);
        glNormal3f(normalInnerX, 0.0f, normalInnerZ);
        // Bottom of the window (inner wall)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xInner1, -height / 2 + windowHeight / 2, zInner1);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xInner2, -height / 2 + windowHeight / 2, zInner2);
        // Top of the window (inner wall)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xInner2, height / 2 - windowHeight / 2, zInner2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xInner1, height / 2 - windowHeight / 2, zInner1);
        glEnd();
    }

    // Disable texturing if needed (to avoid affecting other parts)
    glDisable(GL_TEXTURE_2D);
}

void draw3DCubes(float outerRadius) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);  // Bind texture

    const int sides = 100;
    const float cubeSize = 1.5f;  // Size of the cube (also its height)
    
    const int cubeCount = sides / 5;  // Number of cubes

    for (int i = 0; i < cubeCount; i++) {
        // Calculate position for each cube
        float angleStart = 2.0f * 3.1415926f * (float)i / (float)cubeCount + 15;
        float xCenter = outerRadius * cosf(angleStart);
        float zCenter = outerRadius * sinf(angleStart);

        // Position cubes at a fixed height (adjust as needed)
        float cubeHeight = cubeSize * 0.5f;  // Place cubes at the center height (adjust as needed)

        glPushMatrix();  // Save current matrix

        // Translate the cube to its position on the cylinder surface
        glTranslatef(xCenter, cubeHeight, zCenter);

        // Apply rotation to align the cube's face with the wall
        glRotatef(angleStart * -180.0f / 3.1415926f, 0.0f, 1.0f, 0.0f); // Rotate around Y-axis

        // Draw the cube using GL_QUADS (6 faces)
        glBegin(GL_QUADS);

        // Front face
        float nx = cosf(angleStart);   // Normal X direction based on the angle
        float nz = sinf(angleStart);   // Normal Z direction based on the angle
        glNormal3f(nx, 0.0f, nz);  // Normal facing outward in the direction of the cube
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, cubeSize / 2);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize / 2, -cubeSize / 2, cubeSize / 2);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize / 2, cubeSize / 2, cubeSize / 2);    // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, cubeSize / 2);   // Top left

        // Back face
        glNormal3f(-nx, 0.0f, -nz);  // Normal opposite to the front face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, -cubeSize / 2); // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, cubeSize / 2);  // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, cubeSize / 2);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, -cubeSize / 2);  // Top left

        // Left face
        float lx = sinf(angleStart);  // Normal for the rotated left face
        float lz = -cosf(angleStart); // Normal for the rotated left face
        glNormal3f(lx, 0.0f, lz);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, -cubeSize / 2); // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, cubeSize / 2);  // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, cubeSize / 2);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, -cubeSize / 2);  // Top left

        // Right face
        float rx = -sinf(angleStart);  // Normal for the rotated right face
        float rz = cosf(angleStart);   // Normal for the rotated right face
        glNormal3f(rx, 0.0f, rz);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize / 2, -cubeSize / 2, -cubeSize / 2);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize / 2, -cubeSize / 2, cubeSize / 2);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize / 2, cubeSize / 2, cubeSize / 2);    // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize / 2, cubeSize / 2, -cubeSize / 2);   // Top left


        // Top face (doesn't change with rotation)
        glNormal3f(0.0f, 1.0f, 0.0f);  // Normal facing up
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, cubeSize / 2);   // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize / 2, cubeSize / 2, cubeSize / 2);    // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize / 2, cubeSize / 2, -cubeSize / 2);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize / 2, cubeSize / 2, -cubeSize / 2);  // Top left

        // Bottom face (doesn't change with rotation)
        glNormal3f(0.0f, -1.0f, 0.0f);  // Normal facing down
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, cubeSize / 2);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize / 2, -cubeSize / 2, cubeSize / 2);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize / 2, -cubeSize / 2, -cubeSize / 2);  // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize / 2, -cubeSize / 2, -cubeSize / 2); // Top left

        glEnd();

        glPopMatrix();  // Restore matrix
    }

    glDisable(GL_TEXTURE_2D);
}

void draw3DRectangle(float outerRadius) {
    float rectWidth = 1.5; 
    float rectHeight = 3;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);  // Bind texture

    const int sides = 100;
    const int rectCount = sides / 5;  // Number of rectangles

    for (int i = 0; i < rectCount; i++) {
        // Calculate position for each rectangle
        float angleStart = 2.0f * 3.1415926f * (float)i / (float)rectCount + 15;
        float xCenter = outerRadius * cosf(angleStart);
        float zCenter = outerRadius * sinf(angleStart);

        // Position rectangles at a fixed height (adjust as needed)
        float rectHeightHalf = rectHeight * 0.5f;  // Place rectangles at the center height

        glPushMatrix();  // Save current matrix

        // Translate the rectangle to its position on the cylinder surface
        glTranslatef(xCenter, rectHeightHalf, zCenter);

        // Apply rotation to align the rectangle's face with the wall
        glRotatef(angleStart * -180.0f / 3.1415926f, 0.0f, 1.0f, 0.0f); // Rotate around Y-axis

        // Draw the rectangle using GL_QUADS (6 faces)
        glBegin(GL_QUADS);

        // Front face
        float nx = cosf(angleStart);   // Normal X direction based on the angle
        float nz = sinf(angleStart);   // Normal Z direction based on the angle
        glNormal3f(nx, 0.0f, nz);  // Normal facing outward in the direction of the rectangle
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, rectHeight / 2);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(rectWidth / 2, -rectHeight / 2, rectHeight / 2);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(rectWidth / 2, rectHeight / 2, rectHeight / 2);    // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, rectHeight / 2);   // Top left

        // Back face
        glNormal3f(-nx, 0.0f, -nz);  // Normal opposite to the front face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, -rectHeight / 2); // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, rectHeight / 2);  // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, rectHeight / 2);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, -rectHeight / 2);  // Top left

        // Left face
        float lx = sinf(angleStart);  // Normal for the rotated left face
        float lz = -cosf(angleStart); // Normal for the rotated left face
        glNormal3f(lx, 0.0f, lz);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, -rectHeight / 2); // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, rectHeight / 2);  // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, rectHeight / 2);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, -rectHeight / 2);  // Top left

        // Right face
        float rx = -sinf(angleStart);  // Normal for the rotated right face
        float rz = cosf(angleStart);   // Normal for the rotated right face
        glNormal3f(rx, 0.0f, rz);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(rectWidth / 2, -rectHeight / 2, -rectHeight / 2);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(rectWidth / 2, -rectHeight / 2, rectHeight / 2);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(rectWidth / 2, rectHeight / 2, rectHeight / 2);    // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(rectWidth / 2, rectHeight / 2, -rectHeight / 2);   // Top left

        // Top face (doesn't change with rotation)
        glNormal3f(0.0f, 1.0f, 0.0f);  // Normal facing up
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, rectHeight / 2);   // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(rectWidth / 2, rectHeight / 2, rectHeight / 2);    // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(rectWidth / 2, rectHeight / 2, -rectHeight / 2);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rectWidth / 2, rectHeight / 2, -rectHeight / 2);  // Top left

        // Bottom face (doesn't change with rotation)
        glNormal3f(0.0f, -1.0f, 0.0f);  // Normal facing down
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, rectHeight / 2);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(rectWidth / 2, -rectHeight / 2, rectHeight / 2);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(rectWidth / 2, -rectHeight / 2, -rectHeight / 2);  // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rectWidth / 2, -rectHeight / 2, -rectHeight / 2); // Top left

        glEnd();

        glPopMatrix();  // Restore matrix
    }

    glDisable(GL_TEXTURE_2D);
}

void drawCylinder(float x, float y, float z, float radius) {
    const float height = 5.2f;
    const int numSegments = 36;  // Number of segments for the cylinder's circumference
    const float angleIncrement = 2.0f * 3.1415926f / numSegments;  // Angle between each segment

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Bind the texture for the cylinder
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Draw the side of the cylinder
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; i++) {
        float angle = i * angleIncrement;
        float xPos = x + radius * cosf(angle);
        float zPos = z + radius * sinf(angle);

        // Calculate normals for the side of the cylinder
        float normalX = cosf(angle);
        float normalZ = sinf(angle);
        
        // Normals for the side of the cylinder (pointing radially outward)
        glNormal3f(normalX, 0.0f, normalZ);

        // Set texture coordinates
        glTexCoord2f(i / (float)numSegments, 0.0f);  // Bottom ring texture coordinates
        glVertex3f(xPos, -height / 2, zPos);  // Bottom ring
        glTexCoord2f(i / (float)numSegments, 1.0f);  // Top ring texture coordinates
        glVertex3f(xPos, height / 2, zPos);   // Top ring
    }
    glEnd();

    // Draw the bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);  // Normal pointing downward for the bottom cap
    glVertex3f(x, -height / 2, z);  // Center of the bottom cap
    for (int i = 0; i <= numSegments; i++) {
        float angle = i * angleIncrement;
        float xPos = x + radius * cosf(angle);
        float zPos = z + radius * sinf(angle);
        glTexCoord2f(i / (float)numSegments, 0.0f);  // Bottom ring texture coordinates
        glVertex3f(xPos, -height / 2, zPos);  // Bottom ring
    }
    glEnd();

    // Draw the top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);  // Normal pointing upward for the top cap
    glVertex3f(x, height / 2, z);  // Center of the top cap
    for (int i = 0; i <= numSegments; i++) {
        float angle = i * angleIncrement;
        float xPos = x + radius * cosf(angle);
        float zPos = z + radius * sinf(angle);
        glTexCoord2f(i / (float)numSegments, 1.0f);  // Top ring texture coordinates
        glVertex3f(xPos, height / 2, zPos);  // Top ring
    }
    glEnd();

    // Disable texturing if you don't need it elsewhere
    glDisable(GL_TEXTURE_2D);
}

void threeWindow(float outerRadius, float innerRadius, float height) {
    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Bind the window texture
    glBindTexture(GL_TEXTURE_2D, texture[1]);  // Texture for the window

    const int totalSections = 16;  // Divide the cylinder into 16 sections
    const float fullCircleAngle = 2.0f * 3.1415926f;  // Full circle in radians
    const float sectionAngle = fullCircleAngle / totalSections;  // Angle per section
    const float windowHeight = height * 0.2f;  // Height of each window (20% of cylinder height)
    const float windowWidthRatio = 0.6f;  // Windows occupy 60% of their section
    const float gapWidthRatio = 1.0f - windowWidthRatio;  // Remaining space is the gap width

    // Draw the windows
    for (int i = 0; i < totalSections; i++) {
        // Create a repeating pattern: 3 windows, 1 large space
        if (i % 4 == 3) {
            continue;  // Skip this section for the window
        }

        // Calculate the start and end angles for the window within its section
        float angleStart = i * sectionAngle + (sectionAngle * gapWidthRatio / 2);  // Add small gap offset
        float angleEnd = angleStart + sectionAngle * windowWidthRatio;

        // Calculate coordinates for the four corners of the window
        float xOuter1 = outerRadius * cosf(angleStart);
        float zOuter1 = outerRadius * sinf(angleStart);
        float xOuter2 = outerRadius * cosf(angleEnd);
        float zOuter2 = outerRadius * sinf(angleEnd);

        float xInner1 = innerRadius * cosf(angleStart);
        float zInner1 = innerRadius * sinf(angleStart);
        float xInner2 = innerRadius * cosf(angleEnd);
        float zInner2 = innerRadius * sinf(angleEnd);

        // Normals for the outer and inner walls (perpendicular to the surface)
        float normalOuterX = cosf(angleStart); // Normal for outer wall pointing radially outward
        float normalOuterZ = sinf(angleStart);

        float normalInnerX = -cosf(angleStart); // Normal for inner wall pointing radially inward
        float normalInnerZ = -sinf(angleStart);

        // Outer wall window (normal pointing outward)
        glBegin(GL_QUADS);
        glNormal3f(normalOuterX, 0.0f, normalOuterZ);
        // Bottom of the window (outer wall)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xOuter1, -height / 2 + windowHeight / 2, zOuter1);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xOuter2, -height / 2 + windowHeight / 2, zOuter2);
        // Top of the window (outer wall)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xOuter2, height / 2 - windowHeight / 2, zOuter2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xOuter1, height / 2 - windowHeight / 2, zOuter1);
        glEnd();

        // Inner wall window (normal pointing inward)
        glBegin(GL_QUADS);
        glNormal3f(normalInnerX, 0.0f, normalInnerZ);
        // Bottom of the window (inner wall)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xInner1, -height / 2 + windowHeight / 2, zInner1);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xInner2, -height / 2 + windowHeight / 2, zInner2);
        // Top of the window (inner wall)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xInner2, height / 2 - windowHeight / 2, zInner2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xInner1, height / 2 - windowHeight / 2, zInner1);
        glEnd();
    }

    // Draw cylinders in the gap area (add cylinder logic)
    for (int i = 0; i < totalSections; i++) {
        // Calculate the start and end angles for the gap area (after the window)
            float angleStart = i * sectionAngle + (sectionAngle * windowWidthRatio / 2);  // Start after the window
            float angleEnd = angleStart + sectionAngle * windowWidthRatio;  // End of the window, gap begins here
            
            // The gap occupies the remaining space
            float gapStartAngle = angleEnd;  // The start of the gap after the window
            float gapEndAngle = gapStartAngle + sectionAngle * gapWidthRatio / 2;  // The end of the gap
            // Cylinder should be placed in the center of the gap area
            float gapCenterAngle = gapStartAngle + (gapEndAngle - gapStartAngle) / 2;  // Center of the gap

            // Calculate the (x, z) position for the cylinder at the inner radius
            float xCenter = innerRadius * cosf(gapCenterAngle);  // Position at the inner radius along the gap center angle
            float zCenter = innerRadius * sinf(gapCenterAngle);  // Same for z-coordinate

            // Draw the cylinder at the inner radius (adjust size as needed)
            drawCylinder(xCenter, 0.0f, zCenter, innerRadius * 0.017f);  // Example: small radius for the cylinder
    }

    // Disable texturing if needed (to avoid affecting other parts)
    glDisable(GL_TEXTURE_2D);
}


void wall(float outerRadius, float innerRadius, float height) {
    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);  // Bind the wall texture

    const int sides = 100;  // Resolution for the circle (higher = smoother)

    // Draw the outer side wall (between outer radius and top/bottom)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float outerX = outerRadius * cosf(angle);
        float outerZ = outerRadius * sinf(angle);

        // Calculate normal for the outer side (pointing outward)
        float normalX = cosf(angle);
        float normalZ = sinf(angle);

        // Calculate texture coordinates
        float texX = (float)i / (float)sides;
        float texYTop = 1.0f;
        float texYBottom = 0.0f;

        // Outer side of the cylinder
        glNormal3f(normalX, 0.0f, normalZ);  // Normal pointing outwards

        // Assign texture coordinates and vertices
        glTexCoord2f(texX, texYBottom);
        glVertex3f(outerX, -height / 2, outerZ);
        glTexCoord2f(texX, texYTop);
        glVertex3f(outerX, height / 2, outerZ);
    }
    glEnd();

    // Draw the inner side wall (smaller radius)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float innerX = innerRadius * cosf(angle);
        float innerZ = innerRadius * sinf(angle);

        // Calculate normal for the inner side (pointing inward)
        float normalX = -cosf(angle);  // Inverted normal (pointing inwards)
        float normalZ = -sinf(angle);  // Inverted normal (pointing inwards)

        // Calculate texture coordinates
        float texX = (float)i / (float)sides;
        float texYTop = 1.0f;
        float texYBottom = 0.0f;

        // Inner side of the cylinder
        glNormal3f(normalX, 0.0f, normalZ);  // Normal pointing inwards

        // Assign texture coordinates and vertices
        glTexCoord2f(texX, texYBottom);
        glVertex3f(innerX, -height / 2, innerZ);
        glTexCoord2f(texX, texYTop);
        glVertex3f(innerX, height / 2, innerZ);
    }
    glEnd();

    // Disable texturing for other parts (if necessary)
    glDisable(GL_TEXTURE_2D);
}

void cylinder(float outerRadius, float height) {

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);  // Bind the wall texture

    // Color for the bottom and top faces (light lavender)
    // glColor3f(0.82f, 0.74f, 0.98f);  // Set a color for the faces (light lavender)

    const int sides = 100;  // Resolution for the circle (higher = smoother)

    // Draw the outer side wall (between outer radius and top/bottom)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= sides; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)sides;
        float outerX = outerRadius * cosf(angle);
        float outerZ = outerRadius * sinf(angle);

        // Calculate normal for the outer side
        float normalX = cosf(angle);  // Normal vector on the cylinder's surface
        float normalZ = sinf(angle);

        // Map texture coordinates based on the angle
        float texCoord = (float)i / (float)sides;  // Texture coordinate for wrapping around

        // Outer side of the cylinder
        glNormal3f(normalX, 0, normalZ);  // Smooth normal for outer side
        
        // Apply texture coordinates before vertex
        glTexCoord2f(texCoord, 0.0f);  // Bottom of the cylinder (y = -height/2)
        glVertex3f(outerX, -height / 2, outerZ);  // Outer edge at bottom
        
        glTexCoord2f(texCoord, 1.0f);  // Top of the cylinder (y = height/2)
        glVertex3f(outerX, height / 2, outerZ);  // Outer edge at top
    }
    glEnd();

    // Disable texturing for other parts (if necessary)
    glDisable(GL_TEXTURE_2D);
}

void cylindricalArchwayWall(float radius, float height, int archCount) {
    // Only want to place 3 arches closely together
    const float totalArcAngle = 0.899 * 3.1415926 / 5.0; // This gives a smaller angular width for the arches
    const float angleIncrement = totalArcAngle / (archCount - 1); // Spread the 3 arches over the totalArcAngle


    // Place arches close together (within a small angular range)
    for (int i = 0; i < archCount; ++i) {
        float angle = i * angleIncrement - totalArcAngle / 2 + 146.95; // Center the 3 arches around 0 angle
        float x = radius * cosf(angle);  // X position on the circle
        float z = radius * sinf(angle);  // Z position on the circle

        glPushMatrix(); // Save the current transformation matrix

        // Translate to the position on the circle
        glTranslatef(x, 0.0f, z);

        // Rotate to face the circle center
        float rotationAngle = angle * -180.0f / 3.1415926f - 90.0f; // Corrected rotation to face inward
        glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

        // Draw the archway at the current position
        float archWidth = 6.3;
        float archHeight = 8.6;
        float wallDepth = 1.2;
        float wallThickness = 1.7;
        float archRadius = archWidth / 2.0f; // Radius of the semicircular arch
        float archSegments = 100; // Number of segments to approximate the semicircular arch
        float verticalWallHeight = archHeight * 1.9; // Make the vertical walls 1.5 times the height of the arch


        glShadeModel(GL_SMOOTH);
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[2]);

        glBegin(GL_QUAD_STRIP);
        
        for (int i = 0; i <= archSegments; ++i) {
            // Calculate angle for the current segment
            float theta = (float)i / archSegments * 3.1415926f; // Angle in radians (0 to PI for half-circle)
            float x = archRadius * cosf(theta); // X position of the arch
            float y = archHeight + archRadius * sinf(theta); // Y position of the arch

            // Texture coordinates (X-coordinate maps along the width)
            float texCoord = (float)i / archSegments; // Texture X coordinate
            float texCoordY = (y - archHeight) / (archHeight + archRadius); // Texture Y coordinate (maps from bottom to top)

            // Normal calculation for a curved surface (pointing along the Z axis)
            float normalX = -cosf(theta); // Normal in the X direction
            float normalY = -sinf(theta); // Normal in the Y direction
            float normalZ = 0.0f;       // Normal in the Z direction (flat on the X-Y plane)

            // Front edge of the arch
            glNormal3f(normalX, normalY, normalZ); // Specify normal for the vertex
            glTexCoord2f(texCoord, texCoordY);     // Texture coordinate
            glVertex3f(x, y, wallDepth / 2.0f);    // Vertex position for front edge

            // Back edge of the arch
            glNormal3f(normalX, normalY, normalZ); // Normal stays the same
            glTexCoord2f(texCoord, texCoordY);     // Texture coordinate
            glVertex3f(x, y, -wallDepth / 2.0f);   // Vertex position for back edge
        }

        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        //float normalInnerX = -cosf(angle); // Normal for inner wall pointing radially inward
        //float normalInnerZ = -sinf(angle);

        float theta = (float)i / archSegments * 3.1415926f; // Angle in radians (0 to PI for half-circle)
        float normalX = -cosf(theta); // Normal in the X direction
        float normalY = -sinf(theta); // Normal in the Y direction
        float normalZ = 0.0f;

        // Bind the texture for the left and right walls
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[2]); // Assuming texture[2] is for the walls

        glBegin(GL_QUADS);

        // Left side wall
        glNormal3f(-normalX, -normalY, normalZ); // Normal pointing to the left
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius, 0.0f, wallDepth / 2.0f); // Bottom-left corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius, 0.0f, -wallDepth / 2.0f); // Bottom-left corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius, archHeight, -wallDepth / 2.0f); // Top-left corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius, archHeight, wallDepth / 2.0f); // Top-left corner (front)

        // Right side wall
        glNormal3f(normalX, normalY, normalZ); // Normal pointing to the right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius, 0.0f, wallDepth / 2.0f); // Bottom-right corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius, 0.0f, -wallDepth / 2.0f); // Bottom-right corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius, archHeight, -wallDepth / 2.0f); // Top-right corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius, archHeight, wallDepth / 2.0f); // Top-right corner (front)

        glEnd();

        // Optionally unbind the texture if you're done with it
        glBindTexture(GL_TEXTURE_2D, 0);

        // Calculate normal for the inner side
        float normalInnerX = -cosf(angle);  // Normal vector on the cylinder's surface
        float normalInnerZ = -sinf(angle);

        // Calculate normal for the outer side (pointing inward)
        float normalOuterX = cosf(angle);  // Inverted normal (pointing inwards)
        float normalOuterZ = sinf(angle);  // Inverted normal (pointing inwards)

        // Add the vertical side walls on both sides of the arch (connected to top of the arch)
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[0]); // Assuming texture[2] is for the side walls

        glBegin(GL_QUADS);

        // Left side wall (front face)
        glNormal3f(normalOuterX, 0.0f, normalOuterZ); // Normal pointing forward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, wallDepth / 2.0f); // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius, verticalWallHeight, wallDepth / 2.0f);                 // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius, 0.0f, wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, wallDepth / 2.0f);             // Bottom-left corner

        // Left side wall (back face)
        glNormal3f(normalInnerX, 0.0f, normalInnerZ); // Normal pointing backward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, -wallDepth / 2.0f); // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius, verticalWallHeight, -wallDepth / 2.0f);                // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius, 0.0f, -wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, -wallDepth / 2.0f);             // Bottom-left corner

        // Right side wall (front face)
        glNormal3f(normalOuterX, 0.0f, normalOuterZ); // Normal pointing forward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, wallDepth / 2.0f);  // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius, verticalWallHeight, wallDepth / 2.0f);                 // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius, 0.0f, wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, wallDepth / 2.0f);              // Bottom-left corner

        // Right side wall (back face)
        glNormal3f(normalInnerX, 0.0f, normalInnerZ); // Normal pointing backward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, -wallDepth / 2.0f); // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius, verticalWallHeight, -wallDepth / 2.0f);                // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius, 0.0f, -wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, -wallDepth / 2.0f);             // Bottom-left corner

        glEnd();

        // Add the vertical walls on both sides of the arch (against the thickness walls)
        glBegin(GL_QUADS);

        // Left vertical wall (against the left thickness wall)
        glNormal3f(normalX, normalY, normalZ); // Normal pointing left
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, wallDepth / 2.0f);       // Bottom-left corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, -wallDepth / 2.0f);      // Bottom-left corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, -wallDepth / 2.0f); // Top-left corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, wallDepth / 2.0f);  // Top-left corner (front)

        // Right vertical wall (against the right thickness wall)
        glNormal3f(-normalX, -normalY, normalZ); // Normal pointing right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, wallDepth / 2.0f);        // Bottom-right corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, -wallDepth / 2.0f);       // Bottom-right corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, -wallDepth / 2.0f);  // Top-right corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, wallDepth / 2.0f);   // Top-right corner (front)

        glEnd();

        //front arc wall
        float positionOffsetY = 8.45; // Adjust this value to raise the arc vertically
        float frontpositionOffsetZ = 0.595; // Z-axis position offset (move along the Z-axis)
        float backpositionOffsetZ = -0.595; // Z-axis position offset (move along the Z-axis)
        float height = 1.5;
        float wallRadius = 3.25;

        glBegin(GL_QUADS);

        for (unsigned int i = 0; i < archSegments; ++i) {
            float curAngle = (i / (float)archSegments) * 3.14159f;     // Current angle in radians
            float nxtAngle = ((i + 1) / (float)archSegments) * 3.14159f; // Next angle in radians

            // Compute vertices for current and next segments
            float curX = wallRadius * cos(curAngle);
            float curY = wallRadius * sin(curAngle) + positionOffsetY; // Add positionOffsetY
            float nxtX = wallRadius * cos(nxtAngle);
            float nxtY = wallRadius * sin(nxtAngle) + positionOffsetY; // Add positionOffsetY

            // Normals for the front arc wall (all pointing outward along Z-axis)
            glNormal3f(normalOuterX, 0.0f, normalOuterZ);

            // Texture coordinates (mapped linearly across the surface)
            float curTexCoord = (float)i / archSegments;
            float nxtTexCoord = (float)(i + 1) / archSegments;

            // Define corners of the quad with normals and texture coordinates
            glTexCoord2f(curTexCoord, 0.0f);
            glVertex3f(curX, curY, frontpositionOffsetZ);                     // Bottom-left corner

            glTexCoord2f(curTexCoord, 1.0f);
            glVertex3f(curX, wallRadius * (1 + height) + positionOffsetY, frontpositionOffsetZ); // Top-left corner

            glTexCoord2f(nxtTexCoord, 1.0f);
            glVertex3f(nxtX, wallRadius * (1 + height) + positionOffsetY, frontpositionOffsetZ); // Top-right corner

            glTexCoord2f(nxtTexCoord, 0.0f);
            glVertex3f(nxtX, nxtY, frontpositionOffsetZ);                     // Bottom-right corner
        }

        glEnd();


        //back arc wall
        glBegin(GL_QUADS);

        for (unsigned int i = 0; i < archSegments; ++i) {
            float curAngle = (i / (float)archSegments) * 3.14159f;     // Current angle in radians
            float nxtAngle = ((i + 1) / (float)archSegments) * 3.14159f; // Next angle in radians

            // Compute vertices for current and next segments
            float curX = wallRadius * cos(curAngle);
            float curY = wallRadius * sin(curAngle) + positionOffsetY; // Add positionOffsetY
            float nxtX = wallRadius * cos(nxtAngle);
            float nxtY = wallRadius * sin(nxtAngle) + positionOffsetY; // Add positionOffsetY

            // Normals for the back arc wall (all pointing inward along Z-axis)
            glNormal3f(normalInnerX, 0.0f, normalInnerZ);

            // Texture coordinates (mapped linearly across the surface)
            float curTexCoord = (float)i / archSegments;
            float nxtTexCoord = (float)(i + 1) / archSegments;

            // Define corners of the quad with normals and texture coordinates
            glTexCoord2f(curTexCoord, 0.0f);
            glVertex3f(curX, curY, backpositionOffsetZ);                     // Bottom-left corner

            glTexCoord2f(curTexCoord, 1.0f);
            glVertex3f(curX, wallRadius * (1 + height) + positionOffsetY, backpositionOffsetZ); // Top-left corner

            glTexCoord2f(nxtTexCoord, 1.0f);
            glVertex3f(nxtX, wallRadius * (1 + height) + positionOffsetY, backpositionOffsetZ); // Top-right corner

            glTexCoord2f(nxtTexCoord, 0.0f);
            glVertex3f(nxtX, nxtY, backpositionOffsetZ);                     // Bottom-right corner
        }

        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);

        glPopMatrix(); // Restore the previous transformation matrix
    }
}

void cylindricalArchBackWall(float radius, float height, int archCount) {
    // Only want to place 3 arches closely together
    const float totalArcAngle = 6.99 * 3.1415926 / 10.0; // This gives a smaller angular width for the arches
    const float angleIncrement = totalArcAngle / (archCount - 1); // Spread the 3 arches over the totalArcAngle

    // Place arches close together (within a small angular range)
    for (int i = 0; i < archCount; ++i) {
        float angle = i * angleIncrement - totalArcAngle / 2 + 30.35; // Center the 3 arches around 0 angle
        float x = radius * cosf(angle);  // X position on the circle
        float z = radius * sinf(angle);  // Z position on the circle

        glPushMatrix(); // Save the current transformation matrix

        // Translate to the position on the circle
        glTranslatef(x, 0.0f, z);

        // Rotate to face the circle center
        float rotationAngle = angle * -180.0f / 3.1415926f - 90.0f; // Corrected rotation to face inward
        glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

        // Draw the archway at the current position
        float archWidth = 6.3;
        float archHeight = 8.6;
        float wallDepth = 1.2;
        float wallThickness = 1.7;
        float archRadius = archWidth / 2.0f; // Radius of the semicircular arch
        float archSegments = 100; // Number of segments to approximate the semicircular arch
        float verticalWallHeight = archHeight * 1.9; // Make the vertical walls 1.5 times the height of the arch


        glShadeModel(GL_SMOOTH);
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[2]);

        glBegin(GL_QUAD_STRIP);
        
        for (int i = 0; i <= archSegments; ++i) {
            // Calculate angle for the current segment
            float theta = (float)i / archSegments * 3.1415926f; // Angle in radians (0 to PI for half-circle)
            float x = archRadius * cosf(theta); // X position of the arch
            float y = archHeight + archRadius * sinf(theta); // Y position of the arch

            // Texture coordinates (X-coordinate maps along the width)
            float texCoord = (float)i / archSegments; // Texture X coordinate
            float texCoordY = (y - archHeight) / (archHeight + archRadius); // Texture Y coordinate (maps from bottom to top)

            // Normal calculation for a curved surface (pointing along the Z axis)
            float normalX = -cosf(theta); // Normal in the X direction
            float normalY = sinf(theta); // Normal in the Y direction
            float normalZ = 0.0f;       // Normal in the Z direction (flat on the X-Y plane)

            // Front edge of the arch
            glNormal3f(normalX, normalY, normalZ); // Specify normal for the vertex
            glTexCoord2f(texCoord, texCoordY);     // Texture coordinate
            glVertex3f(x, y, wallDepth / 2.0f);    // Vertex position for front edge

            // Back edge of the arch
            glNormal3f(normalX, normalY, normalZ); // Normal stays the same
            glTexCoord2f(texCoord, texCoordY);     // Texture coordinate
            glVertex3f(x, y, -wallDepth / 2.0f);   // Vertex position for back edge
        }

        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        
        // Bind the texture for the left and right walls
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[2]); // Assuming texture[2] is for the walls

        glBegin(GL_QUADS);

        float theta = (float)i / archSegments * 3.1415926f; // Angle in radians (0 to PI for half-circle)
        float normalX = cosf(theta); // Normal in the X direction
        float normalY = sinf(theta); // Normal in the Y direction
        float normalZ = 0.0f;

        // Left side wall
        glNormal3f(normalX, normalY, normalZ); // Normal pointing to the left
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius, 0.0f, wallDepth / 2.0f); // Bottom-left corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius, 0.0f, -wallDepth / 2.0f); // Bottom-left corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius, archHeight, -wallDepth / 2.0f); // Top-left corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius, archHeight, wallDepth / 2.0f); // Top-left corner (front)

        // Right side wall
        glNormal3f(-normalX, -normalY, normalZ); // Normal pointing to the right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius, 0.0f, wallDepth / 2.0f); // Bottom-right corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius, 0.0f, -wallDepth / 2.0f); // Bottom-right corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius, archHeight, -wallDepth / 2.0f); // Top-right corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius, archHeight, wallDepth / 2.0f); // Top-right corner (front)

        glEnd();

        // Optionally unbind the texture if you're done with it
        glBindTexture(GL_TEXTURE_2D, 0);

        // Calculate normal for the inner side
        float normalInnerX = cosf(angle);  // Normal vector on the cylinder's surface
        float normalInnerZ = sinf(angle);

        // Calculate normal for the outer side (pointing inward)
        float normalOuterX = -cosf(angle);  // Inverted normal (pointing inwards)
        float normalOuterZ = -sinf(angle);  // Inverted normal (pointing inwards)

        // Add the vertical side walls on both sides of the arch (connected to top of the arch)
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[0]); // Assuming texture[2] is for the side walls

        glBegin(GL_QUADS);

        // Left side wall (front face)
        glNormal3f(normalOuterX, 0.0f, normalOuterZ); // Normal pointing forward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, wallDepth / 2.0f); // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius, verticalWallHeight, wallDepth / 2.0f);                 // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius, 0.0f, wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, wallDepth / 2.0f);             // Bottom-left corner

        // Left side wall (back face)
        glNormal3f(normalInnerX, 0.0f, normalInnerZ); // Normal pointing backward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, -wallDepth / 2.0f); // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius, verticalWallHeight, -wallDepth / 2.0f);                // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius, 0.0f, -wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, -wallDepth / 2.0f);             // Bottom-left corner

        // Right side wall (front face)
        glNormal3f(normalOuterX, 0.0f, normalOuterZ); // Normal pointing forward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, wallDepth / 2.0f);  // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius, verticalWallHeight, wallDepth / 2.0f);                 // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius, 0.0f, wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, wallDepth / 2.0f);              // Bottom-left corner

        // Right side wall (back face)
        glNormal3f(normalInnerX, 0.0f, normalInnerZ); // Normal pointing backward
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, -wallDepth / 2.0f); // Top-left corner
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius, verticalWallHeight, -wallDepth / 2.0f);                // Top-right corner
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius, 0.0f, -wallDepth / 2.0f);                              // Bottom-right corner
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, -wallDepth / 2.0f);             // Bottom-left corner

        glEnd();

        // Add the vertical walls on both sides of the arch (against the thickness walls)
        glBegin(GL_QUADS);

        // Left vertical wall (against the left thickness wall)
        glNormal3f(normalX, normalY, normalZ); // Normal pointing left
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, wallDepth / 2.0f);       // Bottom-left corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-archRadius - wallThickness, 0.0f, -wallDepth / 2.0f);      // Bottom-left corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, -wallDepth / 2.0f); // Top-left corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-archRadius - wallThickness, verticalWallHeight, wallDepth / 2.0f);  // Top-left corner (front)

        // Right vertical wall (against the right thickness wall)
        glNormal3f(-normalX, -normalY, normalZ); // Normal pointing right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, wallDepth / 2.0f);        // Bottom-right corner (front)
        glTexCoord2f(1.0f, 0.0f); glVertex3f(archRadius + wallThickness, 0.0f, -wallDepth / 2.0f);       // Bottom-right corner (back)
        glTexCoord2f(1.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, -wallDepth / 2.0f);  // Top-right corner (back)
        glTexCoord2f(0.0f, 1.0f); glVertex3f(archRadius + wallThickness, verticalWallHeight, wallDepth / 2.0f);   // Top-right corner (front)

        glEnd();
        
        glBegin(GL_QUADS);  // Begin drawing quads

        float width = 4.0;
        float backWallheight = 16.0;
        float wallArchDepth = 0.1; 

        // Define the normals for the square (pointing along the Z-axis)
        glNormal3f(normalOuterX, 0.0f, normalOuterZ);  // Normal pointing in the positive Z direction

        glTexCoord2f(0.0, 0.0); glVertex3f(-width, 0.0, 0.0); // Bottom-left corner
        glTexCoord2f(0.0, 1.0); glVertex3f(-width, backWallheight, 0.0); // Top-left corner
        glTexCoord2f(1.0, 1.0); glVertex3f(width, backWallheight, 0.0); // Top-right corner
        glTexCoord2f(1.0, 0.0); glVertex3f(width, 0.0, 0.0); // Bottom-right corner

        glEnd();  // End drawing quads

        glBegin(GL_QUADS);  // Begin drawing quads

        // Define the normals for the square (pointing along the Z-axis)
        glNormal3f(normalInnerX, 0.0f, normalInnerZ);  // Normal pointing in the positive Z direction

        glTexCoord2f(0.0, 0.0); glVertex3f(-width, 0.0, -wallArchDepth); // Bottom-left corner
        glTexCoord2f(0.0, 1.0); glVertex3f(-width, backWallheight, -wallArchDepth); // Top-left corner
        glTexCoord2f(1.0, 1.0); glVertex3f(width, backWallheight, -wallArchDepth); // Top-right corner
        glTexCoord2f(1.0, 0.0); glVertex3f(width, 0.0, -wallArchDepth); // Bottom-right corner

        glEnd();  // End drawing quads


        //front arc wall
        float positionOffsetY = 8.45; // Adjust this value to raise the arc vertically
        float frontpositionOffsetZ = 0.595; // Z-axis position offset (move along the Z-axis)
        float backpositionOffsetZ = -0.595; // Z-axis position offset (move along the Z-axis)
        float height = 1.5;
        float wallRadius = 3.25;

        glBegin(GL_QUADS);

        for (unsigned int i = 0; i < archSegments; ++i) {
            float curAngle = (i / (float)archSegments) * 3.14159f;     // Current angle in radians
            float nxtAngle = ((i + 1) / (float)archSegments) * 3.14159f; // Next angle in radians

            // Compute vertices for current and next segments
            float curX = wallRadius * cos(curAngle);
            float curY = wallRadius * sin(curAngle) + positionOffsetY; // Add positionOffsetY
            float nxtX = wallRadius * cos(nxtAngle);
            float nxtY = wallRadius * sin(nxtAngle) + positionOffsetY; // Add positionOffsetY

            // Normals for the front arc wall (all pointing outward along Z-axis)
            glNormal3f(normalOuterX, 0.0f, normalOuterZ);

            // Texture coordinates (mapped linearly across the surface)
            float curTexCoord = (float)i / archSegments;
            float nxtTexCoord = (float)(i + 1) / archSegments;

            // Define corners of the quad with normals and texture coordinates
            glTexCoord2f(curTexCoord, 0.0f);
            glVertex3f(curX, curY, frontpositionOffsetZ);                     // Bottom-left corner

            glTexCoord2f(curTexCoord, 1.0f);
            glVertex3f(curX, wallRadius * (1 + height) + positionOffsetY, frontpositionOffsetZ); // Top-left corner

            glTexCoord2f(nxtTexCoord, 1.0f);
            glVertex3f(nxtX, wallRadius * (1 + height) + positionOffsetY, frontpositionOffsetZ); // Top-right corner

            glTexCoord2f(nxtTexCoord, 0.0f);
            glVertex3f(nxtX, nxtY, frontpositionOffsetZ);                     // Bottom-right corner
        }

        glEnd();


        //back arc wall
        glBegin(GL_QUADS);

        for (unsigned int i = 0; i < archSegments; ++i) {
            float curAngle = (i / (float)archSegments) * 3.14159f;     // Current angle in radians
            float nxtAngle = ((i + 1) / (float)archSegments) * 3.14159f; // Next angle in radians

            // Compute vertices for current and next segments
            float curX = wallRadius * cos(curAngle);
            float curY = wallRadius * sin(curAngle) + positionOffsetY; // Add positionOffsetY
            float nxtX = wallRadius * cos(nxtAngle);
            float nxtY = wallRadius * sin(nxtAngle) + positionOffsetY; // Add positionOffsetY

            // Normals for the back arc wall (all pointing inward along Z-axis)
            glNormal3f(normalInnerX, 0.0f, normalInnerZ);

            // Texture coordinates (mapped linearly across the surface)
            float curTexCoord = (float)i / archSegments;
            float nxtTexCoord = (float)(i + 1) / archSegments;

            // Define corners of the quad with normals and texture coordinates
            glTexCoord2f(curTexCoord, 0.0f);
            glVertex3f(curX, curY, backpositionOffsetZ);                     // Bottom-left corner

            glTexCoord2f(curTexCoord, 1.0f);
            glVertex3f(curX, wallRadius * (1 + height) + positionOffsetY, backpositionOffsetZ); // Top-left corner

            glTexCoord2f(nxtTexCoord, 1.0f);
            glVertex3f(nxtX, wallRadius * (1 + height) + positionOffsetY, backpositionOffsetZ); // Top-right corner

            glTexCoord2f(nxtTexCoord, 0.0f);
            glVertex3f(nxtX, nxtY, backpositionOffsetZ);                     // Bottom-right corner
        }

        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);

        glPopMatrix(); // Restore the previous transformation matrix
    }
}

void drawPartialSkybox(float size) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Dimensions to control individual side extensions
    float backExtension = size * 0.5f;   // Extend back face
    float leftExtension = size * 0.5f;   // Extend left face
    float rightExtension = size * 0.5f;  // Extend right face
    float bottomExtension = size * 0.5f; // Extend bottom face
    float topExtension = size * 0.5f;    // Extend top face
    float closerY = -size / 3.5f; 

    // Draw back face
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f); // Normal points out of the back face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - backExtension, closerY, -size - backExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + backExtension, closerY, -size - backExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + backExtension, size, -size - backExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - backExtension, size, -size - backExtension);
    glEnd();

    // Draw left face
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f); // Normal points out of the left face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - leftExtension, closerY, size + leftExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size - leftExtension + 0.5, closerY, -size - leftExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size - leftExtension + 0.5, size, -size - leftExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - leftExtension, size, size + leftExtension);
    glEnd();

    // Draw right face
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f); // Normal points out of the right face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(size + rightExtension - 0.5, closerY, -size - rightExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + rightExtension, closerY, size + rightExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + rightExtension, size, size + rightExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(size + rightExtension - 0.5, size, -size - rightExtension);
    glEnd();

    // Draw bottom face
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); // Normal points out of the bottom face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - bottomExtension, closerY + 0.1, size + bottomExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + bottomExtension, closerY + 0.1, size + bottomExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + bottomExtension, closerY + 0.1, -size - bottomExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - bottomExtension, closerY + 0.1, -size - bottomExtension);
    glEnd();

    // Top face
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glPushMatrix();

    // Move to the correct height for the top face
    glTranslatef(0.0f, size, 0.0f);

    // Rotate the top face by 180 degrees around the x-axis
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

    glTranslatef(0.0f, -size, 0.0f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f); // Normal points out of the top face
        // Adjusted vertices to reflect the rotated position
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - topExtension, size + 0.5, size + topExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + topExtension, size + 0.5, size + topExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + topExtension, size + 0.5, -size - topExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - topExtension, size + 0.5, -size - topExtension);
    glEnd();

    glPopMatrix();

    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}


void drawPartialOuterSkybox(float size) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Dimensions to control individual side extensions
    float backExtension = size * 0.51f;   // Extend back face
    float leftExtension = size * 0.51f;   // Extend left face
    float rightExtension = size * 0.51f;  // Extend right face
    float bottomExtension = size * 0.51f; // Extend bottom face
    float topExtension = size * 0.51f;    // Extend top face
    float closerY = -size / 3.5f;

    // Back face
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f); // Normal points out of the back face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - backExtension, closerY, -size - backExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + backExtension, closerY, -size - backExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + backExtension, size - 0.12, -size - backExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - backExtension, size - 0.12, -size - backExtension);
    glEnd();

    // Left face
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f); // Normal points out of the left face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - leftExtension, closerY, size + leftExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size - leftExtension, closerY, -size - leftExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size - leftExtension, size - 0.12, -size - leftExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - leftExtension, size - 0.12, size + leftExtension);
    glEnd();

    // Right face
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f); // Normal points out of the right face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(size + rightExtension, closerY, -size - rightExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + rightExtension, closerY, size + rightExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + rightExtension, size - 0.12, size + rightExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(size + rightExtension, size - 0.12, -size - rightExtension);
    glEnd();

    // Bottom face
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); // Normal points out of the bottom face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - bottomExtension, closerY, size + bottomExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + bottomExtension, closerY, size + bottomExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + bottomExtension, closerY, -size - bottomExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - bottomExtension, closerY, -size - bottomExtension);
    glEnd();

    // Top face
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f); // Normal points out of the top face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size - topExtension, size - 0.10, size + topExtension);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size + topExtension, size - 0.10, size + topExtension);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size + topExtension, size - 0.10, -size - topExtension);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size - topExtension, size - 0.10, -size - topExtension);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void rocksInsideBuilding(){
    glPushMatrix();
    transformObject(25, -12.5, -13, 90, 5, 0.08, 0.08, 0.08); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(20, -12.5, -15, 165, 5, 0.05, 0.05, 0.05); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(21, -12.5, -12, 50, 5, 0.05, 0.05, 0.05); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();
}

void build(){

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1,1,1);


    //gluLookAt(0.0, 2.0, zoom, 0.0, 0.0, -10.0, 0.0, 3.0, 0.0);
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);

    //skybox
    glPushMatrix();
    drawPartialSkybox(50);
    drawPartialOuterSkybox(50.1);
    glPopMatrix();

    //floor
    glPushMatrix();
    transformObject(-1.0, -13.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply transformations (translation, rotation, scaling)
    drawFloor(34, 1.0); // Create a circular floor with radius 21.0 and height 0.8
    glPopMatrix();

    //first platform
    glPushMatrix();
    transformObject(0.0, 16.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    platform(21.0, 14.5, 0.8);
    glPopMatrix();

    //the walls for the windows
    glPushMatrix();
    transformObject(0.0, 13.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    wall(18, 17.9, 5.5);
    threeWindow(18.3, 17.9, 5);
    glPopMatrix();

    //second platform
    glPushMatrix();
    transformObject(0.0, 10.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    platform(26, 17.0, 0.8);
    glPopMatrix();

    //the walls for the windows
    glPushMatrix();
    transformObject(0.0, 7.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    wall(24, 23.9, 5.5);
    window(24.15, 23.9, 5);
    glPopMatrix();

    //third platform
    glPushMatrix();
    transformObject(0.0, 4.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    platform(33.0, 21, 0.5);
    glPopMatrix();

    //fourth platform
    glPushMatrix();
    transformObject(0.0, 3.5, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    platform(33.0, 25, 1.5);
    glPopMatrix();

    //cube on fourth platform
    glPushMatrix();
    transformObject(0.0, 2.8, 0.0, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    draw3DCubes(24.9);
    glPopMatrix();
    
    //cylinder on right side
    glPushMatrix();
    transformObject(19.5, -4.2, 22, 0.0, 0.0, 1, 1, 1); // Apply different transformations
    cylinder(2.5, 17.5);
    glPopMatrix();

    // Draw the wall with archway
    glPushMatrix();
    transformObject(2, -13.4, -1, 5, 0.0, 1, 1, 1); // Apply different transformations
    cylindricalArchwayWall(33.5, 5.0, 3);
    glPopMatrix();

    // Draw the wall with arch back wall
    glPushMatrix();
    transformObject(0, -13.4, 0, 5, 0.0, 1, 1, 1); // Apply different transformations
    cylindricalArchBackWall(30, 5.0, 8);
    glPopMatrix();

    //rocks right side of the building
    glPushMatrix();
    transformObject(34.9, -5.6, 16, 0, -2, 0.59, 0.48, 1.1); // Apply different transformations
    glCallList(rock[2]);
    
    transformObject(35, -9.9, -2, -13, -5.9, 0.9, 1, 1); // Apply different transformations
    glCallList(rock[5]);
    glPopMatrix();

    glPushMatrix();
    transformObject(26.5, -11.95, 28, 10, 8, 0.35, 0.35, 0.35); // Apply different transformations
    glCallList(rock[4]);
    glPopMatrix();

    glPushMatrix();
    transformObject(40, -11.8, 25, 200, 8, 0.4, 0.4, 0.4); // Apply different transformations
    glCallList(rock[4]);
    glPopMatrix();

    //rocks left side of the building
    glPushMatrix();
    transformObject(-24, -9.95, 36, 200, 10, 0.3, 0.23, 0.2); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-40, -4.10, 36, 190, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-35, -11.2, 42, 0, 5, 0.2, 0.15, 0.2); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-50, -10.2, 60, 210, 10, 0.25, 0.18, 0.2); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-58, -11.2, 42, 230, 5, 0.2, 0.15, 0.2); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-60, -11.2, 60, 165, 5, 0.1, 0.1, 0.1); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-63, -11.2, 30, 165, 5, 0.1, 0.1, 0.1); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    //rocks inside of the building
    glPushMatrix();
    transformObject(0, 0, 0, 0, 0, 1, 1, 1); // Apply different transformations
    rocksInsideBuilding();
    glPopMatrix();

    glPushMatrix();
    transformObject(1, 0, 8, -15, 0, 1, 1, 1); // Apply different transformations
    rocksInsideBuilding();
    glPopMatrix();

    glPushMatrix();
    transformObject(18.5, -12, 25, 50, 5, 0.1, 0.08, 0.09); // Apply different transformations
    glCallList(rock[3]);
    glPopMatrix();

    //rocks behind the building
    glPushMatrix();
    transformObject(-40, -4.0, -50, 0, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-30, -4.0, -50, 0, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-20, -4.0, -50, 0, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(-50, -7.5, -50, 35, -4.5, 0.5, 0.4, 0.8); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();
    
    glPushMatrix();
    transformObject(40, -4.0, -50, 0, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(30, -4.0, -50, 0, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(20, -4.0, -50, 0, -4.5, 0.7, 0.6, 0.5); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();

    glPushMatrix();
    transformObject(50, -7.5, -50, 35, -4.5, 0.5, 0.4, 0.8); // Apply different transformations
    glCallList(rock[2]);
    glPopMatrix();
    
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
    
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    //  White ball with yellow specular
    float yellow[]   = {1.0,1.0,0.0,1.0};
    float Emission[] = {0.0,0.0,0.01*emission,1.0};
    glColor3f(1,1,0);
    glMaterialf(GL_FRONT,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    //  Bands of latitude
    for (int ph = -90; ph < 90; ph += inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 360; th += 2* inc)
        {
            Vertex(th, ph);
            Vertex(th, ph + inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glPopMatrix();
}

void transformObject(float translateX, float translateY, float translateZ, float rotateAngleY, float rotateAngleZ, float scaleX, float scaleY, float scaleZ) {
    glTranslatef(translateX, translateY, translateZ);
    glRotatef(rotateAngleY, 0.0, 1.0, 0.0);
    glRotatef(rotateAngleZ, 0.0, 0.0, 1.0);
    glScalef(scaleX, scaleY, scaleZ);
}


void printParameters() {
    switch (viewMode) {
        case 0: 
            drawText("Replicated View", GLUT_BITMAP_HELVETICA_18, 1.0, 1.0, 1.0);
            break;
        case 1:
            drawText("Overview of scenery", GLUT_BITMAP_HELVETICA_18, 1.0, 1.0, 1.0);
            break;
    }
}

void drawText(const char* text, void* font, float red, float green, float blue) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600); // Set up a 2D orthographic projection
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(red, green, blue);
    
    // Position the text at the bottom left corner
    glRasterPos2i(10, 10);
    
    // Render each character of the text
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}


void getWindowDimensions(int *width, int *height) {
    *width = windowWidth;
    *height = windowHeight;
}

void display() {
    //glClearColor(0.53, 0.81, 0.98, 2.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    

    printParameters();
    // Calculate aspect ratio based on your window dimensions
    int width, height;
    // Assume you have a function to get current window dimensions
    getWindowDimensions(&width, &height);
    float aspectRatio = (float)width / (float)height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float forwardX = sin(angleY * M_PI / 180.0);
    float forwardZ = -cos(angleY * M_PI / 180.0);
    
    //gluPerspective(60.0, aspectRatio, 1.0, 400.0);
    //gluLookAt(eyeX, eyeY, eyeZ, eyeX + forwardX, eyeY, -eyeZ + forwardZ, 0.0, 1.0, 0.0);

    // Set up projection based on the current view mode
    switch (viewMode) {
        case 0: // Perspective view with oblique overhead
            gluPerspective(60.0, aspectRatio, 1.0, 500.0);
            break;

        case 1: // First-person view
            gluPerspective(60.0, aspectRatio, 1.0, 500.0);
            break;
    }

    // Set up the view for the model
    glMatrixMode(GL_MODELVIEW);
    switch (viewMode) {
        case 0:
            gluLookAt(eyeX, eyeY, eyeZ, eyeX + forwardX, eyeY, -eyeZ + forwardZ, 0.0, 1.0, 0.0);
            break;
        case 1:
            gluLookAt(eyeX2, eyeY + 70, eyeZ + 100, eyeX + forwardX, eyeY, -eyeZ + forwardZ, 0.0, 1.0, 0.0);
            break;
        
    }


    build();

    //  Draw axes - no lighting from here on
    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);
    if (axes)
    {
        const double len=5.0;  //  Length of axes
        glBegin(GL_LINES);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(len,0.0,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,len,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,0.0,len);
        glEnd();
        //  Label axes
        glRasterPos3d(len,0.0,0.0);
        Print("X");
        glRasterPos3d(0.0,len,0.0);
        Print("Y");
        glRasterPos3d(0.0,0.0,len);
        Print("Z");
    }

    //  Light switch
    if (light){
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),0.5};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 1.5);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
        
    } else{
      glDisable(GL_LIGHTING);
    }

        
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();

}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {

    if (viewMode == 0) {
        if (key == 'w') {
            eyeZ -= 1.0; // Move forward
        } else if (key == 's') {
            eyeZ += 1.0; // Move backward
        } else if (key == 'e') {
            eyeY += 1.0; // Move up
        } else if (key == 'q') {
            eyeY -= 1.0; // Move down
        } else if (key == 'a') {
            eyeX -= 1.0; // Move left
        } else if (key == 'd') {
            eyeX += 1.0; // Move right
        }
    
        glutPostRedisplay();
    }

    if (viewMode == 1) { // Only enable navigation in first-person view mode
        if (key == 'w' && eyeZ > minZ) {
            eyeZ -= 1.0; // Move forward
        } else if (key == 's' && eyeZ < maxZ) {
            eyeZ += 1.0; // Move backward
        } else if (key == 'e') {
            eyeY += 1.0; // Move up
        } else if (key == 'q') {
            eyeY -= 1.0; // Move down
        } else if (key == 'a' && eyeX2 > minX) {
            eyeX2 -= 1.0; // Move left
        } else if (key == 'd' && eyeX2 < maxX) {
            eyeX2 += 1.0; // Move right
        }
        
        glutPostRedisplay();
    }


    if (key == 'r') { // Reset angles
        angleX = -17.0;
        angleY = -5;
        eyeX = 0; // Reset eye position
        eyeY = 5; // Reset eye position
        eyeZ = 60; // Reset eye position
        eyeX2 = -3.5;
    }
    if (key == 'o') {
        viewMode = (viewMode + 1) % 2; // Switch between the three view modes
        angleX = -17.0;
        angleY = -5;
        eyeX = 0; // Reset eye position
        eyeY = 5; // Reset eye position
        eyeZ = 60; // Reset eye position
        eyeX2 = -3.5;
    }else if (key == 'x'){
        axes = 1-axes;
    }else if (key == 'l'){ //  Toggle lighting
        light = 1-light;
        glutPostRedisplay();
    } else if (key == 'm'){
        move = 1-move;
    } else if (key=='['){ //  Light elevation
        ylight -= 0.1;
    } else if (key==']'){
        ylight += 0.1;
    } else if (key == 'g' && ambient > 0){ //  Ambient level
        ambient -= 5;
    } else if (key == 'G' && ambient < 100){
        ambient +=5;
    } else if (key=='j' && diffuse>0){ //  Diffuse level
        diffuse -= 5;
    } else if (key=='J' && diffuse<100){
        diffuse += 5;
    } else if (key=='h' && specular>0){ //  Specular level
        specular -= 5;
    } else if (key=='H' && specular<100){
        specular += 5;
    } else if (key=='u' && emission>0){ //  Emission level
        emission -= 5;
    } else if (key=='U' && emission<100){
        emission += 5;
    } else if (key=='n' && shininess>-1){
        shininess -= 1;
    } else if (key=='N' && shininess<7){
        shininess += 1;
    }
    shiny = shininess<0 ? 0 : pow(2.0,shininess);
    glutIdleFunc(move?idle:NULL);
    
    
    glutPostRedisplay();
}


void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT:
            angleY += 5.0; // Rotate right by 5 degrees
            break;
        case GLUT_KEY_LEFT:
            angleY -= 5.0; // Rotate left by 5 degrees
            break;
        case GLUT_KEY_UP:
            angleX -= 5.0; // Rotate down by 5 degrees
            break;
        case GLUT_KEY_DOWN:
            angleX += 5.0; // Rotate up by 5 degrees
            break;
    }
    glutPostRedisplay();
    
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 700);
    glutCreateWindow("Linda Ngo's Purple Building and Scenery from Loona's Butterfly Music Video");

    texture[0] = LoadTexBMP("lavender.bmp");
    texture[1] = LoadTexBMP("window.bmp");
    texture[2] = LoadTexBMP("gold.bmp");
    texture[3] = LoadTexBMP("floor.bmp");
    texture[4] = LoadTexBMP("Daylight Box_Right.bmp");
    texture[5] = LoadTexBMP("Daylight Box_Left.bmp");
    texture[6] = LoadTexBMP("Daylight Box_Front.bmp");
    texture[7] = LoadTexBMP("Daylight Box_Bottom.bmp");
    texture[8] = LoadTexBMP("Daylight Box_Top.bmp");

    
    if (texture[0] == 0 || texture[1] == 0 || texture[2] == 0 || texture[3] == 0 
    || texture[4] == 0 || texture[5] == 0 || texture[6] == 0 || texture[7] == 0 || texture[8] == 0) {
        printf("Failed to load texture!\n");
    } else {
        printf("Texture loaded successfully!\n");
    }

    // Load the OBJ file and create a display list
    rock[0] = LoadOBJ("rock1.obj");
    rock[1] = LoadOBJ("rock2.obj");
    rock[2] = LoadOBJ("rock3.obj");
    rock[3] = LoadOBJ("rock4.obj");
    rock[4] = LoadOBJ("rock5.obj");
    rock[5] = LoadOBJ("rock6.obj");

    // Check if OBJ file loaded successfully
    if (rock[0] == 0 || rock[1] == 0 || rock[2] == 0 || rock[3] == 0 
    || rock[4] == 0 || rock[5] == 0) {
        printf("Failed to load OBJ file.\n");
    } else {
        printf("OBJ file loaded successfully!\n");
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); 
    glutSpecialFunc(specialKeys);
    glutIdleFunc(idle);
    ErrCheck("init");
    glutMainLoop();

    return 0;
}

