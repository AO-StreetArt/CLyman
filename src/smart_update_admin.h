//This is the Smart Update Admin Interface
//This defines the methods that must be
//implemented in order to add support
//for a particular storage system/driver
//to be used during smart updates

class SmartUpdateAdmin
{
public:
	virtual const char * load ( const char * key );
	virtual bool save ( const char * key, const char * msg );
	virtual bool del ( const char * key );
	virtual bool exists (const char * key);
};
