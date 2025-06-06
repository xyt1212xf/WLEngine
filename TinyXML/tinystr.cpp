/*
www.sourceforge.net/projects/tinyxml
Original file by Yves Berquin.

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/
#include "tinyxml.h"

#ifndef TIXML_USE_STL


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tinystr.h"

// TiXmlString constructor, based on a C string
TiXmlString::TiXmlString (const char* instring)
{
    unsigned int newlen;
    char * newstring;

    if (!instring)
    {
        allocated = 0;
        cstring = NULL;
        return;
    }
    newlen = static_cast<int>(strlen (instring) + 1);
    newstring = new char [newlen];
    strcpy (newstring, instring);
    allocated = newlen;
    cstring = newstring;
}

// TiXmlString copy constructor
TiXmlString::TiXmlString (const TiXmlString& copy)
{
    unsigned int newlen;
    char * newstring;

	// Prevent copy to self!
	if ( &copy == this )
		return;

    if (! copy . allocated)
    {
        allocated = 0;
        cstring = NULL;
        return;
    }
    newlen = static_cast<unsigned int>(strlen(copy.cstring) + unsigned int(1));
    newstring = new char [newlen];
    strcpy (newstring, copy . cstring);
    allocated = newlen;
    cstring = newstring;
}

// TiXmlString = operator. Safe when assign own content
void TiXmlString ::operator = (const char * content)
{
    unsigned int newlen;
    char * newstring;

    if (! content)
    {
        empty_it ();
        return;
    }
    newlen = static_cast<unsigned int>(strlen (content) + 1);
    newstring = new char [newlen];
    strcpy (newstring, content);
    empty_it ();
    allocated = newlen;
    cstring = newstring;
}

// = operator. Safe when assign own content
void TiXmlString ::operator = (const TiXmlString & copy)
{
    unsigned newlen;
    char * newstring;

    if (! copy . length ())
    {
        empty_it ();
        return;
    }
    newlen = copy . length () + 1;
    newstring = new char [newlen];
    strcpy (newstring, copy . c_str ());
    empty_it ();
    allocated = newlen;
    cstring = newstring;
}


////< Checks if a TiXmlString contains only whitespace (same rules as isspace)
//bool TiXmlString::isblank () const
//{
//    char * lookup;
//    for (lookup = cstring; * lookup; lookup++)
//        if (! isspace (* lookup))
//            return false;
//    return true;
//}

// append a const char * to an existing TiXmlString
void TiXmlString::append( const char* str, int len )
{
    char * new_string;
    unsigned new_alloc, new_size;

    new_size = length () + len + 1;
    // check if we need to expand
    if (new_size > allocated)
    {
        // compute new size
        new_alloc = assign_new_size (new_size);

        // allocate new buffer
        new_string = new char [new_alloc];        
        new_string [0] = 0;

        // copy the previous allocated buffer into this one
        if (allocated && cstring)
            strcpy (new_string, cstring);

        // append the suffix. It does exist, otherwize we wouldn't be expanding 
        strncat (new_string, str, len);

        // return previsously allocated buffer if any
        if (allocated && cstring)
            delete [] cstring;

        // update member variables
        cstring = new_string;
        allocated = new_alloc;
    }
    else
        // we know we can safely append the new string
        strncat (cstring, str, len);
}


// append a const char * to an existing TiXmlString
void TiXmlString::append( const char * suffix )
{
    char * new_string;
    unsigned new_alloc, new_size;

    new_size = length () + strlen (suffix) + 1;
    // check if we need to expand
    if (new_size > allocated)
    {
        // compute new size
        new_alloc = assign_new_size (new_size);

        // allocate new buffer
        new_string = new char [new_alloc];        
        new_string [0] = 0;

        // copy the previous allocated buffer into this one
        if (allocated && cstring)
            strcpy (new_string, cstring);

        // append the suffix. It does exist, otherwize we wouldn't be expanding 
        strcat (new_string, suffix);

        // return previsously allocated buffer if any
        if (allocated && cstring)
            delete [] cstring;

        // update member variables
        cstring = new_string;
        allocated = new_alloc;
    }
    else
        // we know we can safely append the new string
        strcat (cstring, suffix);
}

// Check for TiXmlString equuivalence
//bool TiXmlString::operator == (const TiXmlString & compare) const
//{
//    return (! strcmp (c_str (), compare . c_str ()));
//}

unsigned TiXmlString::length () const
{
    if (allocated)
        return static_cast<unsigned int>(strlen(cstring));
    return 0;
}


unsigned TiXmlString::find (char tofind, unsigned offset) const
{
    char * lookup;

    if (offset >= length ())
        return (unsigned) notfound;
    for (lookup = cstring + offset; * lookup; lookup++)
        if (* lookup == tofind)
            return static_cast<unsigned int>(lookup - cstring);
    return (unsigned) notfound;
}


bool TiXmlString::operator == (const TiXmlString & compare) const
{
	if ( allocated && compare.allocated )
	{
		assert( cstring );
		assert( compare.cstring );
		return ( strcmp( cstring, compare.cstring ) == 0 );
 	}
	return false;
}


bool TiXmlString::operator < (const TiXmlString & compare) const
{
	if ( allocated && compare.allocated )
	{
		assert( cstring );
		assert( compare.cstring );
		return ( strcmp( cstring, compare.cstring ) > 0 );
 	}
	return false;
}


bool TiXmlString::operator > (const TiXmlString & compare) const
{
	if ( allocated && compare.allocated )
	{
		assert( cstring );
		assert( compare.cstring );
		return ( strcmp( cstring, compare.cstring ) < 0 );
 	}
	return false;
}


#endif	// TIXML_USE_STL
