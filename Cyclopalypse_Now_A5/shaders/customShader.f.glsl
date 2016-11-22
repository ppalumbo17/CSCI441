/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 120
varying vec4 theColor;
void main() {

    /*****************************************/
    /******* Final Color Calculations ********/
    /*****************************************/
    
    // TODO #10: set the fragment color!
    gl_FragColor = vec4(1,1,1,1);
    // TODO #13: use our varying variable
	
	gl_FragColor = theColor;
}
