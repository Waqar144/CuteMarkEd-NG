## Change Log

#### Version 0.12

- `INFO` Revival of project and renamed as CuteMarkEd NG
- `IMPROVED` Removed Discount and Hoedown libraries in favour of MD4C for markdown to HTML Conversions
- `IMPROVED` Switched to QWebEngineView in preview.
- `IMPROVED` Updated highlight.js library
- `IMPROVED` Switched to QMarkdownTextEdit in the editor for better performance and more features
- `IMPROVED` Updated code highlighting css styles
- Dropped usage Qt foreach in favour of cpp-11 range for.

#### Version 0.11.3

Improvements:

* `IMPROVED` Update Russian translation

Fixes:

* `FIXED` Missing links in exported PDF file (#275)
* `FIXED` Use of non-native line endings when a file is saved (#97)
* `FIXED` Corrupt files on system crashes or running out of disk space (#285)
* `FIXED` Accessing all drives although the file explorer is not visible (#273)
* `FIXED` Relative paths in recently used files menu (#256)
 
#### Version 0.11.2

Improvements:

* `IMPROVED` Added Hungarian translation

Fixes:

* `FIXED` Editor pane jumping up and down during editing (#232)
* `FIXED` Missing mermaid CSS for styling in preview (#241)
* `FIXED` Correct order of HTML Preview/Source menu item (#242)
* `FIXED` Retrieval of last used style on application start on Linux (#257)
* `FIXED` Crash when switching between markdown converters (#260)

#### Version 0.11.1

Improvements:

* `IMPROVED` Updated French translation

Fixes:

* `FIXED` Custom shortcuts not working (#224)
* `FIXED` Disappearing spell checker highlighting (#228)
* `FIXED` Wrong german quotes snippet (#229)

#### Version 0.11.0

Highlights:

The 0.11.0 release offers support to create flowchart and sequence diagrams using [mermaid](https://github.com/knsv/mermaid). 

![screenshot](http://cloose.github.io/CuteMarkEd/images/20150426-cutemarked-diagrams.png)

The snippet completer was extended to also auto complete words from the document:

![screenshot](http://cloose.github.io/CuteMarkEd/images/20150426-cutemarked-word-completion.png)

New Features:

* `NEW` Added support to create diagrams using [mermaid](https://github.com/knsv/mermaid) (#215).
* `NEW` Added auto completion for words extracted from the document (#194)
* `NEW` Added option to ignore YAML header in editor and preview (#136, #139)
* `NEW` Added possibility to change keyboard shortcuts to the options dialog (#144)
* `NEW` Added zoom to HTML preview and the option to change the default font family and size for the HTML preview (#169)
* `NEW` Added synchronization of the current slide between editor and preview in presentation
mode (#184)

Improvements:

* `IMPROVED` More mnemonics in main menu and option dialog (#104)
* `IMPROVED` Also support file extension .mdown (#155)
* `IMPROVED` Save last used style on application exit (#159)
* `IMPROVED` Find/Replace widget can be closed with ESC key (#162) 

Fixes:

* `FIXED` Build with MSVC 2013 and MacOSX
* `FIXED` Parallel build with e.g. make -j2