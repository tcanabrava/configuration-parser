Description:

The language to define the settings has a really small subset of actions, so it's easy to use any class or object for it.
If you plan to use a QObject, than the QObject has to have a default constructor for auto-creation purposes, and all Q_PROPERTY
will be saved and restored automatically.

All Objects that are supported by QSettings can also be used automatically, and you can also have scoped objects defined directly into the settings configuration file.

Language Definition:

Comment:
	-> // string
	-> /* string */

Property:
	type name = value
	type name = { setter(without 'set') = value, ... }

Classs:
	Class | Array { Class / Property, ... }
	Property | Array
	Comment

InitialState:
	-> '#include <string>'
	-> Class
	-> Comment
