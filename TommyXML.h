#ifndef TommyXML
#define TommyXML

#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<algorithm>
#include<ctype.h>
#include<stack>
#include<tuple>

class XMLtag {
public:
	std::string name;
	std::string value;

	XMLtag(std::string Name, std::string Value) { name = Name; value = Value; }
};

class XMLelem {
public:
	std::vector<XMLelem> elems;
	std::vector<XMLtag> tags;
	std::string name;

	//Bunch of filtering functions
	std::vector<XMLtag> getTags(std::string Name);
	std::vector<XMLelem> getElems(std::string Name);
	std::vector<XMLelem> getElemsWithTag(std::string tagName);
	//Use for only tags matching in both name and value:
	std::vector<XMLtag> getTags(XMLtag tag);
	std::vector<XMLelem> getElemsWithTag(XMLtag tag);
	std::string str(int indentAmount = 0);

	XMLelem(std::string Name) { name = Name; }
	XMLelem(std::string Name, std::vector<XMLelem> Elems) { name = Name; elems = Elems; }
	XMLelem(std::string Name, std::vector<XMLtag> Tags) { name = Name; tags = Tags; }
	XMLelem(std::string Name, std::vector<XMLtag> Tags, std::vector<XMLelem> Elems) { name = Name; tags = Tags; elems = Elems; }

};

//Only used for internal parsing purposes
class XMLparseObject {
public:
	//isElem is true for both open and closing elems
	bool isElem = false;
	bool isClose = false;
	bool isOpen = false;

	bool isTag = false;
	bool isValue = false;
	//Means it has no seperate closing tag, can only be true if isElem and isOpen is true
	bool hasChildren = false;

	std::string content;


	XMLparseObject(std::string Content, bool IsValue) { content = Content; isValue = IsValue; }
	XMLparseObject(std::string Content) { content = Content; }
};

//Contains general file handling and parsing
class XML {
public:
	//returns single big XMLelem using filename as name
	static XMLelem openFile(std::string path);

	//Does not take multiple non-hierarchal elements,
	//this is why open and closing elements with filename are placed
	//at the start and end of files before reading
	static XMLelem XMLelemFromObjects(std::vector<XMLparseObject> data);

private:
	static std::string readFile(std::string path);
	//Splits file into XMLparseObjects
	static std::vector<XMLparseObject> preprocessFile(std::string file);
};




//XMLelem function definitions

std::vector<XMLtag> XMLelem::getTags(std::string Name) {
	std::vector<XMLtag> returnTags;
	for(int i = 0; i < tags.size(); i++) {
		if(tags[i].name == Name) {
			returnTags.push_back(tags[i]);
		}
	}
	return returnTags;
}

std::vector<XMLelem> XMLelem::getElems(std::string Name) {
	std::vector<XMLelem> returnElements;
	for(int i = 0; i < elems.size(); i++) {
		if(elems[i].name == Name) {
			returnElements.push_back(returnElements[i]);
		}
	}
	return returnElements;
}

std::vector<XMLelem> XMLelem::getElemsWithTag(std::string Name) {
	std::vector<XMLelem> returnElements;
	for(int i = 0; i < elems.size(); i++) {
		if(elems[i].getTags(Name).size() > 0) {
			returnElements.push_back(elems[i]);
		}
	}
	return returnElements;
}

std::vector<XMLtag> XMLelem::getTags(XMLtag tag) {
	std::vector<XMLtag> returnTags;
	for(int i = 0; i < tags.size(); i++) {
		if(tags[i].name == tag.name && tags[i].value == tag.value) {
			returnTags.push_back(tags[i]);
		}
	}
	return returnTags;
}

std::vector<XMLelem> XMLelem::getElemsWithTag(XMLtag tag) {
	std::vector<XMLelem> returnElements;
	for(int i = 0; i < elems.size(); i++) {
		if(elems[i].getTags(tag).size() > 0) {
			returnElements.push_back(elems[i]);
		}
	}
	return returnElements;
}

std::string XMLelem::str(int indentAmount) {
	std::string returnStr = "";
	for(int i = 0; i < indentAmount; i++) {
		returnStr += "\t";
	}
	returnStr += name;
	for(int i = 0; i < tags.size(); i++) {
		returnStr += " " + tags[i].name + ": " + tags[i].value + ", ";
	}
	returnStr += "\n";
	for(int i = 0; i < elems.size(); i++) {
		returnStr += elems[i].str(indentAmount+1) + "\n";
	}
	return returnStr;
}

//XML function definitions

XMLelem XML::openFile(std::string path) {
	std::vector<XMLparseObject> file = preprocessFile(readFile(path));
	std::string filename = path.substr(path.find_last_of("/\\") + 1);

	int dotIndex = filename.find_last_of("."); 
	if(dotIndex != std::string::npos) {
		filename = filename.substr(0, dotIndex); 
	}

	XMLparseObject docOpenElem(filename);
	docOpenElem.isOpen = true;
	docOpenElem.hasChildren = true;
	docOpenElem.isElem = true;
	XMLparseObject docCloseElem(filename);
	docCloseElem.isClose = true;
	docCloseElem.isElem = true;

	file.insert(file.begin(), docOpenElem);
	file.push_back(docCloseElem);

	XMLelem docElement = XMLelemFromObjects(file);

	return docElement;
}

