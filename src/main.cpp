#include <cstdlib>
#include <iostream>
#include <QtWidgets/qapplication.h>
#include "commit.h"

int main(int argc, char * argv[])
{
	std::cout << "Qt version: " << qVersion() << std::endl;

	QApplication app(argc, argv);
	setlocale(LC_NUMERIC, "C");

	/*
	StartScreen startScreen(NULL, argc, argv);
	startScreen.show();
	*/
	
	std::cout << "Blot version: " << BLOT_VERSION_COMMIT_ID << std::endl;

	int status = app.exec();
	
	return status;
}
