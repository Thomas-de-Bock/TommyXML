# TommyXML
## Single-header XML parser
 
Make mainly for ThomasTheGameEngine

Able to parse files like:

```
<Player path="objects/MyGame/player/" start="appear">
        <Anim id="appear" path="appear" delay="0.1" frames="0-2" goto="idle"/>
        <Loop id="idle" path="idle" delay="0.1" frames="0"/>
        <Anim id="walk" path="walk" delay="0.01" frames="0-4"/>
</Player>
```
Note how all values are stored in element properties. It is unable to parse unnamed values such as:
```
<Name>Thomas</Name>
```

Open and parse files using the `XML::openFile(path)` function. This function returns an XMLelem object, this object holds a child element vector named: `elems`, an XMLtag vector named: `tags` and a string called `name`. XMLtag objects hold only name and value parameters.
The XMLelem class contains many filtering functions:
```
std::vector<XMLtag> getTags(std::string Name);
	std::vector<XMLelem> getElems(std::string Name);
	std::vector<XMLelem> getElemsWithTag(std::string tagName);
	std::vector<XMLtag> getTags(XMLtag tag);
	std::vector<XMLelem> getElemsWithTag(XMLtag tag);
```
Parsing a file returns a single XMLelem named after the filename.
To make sure a file has been properly parsed, use the `str` function of the XMLelem class.

Example usage:
```cpp
std::cout << XML::openFIle("/path/to/playerAnimations.xml").str();
```
Executed on the previously displayed example file, prints out the result:
```
PlayerAnimations
        Player path: objects/MyGame/player/,  start: appear,
                Anim id: appear,

                Loop id: idle,

                Anim id: walk,
```