XMLelem XML::XMLelemFromObjects(std::vector<XMLparseObject> data) {
	//Use recursion to get all child elements and get own tags and name
	std::string name = data[0].content;
	if(data.size() <= 1) {
		return XMLelem(name);
	}


	std::vector<XMLtag> allTags;
	std::vector<XMLelem> allElements;
	bool scanningOwnTags = true;
	bool selectingElem = false;
	int selectedElemStartIndex = -1;
	for(int i = 1; i < data.size(); i++) {
		if(scanningOwnTags && data[i].isTag) {
			allTags.push_back(XMLtag(data[i].content, data[i+1].content));
		} else if(!selectingElem && data[i].isOpen) {
			scanningOwnTags = false;
			if(data[i].hasChildren) {
				selectingElem = true;
				selectedElemStartIndex = i;
			} else {
				if(i == data.size()-1 || !data[i+1].isTag) {
					allElements.push_back(XMLelem(data[i].content));
					continue;
				}
				std::vector<XMLtag> noChildElemTags;
				for(int j = i+1; j < data.size(); j++) {
					if(data[j].isTag) {
						noChildElemTags.push_back(XMLtag(data[j].content, data[j+1].content));
					} else{
						break;
					}
				}
				allElements.push_back(XMLelem(data[i].content, noChildElemTags));
			}
		} else if(selectingElem && data[i].isClose && data[i].content == data[selectedElemStartIndex].content) {
			std::vector<XMLparseObject> subData = {data.begin() + selectedElemStartIndex, data.end() - (data.size()-i-1)};
			std::cout << "recursion on: " << std::endl;
			for(int i = 0; i < subData.size(); i++) {
				std::cout << "- " << subData[i].content << std::endl;
			}
			allElements.push_back(XMLelemFromObjects(subData));
			selectedElemStartIndex = -1;
			selectingElem = false;
		}
	}
	return XMLelem(name, allTags, allElements);
}

std::string XML::readFile(std::string path) {
    std::ifstream inFile;
    inFile.open(path);

    std::stringstream strStream;
    strStream << inFile.rdbuf(); 
    return strStream.str(); 
}

std::vector<XMLparseObject> XML::preprocessFile(std::string file) {
	//Remove newlines
	file.erase(std::remove(file.begin(), file.end(), '\n'), file.cend());
	file.erase(std::remove(file.begin(), file.end(), '\t'), file.cend());
	file.erase(std::remove(file.begin(), file.end(), '\v'), file.cend());
	std::cout << file << std::endl;
	std::vector<XMLparseObject> cutFile;
	std::string currentStr = "";
	bool isVal = false;
	for(int i = 0; i < file.length(); i++) {
		if((file[i] == '<' || file[i] == '>' || file[i] == '=' || file[i] == '/' || file[i] == ' ') && !isVal) {
			if(!currentStr.empty()) {
				cutFile.push_back(XMLparseObject(currentStr, isVal));
			}
			if(file[i] != ' ') { //So it's also split by spaces without including them
				currentStr = file[i];
				if(!currentStr.empty()) {
					cutFile.push_back(XMLparseObject(currentStr, isVal));
				}
			}
			currentStr = "";
			continue;
		}
		if(file[i] == '"') {
			if(!currentStr.empty()) {
				cutFile.push_back(XMLparseObject(currentStr, isVal));
			}
			currentStr = "";
			isVal = !isVal;
			continue;
		}
		currentStr += file[i];
	}
	//Check for characters: <, >, / and =. Then assign the XMLparseObject bools accordingly
	for(int i = 1; i < cutFile.size()-1; i++) {
		if(cutFile[i].isValue) {
			continue;
		}
		if(cutFile[i-1].content == "<") {
			cutFile[i].isOpen = true;
			cutFile[i].isElem = true;
			cutFile[i].hasChildren = true;
			cutFile.erase(cutFile.begin() + i-1);
			i--;
		} else if(cutFile[i-1].content == "/") {
			cutFile[i].isClose = true;
			cutFile[i].isElem = true;
		} else if(cutFile[i+1].content == "=") {
			cutFile[i].isTag = true;
			cutFile.erase(cutFile.begin() + i+1);
			i--;
		}
	}

	//Assign the hasChildren parameter of the XMLparsingObjects
	//Can't remove chars here because of lastOpenIndex
	bool isLookingForEnd = false;
	int lastOpenIndex = -1;
	for(int i = 0; i < cutFile.size()-1; i++) {
		if(cutFile[i].isElem && cutFile[i].isOpen) {
			isLookingForEnd = true;
			lastOpenIndex = i;
		} else if(cutFile[i].content == "/" && cutFile[i+1].content == ">" && isLookingForEnd) {
			cutFile[lastOpenIndex].hasChildren = false;
		}
	}

	for(int i = 0; i < cutFile.size(); i++) {
		if(cutFile[i].content == "/" || cutFile[i].content == "" || cutFile[i].content == ">" || cutFile[i].content == "<") {
			cutFile.erase(cutFile.begin() + i);
			i--;
		}
	}

	return cutFile;
}



#endif