#include "CsvParser.hpp"

/* This structure is due to the libcsv parser; it uses function pointers to
   do any work inside of an actual tuple. So the cb1 is called after a field
   is parsed and cb2 is called after a tuple/row is parsed. */
static void 
cb1 (void * s, size_t length, void * data) {

}

static void
cb2 (int c, void * data) {

}

void *
CsvParser::run (void * null) {
 this->running = true;
 struct csv_parser csv;
    
 if (csv_init (&csv, CSV_STRICT) != 0)
   {
     std::cerr << "Failed initializing libcsv parser\n";
     return NULL;
   }

 while (this->running == true)
   {
     while (this->inputQueue.size() > 0)
       {
	 std::string buf = this->inputQueue.pop();
	 size_t bytes = buf.length();
	 
	 // Parse the CSV input
	 if ((bytes = csv_parse(&csv, 
				buf.c_str(), 
				bytes, 
				cb1,
				cb2,
				this->cell)) != buf.length())
	   {
	     // Something went wrong here. Discard and continue.
	     if (csv_error (&csv) == CSV_EPARSE) 
	       std::cerr << "Parsing error on input:\n" << buf;
	     continue;
	   }
       }

   }

 csv_free (&csv);
 return NULL;
}
