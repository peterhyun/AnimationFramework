#version 330 core
out vec4 FragColor;

in float color_flag;

void main()
{
    if((color_flag - 1.0) < 0.001){
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if((color_flag - 2.0) < 0.001){
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if((color_flag - 3.0) < 0.001){
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else{   //This is an error for sure
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
