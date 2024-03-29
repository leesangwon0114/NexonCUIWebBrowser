#include "HTMLParser.h"

string replaceAll(const string &str, const string &pattern, const string &replace)
{
	string result = str;
	string::size_type pos = 0;
	string::size_type offset = 0;

	while ((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}
	return result;
}

vector<string> &htmlsplit(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<string> htmlsplit(const string &s, char delim) {
	vector<string> elems;
	htmlsplit(s, delim, elems);
	return elems;
}

HTMLParser::HTMLParser(string text) : html(text)
{
	moreParse(text);
	//testParse(text);
}

void HTMLParser::testParse(const string &resource)
{
	typedef string::const_iterator iterator_t;

	if (resource.length() == 0)
		return;

	iterator_t htmlEnd = resource.end();
	iterator_t parseStart = find(resource.begin(), htmlEnd, '<');
	iterator_t current = parseStart;
	iterator_t currentEnd = find(resource.begin(), htmlEnd, '>');
	string tag(current+1, currentEnd);
	vector<string> tempSplit = htmlsplit(tag, ' ');
	string tagname = tempSplit[0];
	Node *initNode = new Node(tagname);
	head = initNode;
	string nextResource = string(currentEnd + 1,  htmlEnd);
	recursion(nextResource, tagname, initNode);
}

void HTMLParser::recursion(const string &resource, const string &curtagname, Node* curNode)
{
	typedef string::const_iterator iterator_t;
	string curtag = curtagname;
	while (curtag != curtagname)
	{
		iterator_t htmlEnd = resource.end();
		iterator_t parseStart = find(resource.begin(), htmlEnd, '<');
		iterator_t current = parseStart;
		iterator_t currentEnd = find(resource.begin(), htmlEnd, '>');
		string tag(current+1, currentEnd);
		vector<string> tempSplit = htmlsplit(tag, ' ');
		string tagname = tempSplit[0];
		if (tagname == ("/" + curtagname))
		{
			string content = string(resource.begin(), parseStart);
			curNode->setTagContent(content);
		}
		else
		{
			Node *newNode = new Node(tagname);
			curNode->insertNode(newNode);
			string restHtml = string(currentEnd + 1, htmlEnd);
			recursion(restHtml, tag, newNode);
		}
	}
}

void HTMLParser::moreParse(const string &resource)
{
	typedef string::const_iterator iterator_t;

	if (resource.length() == 0)
		return;

	iterator_t htmlEnd = resource.end();

	iterator_t parseStart = find(resource.begin(), htmlEnd, '<');

	iterator_t current = parseStart;

	while (current != htmlEnd)
	{
		iterator_t tagStart = find(current, htmlEnd, '<');
		iterator_t tagEnd = find(tagStart, htmlEnd, '>');
		string tag(tagStart, tagEnd);
		tag = tag.erase(0, 1);
		string tagname = getTag(tag);
		
		if (tagname == "img")
		{
			string src(strstr(tag.c_str(), "src"));
			vector<string> tempSplit = htmlsplit(src, ' ');
			int s = tempSplit[0].size();
			if ((s - 5) >= 0)
				src = tempSplit[0].substr(5, tempSplit[0].size() - 5) + "image";
			if(src.length()!=0)
				result.push_back(src);
			current = tagEnd;
		}
		else if(tagname == "a")
		{ 
			string link(strstr(tag.c_str(), "href"));
			vector<string> tempSplit = htmlsplit(link, ' ');
			int s = tempSplit[0].size();
			if((s - 6) >=0)
				link = tempSplit[0].substr(6, tempSplit[0].size() - 6);
			if (link.length() != 0)
			{
				link = link.erase(link.length() - 1, 1);
				string temp(current, htmlEnd);
				string hyperText = removeTag(temp, tagname, tag.length());// + "hyperText";
				hyperText = replaceAll(hyperText, "<span>", "");
				hyperText = replaceAll(hyperText, "</span>", "");
				pair<string, string> p(hyperText, link);
				hyperLink.insert(p);
				result.push_back(hyperText + "hyperText");
			}
			current = tagEnd;
		}
		else
		{
			if (tagname != "none")
			{
				// tag를 제거한 string에서 <br>, <span> 태그 제거
				string temp(current, htmlEnd);
				temp = removeTag(temp, tagname, tag.length()) + tagname;
				temp = replaceAll(temp, "<br>", "\n");
				temp = replaceAll(temp, "</br>", "\n");
				temp = replaceAll(temp, "<span>", "");
				temp = replaceAll(temp, "</span>", "");
				result.push_back(temp);
			}
			current = tagEnd;
		}
	}
}

string HTMLParser::getTag(string tag)
{
	string htag = tag.substr(0, 2);
	if (tag.substr(0, 2) == "a ")
	{
		return "a";
	}
	else if (tag.substr(0, 1) == "p")
	{
		return "p";
	}
	else if (htag == "h1" || htag == "h2" || htag == "h3" || htag == "h4" || htag == "h5" || htag == "h6")
	{
		return htag;
	}
	else if (tag.substr(0, 3) == "img")
	{
		return "img";
	}
	else if (tag.substr(0, 5) == "title")
	{
		return "title";
	}
	else if (tag.substr(0, 6) == "center")
	{
		return "center";
	}
	else
		return "none";
}

// <> </> Tag 제거하기
string HTMLParser::removeTag(string tag, string tagname, int len)
{
	string temp = "";
	temp = tag.substr((tag.find("<" + tagname) + len + 1), (tag.find("</" + tagname + ">")) - (tag.find("<" + tagname) + len + 1));
	temp = strstr(temp.c_str(), ">");
	temp = temp.erase(0, 1);
	return temp;
}