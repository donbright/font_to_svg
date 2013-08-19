// main.cpp font_to_svg - public domain

#include "font_to_svg.hpp"

int main( int argc, char * argv[] )
{
	if (argc!=3) {
		std::cerr << "usage: " << argv[0] << " file.ttf 0x0042\n";
		exit( 1 );
	}

	font2svg::glyph g( argv[1], argv[2] );
	std::cout << g.svgheader()
	  << g.svgborder()
		<< g.svgtransform()
		<< g.axes()
		<< g.typography_box()
		<< g.points()
		<< g.pointlines()
		<< g.outline()
		<< g.svgfooter();

	g.free();

  return 0;
}
