this is a simple configuration parser for QSettings based projects.

Purpose:
QSettings works nicely but we have a problem with runtime errors as it's impossible for QSettings
to know if you wanna use the "locale" or the "Iocale", it's really easy to commit mistakes using
string based key-value pairs. Also it's hard to know when a value is being saved on disk for the
first time or being loaded, as QSettings tends to make those thigns not explicit.

The parser acts on a Configuration file and generates correct C++ / Qt source code that can be
directly imported into your project, making the usage of QSettings much safer, as it's

- Compile Time Safe (you will get errors if you did something wrong on the configuration file)
- Run time safe (you will get errors if yo pass invalid types to the settings)
- Not possible to have typos on keys anymore
- supports Enums natively (while QSettings doesn't)

On the configuration file, Each level of brackets opens a new configuration group,
and normal c++ objects can be used if QSettings supports them, you can also specify default values
and use includes and enums:

#include <QString>

napespace Pref {
	Preferences {
		MainWindow {
			QSize size
			QString title
		}
		Network {
			Proxy {
				QString port
				QString address
			}
			QString username = "untitled"
		}
	}
}


this example will create Preferences, MainWindow and Network classes ( I probably should add them into a namespace),
while the Preferences is a singleton, and have all it's children. you can invoke like this:

Preferences::self()->mainWindow()->title();
Preferences::self()->network()->proxy()->port();

every configuration has a Changed signal, just like QML, and you can connect on them easily:

connect(Preferences::self()->mainWindow(), &MainWindow::titleChanged, [](const QString& s) {
    qCDebug(test) << "Title Changed to" << s;
})

TODO:
watch the cofiguration file on disk and act upon it's changes.
accept includes with ""  and <>, currently just <> is accepted.
