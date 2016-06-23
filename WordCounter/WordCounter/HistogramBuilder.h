#pragma once
class HistogramBuilder
{
public:
	static map<string, int> CreateFromFile(string fname);
	static map<string, int> CreateFromString(string text);
	static double CompareTwo(map<string, int> hist1, map<string, int> hist2);
};

