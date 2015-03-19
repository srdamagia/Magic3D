uniform vec2 tile;
uniform vec2 scroll;

void main()
{	
    gl_FragColor = light(getTexturesMix(0.0, tile, scroll));
}
