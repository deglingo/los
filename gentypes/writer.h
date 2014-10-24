/* writer.h -
 */

#ifndef _WRITER_H_
#define _WRITER_H_

#include "base.h"



typedef struct _Writer Writer;

typedef enum _WNodeType WNodeType;
typedef union _WNode WNode;
typedef struct _WSection WSection;
typedef struct _WText WText;

#define WNODE_HEADER \
  WNodeType type



/* Writer:
 */
struct _Writer
{
  gchar *fname;
  WNode *root;
};



/* WNodeType:
 */
enum _WNodeType
  {
    WNODE_TYPE_SECTION,
    WNODE_TYPE_TEXT,
  };



/* WSection:
 */
struct _WSection
{
  WNODE_HEADER;
  gchar *name;
  GPtrArray *children;
};



/* WText:
 */
struct _WText
{
  WNODE_HEADER;
  gchar *text;
};



/* WNode:
 */
union _WNode
{
  WNodeType type;
  WSection section;
  WText text;
};



Writer *writer_new ( const gchar *fname );
void writer_write ( Writer *writer );

WNode *wsection_new ( WNode *parent,
                      const gchar *name );
void wsection_writef ( WNode *section,
                       const gchar *format,
                       ... )
  G_GNUC_PRINTF(2, 3);
void wsection_writev ( WNode *section,
                       const gchar *format,
                       va_list args );
void wsection_write ( WNode *section,
                      const gchar *text );
void wsection_add ( WNode *section,
                    WNode *first_node,
                    ... )
  G_GNUC_NULL_TERMINATED;
WNode *wtext_newf ( const gchar *format,
                    ... )
G_GNUC_PRINTF(1, 2);



#endif
