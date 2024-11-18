#include <iostream>
#include <cmath>
#include <random>

#include "main.h"

int getTotalNeighbors(int location);

unsigned char* screenPixels = new unsigned char[screenWidth * screenHeight * 3]();

int tileWidth = 100;
int tileHeight = 100;

bool* tiles = new bool[tileWidth * tileHeight]();

// Seed the random number generator
std::random_device rd;  // Non-deterministic random device
std::mt19937 gen(rd()); // Mersenne Twister engine
std::uniform_int_distribution<> dist(0, 1); // Range: [0, 1]

void initPixelArray() {
	for (int i = 0; i < tileWidth * tileHeight; i++) {
		tiles[i] = dist(gen);
	}
}

void debug() {
	for (int i = 0; i < screenWidth * screenHeight; i++) {
		int pixelVal = tiles[(int) floor((i % screenWidth) / (screenWidth / tileWidth)) + tileWidth * (int) (floor(i / screenWidth) / (screenHeight / tileHeight))] * 255;

		screenPixels[i * 3] = pixelVal;
		screenPixels[i * 3 + 1] = pixelVal;
		screenPixels[i * 3 + 2] = pixelVal;
	}
}

unsigned char* updateGameState() {
	debug();

	return screenPixels;
}

void processPixels() {
	//Processing
	for (int iterations = 0; iterations < 1; iterations++) {
		for (int i = 0; i < tileWidth * tileHeight; i++) {
			if (tiles[i] == 1) {
				if (getTotalNeighbors(i) < 2) {
					tiles[i] = 0;
				}

				if (getTotalNeighbors(i) > 3) {
					tiles[i] = 0;
				}
			}
			else {
				if (getTotalNeighbors(i) == 3) {
					tiles[i] = 1;
				}
			}
		}
	}
}

int getTotalNeighbors(int location) {
	int total = 0;

	if (location % tileWidth != 0) {
		total += tiles[location - tileWidth - 1]; //Top left
		total += tiles[location - 1]; //Middle left
		total += tiles[location + tileWidth - 1]; //Bottom left
	}

	if (location >= tileWidth) {
		total += tiles[location - tileWidth]; //Top middle
	}

	if (location + tileWidth <= tileWidth * tileHeight) {
		total += tiles[location + tileWidth]; //Bottom middle
	}

	if (location % tileWidth != tileWidth - 1) {
		total += tiles[location - tileWidth + 1]; //Top right
		total += tiles[location + 1]; //Middle right
		total += tiles[location + tileWidth + 1]; //Bottom right
	}

	return total;
}