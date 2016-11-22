/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 120
varying vec4 theColor;
uniform float time;
void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/
    
    // TODO #14: modify our vertex in object space
	//if(gl_Vertex.x > 0 && gl_Vertex.y > 0 && gl_Vertex.z > 0){
		//gl_Vertex = gl_Vertex + (1.2 * (sin(time)+1)/2) - 0.2;
	//}

    // TODO #8: Perform the Vertex Transformation from Object Space to Clip Space
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    // TODO #12: set our varying variable equal to the Vertex
	
	theColor = gl_Vertex;
}
