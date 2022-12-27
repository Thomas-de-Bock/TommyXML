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
