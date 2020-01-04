#include "stdafx.h"
#include "PolygonSprite.h"

PolygonSprite::PolygonSprite(void)
{
}

PolygonSprite::~PolygonSprite(void)
{
}

void PolygonSprite::Render() {
	Sprite::Render();

	if (isVisible) {
		glBegin(GL_POLYGON);
		  glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0.0f, 0.0f );
		  glTexCoord2f( 1.0f, 0.0f ); glVertex2f( size.width, 0.0f );
		  glTexCoord2f( 1.0f, 1.0f ); glVertex2f( size.width, size.height );
		  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0.0f, size.height );
		glEnd( );
	}
}