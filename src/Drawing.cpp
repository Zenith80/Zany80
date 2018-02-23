#include <Zany80/Drawing.hpp>
#include <Zany80/Zany80.hpp>

#include <cstring>
#include <iostream>

Text::Text(const char *string, int x, int y) : length(strlen(string)) {
	vertices = new sf::Vertex[length * 4];
	for (int i = 0; i < length; i++) {
		// general info
		char c = string[i] - 32;
		int cY = c / GLYPHS_PER_ROW, cX = c % GLYPHS_PER_ROW;
		cX *= GLYPH_WIDTH; cY *= GLYPH_HEIGHT;
		// top left vertex
		vertices[i * 4] = sf::Vertex(sf::Vector2f(x + i * GLYPH_WIDTH,y));
		vertices[i * 4].texCoords = sf::Vector2f(cX, cY);
		// top right vertex
		vertices[i * 4 + 1] = sf::Vertex(sf::Vector2f(x + (i + 1) * GLYPH_WIDTH,y));
		vertices[i * 4 + 1].texCoords = sf::Vector2f(cX + GLYPH_WIDTH, cY);
		// bottom right vertex
		vertices[i * 4 + 2] = sf::Vertex(sf::Vector2f(x + (i + 1) * GLYPH_WIDTH,y + GLYPH_HEIGHT));
		vertices[i * 4 + 2].texCoords = sf::Vector2f(cX + GLYPH_WIDTH, cY + GLYPH_HEIGHT);
		 //bottom left vertex
		vertices[i * 4 + 3] = sf::Vertex(sf::Vector2f(x + i * GLYPH_WIDTH,y + GLYPH_HEIGHT));
		vertices[i * 4 + 3].texCoords = sf::Vector2f(cX, cY + GLYPH_HEIGHT);
	}
}

//Text::Text(const char *string) : length(strlen(string)) {
	//vertices = new sf::Vertex[length];
	//for (int i = 0; i < strlen(string); i++) {
		//vertices[i] = sf::Vertex(sf::Vector2f(0,0));
	//}
//}

Text::~Text() {
	if (vertices != nullptr)
		delete[] vertices;
}

void Text::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(vertices, length * 4, sf::Quads, states);
}

void text(std::string string, int x, int y) {
	static std::map<std::string, Text*> texts;
	if (texts.find(string) == texts.end()) {
		texts[string] = new Text(string.c_str(), x, y);
	}
	zany->window->draw(*texts[string], &zany->font);
}
