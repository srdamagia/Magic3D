uniform vec2 tile;
uniform vec2 scroll;
uniform vec2 speed;

void main()
{	
    gl_FragColor = light(getTexturesMix(0.0, tile, scroll, speed));
}
