#include "conf_xsltsheet.h"
#include "conf_xslt_file.h"
#include "conf_home.h"
const string conf_xsltsheet = conf_xslt_file[0] == '/' ? conf_xslt_file : conf_home+"/share/vqwww/"+conf_xslt_file;
