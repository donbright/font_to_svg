// truetype_to_svhpp - Read TrueType (R) outline, write SVG
// Copyright Don Bright 2013 <hugh.m.bright@gmail.com>
/*

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  License based on zlib license, by Jean-loup Gailly and Mark Adler
*/

#ifndef __truetype_to_svg_h__
#define __truetype_to_svg_h__

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace font2svg {

std::stringstream debug;

FT_Vector halfway_between( FT_Vector p1, FT_Vector p2 )
{
	FT_Vector newv;
	newv.x = p1.x + (p2.x-p1.x)/2.0;
	newv.y = p1.y + (p2.y-p1.y)/2.0;
	return newv;
}

class ttf_file
{
public:
	std::string filename;
  FT_Library library;
  FT_Face face;
	FT_Error error;

	ttf_file()
	{
		filename = std::string("");
	}

	ttf_file( std::string fname )
	{
		filename = fname;
		error = FT_Init_FreeType( &library );
		debug << "Init error code: " << error;

		// Load a typeface
		error = FT_New_Face( library, filename.c_str(), 0, &face );
		debug << "\nFace load error code: " << error;
		debug << "\nTrueType filename: " << filename;
		if (error) {
			std::cerr << "problem loading file " << filename << "\n";
			exit(1);
		}
		debug << "\nFamily Name: " << face->family_name;
		debug << "\nStyle Name: " << face->style_name;
		debug << "\nNumber of faces: " << face->num_faces;
		debug << "\nNumber of glyphs: " << face->num_glyphs;
	}

	void free()
	{
		debug << "\n<!--";
		error = FT_Done_Face( face );
		debug << "\nFree face. error code: " << error;
		error = FT_Done_FreeType( library );
		debug << "\nFree library. error code: " << error;
		debug << "\n-->\n";
	}

};

class glyph
{
public:
	int codepoint;
	FT_GlyphSlot slot;
	FT_Error error;
	FT_Outline ftoutline;
	FT_Vector* ftpoints;
	FT_Glyph_Metrics gm;
  FT_Face face;
	ttf_file file;
	char* tags;
	short* contours;
	std::stringstream debug, tmp;
	int bbwidth, bbheight;

	glyph( ttf_file &f, std::string unicode_s )
	{
		file = f;
		init( unicode_s );
	}

	glyph( char * filename, char * unicode_c_str )
	{
		std::string fname( filename );
		std::string unicode_s( unicode_c_str );
		this->file = ttf_file( fname );
		init( unicode_s );
	}

	void free()
	{
		file.free();
	}

	void init( std::string unicode_s )
	{
	  face = file.face;
		codepoint = strtol( unicode_s.c_str() , NULL, 0 );
		// Load the Glyph into the face's Glyph Slot + print details
		FT_UInt glyph_index = FT_Get_Char_Index( face, codepoint );
		debug << "\nUnicode requested: " << unicode_s;
		debug << " (decimal: " << codepoint << " hex: 0x"
			<< std::hex << codepoint << std::dec << ")";
		debug << "\nGlyph index for unicode: " << glyph_index;
		error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );
		debug << "\nLoad Glyph into Face's glyph slot. error code: " << error;
		slot = face->glyph;
		ftoutline = slot->outline;
		char glyph_name[1024];
		FT_Get_Glyph_Name( face, glyph_index, glyph_name, 1024 );
		debug << "\nGlyph Name: " << glyph_name;
		debug << "\nGlyph Width: " << gm.width
			<< " Height: " << gm.height
			<< " Hor. Advance: " << gm.horiAdvance
			<< " Vert. Advance: " << gm.vertAdvance;
		gm = slot->metrics;

		// Print outline details, taken from the glyph in the slot.
	  debug << "\nNum points: " << ftoutline.n_points;
	  debug << "\nNum contours: " << ftoutline.n_contours;
		debug << "  Endpoint indexes:";
		for ( int i = 0 ; i < ftoutline.n_contours ; i++ ) debug << " " << ftoutline.contours[i];
		debug << "\n-->\n";

		// Invert y coordinates (SVG = neg at top, TType = neg at bottom)
		ftpoints = ftoutline.points;
		for ( int i = 0 ; i < ftoutline.n_points ; i++ )
			ftpoints[i].y *= -1;

		bbheight = face->bbox.yMax - face->bbox.yMin;
		bbwidth = face->bbox.xMax - face->bbox.xMin;
		tags = ftoutline.tags;
		contours = ftoutline.contours;

	}

	std::string svgheader() {
		tmp.str("");

		tmp << "\n<svg width='" << bbwidth << "px'"
			<< " height='" << bbheight << "px'"
	    << " xmlns='http://www.w3.org/2000/svg' version='1.1'>";

		return tmp.str();
	}

	std::string svgborder()  {
		tmp.str("");
		tmp << "\n\n <!-- draw border -->";

		tmp << "\n <rect fill='none' stroke='black'"
			<< " width='" << bbwidth - 1 << "'"
			<< " height='" << bbheight - 1 << "'/>";
		return tmp.str();
	}

	std::string svgtransform() {
		// Truetype points are not in the range usually visible by SVG.
		// they often have negative numbers etc. So.. here we
		// 'transform' to make visible.
		//
		// note also that y coords of all points have been flipped during
		// init() so that SVG Y positive = Truetype Y positive
		tmp.str("");
		tmp << "\n\n <!-- make sure glyph is visible within svg window -->";
		int yadj = gm.horiBearingY + gm.vertBearingY + 100;
		int xadj = 100;
		tmp << "\n <g fill-rule='nonzero' "
			<< " transform='translate(" << xadj << " " << yadj << ")'"
			<< ">";
		return tmp.str();
	}

	std::string axes()  {
		tmp.str("");
		tmp << "\n\n  <!-- draw axes --> ";
		tmp << "\n <path stroke='blue' stroke-dasharray='5,5' d='"
			<< " M" << -bbwidth << "," << 0
			<< " L" <<  bbwidth << "," << 0
			<< " M" << 0 << "," << -bbheight
			<< " L" << 0 << "," << bbheight
			<< " '/>";
		return tmp.str();
	}

	std::string typography_box()  {
		tmp.str("");
		tmp << "\n\n  <!-- draw bearing + advance box --> ";
		int x1 = 0;
		int x2 = gm.horiAdvance;
		int y1 = -gm.vertBearingY-gm.height;
		int y2 = y1 + gm.vertAdvance;
		tmp << "\n <path stroke='blue' fill='none' stroke-dasharray='10,16' d='"
			<< " M" << x1 << "," << y1
			<< " M" << x1 << "," << y2
			<< " L" << x2 << "," << y2
			<< " L" << x2 << "," << y1
			<< " L" << x1 << "," << y1
			<< " '/>";

		return tmp.str();
	}

	std::string points()  {
		tmp.str("");
		tmp << "\n\n  <!-- draw points as circles -->";
		for ( int i = 0 ; i < ftoutline.n_points ; i++ ) {
			int radius = 5;
			if ( i == 0 ) radius = 10;
			std::string color;
			if (tags[i] & 1) color = "blue"; else color = "none";
			tmp << "\n  <!--" << i << "-->";
			tmp << "<circle"
				<< " fill='" << color << "'"
				<< " stroke='black'"
				<< " cx='" << ftpoints[i].x << "' cy='" << ftpoints[i].y << "'"
				<< " r='" << radius << "'"
				<< "/>";
		}

		return tmp.str();
	}

	std::string pointlines()  {
		tmp.str("");
		tmp << "\n\n  <!-- draw straight lines between points -->";
		tmp << "\n  <path fill='none' stroke='green' d='";
		tmp << "\n   M " << ftpoints[0].x << "," << ftpoints[0].y << " L ";
		for ( int i = 0 ; i < ftoutline.n_points ; i++ ) {
			tmp << " " << ftpoints[i].x << "," << ftpoints[i].y;
		}
		tmp << "\n  '/>";
		return tmp.str();
	}

	std::string outline()  {
		/* SVG output. See these sites for more info.
		Basic Terms: http://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
		FType + outlines: http://www.freetype.org/freetype2/docs/reference/ft2-outline_processinhtml
		FType + contours: http://www.freetype.org/freetype2/docs/glyphs/glyphs-6.html
		TType contours: https://developer.apple.com/fonts/TTRefMan/RM01/Chap1.html
		TType contours2: http://www.truetype-typography.com/ttoutln.htm
		Non-zero winding rule: http://en.wikipedia.org/wiki/Nonzero-rule
		SVG paths: http://www.w3schools.com/svg/svg_path.asp
		SVG paths + nonzero: http://www.w3.org/TR/SVG/paintinhtml#FillProperties
		*/
		tmp.str("");
		tmp << "\n\n  <!-- draw actual outline using lines and Bezier curves-->";
		tmp	<< "\n  <path fill='black' stroke='black'"
			<< " fill-opacity='0.45' "
			<< " stroke-width='2' "
			<< " d='";
		int contour_starti = 0;
		int contour_endi = contours[0];
		int contour_counter = 0;
		tmp << "\n   M" << ftpoints[0].x << "," << ftpoints[0].y;
		for ( int i = 0 ; i < ftoutline.n_points ; i++ ) {
			int previndex = i-1;
			int currindex = i;
			int nextindex = i+1;
			if ( nextindex > contour_endi ) nextindex = contour_starti;
			if ( previndex < contour_starti ) previndex = contour_endi;
			// tag bit 1 indicates whether its a control point on a bez curve or not.
			// two consecutive control points imply another point halfway between them
			if ( tags[currindex] & 1 ) {
				if ( tags[previndex] & 1 ) {
					tmp << "\n    L" << ftpoints[currindex].x << "," << ftpoints[currindex].y;
				}
			} else {
				tmp << "\n    Q" << ftpoints[currindex].x << "," << ftpoints[currindex].y;
				FT_Vector nextp = ftpoints[nextindex];
				if ( ! ( tags[nextindex] & 1 ) ) {
					nextp = halfway_between( ftpoints[currindex], ftpoints[nextindex] );
				}
				tmp << " " << nextp.x << "," << nextp.y;
			}
			if ( currindex == contour_endi ) {
				contour_counter = ( contour_counter + 1 ) % ftoutline.n_contours;
				contour_starti = ( currindex + 1 ) % ftoutline.n_points;;
				contour_endi = contours[contour_counter];
				FT_Vector firstp;
				if ( ! ( tags[contour_starti] & 1 ) )
					firstp = halfway_between( ftpoints[contour_endi], ftpoints[contour_starti] );
				else
					firstp = ftpoints[contour_starti];
				tmp << "\n  Z M" << firstp.x << "," << firstp.y;
			}
		}
		tmp << "\n  '/>";
		return tmp.str();
	}

	std::string svgfooter()  {
		tmp.str("");
		tmp << "\n </g>\n</svg>\n";
		return tmp.str();
	}
};

} // namespace

#endif

