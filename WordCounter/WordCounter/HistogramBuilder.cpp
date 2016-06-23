#include "stdafx.h"
#include "HistogramBuilder.h"
#include "WordCutter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "math.h"


map<string, int> HistogramBuilder::CreateFromString(string text)
{
	stringstream buffer0(text);
	stringstream buffer;
	buffer << regex_replace(buffer0.str(), regex("[^àáâãäå¸æçèéêëìíîïğñòóôõö÷øùüûúışÿÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÜÛÚİŞß]"), string(" "));


	map<string, int> histogram;
	string word;

	while (buffer >> word)
	{
		//ğàáîòàåì ñî ñëîâîì
		word = WordCutter::CutWord(word);
		if (histogram.count(word) == 0)
		{
			histogram.insert(pair<string, int> {word, 1});
		}
		else
		{
			histogram[word] = histogram[word] + 1;
		}
	}

	return histogram;
}



map<string, int> HistogramBuilder::CreateFromFile(string fname)
{
	ifstream fin(fname);
	stringstream buffer0;
	buffer0 << fin.rdbuf();
	stringstream buffer;
	buffer << regex_replace(buffer0.str(), regex("[^àáâãäå¸æçèéêëìíîïğñòóôõö÷øùüûúışÿÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÜÛÚİŞß]"), string(" "));
	
	map<string, int> histogram;
	string word;
	
	while (buffer >> word)
	{
		//ğàáîòàåì ñî ñëîâîì
		word = WordCutter::CutWord(word);
		if (histogram.count(word) == 0)
		{
			histogram.insert(pair<string, int> {word, 1});
		}
		else
		{
			histogram[word] = histogram[word] + 1;
		}
	}

	return histogram;
}

double HistogramBuilder::CompareTwo(map<string, int> hist1, map<string, int> hist2)
{
	map<string, int> compared;
	double match = 0, mismatch = 0;

	for each (pair<string, int> p1 in hist1)
	{
		int n1 = p1.second;
		int n2 = (hist2.find(p1.first) == hist2.end()) ? 0 : hist2.find(p1.first)->second;
		compared.insert(pair<string, int> {p1.first, 0});
		match += min(n1, n2);
		mismatch += abs(n1 - n2);
	}


	for each (pair<string, int> p2 in hist2)
	{
		if (compared.find(p2.first) == compared.end())
			mismatch += p2.second;
	}

	return match/(mismatch+match);
}






