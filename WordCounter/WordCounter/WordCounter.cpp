#include "stdafx.h"
#include <iostream>
#include "WordCutter.h"
#include <fstream>
#include <boost/regex.hpp>
#include "HistogramBuilder.h"
#include <math.h>


struct Word
{
	string word;
	int count;
};

bool cmp(const Word first,
	const Word second)
{
	if (first.count == second.count)
		return first.word < second.word;
	return first.count > second.count;
}

void ShowHist()
{
	cout << "Обработка началась. Ждите" << endl;
	map<string, int> hist = HistogramBuilder::CreateFromFile("input.txt");
	cout << "Гистограмма составлена. Началась запись в файл." << endl;
	vector<Word> arr;
	for each (pair<string, int> p in hist)
	{
		Word w;
		w.word = p.first;
		w.count = p.second;
		arr.insert(arr.end(), w);
	}
	ofstream f("output.txt");
	std::sort(arr.begin(), arr.end(), cmp);
	for (auto it = arr.begin(); it != arr.end(); ++it)
		f << it->word << ": " << it->count << std::endl;
}


void CompareTwo()
{
	map<string, int> hist1 = HistogramBuilder::CreateFromFile("13.txt");
	map<string, int> hist2 = HistogramBuilder::CreateFromFile("14.txt");
	double res = HistogramBuilder::CompareTwo(hist1, hist2);
	cout << "Совпадение: " << res * 100 << "%" << endl;
}



vector<Word> GetHist(string filename)
{
	map<string, int> _hist = HistogramBuilder::CreateFromFile(filename);
	vector<Word> hist;
	for each (pair<string, int> p in _hist)
	{
		Word w;
		w.word = p.first;
		w.count = p.second;
		hist.insert(hist.end(), w);
	}
	std::sort(hist.begin(), hist.end(), cmp);

	return hist;
}



std::vector<std::string> split_string(const std::string& str,
	const std::string& delimiter)
{
	std::vector<std::string> strings;

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = str.find(delimiter, prev)) != std::string::npos)
	{
		strings.push_back(str.substr(prev, pos - prev));
		prev = pos + 1;
	}

	// To get the last substring (or only, if delimiter is not found)
	strings.push_back(str.substr(prev));

	return strings;
}

vector<string> GetIndents(string filename)
{
	std::ifstream t(filename);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	vector<string> res = split_string(str, "\n");
	return res;
}

vector<string> GetSentences(string filename)
{
	std::ifstream t(filename);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	str = regex_replace(str, regex("[\n!]"), string("."));

	vector<string> res = split_string(str, ".");
	return res;
}


double GetPartWeight(string part, map<string, int>* stopwords, map<string, int>* hist, int minWord,
	bool notUseWordCount = false)
{
	//будем вычислять вес части
	double w = 0;
	map<string, int> partHist = HistogramBuilder::CreateFromString(part);
	int wordCount = 0;

	for each (pair<string, int> word in partHist)
	{
		wordCount += word.second;

		//стопслова не учитываем
		if (stopwords->count(word.first))
			continue;

		//находим вес слова
		int wordW = 0;
		if (hist->count(word.first))
		{
			wordW = hist->find(word.first)->second;
		}

		w += pow(wordW, 1.5);
	}

	if (!notUseWordCount)
		w /= wordCount;

	if (wordCount < minWord)
		w = 0;

	return w;
}


string FindAnnotationBySentences()
{
	//строим гистограмму слов
	map<string, int> hist = HistogramBuilder::CreateFromFile("input.txt");

	vector<Word> h = GetHist("input.txt");

	//загружаем стоп-слова
	map<string, int> stopwords = HistogramBuilder::CreateFromFile("stopwords.txt");

	//загружаем части текста - один из них потом выберем
	vector<string> parts = GetSentences("input.txt");

	map<string, double> wei;
	int i = 0;
	for each (string part in parts)
	{
		double w = GetPartWeight(part, &stopwords, &hist,2,true);
		
		//многократно усиливаем вес 1 предложения, но все же не бесконечно
		if (i == 0)
			w *= 50;

		i++;
		
		if (w!=0)
			wei.insert(pair<string, double> {part, w});
	}


	vector<Word> refHist;
	for each (pair<string, int> p in wei)
	{
		Word w;
		w.word = p.first;
		w.count = p.second;
		refHist.insert(refHist.end(), w);
	}
	std::sort(refHist.begin(), refHist.end(), cmp);


	string result = "";
	int sentenceCount = parts.size() / 10;
	if (sentenceCount < 3)
		sentenceCount = 3;
	if (sentenceCount > 7)
		sentenceCount = 7;

	for each (string p in parts)
	{
		bool found = false;
		for (int i = 0; i < sentenceCount; i++)
		{
			if (refHist[i].word == p)
			{
				found = true;
				break;
			}
		}
		if (found)
			result += p +  ".\n";
	}

	return result;
}



string FindAnnotationByIndent()
{
	//строим гистограмму слов
	map<string, int> hist = HistogramBuilder::CreateFromFile("input.txt");

	vector<Word> h = GetHist("input.txt");

	//загружаем стоп-слова
	map<string, int> stopwords = HistogramBuilder::CreateFromFile("stopwords.txt");

	//загружаем части текста - один из них потом выберем
	vector<string> parts = GetIndents("input.txt");

	double bestW = 0;
	string bestStr = "";
	map<string, double> wei;

	for each (string part in parts)
	{
		double w = GetPartWeight(part, &stopwords, &hist,12);
		wei.insert(pair<string, double> {part, w});
		if (w > bestW)
		{
			bestW = w;
			bestStr = part;
		}
	}

	return bestStr;
}




int _tmain(int argc, _TCHAR* argv[])
{
	//ввод текста
	setlocale(LC_ALL, "Russian" );
//	std::locale::global(std::locale("rus"));
	CompareTwo();
	//cout << FindAnnotationBySentences() << endl;
	system("pause");
	return 0;
}




