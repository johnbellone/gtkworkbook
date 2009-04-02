#ifndef HPP_PACKET
#define HPP_PACKET

#include <workbook/cell.h>
#include <glib.h>
#include <string>
#include <vector>
#include <map>

#define String std::string
#define Array  std::vector
#define Map    std::map

class Packet
{
private:
  /* Members */
  String time;
  String type;
  Array<String> fields;
  gchar delimiter;
public:
  enum 
    {
      TYPE_UPDATECELL = 0,
      TYPE_ADDSHEET,
      TYPE_REMSHEET,
      TYPE_MOVESHEETINDEX,
      TYPE_MOVESHEET,
      TYPE_SAVESHEET,
      TYPE_LOADSHEET,
      /**/
      MAX_TYPES
    };
  
  Packet (void);
  ~Packet (void);
  
  /* Methods */
  gboolean parse (const gchar *);
  Map<String,String> parseFormatString (const gchar *);

  inline gchar getDelimiter (void) const { return this->delimiter; }
  inline gint getType (void) const { return atoi (this->type.c_str()); }
  inline const gchar * getTime (void) const { return this->time.c_str(); }
  inline gint getFieldSize (void) const { return this->fields.size(); }
  inline const gchar * operator[] (gint index) const 
  {
    return this->fields.at(index).c_str();
  }
  inline size_t size (void) const { return this->fields.size(); }
};

#endif
