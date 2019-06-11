#include <cstdlib>
#include <iostream>
#include <QtWidgets/qapplication.h>
#include "commit.h"
#include "StartScreen.h"
#include "Library.h"

int main(int argc, char * argv[])
{
	std::cout << "Qt version: " << qVersion() << std::endl;

	QApplication app(argc, argv);
	setlocale(LC_NUMERIC, "C");

	Library library;
	library.show();
	
	std::cout << "Blot version: " << BLOT_VERSION_COMMIT_ID << std::endl;

	int status = app.exec();
	
	return status;
}
