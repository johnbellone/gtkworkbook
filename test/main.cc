#include <gtest/gtest.h>
#include <gtk/gtk.h>

int
main (int argc, char * argv[]) {
	std::cout << "Running GTKWorkbook Unit Tests from main.cc\r\n";
	testing::InitGoogleTest (&argc, argv);
	gtk_init(&argc, &argv);
	return RUN_ALL_TESTS();
}
