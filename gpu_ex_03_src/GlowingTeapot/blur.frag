uniform sampler2D texture;

void main() 
{
	// Hier soll der Filter implementiert werden
	
	// Schrittweite fuer ein Pixel (bei Aufloesung 512)
	float texCoordDelta = 1. / 512.;
	
	// Filtergroesse (gesamt)
	int filterWidth = 55;	
	
	// linker Ecke von Filter
	vec2 texCoord;
	texCoord.x = gl_TexCoord[0].s - (float(filterWidth / 2) * texCoordDelta);
	texCoord.y = gl_TexCoord[0].t - (float(filterWidth / 2) * texCoordDelta);

	// Wert zum Aufakkumulieren der Farbwerte
	vec3 val = vec3(0); 
	//for(...)

     float orig_x;
     for(int x=0; x< filterWidth; x++) 
	{
	orig_x= texCoord.x;  //x Koordinate speichern
        
		for(int y=0; y<filterWidth;y++)
		{
			val= val + texture2D(texture, texCoord).xyz;

			//TODO: Verschieben der Texturkoordinate -> naechstes Pixel in x Richtung	
			texCoord.x= texCoord.x + texCoordDelta;
		}
		// TODO: Zurücksetzen von texCoord.x und weiterschieben von texCoord.y
                texCoord.x= orig_x; //wieder zurücksetzen
		texCoord.y= texCoord.y + texCoordDelta;

	}

	// Durch filterWidth^2 teilen, um zu normieren.
	val = 2.0 * val / float(filterWidth*filterWidth);   

	// TODO: Ausgabe von val
	gl_FragColor.rgb = val.xyz;

	// Die folgende Zeile dient nur zu Debugzwecken!
	// Wenn das Framebuffer object richtig eingestellt wurde und die Textur an diesen Shader übergeben wurde
	// wird die Textur duch den folgenden Befehl einfach nur angezeigt.
	//gl_FragColor.rgb = texture2D(texture,gl_TexCoord[0].st).xyz;
}