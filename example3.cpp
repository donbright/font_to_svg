// based on github user ebraminio's code to process Xerxes.ttf into svg

#include <fstream>
#include "font_to_svg.hpp"

void genSvg(std::string name, std::string charCode) {
	font2svg::glyph g("Xerxes.ttf", charCode);
	std::string fname = std::string("Output/OldPersian-");
	fname += name;
	fname += ".svg";
	std::ofstream file( fname.c_str() );
	file << g.svgheader() << g.outline() << g.svgfooter();
	g.free();
	file.close();
}

int main(int argc, char * argv[]) {
	genSvg("A", "0x103A0");
	genSvg("I", "0x103A1");
	genSvg("U", "0x103A2");
	genSvg("KA", "0x103A3");
	genSvg("KU", "0x103A4");
	genSvg("GA", "0x103A5");
	genSvg("GU", "0x103A6");
	genSvg("XA", "0x103A7");
	genSvg("CA", "0x103A8");
	genSvg("JA", "0x103A9");
	genSvg("JI", "0x103AA");
	genSvg("TA", "0x103AB");
	genSvg("TU", "0x103AC");
	genSvg("DA", "0x103AD");
	genSvg("DI", "0x103AE");
	genSvg("DU", "0x103AF");
	genSvg("THA", "0x103B0");
	genSvg("PA", "0x103B1");
	genSvg("BA", "0x103B2");
	genSvg("FA", "0x103B3");
	genSvg("NA", "0x103B4");
	genSvg("NU", "0x103B5");
	genSvg("MA", "0x103B6");
	genSvg("MI", "0x103B7");
	genSvg("MU", "0x103B8");
	genSvg("YA", "0x103B9");
	genSvg("VA", "0x103BA");
	genSvg("VI", "0x103BB");
	genSvg("RA", "0x103BC");
	genSvg("RU", "0x103BD");
	genSvg("LA", "0x103BE");
	genSvg("SA", "0x103BF");
	genSvg("ZA", "0x103C0");
	genSvg("SHA", "0x103C1");
	genSvg("SSA", "0x103C2");
	genSvg("HA", "0x103C3");
	genSvg("AURAMAZDAA", "0x103C8");
	genSvg("AURAMAZDAA-2", "0x103C9");
	genSvg("AURAMAZDAAHA", "0x103CA");
	genSvg("XSHAAYATHIYA", "0x103CB");
	genSvg("DAHYAAUSH", "0x103CC");
	genSvg("DAHYAAUSH-2", "0x103CD");
	genSvg("BAGA", "0x103CE");
	genSvg("BUUMISH", "0x103CF");
	genSvg("WORD DIVIDER", "0x103D0");
	genSvg("ONE", "0x103D1");
	genSvg("TWO", "0x103D2");
	genSvg("TEN", "0x103D3");
	genSvg("TWENTY", "0x103D4");
	genSvg("HUNDRED", "0x103D5");
	return 0;
}
