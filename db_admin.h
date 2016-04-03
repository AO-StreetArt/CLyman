#include "obj3.h"

class DBAdmin
{
public:
	virtual Obj3 load_object ( const char * key ) = 0;
	virtual void save_object ( Obj3& obj ) = 0;
};
