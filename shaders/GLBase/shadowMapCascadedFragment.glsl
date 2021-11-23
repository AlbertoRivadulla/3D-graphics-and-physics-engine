#version 420 core

void main()
{
    // There is only a depth attachment in this buffer, so only this is written, 
    // with no need to do it manually
    // gl_FragDepth = gl_FragCoord.z;
}
