#version 150

out vec4 finalColor;

in vec4 vertexColor;

void main()
{
    //set every drawn pixel to white
    // finalColor = vec4(0.2, 1.0, 0.2, 1.0);
	finalColor = vertexColor;
}