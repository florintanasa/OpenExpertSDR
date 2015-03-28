/*
 * This file is part of ExpertSDR
 *
 * ExpertSDR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * ExpertSDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Copyright (C) 2014 Tsukihime
 */

#ifndef GLFONT_H
#define GLFONT_H

#include <QtOpenGL/qgl.h>
#include <QFont>

typedef struct {
	GLuint list;
	QRectF rect;
} GL_glyph;

class GLFont
{
private:
	QGLWidget *parent;

	GLuint font_texture;
	GL_glyph glyphs[256];
	QFont tex_fnt;
	QFontMetrics *font_metrics;
	int px_font_descent;

	int calc_texture_side();
	QImage create_font_img(const QFont &fnt, int texture_side);
	GLuint create_font_texture(QImage &font_img);
	void fill_call_lists(int tex_side);

public:
	GLFont(QGLWidget *parent, const QFont &fnt);
	~GLFont();
	void draw(GLfloat x, GLfloat y, GLfloat z, const QString &str);
	void draw(GLfloat x, GLfloat y, GLfloat z, const QString &str, const QColor &col);
	int width(const QString &str);
	int height();
};

#endif // GLFONT_H
