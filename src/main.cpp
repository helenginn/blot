#include <cstdlib>
#include <time.h>
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
	srand(time(NULL));

	/*
	Library library;
	library.show();
	*/
	
	StartScreen start;
	start.show();
	
	std::cout << "Blot version: " << BLOT_VERSION_COMMIT_ID << std::endl;
	
	if (argc > 1)
	{
		start.openLibraryFile(argv[1]);
	}

	int status = app.exec();
	
	return status;
}
