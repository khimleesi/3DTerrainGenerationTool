#pragma once
/*!
	@file NoCopyOrAssignment.h
	@brief Disables copying or assignment within classes/structs.
	@author PrettyBinary
	@date 23/07/2018

	@class Pretty::Utilities::_NoCopyOrAssignment
	@brief Disables copying or assignment within classes/structs.
	
	@warning __NO DIRECT ACCESS__
	- This class should not be accessed directly. A class/struct must derive from this class for access.

	This class allows the user to easily make copy constructors and assignment operators private
	within a given class. This will disable any copying or move operations on their chosen class.
	
	> __Usage__

	Derive your class/struct from this class as follows (make sure the access level is private):

		class MyClass : private _NoCopyOrAssignment

	Another nice feature to using inheritance over, say, a macro is that any time you access
	your chosen class directly, you will now see whether or not it is copyable:

	![_imgNoCopyOrAssignment](_imgNoCopyOrAssignment.png)

*******************************************************************************************************************/

class NoCopyOrAssignment {

protected:
	NoCopyOrAssignment()										= default;
	~NoCopyOrAssignment()										= default;
	NoCopyOrAssignment(const NoCopyOrAssignment&)				= delete;
	NoCopyOrAssignment& operator=(const NoCopyOrAssignment&)	= delete;
};
