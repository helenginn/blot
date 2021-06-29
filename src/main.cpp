#include <cstdlib>
#include <time.h>
#include <iostream>
#include <QApplication>
#include <QOpenGLContext>
#include "commit.h"
#include "StartScreen.h"
#include "Library.h"

int main(int argc, char * argv[])
{
	QSurfaceFormat fmt;
	if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) 
	{
		std::cout << "OpenGL 3.3 context" << std::endl;
		fmt.setVersion(3, 3);
		fmt.setProfile(QSurfaceFormat::CoreProfile);
	}
	else 
	{
		std::cout << "OpenGL 3.0 context" << std::endl;
		fmt.setVersion(3, 0);
	}

	std::cout << "OpenGL Version: " << fmt.version().first << "." <<
	fmt.version().second << std::endl;
	QSurfaceFormat::setDefaultFormat(fmt);

	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
	QApplication app(argc, argv);

	QOpenGLContext *global = QOpenGLContext::globalShareContext();
	global->setShareContext(global);
	global->create();

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
